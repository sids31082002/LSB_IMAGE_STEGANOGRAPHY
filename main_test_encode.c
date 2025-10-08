#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc , char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    OperationType op = check_operation_type(argv);

    if(op == e_encode){
        if(argc >= 4 && read_and_validate_encode_args(argv , &encInfo) == e_success){
            do_encoding(&encInfo);
            printf("Encoded file size = %ld\n", encInfo.size_secret_file);
        }    
        else{
            printf("/lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.txt_file> [output file]\n");
            return 1;
        }    

    }
    if(op == e_decode){
        if(argc < 3) {
            fprintf(stderr, "ERROR: Missing stego image filename\n");
            return 1;
        }
        if(argc >= 3 && argc <= 4 && read_and_validate_decode_bmp(argv , &decInfo) == d_success){
            do_decoding(&decInfo , argv);
            printf("Decoded file size = %ld\n", decInfo.size_decoded_file);
        }    
        else{
            printf("/lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
            return 1;
        }    
    }
    if(op == e_unsupported){
        printf("/lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.txt_file> [output file]\n");
        printf("/lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        return 1;
    }


    return 0;

    #if 0
    uint img_size;
    Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";

    // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return 1;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);

    return 0;
    #endif
    
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "encode.h"
// #include "decode.h"
// #include "types.h"

// int main(int argc, char *argv[])
// {
//     EncodeInfo encInfo;
//     DecodeInfo decInfo;

//     if(argc < 2){
//         printf("Usage:\n");
//         printf("Encoding: ./lsb_steg -e <.bmp_file> <.txt_file> [output file]\n");
//         printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
//         return 1;
//     }

//     OperationType op = check_operation_type(argv); // Call once

//     if(op == e_encode){
//         if(argc >= 4){
//             if(read_and_validate_encode_args(argv, &encInfo) == e_success){
//                 do_encoding(&encInfo);
//             }
//             else{
//                 printf("ERROR: Invalid encode arguments\n");
//                 printf("Usage: ./lsb_steg -e <.bmp_file> <.txt_file> [output file]\n");
//                 return 1;
//             }
//         }
//         else{
//             printf("ERROR: Not enough arguments for encoding\n");
//             printf("Usage: ./lsb_steg -e <.bmp_file> <.txt_file> [output file]\n");
//             return 1;
//         }
//     }
//     else if(op == e_decode){
//         if(argc >= 3){
//             if(read_and_validate_decode_args(argv, &decInfo) == d_success){
//                 do_decoding(&decInfo);
//             }
//             else{
//                 printf("ERROR: Invalid decode arguments\n");
//                 printf("Usage: ./lsb_steg -d <.bmp_file> [output file]\n");
//                 return 1;
//             }
//         }
//         else{
//             printf("ERROR: Not enough arguments for decoding\n");
//             printf("Usage: ./lsb_steg -d <.bmp_file> [output file]\n");
//             return 1;
//         }
//     }
//     else{
//         printf("ERROR: Unsupported operation\n");
//         printf("Usage:\n");
//         printf("Encoding: ./lsb_steg -e <.bmp_file> <.txt_file> [output file]\n");
//         printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
//         return 1;
//     }

//     return 0;
// }
