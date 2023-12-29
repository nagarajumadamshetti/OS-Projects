
#include "disk_image.h"

char* outfile_name = "disk_defrag";

/* Disk image created from input file */
diskImage di;
/* Pointer to defragmented disk image */
diskImage *defragged = NULL;

int main(int argc, char *argv[]) {
    // Open the input file
    FILE *in;
    char *fileName = argv[1];
    in = fopen(fileName, "r");

    // Check for errors opening the input file
    if (!in) {
        // Handle error: could not open file
    }

    // Load a disk image from the input file
    readDiskImage(in, &di);
    fclose(in);

    // Defrag the disk image
    defragged = defragDiskImage(&di);


    // Cleanup
    freeDiskImage(&di);
    freeDiskImage(defragged);
    return 0;
}
