/**
 * linux.c
 * Deals with Linux devices.
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
#include <mntent.h>

// Constants.
#define SYSFS_BLOCKDEVS_PATH "/sys/block/"
#define MOUNTPOINT_DEF_PATH "/etc/mtab"


// Private methods.
bool ignore_dir_entry(const struct dirent *dir);
bool freadnum(const char *fpath, size_t *num);
bool get_device_size(stdev_t *sd);
bool get_device_permission(stdev_t *sd);
bool get_partitions(stdev_t *sd);
bool get_partitions_mountpoints(stdev_t *sd);
bool sysfs_exists();
bool sysfs_device_info(stdev_t *sd);
bool get_partitions_size(stdev_t *sd);
bool get_partitions_permission(stdev_t *sd);
bool blkid_info(stdev_t *sd);
bool sysfs_device_list(stdev_container *devlist);


/**
 * Populates a storage device container.
 *
 * @param  container Storage device structure container.
 * @param  useblkid  Use blkid for probing? (requires root)
 * @return           TRUE if everything went fine.
 */
bool populate_devices(stdev_container *container, const bool useblkid) {
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
	if (useblkid) {
		for (int i = 0; i < container->count; i++) {
			if (!blkid_info(&container->list[i]))
				return false;
		}
	}

	return true;
}

/**
 * Checks if the sysfs block device folders exists.
 *
 * @return TRUE if there are block device folders.
 */
bool sysfs_exists() {
	return access(SYSFS_BLOCKDEVS_PATH, F_OK) != -1;
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
	
	// Initialize the container.
	devlist->count = 0;

	// Open the block device folder.
	dh = opendir(SYSFS_BLOCKDEVS_PATH);
	if (dh == NULL) {
		fprintf(stderr, "Couldn't open %s to list block devices.\n",
				SYSFS_BLOCKDEVS_PATH);
		return false;
	}

	// Get the directory listing.
	while ((dir = readdir(dh)) != NULL) {
		// Filter out anything that isn't a block device.
		if (ignore_dir_entry(dir)) {
			continue;
		}

		// Filter out the special "boot" devices.
		if (strstr(dir->d_name, "boot") != NULL) {
			continue;
		}

		// Get device information.
		stdev_t sd;
		strncpy(sd.name, dir->d_name, PARTITION_NAME_MAX_LEN);
		sysfs_device_info(&sd);
		if (sd.size == 0)
			continue;
		
		// Get partitions and information on them.
		sd.partitions.list = malloc(sizeof(partition_t));
		get_partitions(&sd);
		get_partitions_size(&sd);
		get_partitions_permission(&sd);
		get_partitions_mountpoints(&sd);

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
 * @param  sd    Storage device structure to be populated with information.
 * @return       TRUE if the parsing was successful.
 */
bool sysfs_device_info(stdev_t *sd) {
	// Build device path.
	strcpy(sd->path, SYSFS_BLOCKDEVS_PATH);
	strcat(sd->path, sd->name);

	// Get device size.
	if (!get_device_size(sd))
		return false;

	// Get device permission.
	if (!get_device_permission(sd))
		return false;

	return true;
}

/**
 * Gets the size of the block device in bytes.
 *
 * @param  sd Storage device structure to be populated with information.
 * @return    TRUE if the parsing was successful.
 */
bool get_device_size(stdev_t *sd) {
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
bool get_device_permission(stdev_t *sd) {
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
 * Gets the partitions from a block device. Just populates the number of
 * partitions and their names. For more information on each partition check
 * `blkid_info`.
 *
 * @param  sd    Storage device structure to be populated with information.
 * @return       TRUE if the operation was successful.
 */
bool get_partitions(stdev_t *sd) {
	DIR *dh;
	struct dirent *dir;

	// Open the block device folder.
	dh = opendir(sd->path);
	if (dh == NULL) {
		fprintf(stderr, "Couldn't open %s to list partitions.\n", sd->path);
		return false;
	}

	// Get the directory listing.
	sd->partitions.count = 0;
	while ((dir = readdir(dh)) != NULL) {
		// Filter out anything that isn't a partition device.
		if (strncmp(dir->d_name, sd->name, strlen(sd->name)) != 0)
			continue;

		// Filter out the special "boot" partitions.
		if (strstr(dir->d_name, "boot") != NULL) {
			continue;
		}

		// Add the partition to the list.
		device_partition_push(&sd->partitions, dir->d_name);
	}

	// Clean up.
	closedir(dh);
	dh = NULL;
	return true;
}

/**
 * Gets the size of every partition in a block device in bytes.
 *
 * @param  sd Storage device structure to be populated with information.
 * @return    TRUE if the parsing was successful.
 */
bool get_partitions_size(stdev_t *sd) {
	char attrpath[PATH_MAX];

	for (uint8_t i = 0; i < sd->partitions.count; i++) {
		// Get the number of bytes per sector.
		snprintf(attrpath, PATH_MAX, "%s/%s/size", sd->path,
				sd->partitions.list[i].name);
		
		if (!freadnum(attrpath, &sd->partitions.list[i].sectors)) {
			fprintf(stderr, "Failed to read the sector size for %s.\n",
					sd->partitions.list[i].name);
			return false;
		}
		
		// Calculate the size.
		sd->partitions.list[i].size = sd->partitions.list[i].sectors * sd->sector_size;
	}

	return true;
}

/**
 * Gets the permission of every partition in a block device. (Read/Write)
 *
 * @param  sd Storage device structure to be populated with information.
 * @return    TRUE if the parsing was successful.
 */
bool get_partitions_permission(stdev_t *sd) {
	char attrpath[PATH_MAX];
	size_t perm;

	for (uint8_t i = 0; i < sd->partitions.count; i++) {
		// Get the permission.
		snprintf(attrpath, PATH_MAX, "%s/%s/ro", sd->path,
				sd->partitions.list[i].name);
		
		if (!freadnum(attrpath, &perm)) {
			fprintf(stderr, "Failed to read %s permissions.\n",
					sd->partitions.list[i].name);
			return false;
		}

		sd->partitions.list[i].ro = (perm & true);
	}

	return true;
}

/**
 * Gets the mount points for partitions.
 *
 * @param  sd Storage device structure to be populated with information.
 * @return    TRUE if the parsing was successful.
 */
bool get_partitions_mountpoints(stdev_t *sd) {
	FILE *fp;
	struct mntent *fs;

	// Open the mount point file.
	fp = setmntent(MOUNTPOINT_DEF_PATH, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to read the %s file.\n", MOUNTPOINT_DEF_PATH);
		return false;
	}

	// Loop through the mount points in the system.
	while ((fs = getmntent(fp)) != NULL) {
		// Check if it's a real device and check for a match with a known partition..
		if (fs->mnt_fsname[0] == '/') {
			for (uint8_t i = 0; i < sd->partitions.count; i++) {
				if (strcmp(fs->mnt_fsname, sd->partitions.list[i].path) == 0) {
					// Store the mount point.
					strncpy(sd->partitions.list[i].mntpoint, fs->mnt_dir,
							DEVICE_PATH_MAX_LEN);

					// Store the filesystem type.
					strncpy(sd->partitions.list[i].type, fs->mnt_type,
							PARTITION_TYPE_MAX_LEN);
				}
			}
		}
	}

	// Clean up.
	endmntent(fp);
	return true;
}

/**
 * Gets a device information using blkid.
 *
 * @param  sd Storage device structure.
 * @return    TRUE if the probing went fine.
 */
bool blkid_info(stdev_t *sd) {
	char partpath[DEVICE_PATH_MAX_LEN];
	const char *uuid;
	const char *label;
	const char *type;

	// Get partitions information.
	for (int i = 0; i < sd->partitions.count; i++) {
		// Get partition location.
		snprintf(partpath, DEVICE_PATH_MAX_LEN, "/dev/%s",
				sd->partitions.list[i].name);

		// Initialize the parameter strings.
		sd->partitions.list[i].uuid[0] = '\0';
		sd->partitions.list[i].label[0] = '\0';
		sd->partitions.list[i].type[0] = '\0';

		// Create a partition probe.
		blkid_probe pr = blkid_new_probe_from_filename(partpath);
		if (!pr) {
			fprintf(stderr, "Failed to create a blkid probe for %s. "
					"Maybe run this program as root.\n", partpath);
			printf("To suppress the error above at the cost of a bit less "
					"information, just use the --no-blkid flag.\n");
			return false;
		}

		// Probe partition information.
		blkid_do_probe(pr);
		if (!blkid_probe_lookup_value(pr, "UUID", &uuid, NULL))
			strncpy(sd->partitions.list[i].uuid, uuid, PARTITION_NAME_MAX_LEN);
		if (!blkid_probe_lookup_value(pr, "LABEL", &label, NULL))
			strncpy(sd->partitions.list[i].label, label, PARTITION_NAME_MAX_LEN);
		if (!blkid_probe_lookup_value(pr, "TYPE", &type, NULL))
			strncpy(sd->partitions.list[i].type, type, PARTITION_TYPE_MAX_LEN);
		
		// Clean up.
		blkid_free_probe(pr);
	}

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

