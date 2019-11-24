/**
 * sysfs.h
 * Parses device information from a sysfs-based system.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _SYSFS_H
#define _SYSFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "device.h"

#define SYSFS_PARTITIONS_PATH "/sys/block/"

bool sysfs_exists();
bool sysfs_device_info(stdev_t *sd);
bool sysfs_device_list(stdev_t **devlist);

#endif  //_SYSFS_H

