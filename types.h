#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

//Status will be used as function return type
typedef enum
{
    e_success,
    e_failure
} Status;

//OperationType will be used as function return type to check type of operation i.e., being performed
typedef enum
{
    mp3_viewinfo,
    mp3_editinfo,
    mp3_help,
    mp3_unsupported
} OperationType;

#endif