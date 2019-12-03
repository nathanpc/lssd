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
#include <limits.h>

// Constants.
#define PARTITION_NAME_MAX_LEN 128
#define DEVICE_PATH_MAX_LEN    PARTITION_NAME_MAX_LEN * 2

// Device partition structure.
typedef struct {
	char        name[PARTITION_NAME_MAX_LEN];
	const char *uuid;
	const char *label;
	const char *type;
	size_t      size;
} partition_t;

// Partition dynamic array.
typedef struct {
	uint8_t      count;
	partition_t *list;
} partition_container;

// Storage device structure.
typedef struct {
	char         name[PARTITION_NAME_MAX_LEN];
	char         path[DEVICE_PATH_MAX_LEN];
	size_t       sectors;
	size_t       sector_size;
	size_t       size;
	bool         ro;
	partition_container partitions;
} stdev_t;

// Storage device dynamic array.
typedef struct {
	uint8_t count;
	stdev_t *list;
} stdev_container;

// Checking.
bool device_exists(const char *devpath);

// List operation.
void device_list_push(stdev_container *list, stdev_t sd);
void device_partition_push(partition_container *parts, const char *name);

// Debugging.
void device_print_info(const stdev_t sd);

#endif  //_DEVICE_H

