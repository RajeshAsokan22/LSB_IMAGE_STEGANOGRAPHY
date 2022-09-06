/*
NAME: RAJESH.A
DATE: 10/8/2022
DESCRIPTION: LSB IMAGE STEGANOGRAPHY
SAMPLE I/P:
SAMPLE O/P:
 */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    DecodeInfo decodeinfo;
    EncodeInfo encodeinfo;


    if(check_operation_type(argv) == e_encode)
    {
	if(read_and_validate_encode_args(argv, &encodeinfo) == e_success)
	{
	    if(do_encoding(&encodeinfo) == e_success)
	    {
		printf("INFO: ## Encoding Done Successfully ##\n");
	    }
	    else
		printf("ERROR: Encoding failure!!\n");
	}
	else
	    printf("ERROR: Arguments should be given in sequence ./a.out -e <.bmp_file> <.text_file> [output file]\n");
    }
    else if(check_operation_type(argv) == e_decode)
    {
	if(read_and_validate_decode_args(argv, &decodeinfo) == e_success)
	{
	    if(do_decoding(&decodeinfo) == e_success)
	    {
		printf("INFO: ## Decoding Done Successfully ##\n");
	    }
	    else
		printf("ERROR: Decoding failure!!\n");
	}
	else
	   printf("ERROR: Arguments should be given in sequence ./a.out -d <.bmp_file> <.text_file>\n");
    }
    else
	printf("ERROR: Invalid option\n");


    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
	return e_encode;
 else if(strcmp(argv[1],"-d") == 0)
	return e_decode;
    else
	return e_unsupported;
}
