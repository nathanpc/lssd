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
#include "utils.h"

#define SIZE_PRINTF "%.2f%c"

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
	snprintf(parts->list[parts->count - 1].path, DEVICE_PATH_MAX_LEN, "/dev/%s",
			parts->list[parts->count - 1].name);
	parts->list[parts->count - 1].mntpoint[0] = '\0';
}

/**
 * Frees the whole storage device container.
 *
 * @param container Storage device container to be freed.
 */
void device_container_free(stdev_container *container) {
	for (uint8_t i = 0; i < container->count; i++) {
		free(container->list[i].partitions.list);
	}

	free(container->list);
	container->count = 0;
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
 * @param sd     Storage device structure to be printed.
 * @param pretty FALSE will print everything we have on the device.
 */
void device_print_info(const stdev_t sd, const bool pretty) {
	float size;
	char sunit;

	// Print device information.
	pretty_bytes(sd.size, &size, &sunit);
	if (pretty) {
		printf("%s (%s) " SIZE_PRINTF "\n", sd.name, sd.ro ? "R" : "R/W", size, sunit);
	} else {
		printf("Device:\t\t%s\n", sd.name);
		printf("Sectors:\t%zu\n", sd.sectors);
		printf("Sector Size:\t%zu bytes/sector\n", sd.sector_size);
		printf("Size:\t\t" SIZE_PRINTF "\n", size, sunit);
		printf("Permission:\t%s\n", sd.ro ? "Read Only" : "Read and Write");
	}

	// Print partition header.
	if (sd.partitions.count > 0) {
		if (!pretty)
			printf("Partitions (%d):\n", sd.partitions.count);
	} else {
		printf("\tNo partitions available!\n");
	}

	// Loop through the partitions and print their information.
	for (int i = 0; i < sd.partitions.count; i++) {
		// Get pretty size.
		pretty_bytes(sd.partitions.list[i].size, &size, &sunit);

		if (pretty) {
			// Print "tree" thingy.
			if (i == (sd.partitions.count - 1)) {
				printf("\t\u2514 ");
			} else {
				printf("\t\u251C ");
			}

			// Print information.
			printf("%s (%s) [%s] " SIZE_PRINTF "\n", sd.partitions.list[i].name,
					sd.partitions.list[i].ro ? "R" : "R/W",
				   sd.partitions.list[i].type, size, sunit);

			// Print label.
			if (sd.partitions.list[i].label[0] != '\0') {
				printf("\t");

				// If it's not the last partition continue the root branch.
				if (i < (sd.partitions.count - 1)) {
					printf("\u2502");
				}

				if (sd.partitions.list[i].mntpoint[0] != '\0') {
					printf("\t\u251C ");
				} else {
					printf("\t\u2514 ");
				}

				printf("Label: %s\n", sd.partitions.list[i].label);
			}

			// Print mount point.
			if (sd.partitions.list[i].mntpoint[0] != '\0') {
				printf("\t");

				// If it's not the last partition continue the root branch.
				if (i < (sd.partitions.count - 1)) {
					printf("\u2502");
				}

				// Are we the last item or there's the UUID to come?
				if (sd.partitions.list[i].uuid[0] != '\0') {
					printf("\t\u251C ");
				} else {
					printf("\t\u2514 ");
				}

				printf("Mount Point: %s\n", sd.partitions.list[i].mntpoint);
			}

			// Print UUID.
			if (sd.partitions.list[i].uuid[0] != '\0') {
				printf("\t");

				// If it's not the last partition continue the root branch.
				if (i < (sd.partitions.count - 1)) {
					printf("\u2502");
				}

				printf("\t\u2514 ");
				printf("UUID: %s\n", sd.partitions.list[i].uuid);
			}
		} else {
			printf("\t%d: %s\n", i, sd.partitions.list[i].name);
			printf("\t\tUUID:        %s\n", sd.partitions.list[i].uuid);
			printf("\t\tType:        %s\n", sd.partitions.list[i].type);
			printf("\t\tLabel:       %s\n", sd.partitions.list[i].label);
			printf("\t\tSectors:     %zu\n", sd.partitions.list[i].sectors);
			printf("\t\tSize:        " SIZE_PRINTF "\n", size, sunit);
			printf("\t\tPermission:  %s\n", sd.partitions.list[i].ro ? "Read Only" : "Read and Write");
			printf("\t\tMount Point: %s\n", sd.partitions.list[i].mntpoint);
		}
	}

	printf("\n");
}
