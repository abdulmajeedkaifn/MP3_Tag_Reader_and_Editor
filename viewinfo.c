#include <stdio.h>
#include <string.h>
#include "types.h"
#include "viewinfo.h"

void red ()
{
  printf("\033[1;31m");
}
void green()
{
    printf("\033[0;32m");
}
void yellow ()
{
  printf("\033[1;33m");
}
void blue ()
{
    printf("\033[0;34m ");
}
void cyan()
{
    printf("\033[0;36m ");
}

/* Function to reset text color to default */

void reset ()
{
  printf("\033[0m");
}

/* Function defination to read and validate viewinfo argumenst*/

Status read_and_validate_viewinfo_args(char *argv[], ViewInfo *viewInfo)
{
    if(strstr(argv[2], ".mp3") != NULL)
    {
        viewInfo->mp3_fname = argv[2];
        return e_success;
    }
    else
    {
        printf("ERROR: ./a.out : INVALID ARGUMENTS\nUSAGE : \n");
        printf("To view please pass like: ./a.out -v mp3filename\n");
        return e_failure;
    }
}

//Function defination for open files

Status open_files(ViewInfo *viewInfo)
{
    viewInfo->mp3_fptr = fopen(viewInfo->mp3_fname, "r");

    if(viewInfo->mp3_fptr == NULL)
    {
        printf("Unable to open file %s", viewInfo->mp3_fname);
        return e_failure;
    }

    return e_success;
}

Status get_header(ViewInfo *viewInfo)
{
    char id_buffer[4];

    fread(id_buffer, sizeof(char), 3, viewInfo->mp3_fptr);
    //printf("id rewind: %ld\n", ftell(viewInfo->mp3_fptr));

    id_buffer[3] = '\0';

    if(strstr(id_buffer, "ID3") == NULL)
    {
        printf("ERROR: File Type is not ID3...!\n");
    }

    char ver_buffer[2];

    fread(ver_buffer, sizeof(char), 2, viewInfo->mp3_fptr);
    //printf("ver rewind: %ld\n", ftell(viewInfo->mp3_fptr));

    //printf("buffer0: %02x buffer1: %02x\n", ver_buffer[0], ver_buffer[1]);

    if(!(ver_buffer[0] == 0x03 && ver_buffer[1] == 0x00))
    {
        printf("ERROR: %s file version is not 2.3 ...!\n", viewInfo->mp3_fname);
    }

    return e_success;
}

Status validate_tag(char *tag)
{
    char *str[] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};

    int found = 0;

    for(int i = 0; i < 6; i++)
    {
        if(strcmp(str[i], tag) == 0)
        {
            found = 1;
            break;
        }
    }

    if(found == 1)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status print_tag(char *tag)
{
    // Print the valid title
    if(strcmp(tag, "TIT2") == 0)
    {
        cyan();
        printf("\t\tTITLE\t\t:");
        reset();
    }
    else if(strcmp(tag, "TPE1") == 0)
    {
        cyan();
        printf("\t\tARTIST\t\t:");
        reset();
    }
    else if(strcmp(tag, "TALB") == 0)
    {
        cyan();
        printf("\t\tALBUM\t\t:");
        reset();
    }
    else if(strcmp(tag, "TYER") == 0)
    {
        cyan();
        printf("\t\tYEAR\t\t:");
        reset();
    }
    else if(strcmp(tag, "TCON") == 0)
    {
        cyan();
        printf("\t\tMUSIC\t\t:");
        reset();
    }
    else if(strcmp(tag, "COMM") == 0)
    {
        cyan();
        printf("\t\tLANGUAGE\t:");
        reset();
    }

}

Status viewinfo(ViewInfo *viewInfo)
{
    int offset = 10, size = 0;
    char title_buffer[5];
    char size_buffer[5];

    for(int i = 0; i < 6; i++)
    {
        fseek(viewInfo->mp3_fptr, offset, SEEK_SET);

        // Read the title (4 characters)
        fread(title_buffer, 4, 1, viewInfo->mp3_fptr);
        title_buffer[4] = '\0';  // null termination

        // Read the size even if the tag is invalid
        fread(size_buffer, 4, 1, viewInfo->mp3_fptr);

        // Converting size from Big Endian to Little Endian
        for (int i = 0; i < 2; i++)
        {
            char temp = size_buffer[i];
            size_buffer[i] = size_buffer[4 - i - 1];
            size_buffer[4 - i - 1] = temp;
        }

        // Copying the size to an integer variable
        memcpy(&size, size_buffer, sizeof(size));
        size = size - 1;  // Adjust the size if necessary

        // Validate the tag before proceeding
        if (validate_tag(title_buffer) == e_failure)
        {
            // Skip the current frame data if the tag is invalid
            offset += (11 + size);  // Move to the next frame (skip current frame data and header)
            continue;
        }

        print_tag(title_buffer);

        // Read the frame data into new_str
        char new_str[size + 1];  // +1 for null terminator
        //offset + 11, in 11 we have 4(TIT2)+4(size)+3(flag)
        fseek(viewInfo->mp3_fptr, offset + 11, SEEK_SET);  // Move to the data portion of the frame
        fread(new_str, size, 1, viewInfo->mp3_fptr);
        new_str[size] = '\0';  // Null terminate the string

        // Replace any null characters with dots (.)
        for (int i = 0; i < size; i++)
        {
            if (new_str[i] == '\0')
            {
                new_str[i] = ' ';
            }
        }

        // Print the valid frame data
        printf("\t%s\n", new_str);

        // Update offset for the next frame
        offset += (11 + size);
    }
}