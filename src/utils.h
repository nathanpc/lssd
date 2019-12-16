/**
 * utils.h
 * Simple set of utilities that are used throughout the program.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>

void pretty_bytes(const size_t size, float *num, char *unit);
bool freadnum(const char *fpath, size_t *num);

#endif /* _UTILS_H_ */
