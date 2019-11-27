/**
 * linux.h
 * Deals with the Linux devices.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "linux.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <blkid/blkid.h>

#define SYSFS_PARTITIONS_PATH "/sys/block/"

// Private methods.
bool ignore_dir_entry(const struct dirent *dir);
bool freadnum(const char *fpath, size_t *num);
bool get_size(stdev_t *sd);
bool get_permission(stdev_t *sd);
bool sysfs_exists();
bool sysfs_device_info(stdev_t *sd);
bool sysfs_device_list(stdev_container *devlist);
bool blkid_info(stdev_t *sd);


/**
 * Populates a storage device container.
 *
 * @param  container Storage device structure container.
 * @return           TRUE if everything went fine.
 */
bool linux_populate_devices(stdev_container *container) {
	// Check with device discovery system we are going to use.
	if (sysfs_exists()) {
		// Use sysfs.
		if (!sysfs_device_list(container))
			return false;
	} else {
		fprintf(stderr, "Cannot determine a device discovery system to use.\n");
		return false;
	}

	// Use blkid to get more information for our devices.
	for (int i = 0; i < container->count; i++) {
		if (!blkid_info(&container->list[i]))
			return false;
	}

	return true;
}

/**
 * Gets a device information using blkid.
 *
 * @param  sd Storage device structure.
 * @return    TRUE if the probing went fine.
 */
bool blkid_info(stdev_t *sd) {
	char devpath[DEVICE_PATH_MAX_LEN];
	snprintf(devpath, DEVICE_PATH_MAX_LEN, "/dev/%s", sd->name);

	// Create a blkid probe.
	blkid_probe pr = blkid_new_probe_from_filename(devpath);
	if (!pr) {
		fprintf(stderr, "Failed to create a blkid probe for %s.\n", devpath);
		return false;
	}

	// Get partitions.
	blkid_partlist ls = blkid_probe_get_partitions(pr);
	sd->num_partitions = blkid_partlist_numof_partitions(ls);
	sd->partitions = malloc(sizeof(partition_t) * sd->num_partitions);
	for (int i = 0; i < sd->num_partitions; i++) {
		blkid_partition part = blkid_partlist_get_partition(ls, i);
		sd->partitions[i].num = blkid_partition_get_partno(part);
		sd->partitions[i].uuid = "";
		sd->partitions[i].label = "";
		sd->partitions[i].size = blkid_partition_get_size(part);
		sd->partitions[i].type = blkid_partition_get_type_string(part);
	}

	blkid_free_probe(pr);
	return true;
}

/**
 * Checks if the sysfs block device folders exists.
 *
 * @return TRUE if there are block device folders.
 */
bool sysfs_exists() {
	return access(SYSFS_PARTITIONS_PATH, F_OK) != -1;
}

/**
 * Retrieves a block device list.
 *
 * @param  devlist Array of block devices to be populated.
 * @return         TRUE if the operation was successful.
 */
bool sysfs_device_list(stdev_container *devlist) {
	DIR *dh;
	struct dirent *dir;

	// Open the block device folder.
	dh = opendir(SYSFS_PARTITIONS_PATH);
	if (dh == NULL) {
		fprintf(stderr, "Couldn't open %s to list partitions.\n",
				SYSFS_PARTITIONS_PATH);
		return false;
	}

	// Get the directory listing.
	while ((dir = readdir(dh)) != NULL) {
		// Filter out anything that isn't a block device.
		if (ignore_dir_entry(dir)) {
			continue;
		}

		// Get device information.
		stdev_t sd;
		strncpy(sd.name, dir->d_name, PARTITION_NAME_MAX_LEN);
		sysfs_device_info(&sd);
		if (sd.size == 0)
			continue;

		// Add the storage device to the list.
		device_list_push(devlist, sd);
	}

	// Clean up.
	closedir(dh);
	dh = NULL;
	return true;
}

/**
 * Gets information about a given block device.
 *
 * @param  sd      Storage device structure to be populated with information.
 * @return         TRUE if the parsing was successful.
 */
bool sysfs_device_info(stdev_t *sd) {
	// Build device path.
	strcpy(sd->path, SYSFS_PARTITIONS_PATH);
	strcat(sd->path, sd->name);

	// Get device size.
	if (!get_size(sd))
		return false;

	// Get device permission.
	if (!get_permission(sd))
		return false;

	return true;
}

/**
 * Gets the size of the block device in bytes.
 *
 * @param  sd Storage device structure to be populated with information.
 * @return    TRUE if the parsing was successful.
 */
bool get_size(stdev_t *sd) {
	char attrpath[PATH_MAX];

	// Get the number of sectors.
	snprintf(attrpath, PATH_MAX, "%s/size", sd->path);
	if (!freadnum(attrpath, &sd->sectors)) {
		fprintf(stderr, "Failed to read the number of sectors for %s.\n",
				sd->path);
		return false;
	}
	
	// Get the number of bytes per sector.
	snprintf(attrpath, PATH_MAX, "%s/queue/hw_sector_size", sd->path);
	if (!freadnum(attrpath, &sd->sector_size)) {
		fprintf(stderr, "Failed to read the sector size for %s.\n", sd->path);
		return false;
	}

	// Calculate the size.
	sd->size = sd->sectors * sd->sector_size;
	return true;
}

/**
 * Gets the permission of a block device. (Read/Write)
 *
 * @param  sd Storage device structure to be populated with information.
 * @return    TRUE if the parsing was successful.
 */
bool get_permission(stdev_t *sd) {
	char attrpath[PATH_MAX];
	size_t perm;

	// Get the permission.
	snprintf(attrpath, PATH_MAX, "%s/ro", sd->path);
	if (!freadnum(attrpath, &perm)) {
		fprintf(stderr, "Failed to read %s permissions.\n",
				sd->path);
		return false;
	}

	// Convert to boolean and return.
	sd->ro = (perm & true);
	return true;
}

/**
 * Reads a number from a file that only contains it.
 *
 * @param  fpath File path.
 * @param  num   Pointer to the number found in the file.
 * @return       TRUE if the parsing was successful.
 */
bool freadnum(const char *fpath, size_t *num) {
	FILE *fh;
	bool success = false;

	// Open the file.
	fh = fopen(fpath, "r");
	if (fh == NULL) {
		fprintf(stderr, "Couldn't open %s.\n", fpath);
		return false;
	}

	// Read the number from the file.
	success = (fscanf(fh, "%zu[^\n]", num) == 1);

	// Clean up.
	fclose(fh);
	fh = NULL;
	return success;
}

/**
 * Checks if a directory should be ignored because it's not a valid block
 * device.
 *
 * @param  dir Directory structure returned by readdir().
 * @return     TRUE if it *SHOULD* be ignored.
 */
bool ignore_dir_entry(const struct dirent *dir) {
	// Ignore anything starting with a dot.
	if (dir->d_name[0] == '.') {
		return true;
	}

	return false;
}

