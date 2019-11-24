/**
 * device.h
 * Generic representation of a storage device.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "device.h"
#include <stdio.h>
#include <unistd.h>

/**
 * Checks if a partition exists.
 *
 * @param  devpath Path to the device partition.
 * @return         TRUE if there is a partition file.
 */
bool device_exists(const char *devpath) {
	return access(devpath, F_OK) != -1;
}

/**
 * Prints the device information.
 *
 * @param sd Storage device structure to be printed.
 */
void device_print_info(const stdev_t sd) {
	printf("Device:\t\t%s\n", sd.name);
	printf("Sectors:\t%zu\n", sd.sectors);
	printf("Sector Size:\t%zu bytes/sector\n", sd.sector_size);
	printf("Size:\t\t%zu bytes\n", sd.size);
	printf("\n");
}

