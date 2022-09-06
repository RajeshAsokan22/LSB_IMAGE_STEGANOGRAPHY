#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#define FILE_SUFFIX 4

typedef struct _DecodeInfo
{
  /* Stego Image Info */
    char* stego_src_name;
    FILE* fptr_stego_image;

    /*output file*/
    char* output_fname;
    FILE* fptr_output;
    char extn_output_file[FILE_SUFFIX];
    uint extn_size;
    uint secret_file_size;
} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Extract Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode data from image*/
Status decode_data_from_image(int size, FILE* fptr_stego_image, FILE* fptr_output);

/* Decode byte from lsb */
Status decode_byte_from_lsb(char* ch , char *image_data);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode lsb to integer value */
Status decode_int_from_lsb(int* size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(FILE *fptr_output, FILE *fptr_stego_image, DecodeInfo *decInfo);

#endif
