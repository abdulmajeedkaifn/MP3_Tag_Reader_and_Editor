#include <stdio.h>
#include <string.h>
#include "types.h"
#include "viewinfo.h"
#include "editinfo.h"

void print();
void print1();

void red();
void green();
void yellow();
void reset();
void print();

int main(int argc, char *argv[])
{

    if(argc < 2 || argc > 5 || argc == 4)
    {
        printf("ERROR: ./a.out : INVALID ARGUMENTS\nUSAGE : \n");
        printf("To view please pass like: ./a.out -v mp3filename\n");
        printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c changing_text mp3filename\n");
        printf("To get help pass like: ./a.out --help\n");
    }

    ViewInfo viewinfo_var;
    EditInfo editinfo_var;

    //OperationType op_type;
    int op_type = check_operation_type(argv[1]);

    if(argc == 3)
    {
        if(op_type == mp3_viewinfo)
        {
            if(read_and_validate_viewinfo_args(argv, &viewinfo_var) == e_success)
            {
                //printf("Read and Validations are successful...!\n");
                open_files(&viewinfo_var);
                get_header(&viewinfo_var);

                print();
                green();
                printf("\t\t\t\tMP3 Tag Reader and Editor for ID3v2\n");
                reset();
                print();
                viewinfo(&viewinfo_var);
                printf("\n");
                print1();
                green();
                printf("DETAILS DISPLAYED SUCCESSFULLY");
                reset();
                print1();
                printf("\n"); printf("\n");
            }
        }
    }
    else if(argc == 5)
    {
        if(op_type == mp3_editinfo)
        {
            //printf("Editinfo Option Selected\n");
            if(read_and_validate_editinfo_args(argv, &editinfo_var) == e_success)
            {
                //printf("Read and Validations are Successful...!\n");
                open_files_edit(&editinfo_var);
                get_edit_header(&editinfo_var);
                edit_info(&editinfo_var);
                copy(&editinfo_var);
            }
        }
    }
    else if(argc == 2)
    {
        if(op_type == mp3_help)
        {
            for(int i = 0; i < 30; i++)
            {
                printf("-");
            }
            printf("HELP MENU");
            for(int i = 0; i < 30; i++)
            {
                printf("-");
            }
            printf("\n");
            printf("1. -v -> to view mp3 file contents\n2. -e -> to edit mp3 file contents\n");
            printf("\t2.1. -t -> to edit song title\n\t2.2. -a -> to edit artist name\n");
            printf("\t2.3. -A -> to edit album name\n\t2.4. -y -> to edit year\n");
            printf("\t2.5. -m -> to edit content\n\t2.6. -c -> to edit comment\n");

            for(int i = 0; i < 69; i++)
            {
                printf("-");
            }
            printf("\n");
        }
    }
}


OperationType check_operation_type(char *argv)
{
    if(strstr(argv, "-v") != NULL)
    {
        return mp3_viewinfo;
    }
    else if(strstr(argv, "-e") != NULL)
    {
        return mp3_editinfo;
    }
    else if(strstr(argv, "--help") != NULL)
    {
        return mp3_help;
    }
    else
    {
        printf("ERROR: ./a.out : INVALID ARGUMENTS\nUSAGE : \n");
        printf("To view please pass like: ./a.out -v mp3filename\n");
        printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c changing_text mp3filename\n");
        printf("To get help pass like: ./a.out --help\n");
        return mp3_unsupported;
    }
}

void print()
{
    for(int i = 0; i < 100; i++)
    {
        printf("-");
    }
    printf("\n");
}

void print1()
{
    for(int i = 0; i < 35; i++)
    {
        printf("-");
    }
}