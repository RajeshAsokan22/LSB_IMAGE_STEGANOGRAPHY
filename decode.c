/*
NAME: RAJESH.A
DATE: 10/8/2022
DESCRIPTION: Decoding secret text from stegnography image
SAMPLE I/P:
SAMPLE O/P:
 */
#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Read and validate command line arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2])
    {
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{
	    decInfo -> stego_src_name = argv[2];
	}
	else
	    return e_failure;
    }
    else
	return e_failure;
    if(argv[3])
    {
	decInfo -> output_fname = argv[3];
    }
    else
	decInfo -> output_fname = NULL;

    return e_success;
}
//Do Decoding
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    if(open_files_decode(decInfo) == e_success);
    else
	return e_failure;
    if(decode_magic_string(decInfo) == e_success)
    {
	printf("INFO: Done\n");
	printf("INFO: MAGIC_STRING is matching\n");
    }
    else
    {
	printf("ERROR: MAGIC_STRING is not matching\n");
	return e_failure;
    }
    if(decode_secret_file_extn_size(decInfo) == e_success);
    else
	return e_failure;
    if(decode_secret_file_extn(decInfo -> extn_size, decInfo -> fptr_stego_image, decInfo) == e_success)
    {
	int flag = 0;
	if(decInfo -> output_fname == NULL)
	    flag = 1;

	if(strcmp(decInfo -> extn_output_file, ".txt") == 0)
	{
	    decInfo -> fptr_output = fopen("output.txt","w");
	    decInfo -> output_fname = "output.txt";
	}
	else if(strcmp(decInfo -> extn_output_file, ".c") == 0)
	{
	    decInfo -> fptr_output = fopen("output.c","w");
	    decInfo -> output_fname = "output.c";
	}
	printf("INFO: Done\n");
	if(flag)
	    printf("INFO: Output file is not mentioned. Creating  %s as default\n",decInfo -> output_fname);
	else 
	    printf("INFO: secret output file %s created\n",decInfo -> output_fname);
	decInfo -> fptr_output = fopen(decInfo -> output_fname, "w");

    }
    if(decode_secret_file_size(decInfo -> fptr_stego_image, decInfo) == e_success)
    {
	printf("INFO: Done\n");
    }
    if(decode_secret_file_data(decInfo -> fptr_output, decInfo -> fptr_stego_image, decInfo) == e_success);
    {
	printf("INFO: Done\n");
    }
    return e_success;

}
//Opening required files
Status open_files_decode(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");

    //stego image file

    decInfo -> fptr_stego_image = fopen(decInfo -> stego_src_name, "r");
    //do error handling

    if(decInfo -> fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_src_name);
	return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo -> stego_src_name);
    return e_success;
}
//Decoding magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic[3];
    magic[2] = '\0';
    char image_buffer[8];
    printf("INFO: Decoding Magic String Signature\n");
    fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
    for(int i = 0; i < 2; i++)
    {
	fread(image_buffer, 8, 1, decInfo -> fptr_stego_image);
	decode_byte_from_lsb(&magic[i] ,image_buffer);
    }
    if(strcmp(magic, MAGIC_STRING) == 0)
	return e_success;
    else
	return e_failure;
}
//decoding data from image
Status decode_data_from_image(int size, FILE* fptr_stego_image, FILE* fptr_output)
{
    char ch;
    char image_buffer[8];
    for(int i = 0; i < size; i++)
    {
	fread(image_buffer, 8, 1, fptr_stego_image);
	decode_byte_from_lsb(&ch ,image_buffer);
	fwrite(&ch, 1, 1, fptr_output);
    }
    return e_success;
}

//Decoding byte from lsb
Status decode_byte_from_lsb(char* ch , char *image_data)
{
    *ch = 0;
    for(int i = 0; i < 8; i++)
    {
	*ch = *ch | ((image_data[i] & 1) << (7 - i));
    }
}

//Decoding secret file extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    int ext_size;
    decode_int_from_lsb(&ext_size, decInfo -> fptr_stego_image, decInfo);
    decInfo -> extn_size = ext_size;
    return e_success;
}

//Decoding int from lsb
Status decode_int_from_lsb(int* size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    *size = 0;
    char buffer[32];
    fread(buffer, 32, 1, fptr_stego_image);
    for(int i = 0; i < 32; i++)
    {
	*size = *size | (((int)buffer[i] & 1) << (31 - i));
    }
}

//Decoding secret file extension
Status decode_secret_file_extn(int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    printf("INFO: Decoding output file extension\n");
    int i;
    char ch;
    char buffer[8];
    for(i = 0; i < size; i++)
    {
	fread(buffer, 8, 1, fptr_stego_image);
	decode_byte_from_lsb(&ch ,buffer);
	decInfo -> extn_output_file[i] = ch;
    }
    decInfo -> extn_output_file[i] = '\0';
    return e_success;
}

//Decoding secret file size
Status decode_secret_file_size(FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    printf("INFO: Decoding secret file size\n");
    int file_size;
    decode_int_from_lsb(&file_size, decInfo -> fptr_stego_image, decInfo);
    decInfo -> secret_file_size = file_size;
    return e_success;
}

//Decoding secret file data
Status decode_secret_file_data(FILE *fptr_output, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    printf("INFO: Decoding secret file data\n");
    decode_data_from_image(decInfo -> secret_file_size, decInfo -> fptr_stego_image, decInfo -> fptr_output);
    return e_success;
}
