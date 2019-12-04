/**
 * device.h
 * Generic representation of a storage device.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "device.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/**
 * Pushes a storage device into a container.
 *
 * @param list Device container.
 * @param sd   Storage device.
 */
void device_list_push(stdev_container *list, stdev_t sd) {
	list->list = realloc(list->list, sizeof(stdev_t) * (list->count + 1));
	list->list[list->count++] = sd;
}

/**
 * Pushes a partition into a storage device.
 *
 * @param parts Partition container.
 * @param name  Partition name.
 */
void device_partition_push(partition_container *parts, const char *name) {
	parts->list = realloc(parts->list, sizeof(partition_t) * (parts->count + 1));
	strncpy(parts->list[parts->count++].name, name, PARTITION_NAME_MAX_LEN);
}

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
	printf("Permission:\t%s\n", sd.ro ? "Read Only" : "Read and Write");

	if (sd.partitions.count > 0) {
		printf("Partitions (%d):\n", sd.partitions.count);
	} else {
		printf("No partitions available!\n");
	}

	for (int i = 0; i < sd.partitions.count; i++) {
		printf("\t%d: %s\n", i, device_get_partition_param(&sd.partitions.list[i], PARAM_NAME));
		printf("\t\tUUID:        %s\n", device_get_partition_param(&sd.partitions.list[i], PARAM_UUID));
		printf("\t\tType:        %s\n", device_get_partition_param(&sd.partitions.list[i], PARAM_TYPE));
		printf("\t\tLabel:       %s\n", device_get_partition_param(&sd.partitions.list[i], PARAM_LABEL));
		printf("\t\tSectors:     %zu\n", sd.partitions.list[i].sectors);
		printf("\t\tSize:        %zu bytes\n", sd.partitions.list[i].size);
		printf("\t\tPermission:  %s\n", sd.partitions.list[i].ro ? "Read Only" : "Read and Write");
	}

	printf("\n");
}

/**
 * Gets a partition parameter value.
 *
 * @param  part  Partition structure.
 * @param  param Parameter name.
 * @return       Partition parameter value or "" if the parameter name is
 *               invalid.
 */
const char* device_get_partition_param(const partition_t *part,
		const uint8_t param) {
	switch (param) {
		case PARAM_NAME:
			return part->name;
		case PARAM_TYPE:
			return part->type;
		case PARAM_LABEL:
			return part->label;
		case PARAM_UUID:
			return part->uuid;
	}

	return "";
}

