/**
 * procfs.h
 * Parses device information from a procfs-based system.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _PROCFS_H
#define _PROCFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PROCFS_PARTITIONS_PATH "/proc/partitions"

bool procfs_exists();
bool procfs_partition_list(char **ptlist);

#endif  //_PROCFS_H

