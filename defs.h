/* date = July 7th 2025 11:12 pm */

#ifndef DEFS_H
#define DEFS_H

#define uint16_t unsigned short
#define uint32_t unsigned int

#pragma pack(push, 1)
typedef struct{
    char Magic[4]; // CRN\0
    uint16_t Endianness; //00 for LE, 01 for BE (But who tf uses it nowadays?)
    uint32_t NumFiles;
}CRN_Header;
#pragma pack(pop)

#endif //DEFS_H
