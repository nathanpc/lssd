/**
 * linux.h
 * Deals with the Linux devices.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "linux.h"
#include <stdio.h>
#include <blkid/blkid.h>
#include "sysfs.h"

/**
 * Populates a storage device container.
 *
 * @param  container Storage device structure container.
 * @return           TRUE if everything went fine.
 */
bool linux_populate_devices(stdev_container *container) {
	// Check with device discovery system we are going to use.
	if (sysfs_exists()) {
		// Use procfs.
		if (!sysfs_device_list(container)) {
			return false;
		}
	} else {
		fprintf(stderr, "Cannot determine a device discovery system to use.\n");
		return false;
	}

	return true;
}

