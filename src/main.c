/**
 * lssd
 * Lists storage devices. Better than "lsblk". Less information than "fdisk -l".
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <stdlib.h>
#include <stdio.h>
#include "linux.h"

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
	if (!linux_populate_devices(&stdevs, true))
		return EXIT_FAILURE;

	// Print information for all the devices available.
	for (uint8_t i = 0; i < stdevs.count; i++) {
		device_print_info(stdevs.list[i], true);
	}
	
	// Clean up.
	device_container_free(&stdevs);

	return EXIT_SUCCESS;
}

