/**
 * lssd
 * Lists storage devices. Better than "lsblk". Less information than "fdisk -l".
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <stdlib.h>
#include <stdio.h>

#include "procfs.h"

/**
 * Application's main entry point.
 *
 * @param  argc Number of command-line arguments passed.
 * @param  argv Array of command-line arguments passed.
 * @return      Exit code.
 */
int main(int argc, char **argv) {
	// Check with device discovery system we are going to use.
	if (procfs_exists()) {
		// Use procfs.
		if (!procfs_partition_list(NULL))
			return EXIT_FAILURE;
	} else {
		fprintf(stderr, "Cannot determine a device discovery system to use.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

