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
#include "device.h"

// Private methods.
bool ignore_dir_entry(const struct dirent *dir);

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
 * @param  devlist Array of block device names to be populated.
 * @return         TRUE if the operation was successful.
 */
bool sysfs_device_list(char **devlist) {
	DIR *dh;
	struct dirent *dir;
	//char ptname[PARTITION_NAME_MAX_LEN];

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

		printf("%s\n", dir->d_name);
	}

	// Clean up.
	closedir(dh);
	dh = NULL;
	return true;
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

