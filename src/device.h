/**
 * device.h
 * Generic representation of a storage device.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_PATH               4000
#define PARTITION_NAME_MAX_LEN 128

bool device_exists(const char *devpath);

#endif  //_DEVICE_H

