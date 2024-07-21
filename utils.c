#include "utils.h"

// Learning python has left its consequences,
// thus snake_case is my primary naming convetion now.

// Function that breaks up a file into an array of bytes
unsigned char* file_disassembler(const char* path, size_t* size) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        printc("Error: File opening failed!", RED);
        return NULL;
    }

    // Seek to the end of the file to get its size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to store file contents
    unsigned char* buffer = (unsigned char*)malloc(file_size);
    if (!buffer) {
        fclose(file);
        printc("Error: Memory allocation failed!", RED);
        return NULL;
    }

    // Read file contents into buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        fclose(file);
        free(buffer);
        printc("Error: Reading failed!", RED);
        return NULL;
    }

    // Set the size of the file
    *size = file_size;

    fclose(file);

    return buffer;
}

void file_assembler(const char* filename, const unsigned char* bytes, size_t size) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(bytes, sizeof(unsigned char), size, file); // Write bytes to file
        fclose(file);
        printf("Bytes have been written to %s\n", filename);
    } else {
        printc("Error: File opening failed!\n",RED);
    }
}


// For testing
// int main() {

//     // TESTING FILE DISASSEMBLER

//     const char* filename = "db/uhlib.txt";
//     size_t file_size;
//     unsigned char* byte_arr = file_disassembler(filename, &file_size);

//     if (byte_arr) {
//         // Printing the contents of the byte array (for demonstration)
//         printf("Contents of file %s (%zu bytes):\n", filename, file_size);
//         for (size_t i = 0; i < file_size; ++i) {
//             printf("%02X ", byte_arr[i]);
//         }
//         printf("\n");

        
//     }

//     // TESTING FILE ASSEMBLER

//     file_assembler("output.txt", byte_arr, file_size);

//     // DEALLOCATE MEMORY GOSPODE ME BOZE OPROSTI 
//     free(byte_arr);
//     return 0;
// }