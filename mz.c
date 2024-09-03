#include <stdio.h>
#include <string.h>
#include "include/mz.h"


// Function to read an unsigned short from a file
unsigned short bRead(FILE* file) {
    unsigned short buffer;
    if (fread(&buffer, sizeof(unsigned short), 1, file) != 1) {
        fprintf(stderr, "Error reading from file\n");
        return 0; // Error indication
    }
    return buffer;
}

// Function to check if the file has a valid MZ header
int hasMZHeader(FILE* fp) {
    unsigned short buffer[16];
    for (int i = 0; i < 16; i++) {
        buffer[i] = bRead(fp);
    }
    return buffer[0] == DOSMagic;
}

// Function to read the MZ header information into a buffer
void readMZHeader(FILE* fp, unsigned short buffer[]) {
    for (int i = 0; i < 16; i++) {
        buffer[i] = bRead(fp);
    }
}

// Function to print the MZ header information
void printMZHeaderInfo(unsigned short buffer[]) {
    printf("[info] %u Bytes in the Last Page\n", buffer[1]);
    printf("[info] Total Page: %u\n", buffer[2]);
    printf("[info] Initial Stack Segment: %04x\n", buffer[7]);
    printf("[info] Initial Stack Pointer: %04x\n", buffer[8]);
    printf("[info] CS:IP: %04x:[%04x]\n", buffer[11], buffer[10]);
    printf("[info] Relocation Table @Offset %04x:%04x\n", buffer[13], buffer[12]);
    if (buffer[13] == 0x0) { // Check if it's the Main Program
        printf("[info] This is the Main Program\n");
    }
    if (buffer[14] == TLINKMagic) { // Check for Turbo Link magic
        printf("[linker] This Program is linked by Borland Turbo Link\n");
    }
}

// Function to validate the file extension
int isValidExtension(const char* fileName) {
    const char* ext = strrchr(fileName, '.');
    return ext && (strcmp(ext, ".exe") == 0 || strcmp(ext, ".com") == 0);
}

// Main function to handle command-line arguments and file processing
int main(int argc, char *argv[]) {
    FILE* fp;
    unsigned short buffer[16] = {0}; // Initialize buffer
    char* fileName = NULL;

    printf("MZ File Analyzer Made by Phage-Official <c> 2024\n");
    if (argc == 1) {
        printf("Specify a filename to do further operations\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    printf("This program is designed to analyze the structure of certain MZ Executables\n");
                    return 0;
                default:
                    break;
            }
        } else if (fileName == NULL) {
            fileName = argv[i];
            // Check file availability
            fp = fopen(fileName, "rb");
            if (fp) {
                if (!isValidExtension(fileName)) {
                    printf("Invalid file extension\n");
                    fclose(fp);
                    return 0;
                }

                if (hasMZHeader(fp)) {
                    printf("[info] Valid MZ Header Found\n");
                    fseek(fp, 0, SEEK_SET); // Reset file pointer to read the header again
                    readMZHeader(fp, buffer);
                    printMZHeaderInfo(buffer);
                } else {
                    printf("[info] No valid DOS Header\n");
                }
                fclose(fp);
            } else {
                printf("No file called %s\n", argv[i]);
                return 0;
            }
        } else {
            // Handle multiple files
            printf("Unspecified args: %s\n", argv[i]);
            return 0;
        }
    }

    return 0;
}
