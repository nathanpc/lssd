/**
 * linux.h
 * Deals with the Linux devices.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _LINUX_H
#define _LINUX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "../device.h"

bool linux_populate_devices(stdev_container *container);

#endif  //_LINUX_H

