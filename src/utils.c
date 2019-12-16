/**
 * utils.c
 * Simple set of utilities that are used throughout the program.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

#define MAX_BYTE_UNIT_SIZE 1000000000000

/**
 * Grabs a size in bytes and converts it into a smaller float and a unit
 * character. The function determines the best unit for the given size.
 *
 * @param size Size in bytes.
 * @param num  Smaller number as a float.
 * @param unit Unit character.
 */
void pretty_bytes(const size_t size, float *num, char *unit) {
	float simp = 0.0f;

	for (size_t i = 1; i <= MAX_BYTE_UNIT_SIZE; i *= 1000) {
		simp = (float)size / (float)i;

		// Found the appropriate magnitude.
		if ((simp < 1000.0f) || (i == MAX_BYTE_UNIT_SIZE)) {
			// Put the simplified number into the pointer.
			*num = simp;

			// Set the unit character.
			switch (i) {
			case 1:
				*unit = 'B';
				break;
			case 1000:
				*unit = 'K';
				break;
			case 1000000:
				*unit = 'M';
				break;
			case 1000000000:
				*unit = 'G';
				break;
			case 1000000000000:
				*unit = 'T';
				break;
			}

			break;
		}
	}
}

/**
 * Reads a number from a file that only contains it.
 *
 * @param  fpath File path.
 * @param  num   Pointer to the number found in the file.
 * @return       TRUE if the parsing was successful.
 */
bool freadnum(const char *fpath, size_t *num) {
	FILE *fh;
	bool success = false;

	// Open the file.
	fh = fopen(fpath, "r");
	if (fh == NULL) {
		fprintf(stderr, "Couldn't open %s.\n", fpath);
		return false;
	}

	// Read the number from the file.
	success = (fscanf(fh, "%zu[^\n]", num) == 1);

	// Clean up.
	fclose(fh);
	fh = NULL;
	return success;
}
