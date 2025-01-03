#include"includes/MP3.h"
#include<stdlib.h>

#define ID3_TOKEN   0x494433    //3 byte long ID3 signature used to identfy MP3 files
#define TITLE_ID    0x54495432
#define YEAR_ID     0x54594552
#define ALBUM_ID    0x54414C42
#define LEAD_ID     0x54504531
#define BPM_ID      0x5442504D

static int Load_MP3_Header(MP3_Song *data)
{
    if((uint32_t)(data->file_data[2] | data->file_data[1] << 8 | data->file_data[0] << 16) != ID3_TOKEN)
    {
        printf("No ID3 sequence found. Content is either not a MP3 file or in an unsupported ID3 format: %06X\n", (uint32_t)(data->file_data[0] | data->file_data[1] << 8 | data->file_data[2] << 16));
        return 0;
    }else{
        
        data->version = (uint16_t)(data->file_data[4]|data->file_data[3] << 8); //combines two bytes after the ID3 sequence. Identifies version
        data->ID_flags = data->file_data[5] & 0x1F; //flags used to identify MP3 settings

        uint32_t ID_32 = data->file_data[6] << 21;
        ID_32 += data->file_data[7] << 14;
        ID_32 += data->file_data[8] << 7;
        ID_32 += data->file_data[9];
        
        if(data->ID_flags & (1 << 6)) //checks extended header bit and minuses a ceritan number from it
        {
            ID_32 -= 20; //if an extended header is included
        }
        else{

            ID_32 -= 10; //if an extended header is not included
        }
        data->ID3_size = ID_32;

        
        int Offset = 10;
        while(Offset <= data->ID3_size) //keeps looping while in header
        {
            int loop_times = 0;
            loop_times++;

            uint32_t frame_ID = (uint32_t)(data->file_data[Offset + 3]|data->file_data[Offset + 2] << 8|data->file_data[Offset + 1] << 16|data->file_data[Offset] << 24);

            printf("%08X\n", frame_ID);

            uint32_t frame_size = data->file_data[Offset + 4];
            frame_size += data->file_data[Offset+5];
            frame_size += data->file_data[Offset+6];
            frame_size += data->file_data[Offset+7];

            uint16_t frame_flags = (uint16_t)(data->file_data[Offset+9]|data->file_data[Offset+8] << 8);

            if(loop_times == 1)
            {
                Offset = Offset + 4+7; //system only works if these numbers are added to offset.
            }

            switch (frame_ID)
            {
                case TITLE_ID:
                    Offset = Offset + frame_size - 1;
                    printf("MP3 Title Identified");
                    printf("%d", Offset - frame_size);

                    data->MP3_Name = (char*)malloc(frame_size);

                    for(int i = 1; i < frame_size; i++)
                    {
                        data->MP3_Name[i] = (char)data->file_data[i + (Offset-frame_size)];
                    }
                    printf("%s\n", data->MP3_Name);

                    
                    break;
                
                case YEAR_ID:

                    Offset = Offset + frame_size - 1;
                    data->MP3_TrackYear = (char*)malloc(4);
                    for(int i = 0; i < frame_size; i++)
                    {
                        data->MP3_TrackYear[i] = (char)data->file_data[i + (Offset - 4)];
                    }

                    data->Has_Year = true;

                    break;
                
                case ALBUM_ID:
                    Offset = Offset + frame_size - 1;

                    data->MP3_Album = (char*)malloc(frame_size);

                    for(int i = 1; i < frame_size; i++)
                    {
                        data->MP3_Album[i] = (char)data->file_data[i + (Offset-frame_size)];
                    }

                    break;
                case LEAD_ID:
                    Offset = Offset + frame_size - 1;
                    data->MP3_Singer = (char*)malloc(frame_size);

                    for(int i = 1; i < frame_size; i++)
                    {
                        data->MP3_Singer[i] = (char)data->file_data[i + (Offset-frame_size)];
                    }

                    break;
                
                case BPM_ID:

                    Offset = Offset + frame_size - 1;
                    data->MP3_BPM = (char*)malloc(3);
                    for(int i = 0; i < frame_size; i++)
                    {
                        data->MP3_BPM[i] = (char)data->file_data[i + (Offset-3)];
                    }

                    data->Has_BPM = true;
                    break;
                
                default:
                    printf(" Unkonwn Frame ID found\n");
                    Offset = Offset + frame_size - 1;
                    break;
            }
        }
    }

    return 1;
}

MP3_Song Load_MP3_file(char *filename)
{
    MP3_Song new_song;

	FILE *MP3_file = fopen(filename, "r");

	if(MP3_file == NULL)
	{
		printf("No MP3 file found at this location\n");
		fclose(MP3_file);
	}
	
	fseek(MP3_file, 0, SEEK_END);
	new_song.file_size = ftell(MP3_file);

	rewind(MP3_file);
	new_song.file_data = (uint8_t*)malloc(new_song.file_size);
	fread(new_song.file_data, new_song.file_size, 1, MP3_file); //stores all data in a varible so we can easily acess it.
	fclose(MP3_file);

    Load_MP3_Header(&new_song);
    
    return new_song;
}