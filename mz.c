#include <stdio.h>
#include <string.h>
/* #include "include/mz.h" this is not necessary */

#define DOSMagic 0x5a4d
#define TLINKMagic 0xfb

// Function to read an unsigned short from a file
unsigned short bRead(FILE* file) {
    unsigned short buffer;
    fread(&buffer, sizeof(unsigned short), 1, file);
    return buffer;
}

// Function to check if a file has a valid MZ header
int hasMZHeader(FILE* fp) {
    unsigned short buffer[16];
    for (int i = 0; i < 16; i++) {
        buffer[i] = bRead(fp);
    }
    return buffer[0] == DOSMagic;
}

// Function to print MZ header information
void printMZHeaderInfo(unsigned short buffer[16]) {
    printf("[info] Valid MZ Header Found\n");
    printf("[info] %u Bytes in the Last Page\n", buffer[1]);
    printf("[info] Total Page: %u\n", buffer[2]);
    printf("[info] Initial Stack Segment: %04x\n", buffer[7]);
    printf("[info] Initial Stack Pointer: %04x\n", buffer[8]);
    printf("[info] CS:IP: %04x:[%04x]\n", buffer[11], buffer[10]);
    printf("[info] Relocation Table @Offset %04x:%04x\n", buffer[13], buffer[12]);
    if (buffer[13] == 0x0) {
        printf("[info] This is the Main Program\n");
    }
    if(buffer[14] == TLINKMagic){
        printf("[linker] This Program is linked by Borland Turbo Link");
    }
}

// Function to process a single file
void processFile(char* fileName) {
    FILE* fp = fopen(fileName, "rb");
    if (fp) {
        // Extract the file extension
        char postFix[4] = {0};
        size_t len = strlen(fileName);
        if (len > 3) {
            strncpy(postFix, fileName + len - 3, 3);
            postFix[3] = '\0';
        }

        // Check file extension
        if (strcmp(postFix, "exe") != 0 && strcmp(postFix, "com") != 0) {
            printf("Invalid file extension\n");
            fclose(fp);
            return;
        }

        // Check for MZ header
        if (hasMZHeader(fp)) {
            unsigned short buffer[16];
            for (int i = 0; i < 16; i++) {
                buffer[i] = bRead(fp);
            }
            printMZHeaderInfo(buffer);
        } else {
            printf("[info] no invalid DOS Header\n");
        }

        fclose(fp);
    } else {
        printf("No file called %s\n", fileName);
    }
}

int main(int argc, char *argv[]) {
    printf("MZ File Analyzer Made by Phage-Official <c> 2024\n");
    if (argc == 1) {
        printf("Specify a filename to do further operation\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    printf("This program is designed to analyze the structure of certain MZ Executable\n");
                    return 0;
                default:
                    break;
            }
        } else {
            processFile(argv[i]);
        }
    }

    return 0;
}
