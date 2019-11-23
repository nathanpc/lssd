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

