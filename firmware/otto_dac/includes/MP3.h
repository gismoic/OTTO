#pragma once

#include<stdint.h>
#include<stdio.h>
#include<stdbool.h>

typedef struct{

	uint16_t version;
    
    char* MP3_Name;
    char* MP3_Album;
    char* MP3_Singer;
    int* MP3_TrackNumber;
    char* MP3_TrackYear;
    char* MP3_BPM;

    uint8_t ID_flags;
    uint32_t ID3_size;
    uint8_t* file_data;

    int file_size;
    bool Has_BPM;
    bool Has_Year;
	

} MP3_Song;

MP3_Song Load_MP3_file(char *filename);
