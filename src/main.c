/**
 * lssd
 * Lists storage devices. Better than "lsblk". Less information than "fdisk -l".
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * Application's main entry point.
 *
 * @param  argc Number of command-line arguments passed.
 * @param  argv Array of command-line arguments passed.
 * @return      Exit code.
 */
int main(int argc, char **argv) {
	printf("Hello %s\n", argv[0]);
	return 0;
}

