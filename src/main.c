/**
 * lssd
 * Lists storage devices. Better than "lsblk". Less information than "fdisk -l".
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#ifdef __linux__
#include "linux.h"
#elif __NetBSD__
#include "netbsd.h"
#endif

// Prototypes.
void usage();

// Storage device container.
stdev_container stdevs;

/**
 * Application's main entry point.
 *
 * @param  argc Number of command-line arguments passed.
 * @param  argv Array of command-line arguments passed.
 * @return      Exit code.
 */
int main(int argc, char **argv) {
	int option_idx = 0;
	bool pretty = true;
	bool useblkid = true;

	// Set the long options for getopt.
	static struct option loptions[] = {
		{ "ugly", no_argument, NULL, 'u' },
		{ "no-blkid", no_argument, NULL, 'k' },
		{ "help", no_argument, NULL, 'h' }
	};

	// Loop through flags.
	while ((option_idx = getopt_long(argc, argv, "ukh", loptions, NULL)) != -1) {
		switch (option_idx) {
			case 'u':
				pretty = false;
				break;
			case 'k':
				useblkid = false;
				break;
			case 'h':
				usage();
				return EXIT_SUCCESS;
			default:
				usage();
				return EXIT_FAILURE;
		}
	}

	// Populate the device list.
	if (!populate_devices(&stdevs, useblkid))
		return EXIT_FAILURE;

	// Print information for all the devices available.
	for (uint8_t i = 0; i < stdevs.count; i++) {
		device_print_info(stdevs.list[i], pretty);
	}
	
	// Clean up and exit.
	device_container_free(&stdevs);
	return EXIT_SUCCESS;
}

/**
 * Prints the usage text.
 */
void usage() {
	printf("Usage: lssd [-ukh]\n\n");
	printf("Flags:\n");
	printf("    -u or --ugly    \tPrint like fdisk instead of the tree layout.\n");
	printf("    -k or --no-blkid\tDon't use blkid to get information. (no root)\n");
	printf("    -h or --help    \tShows this message.\n");
}

