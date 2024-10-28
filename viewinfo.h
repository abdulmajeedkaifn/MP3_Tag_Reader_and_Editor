#ifndef VIEWINFO_H
#define VIEWINFO_H

typedef struct _ViewInfo
{
    char *mp3_fname; //To store mp3 filename
    FILE *mp3_fptr;  //To store address of mp3file.mp3

} ViewInfo;

//To check the operation type whether it is viewinfo/ editinfo/ help
OperationType check_operation_type(char *argv);


//To read and validate viewinfo arguments
Status read_and_validate_viewinfo_args(char *argv[], ViewInfo *viewInfo);

//To print the mp3 file info

Status viewinfo(ViewInfo *viewInfo);

//To open files
Status open_files(ViewInfo *viewInfo);


//To get Signature and version bytes from starting 10 byte header
Status get_header(ViewInfo *viewInfo);

Status validate_tag(char *tag);

Status print_tag(char *tag);

#endif
