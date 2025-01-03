#include"src/include/SDL2/SDL.h"
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include"includes/MP3.h"
#include<windows.h>
#include"includes/Audio_Player.h"

int main(int argc, char* argv[])
{
	MP3_Song my_song = Load_MP3_file(argv[1]);
	printf("ID3_Size:  %d\n", my_song.ID3_size);
	printf("File Size: %d\n", my_song.file_size);
	MPEG_Header first_header = Get_First_Header(&my_song);


	switch(first_header.version)
	{
		case Ver_1:
			printf("frame version: 1.0\n");
			break;
		
		case Ver_2:
			printf("frame version: 2.0\n");
			break;
		
		default:
			printf("Unsupported Version detected %d\n", first_header.version);
			break;

	}

	switch(first_header.layer_des)
	{
		case L_III:
			printf("MPEG layer: III\n");
			break;
		case L_II:
			printf("MPEG layer: II\n");
			break;
		case L_I:
			printf("MPEG layer: I\n");
			break;
		
		default:
			printf("Unsupported layer detected %d\n", first_header.layer_des);
			break;

	}

	printf("bitrate: %dkb/s\n", first_header.Bitrate);
	printf("sample Freq.: %dkHz\n", first_header.MPEG_Sample_freq);
	printf("frame Length (in bytes): %d\n", first_header.Frame_Length);
	printf("is_protected = %s\n", first_header.is_protected ? "true":"false");
	printf("is_padded = %s\n", first_header.is_padded ? "true":"false");

	switch(first_header.channel_mode)
	{
		case _Dual_Channel_Stero:
			printf("channel mode: Dual Channel");
			break;
		
		case _Stero:
			printf("channel mode: Stero");
			break;
		
		case _Joint_Stero:
			printf("channel mode: Joint Stero");
			break;
		
		case _Single_Channel_Mono:
			printf("channel mode: Single Channel");
			break;

		default:
			printf("channel mode: N/A");
			break;
	}

	

	SDL_Quit();
	return 0;
}
