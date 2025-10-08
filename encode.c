#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

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
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

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
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo -> fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

OperationType check_operation_type(char *argv[]){
    if(argv[1] == NULL){
        return e_unsupported;
    }
    if(strcmp(argv[1] , "-e") == 0){
        return e_encode;
    }
    else if(strcmp(argv[1] , "-d") == 0){
        return e_decode;
    }

    return e_unsupported;
   
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo){
    if(strstr(argv[2] , ".bmp") == NULL){
        return e_failure;
    }
    encInfo -> src_image_fname = argv[2];

    if(strstr(argv[3] , ".") == NULL){
        fprintf(stderr, "ERROR: Secret file must have an extension\n");
        return e_failure;
    }
    encInfo -> secret_fname = argv[3];

    char *dot = strrchr(argv[3] , '.');
    if(dot == NULL || strlen(dot) >= MAX_FILE_SUFFIX){
        fprintf(stderr , "ERROR: Invalid or too long file extension\n");
        return e_failure;
    }
    strcpy(encInfo -> extn_secret_file , dot);

    encInfo -> secret_file_extn_size = strlen(dot);

    if(argv[4]){
        if(strstr(argv[4] , ".bmp") == NULL){
            char image_buff[255];
            strcpy(image_buff)
        }
        encInfo -> stego_image_fname = argv[4];  //possible bcz we are storing the base address
    }
    else{
        sleep(1);
        printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
        encInfo -> stego_image_fname = "stego_img.bmp";   //as encInfo is a pointer and stego_image_fname is also a character pointer , so here we are storing the address of the string literal in the character pointer
    }

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo , const char *magic_string){
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    if(encInfo -> size_secret_file == 0){
        sleep(1);
        printf("INFO: Empty secret string\n");
        exit(0);
    } 
    sleep(1);
    printf("INFO: Done, Not empty\n");

    sleep(1);
    printf("INFO: Checking for %s capacity to handle %s\n" , encInfo -> src_image_fname , encInfo -> secret_fname);

    if((strlen(magic_string) + (encInfo -> secret_file_extn_size) + 8 + 4)*8 > ((encInfo -> image_capacity) - 54))     //2 - magic string size + 4 - fileextension + 8 - secret file size(sizeof(long)) + 4 - secret file extension
        return e_failure;
    
    return e_success;
}

uint get_file_size(FILE *fptr){
    fseek(fptr , 0 , SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image){
    rewind(fptr_src_image);    // to bring the file pointer of the src image to first pos
    rewind(fptr_stego_image);   // to bring the file pointer of the dest image to first pos

    //read 54 bytes of header from source bmp file and copy it into destination bmp file
    char header_buffer[54];
    fread(header_buffer , 1 , 54 , fptr_src_image);     //copy first 54 header bytes of bmp file into 

    fwrite(header_buffer , 1 , 54 , fptr_stego_image);   //from header_buffer to dest image bmp file

    return e_success;

}


Status encode_data_to_image(const char data, FILE *fptr_src_image, FILE *fptr_stego_image){
    char image_buffer[8] = {};
    fread(image_buffer , 1 , 8 , fptr_src_image);

    encode_byte_to_lsb(data , image_buffer);

    fwrite(image_buffer , 1 , 8 , fptr_stego_image);

    return e_success;
}

Status encode_byte_to_lsb(const char data, char image_buffer[]){

    for(int i = 0; i < 8 ;i++){
        image_buffer[i] = (image_buffer[i] & (~1)) | ((data & (1U << i)) >> i);
                //clear lsb bit of image_buffer[i]   //to get each bit of the data 
    }
    return e_success;
}



Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){

    for(int i = 0 ; i < strlen(magic_string); i++){
        encode_data_to_image(magic_string[i] , encInfo -> fptr_src_image , encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(int file_extn_size , EncodeInfo *encInfo){

    unsigned char *ch = (unsigned char *)&file_extn_size;

    for(int i = 0;i < sizeof(int) ; i++){
        encode_data_to_image(ch[i] , encInfo -> fptr_src_image , encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    
    for(int i = 0 ; i < strlen(file_extn);i++){
        encode_data_to_image(file_extn[i] , encInfo -> fptr_src_image , encInfo -> fptr_stego_image);
    }
    return e_success;

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    unsigned char *ch = (unsigned char *)&file_size;

    for(int i = 0;i < sizeof(long);i++){
        encode_data_to_image(*(ch + i) , encInfo -> fptr_src_image , encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo){

    fseek(encInfo -> fptr_secret , 0 , SEEK_SET);

    int ch;
    while((ch = fgetc(encInfo -> fptr_secret)) != EOF){
        encode_data_to_image((char)ch , encInfo -> fptr_src_image , encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char buffer[1024];
    size_t bytes;

    while((bytes = fread(buffer , 1 , sizeof(buffer) , fptr_src)) > 0){

        fwrite(buffer , 1 , bytes , fptr_dest);

    }

    return e_success;

}


Status do_encoding(EncodeInfo *encInfo){
    sleep(1);
    printf("INFO: Opening required files\n");

    if(open_files(encInfo) == e_success){
        sleep(1);
        printf("INFO: Opened %s\n"
                "INFO: Opened %s\n"
                "INFO: Opened %s\n"
                "INFO: Done\n" , encInfo -> src_image_fname , encInfo -> secret_fname ,  encInfo -> stego_image_fname);
    }
    else{
        exit(0);
    }
    sleep(1);
    printf("INFO: ## Encoding Procedure Started ##\n");

    sleep(1);
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);

    if(check_capacity(encInfo , MAGIC_STRING) == e_success){
        sleep(1);
        printf("INFO: Done, Found OK\n");
    }
    else{
        sleep(1);
        printf("INFO: %s cannot handle the %s\n",encInfo->src_image_fname,encInfo -> secret_fname);
        exit(0);
    }

    //encoding starts from here
    sleep(1);
    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo -> fptr_src_image , encInfo -> fptr_stego_image) == e_success){
        sleep(1);
        printf("INFO: Done copying header to stego file\n");
    }
     else
    {
        sleep(1);
        printf("INFO: error copying image header\n");
        exit(0);
    }

    //encoding magic string to stego file
    sleep(1);
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING , encInfo) == e_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error copying magic string\n");
        exit(0);
    }


    //encoding secret file extension size
    sleep(1);
    printf("INFO : Encoding %s File Extension size \n",encInfo -> secret_fname);

    if(encode_secret_file_extn_size(encInfo -> secret_file_extn_size , encInfo) == e_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error copying  secret file's extension size\n");
        exit(0);
    }


    //encode secret file extension
    sleep(1);
    printf("INFO: Encoding %s File Extenstion\n",encInfo -> secret_fname);
    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error copying secret file's extension\n");
        exit(0);
    }

    //encode secret file size
    sleep(1);
    printf("INFO: Encoding %s File Size\n" , encInfo -> secret_fname);
    if(encode_secret_file_size(encInfo -> size_secret_file ,encInfo) == e_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error copying file size \n");
        exit(0);
    }

    //encode secret file data
    sleep(1);
    printf("INFO: Encoding %s file data \n",encInfo->secret_fname);
    if(encode_secret_file_data(encInfo) == e_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error copying secret file data\n");
        exit(0);
    }

    //copy remaining rgb image data to output stego file
    sleep(1);
    printf("INFO: Copying left over data \n");
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success){
        sleep(1);
        printf("INFO: Done\n");
    }
    else{
        sleep(1);
        printf("INFO: error copying remaining image data\n");
        exit(0);
    }

    sleep(1);
    printf("INFO: ## Encoding Done Successfully ##\n");

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;

}
