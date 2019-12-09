/**
 * linux.h
 * Deals with Linux devices.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _LINUX_H
#define _LINUX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "device.h"

bool populate_devices(stdev_container *container, const bool useblkid);

#endif  //_LINUX_H

