#ifndef ENCODE_H
#define ENCODE_H

typedef struct edit
{
    char *mp3_fname; // To store original mp3 file name
    FILE *mp3_fptr; // To store address of our mp3 file

    char *dup_fname; // To store duplicate file name
    FILE *mp3_dup; // To store duplicate file address

    char *edit_tag; // To store the tag id(-t/-a/-A/-y/-m/-c) that is to be edited

    char *changing_text; //To store text that is to be changed in the title of respective tag

} EditInfo;

Status read_and_validate_editinfo_args(char *argv[], EditInfo *editinfo);

Status open_files_edit(EditInfo *editinfo);

Status get_edit_header(EditInfo *editinfo);

Status edit_info(EditInfo *editinfo);

Status copy_data(EditInfo *editinfo);

Status edit_data(EditInfo *editinfo);

Status copy(EditInfo *editinfo);

#endif