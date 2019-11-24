/**
 * sysfs.c
 * Parses device information from a sysfs-based system.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "sysfs.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Private methods.
bool ignore_dir_entry(const struct dirent *dir);
bool freadnum(const char *fpath, size_t *num);
bool get_size(stdev_t *sd);

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
bool sysfs_device_list(stdev_t **devlist) {
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

#ifdef DEBUG
		device_print_info(sd);
#endif
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

