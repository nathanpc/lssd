/**
 * procfs.c
 * Parses device information from a procfs-based system.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "procfs.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "device.h"

// Private methods.
bool get_partition_name(char *ptname, char *line);


/**
 * Checks if the procfs partitions file exists.
 *
 * @return TRUE if there is a procfs partitions file.
 */
bool procfs_exists() {
	return access(PROCFS_PARTITIONS_PATH, F_OK) != -1;
}

/**
 * Retrieves a partition list.
 *
 * @param  ptlist Array of partition names to be populated.
 * @return        TRUE if the parsing operation was successful.
 */
bool procfs_partition_list(char **ptlist) {
	FILE *fp;
	char *line;
	char ptname[PARTITION_NAME_MAX_LEN];
	size_t len = 0;
	ssize_t read;

	// Open the partitions file.
	fp = fopen(PROCFS_PARTITIONS_PATH, "r");
	if (fp == NULL) {
		fprintf(stderr, "Couldn't open %s to list partitions.\n",
				PROCFS_PARTITIONS_PATH);
		return false;
	}

	// Parse the file line-by-line.
	while ((read = getline(&line, &len, fp)) != -1) {
		// Get the partition name.
		if (!get_partition_name(ptname, line))
			continue;

		char devpath[MAX_PATH];
		devpath[0] = '\0';

		// Build the device path.
		strncat(devpath, "/dev/", 6);
		strncat(devpath, ptname, PARTITION_NAME_MAX_LEN);

		printf("%s\n", devpath);
	}

	// Clean up.
	fclose(fp);
	fp = NULL;
	if (line)
		free(line);

	return true;
}

/**
 * Parses the partition name from a procfs partitions file line.
 *
 * @param  ptname String that will contain the partition name. Will be allocated
 *                dynamically.
 * @param  ptline Profs partitions file line.
 * @return        TRUE if the parsing operation was successful.
 */
bool get_partition_name(char *ptname, char *line) {
	return sscanf(line, " %*d %*d %*d %128[^\n ]", ptname) == 1;
}

