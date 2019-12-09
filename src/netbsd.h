/**
 * netbsd.h
 * Deals with NetBSD devices.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _NETBSD_H
#define _NETBSD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "device.h"

bool populate_devices(stdev_container *container, const bool useblkid);

#endif  //_NETBSD_H

