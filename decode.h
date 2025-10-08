#ifndef decode_H
#define decode_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10

typedef struct _decodeInfo
{
   /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    //uint image_capacity; 

    /*Decoded File Info */
    char *decoded_fname;
    FILE *fptr_decoded;
    int secret_file_extn_size;
    char extn_decoded_file[MAX_FILE_SUFFIX];
    //char decoded_file_data[MAX_SECRET_BUF_SIZE];
    long size_decoded_file;

} DecodeInfo;


/* Decoding function prototype */

// /* Check operation type */
// OperationType check_operation_type(char *argv[]);

/* Read and validate decode 1st and 2nd args from argv */
Status_d read_and_validate_decode_bmp(char *argv[], DecodeInfo *decInfo);

/* Read and validate decode output file args from argv */
Status_d read_and_validate_decode_output_extn(char *argv, DecodeInfo *decInfo);

/* Perform the encoding */
Status_d do_decoding(DecodeInfo *decInfo , char *argv[]);

/* Get File pointers for i/p steged files */
Status_d open_steg(DecodeInfo *decInfo);

/* To get file pointers for output decoded secret file*/
Status_d open_secret(DecodeInfo *decInfo);

/* Store Magic String */
Status_d decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* decode secret file extenstion size */
Status_d decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status_d decode_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status_d decode_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status_d decode_secret_file_data(DecodeInfo *decInfo);

/* decode function, which does the real encoding */
Status_d decode_data_from_image(  FILE *fptr_stego_image , char image_buffer[] , char *ch);

/* decode a byte into LSB of image data array */
char decode_byte_from_lsb(const char image_buffer[8]);

// /* Copy remaining image bytes from src to stego image after encoding */
// Status_d copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);



#endif