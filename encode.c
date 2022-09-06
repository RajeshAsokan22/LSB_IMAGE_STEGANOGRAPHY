/*
NAME: RAJESH.A
DATE: 10/8/2022
DESCRIPTION: Encoding secret text to image
SAMPLE I/P:
SAMPLE O/P:
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2])
    {

    if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	encInfo->src_image_fname = argv[2];

    else
	return e_failure;
    }
    else
	return e_failure;

    if(argv[3] != NULL)
    {
	encInfo->secret_fname = argv[3];
	strcpy(encInfo->extn_secret_file, strstr(argv[3],"."));
    }

    else
	return e_failure;

    if(argv[4] != NULL)
	encInfo->stego_image_fname =  argv[4];

    else
	encInfo->stego_image_fname = "stego_img.bmp";

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extenstion size\n",encInfo -> secret_fname);
    encode_int_to_lsb(size, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Size\n", encInfo -> secret_fname);
    encode_int_to_lsb(file_size, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Data\n",encInfo -> secret_fname);

    char buffer[8];
    char ch;
    rewind(encInfo->fptr_secret);
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
	fread(buffer, 8, 1, encInfo->fptr_src_image);
	encode_byte_to_lsb(ch, buffer);
	fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    char ch;
    while(fread(&ch, 1, 1, fptr_src) > 0)
	fwrite(&ch, 1, 1, fptr_dest);
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i = 0; i < size; i++)
    {
	fread(image_buffer, 8, 1, fptr_src_image);
	encode_byte_to_lsb(data[i], image_buffer);
	fwrite(image_buffer, 8, 1, fptr_stego_image);
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
	if(data & (1 << (7 - i)))
	    image_buffer[i] |= 1;
	else
	    image_buffer[i] &= ~1;
    }
}

Status encode_int_to_lsb(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[32];
    fread(buffer, 32, 1, fptr_src_image);
    for(int i = 0; i < 32; i++)
    {
	if(size & (1 << (31 - i)))
	    buffer[i] |= 1;
	else
	    buffer[i] &= ~1;
    }
    fwrite(buffer, 32, 1, fptr_stego_image);
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extenstion\n",encInfo -> secret_fname);

    encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
	printf("INFO: Done\n");
	printf("INFO: ## Encoding Procedure Started ##\n");
    }
    else
    {
	return e_failure;
    }
    if(check_capacity(encInfo) == e_success)
    {
	printf("INFO: Done. Found OK\n");
	if(strcmp(encInfo -> stego_image_fname, "stego_img.bmp"))
	    printf("INFO: Output File not mentioned. Creating %s as default",encInfo -> stego_image_fname);
    }
    else
    {
	printf("ERROR: %s doesn't have the capacity to encode %s\n", encInfo -> src_image_fname, encInfo -> secret_fname);
	return e_failure;
    }
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
	printf("INFO: Done\n");
    }

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
	printf("INFO: Done\n");
    }
    if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
    {
	printf("INFO: Done\n");

    }
    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
    {
	printf("INFO: Done\n");
    }
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
    {
	printf("INFO: Done\n");
    }
    if(encode_secret_file_data(encInfo) == e_success)
    {
	printf("INFO: Done\n");
    }
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
	printf("INFO: Done\n");
    }
    return e_success;

}
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for %s size\n", encInfo -> secret_fname);
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    fseek(encInfo -> fptr_secret,0,SEEK_END);
    encInfo -> size_secret_file = ftell(encInfo -> fptr_secret);

    printf("INFO: Checking for %s capacity to handle %s\n",encInfo -> src_image_fname, encInfo -> secret_fname);
    if(encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + (encInfo -> size_secret_file * 8)))
	return e_success;
    else
	return e_failure;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    char str[54];
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);

    return e_success;
}
