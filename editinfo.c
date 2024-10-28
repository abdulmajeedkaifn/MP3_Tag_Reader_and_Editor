#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "types.h"
#include "editinfo.h"

void red();
void green();
void yellow();
void reset();
void print();

// To validate editinfo arguments

Status read_and_validate_editinfo_args(char *argv[], EditInfo *editinfo)
{
    if(!(strstr(argv[2], "-t") || strstr(argv[2], "-a") || strstr(argv[2], "-A") || strstr(argv[2], "-y") || strstr(argv[2], "-m") || strstr(argv[2], "-c")))
    {
        printf("Enter valid option that is to be edited, check for help...!\n");
        return e_failure;
    }

    if(strcmp(argv[2], "-t") == 0)
    {
        editinfo->edit_tag = "TIT2";
        //printf("tag = %s\n", editinfo->edit_tag);
    }
    else if(strcmp(argv[2], "-a") == 0)
    {
        editinfo->edit_tag = "TPE1";
    }
    else if(strcmp(argv[2], "-A") == 0)
    {
        editinfo->edit_tag = "TALB";
    }
    else if(strcmp(argv[2], "-y") == 0)
    {
        editinfo->edit_tag = "TYER";
    }
    else if(strcmp(argv[2], "-m") == 0)
    {
        editinfo->edit_tag = "TCON";
    }
    else if(strcmp(argv[2], "-c") == 0)
    {
        editinfo->edit_tag = "COMM";
    }
    //printf("tag opted: %s\n", editinfo->edit_tag);

    editinfo->changing_text = argv[3];
    //printf("changing_text: %s\n", editinfo->changing_text);

    if(strstr(argv[4], ".mp3") != NULL)
    {
        editinfo->mp3_fname = argv[4];
        return e_success;
    }
}

Status open_files_edit(EditInfo *editinfo)
{
    editinfo->mp3_fptr = fopen(editinfo->mp3_fname, "r");

    if(editinfo->mp3_fptr == NULL)
    {
        printf("Unable to open file %s", editinfo->mp3_fname);
        return e_failure;
    }

    editinfo->dup_fname = "duplicate.mp3";
    editinfo->mp3_dup = fopen(editinfo->dup_fname, "w");
    //printf("dupfname: %s\n", editinfo->dup_fname);

    if(editinfo->mp3_dup == NULL)
    {
        printf("Unable to open file %s", editinfo->dup_fname);
        return e_failure;
    }

    return e_success;
}

Status get_edit_header(EditInfo *editinfo)
{
    char id_buffer[4];

    fread(id_buffer, sizeof(char), 3, editinfo->mp3_fptr);
    //printf("id rewind: %ld\n", ftell(editinfo->mp3_fptr));

    id_buffer[3] = '\0';

    if(strstr(id_buffer, "ID3") == NULL)
    {
        printf("ERROR: File Type is not ID3...!\n");
    }
    //printf("id: %s\n", id_buffer);

    char ver_buffer[2];

    fread(ver_buffer, sizeof(char), 2, editinfo->mp3_fptr);
    //printf("ver rewind: %ld\n", ftell(editinfo->mp3_fptr));

    //printf("buffer0: %02x buffer1: %02x\n", ver_buffer[0], ver_buffer[1]);

    if(!(ver_buffer[0] == 0x03 && ver_buffer[1] == 0x00))
    {
        printf("ERROR: %s file version is not 2.3 ...!\n", editinfo->mp3_fname);
    }

    return e_success;
}


Status edit_info(EditInfo *editinfo)
{
    char buffer[5]; //4(tag bytes)+1(null terminator)
    long tag_offset = 0;
    buffer[5] = '\0';
    int new_title_len = strlen(editinfo->changing_text);

    while(fread(buffer, 1, 4, editinfo->mp3_fptr) == 4)
    {
        if(strcmp(buffer, editinfo->edit_tag) == 0)
        {
            tag_offset = ftell(editinfo->mp3_fptr) - 4; //get offset from where tag starts
            break;
        }

        fseek(editinfo->mp3_fptr, -3, SEEK_CUR);
    }

    // printf("tag offset = %ld\n", tag_offset);
    rewind(editinfo->mp3_fptr);
    for(int i = 0; i < tag_offset; i++)
    {
        char ch = fgetc(editinfo->mp3_fptr);
        fputc(ch, editinfo->mp3_dup);
    }

    fwrite(buffer, 4, 1, editinfo->mp3_dup);

    uint size = htonl(new_title_len + 1);

    fwrite(&size, 4, 1, editinfo->mp3_dup); //Write new_size in duplicate file

    fseek(editinfo->mp3_dup, 3, SEEK_CUR); //To skip 3 flag bytes

    fwrite(editinfo->changing_text, new_title_len, 1, editinfo->mp3_dup);

    fseek(editinfo->mp3_fptr, 4, SEEK_CUR);
    // printf("orig pointer: %ld\n", ftell(editinfo->mp3_fptr));

    char size_buffer[4];
    int original_size = 0; //Size of original tag data used to sikp that many bytes in original file befor copying remaining data

    fread(size_buffer, 4, 1, editinfo->mp3_fptr);

        // Converting size from Big Endian to Little Endian
        for (int i = 0; i < 2; i++)
        {
            char temp = size_buffer[i];
            size_buffer[i] = size_buffer[4 - i - 1];
            size_buffer[4 - i - 1] = temp;
        }

        // Copying the size to an integer variable
        memcpy(&original_size, size_buffer, sizeof(size));
        original_size = original_size - 1;

    fseek(editinfo->mp3_fptr, tag_offset + 11 + original_size, SEEK_SET);

    char remaining_buffer[1000];
    size_t bytes_read;

    while ((bytes_read = fread(remaining_buffer, sizeof(char), sizeof(remaining_buffer), editinfo->mp3_fptr)) > 0)
    {
        fwrite(remaining_buffer, sizeof(char), bytes_read, editinfo->mp3_dup);
    }
    return e_success;
}

Status copy(EditInfo *editinfo)
{
    char buffer[1000];
    size_t bytes_read;

    fclose(editinfo->mp3_fptr);
    fclose(editinfo->mp3_dup);

    editinfo->mp3_fptr = fopen(editinfo->mp3_fname, "w");

    if(editinfo->mp3_fptr == NULL)
    {
        printf("Unable to open file %s", editinfo->mp3_fname);
        return e_failure;
    }

    editinfo->mp3_dup = fopen(editinfo->dup_fname, "r");

    if(editinfo->mp3_dup == NULL)
    {
        printf("Unable to open file %s", editinfo->dup_fname);
        return e_failure;
    }

    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), editinfo->mp3_dup)) > 0)
    {
        fwrite(buffer, sizeof(char), bytes_read, editinfo->mp3_fptr);
    }
}