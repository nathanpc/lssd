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

#define SYSFS_PARTITIONS_PATH "/sys/block/"

bool sysfs_exists();
bool sysfs_device_list(char **ptlist);

#endif  //_SYSFS_H

