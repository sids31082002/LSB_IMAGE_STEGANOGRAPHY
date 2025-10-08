#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "decode.h"
#include "types.h"
#include "common.h"

//#define SLEEP sleep(1);
char str1[255];
char string1[20];
Status_d read_and_validate_decode_bmp(char *argv[], DecodeInfo *decInfo)
{
    //argv -d .bmp [.txt]optional o/p file
    if (strstr(argv[2], ".bmp") == NULL)
    {
        return d_failure;
    }
    decInfo->stego_image_fname = argv[2];

    return d_success;
}

Status_d read_and_validate_decode_output_extn(char *argv , DecodeInfo *decInfo){
    //char str2[MAX_FILE_SUFFIX];
    if(argv != NULL){
        if(strstr(argv , ".") == NULL){
            strcpy(str1 , argv);
            strcat(str1 , decInfo -> extn_decoded_file);
            decInfo -> decoded_fname = str1;
        }
        else{
            strcpy(str1 , argv);
            char *dot = strtok(str1 , ".");
            strcat(str1 , decInfo -> extn_decoded_file);

            decInfo -> decoded_fname = str1;
        }
    }
    else{
        sleep(1);
        printf("INFO: Output file name not mentioned . Creating a default output file with 'Decoded' as filename and appropriate extension\n");
        //decInfo -> decoded_fname = "decoded";
        
        strcpy(string1 , "Decoded");
        strcat(string1 , decInfo -> extn_decoded_file);
        decInfo -> decoded_fname = string1;
    }

    return d_success;
}

//to open the steged image file
Status_d open_steg(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return d_failure;
    }
    return d_success;
}

//to open secret.txt file
Status_d open_secret(DecodeInfo *decInfo)
{
    decInfo->fptr_decoded = fopen(decInfo->decoded_fname, "wb");
    if (decInfo->fptr_decoded == NULL)
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decoded_fname);
        return d_failure;
    }
    return d_success;
}

//decoding the 8 byte inside image_buffer to a character
char decode_byte_from_lsb(const char image_buffer[8])
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = (ch) | ((image_buffer[i] & 1U) << i);
    }
    return ch;
}

//reading 8 byte and storing in a buffer then decoding
Status_d decode_data_from_image(FILE *fptr_stego_image, char image_buffer[], char *ch)
{
    if (fread(image_buffer, 1, 8, fptr_stego_image) != 8)
    {
        return d_failure; //failed to read 8 bytes from the steged image
    }
    *ch = decode_byte_from_lsb(image_buffer);
    return d_success;
}

Status_d decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    int len = strlen(magic_string);
    char image_buffer[8];
    char decoded_character;

    for (int i = 0; i < len; i++)
    {
        if (decode_data_from_image(decInfo->fptr_stego_image, image_buffer, &decoded_character) != d_success)
            return d_failure;

        printf("Expected :%c\n", decoded_character);

        if (decoded_character != magic_string[i])
            return d_failure;
    }
    return d_success;
}
 
//to decode file extension size
Status_d decode_secret_file_extn_size(DecodeInfo *decInfo) {
    char image_buffer[8];
    char decoded_character;
    unsigned char size_bytes[sizeof(int)];

    // Read sizeof(int) bytes from the stego image
    for (int i = 0; i < sizeof(int); i++) {
        if (decode_data_from_image(decInfo->fptr_stego_image, image_buffer, &decoded_character) != d_success)
            return d_failure;
        size_bytes[i] = decoded_character;
    }

    // Convert the byte array back to int
    memcpy(&decInfo->secret_file_extn_size, size_bytes, sizeof(int));
    return d_success;
}


//to decode file extn
Status_d decode_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char decoded_character;

    for (int i = 0 ; i < decInfo -> secret_file_extn_size ; i++)
    {
        //why '4'? - Because you encoded only .txt as the secret file extension, which is 4 characters including the dot ('.', 't', 'x', 't').
        if (decode_data_from_image(decInfo->fptr_stego_image, image_buffer, &decoded_character) != d_success)
            return d_failure;

        decInfo->extn_decoded_file[i] = decoded_character;
    }
    decInfo->extn_decoded_file[4] = '\0';

    return d_success;
}

//to decode secret file size fro stego image
Status_d decode_file_size(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char decoded_character;
    char decoded_size_bytes[8];

    for (int i = 0; i < sizeof(long); i++)
    {
        //why sizeof(long) ? Because , we have encoded the file size as long using 64 bytes
        if (decode_data_from_image(decInfo->fptr_stego_image, image_buffer, &decoded_character) != d_success)
            return d_failure;

        decoded_size_bytes[i] = decoded_character;
    }

#if 1
    memcpy(&decInfo->size_decoded_file, decoded_size_bytes, sizeof(long)); //converting the character array to int
#endif

#if 0
    //converting the character array to int
    long int size;
    char *ch=(char *)&size;
    for(int i=0;i<8;i++)
    {
        *(ch+i)=file_size[7-i];
    }
    decInfo->size_decoded_file = size;
#endif

    return d_success;
}

//to decode secret file data from the steged image
Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    //char decoded_size_bytes[decInfo -> size_decoded_file];
    char decoded_character;

    for (int i = 0; i < decInfo->size_decoded_file; i++)
    {
        if (decode_data_from_image(decInfo->fptr_stego_image, image_buffer, &decoded_character) != d_success)
            return d_failure;

        //decoded_size_bytes[i] = decoded_character;
        fwrite(&decoded_character, 1, 1, decInfo->fptr_decoded);
    }

#if 0
    decoded_size_bytes[decInfo -> size_decoded_file] = '\0';
    //write the data stored in the character array completely to the opened output file
    fwrite(decoded_size_bytes , 1 , decInfo -> size_decoded_file , decInfo -> fptr_decoded);
#endif

    return d_success;
}

Status_d do_decoding(DecodeInfo *decInfo , char *argv[])
{
    sleep(1);
    printf("INFO: ## Decoding Procedure Started ##\n");

    sleep(1);
    printf("INFO: Opening required files\n");
    if (open_steg(decInfo) == d_success)
    {
        sleep(1);
        printf("INFO: Opened %s\n", decInfo->stego_image_fname);

        //skip the 54 bytes header from the .bmp file before decoding
        fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    }
    else
    {
        exit(1);
    }

    //check if magic string is present
    sleep(1);
    printf("INFO: Decoding Magic String signature\n");
    if (decode_magic_string(MAGIC_STRING, decInfo) == d_success)
    {
        sleep(1);
        printf("INFO: Done\n");
    }
    else
    {
        sleep(1);
        printf("INFO: Magic string not present, Image is not Stegged\n");
        exit(1);
    }

    //decoding file extension size
    sleep(1);
    printf("INFO: Decoding secret file extension size from %s\n",decInfo -> stego_image_fname);
    if(decode_secret_file_extn_size(decInfo) == d_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error decoding file extension\n");
    }
   

    //decoding file extension
    sleep(1);
    printf("INFO: Decoding file extension from %s\n", decInfo->stego_image_fname);
    if (decode_file_extn(decInfo) == d_success)
    {
        sleep(1);
        printf("INFO: Done\n");
    }
    else
    {
        sleep(1);
        printf("INFO: error decoding file extension\n");
    }

    //validating output file name and creating an appropriate output decoded file name
    if(read_and_validate_decode_output_extn(argv[3], decInfo) == d_success){
        printf("INFO: Output file set to '%s'\n", decInfo->decoded_fname);
    } 
    else {
        printf("ERROR: Failed to set the output file name\n");
        exit(1);
    }

    //opening output decoded file with required
    if (open_secret(decInfo) == d_success)
    {
        sleep(1);
        printf("INFO: Opened %s\n", decInfo->decoded_fname);

        sleep(1);
        printf("INFO: Done, Opened all required files\n");
    }
    else
    {
        exit(1);
    }

    //decoding file size
    sleep(1);
    printf("INFO: Decoding file size from %s\n", decInfo->stego_image_fname);
    if (decode_file_size(decInfo) == d_success)
    {
        sleep(1);
        printf("INFO: Done\n");
    }
    else
    {
        sleep(1);
        printf("INFO: error decoding file size\n");
    }

    //decoding file data
    sleep(1);
    printf("INFO: Decoding file data from %s\n", decInfo->stego_image_fname);
    if (decode_secret_file_data(decInfo) == d_success)
    {
        sleep(1);
        printf("INFO: Done\n");
    }
    else
    {
        sleep(1);
        printf("INFO: error decoding file data\n");
    }

    sleep(1);
    printf("INFO: ## Decoding done successfully ##\n");

    // Close all opened files
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_decoded);

    return d_success;
}
