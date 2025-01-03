#pragma once
#include<stdio.h>
#include<stdint.h>
#include"MP3.h"
#include<stdbool.h>

typedef enum {
    
    //Version 2.5 is not supported by this decoder
    Ver_2 = 0b10,
    Ver_1 = 0b11,

} MPEG_Version;

typedef enum{

    L_III = 0b01,
    L_II = 0b10,
    L_I = 0b11,

} MPEG_Layer_des;


typedef enum {

    _Stero,
    _Joint_Stero,
    _Dual_Channel_Stero,
    _Single_Channel_Mono,  

} Channel_Mode;

typedef enum {

    _Off = 0b00,
    _MS_Stero = 0,
    _Intensity_Stero = 1,
    _both = 3

} stero_switch;



typedef struct 
{
    MPEG_Version version;
    MPEG_Layer_des layer_des;
    Channel_Mode channel_mode;
    stero_switch stero_sw;

    int MPEG_Sample_freq;
    int Bitrate;
    bool is_padded;    //Layer 1: 32 extra bits Layer 2&3: 8 extra bits
    int Frame_Length;  //Must be calculated using this formula FL = 144*Bitrate/Sample_freq+Padding(if it exists)
    bool is_protected; //if frame uses a 16-bit CRC for data authentication

    uint16_t main_data_pointer; //points to the location where audio data is being stored. If zero. follows after side info




} MPEG_Header;

typedef struct{

    MP3_Song* song;
    uint32_t song_duration;
    uint32_t song_volume;
    uint8_t* buffer;
    
    bool is_playing;
    bool is_complete;

}MP3_Player;


MPEG_Header Get_First_Header(MP3_Song* song);
void Generate_Samples(MP3_Player player);
