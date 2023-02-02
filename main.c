#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <bmp_file>\n", argv[0]);
    return 1;
  }

  char *file_name = argv[1];
  FILE *fp = fopen(file_name, "rb");
  if (fp == NULL) {
    printf("Error: Could not open file %s\n", file_name);
    return 1;
  }

  // Validate file header
  unsigned char header[54];
  fread(header, 1, 54, fp);
  if (header[0] != 'B' || header[1] != 'M') {
    printf("Error: Not a valid BMP file\n");
    fclose(fp);
    return 1;
  }

  // Get bits offset
  int offset = *(int*)(header + 10);
  printf("Bits offset: %d\n", offset);

  // Get file size
  fseek(fp, 0, SEEK_END);
  int file_size = ftell(fp);
  rewind(fp);

  // Get file content
  unsigned char *content = (unsigned char*) malloc(file_size - offset);
  fseek(fp, offset, SEEK_SET);
  int read_bytes = fread(content, 1, file_size - offset, fp);
  printf("Bytes read: %d\n", read_bytes);

  // Close file
  fclose(fp);

  // Save content to .c file
  char c_file_name[100];
  char array_name[100];
  strncpy(c_file_name, file_name, strlen(file_name) - 4);
  strncpy(array_name, file_name, strlen(file_name) - 4);
  strcat(c_file_name, ".c");
  FILE *c_fp = fopen(c_file_name, "w");
  if (c_fp == NULL) {
    printf("Error: Could not open file %s\n", c_file_name);
    free(content);
    return 1;
  }
  fprintf(c_fp, "const unsigned char %s[%d] = {\n", array_name, read_bytes);
  for (int i = 0; i < read_bytes; i++) {
    fprintf(c_fp, "0x%02x", content[i]);
    if (i < read_bytes - 1) {
      fprintf(c_fp, ",");
      if((i+1)%16==0)
      fprintf(c_fp, "\n");
    }
  }
  fprintf(c_fp, "};\n");
  fclose(c_fp);

  // Print success message
  printf("File content saved to %s\n", c_file_name);

  free(content);
  return 0;
}

