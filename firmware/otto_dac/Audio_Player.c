#include"includes/Audio_Player.h"

uint16_t crc16(uint16_t crcValue, uint8_t newByte) 
{
	uint8_t i;

	for (i = 0; i < 8; i++) {

		if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80)){
			crcValue = (crcValue << 1)  ^ 0x8005;
		}else{
			crcValue = (crcValue << 1);
		}

		newByte <<= 1;
	}
  
	return crcValue;
}

static int Get_Frame_Length(MPEG_Header frame)
{
    int value = 0;

    switch (frame.layer_des)
    {
    case L_I:

        if(frame.is_padded == true) value = (12 * frame.Bitrate / frame.MPEG_Sample_freq + 32)*4;
        else value = (12 * frame.Bitrate / frame.MPEG_Sample_freq)*4;

        break;
    
    default:
        if(frame.is_padded == true) value = ((144*frame.Bitrate/frame.MPEG_Sample_freq) + 1);
        else value = (144*frame.Bitrate/frame.MPEG_Sample_freq);
        
        break;
    }

    return value;
}

static MPEG_Header Decode_MPEG_Frame(MP3_Song* song, int entry_point)
{
    MPEG_Header frame;
    int tries = 0;

    entry_point += 20;
    uint32_t audio_frame = 0;
    
    do
    { 
        audio_frame = (uint32_t)(song->file_data[entry_point+3]|song->file_data[entry_point+2] << 8|song->file_data[entry_point+1] << 16|song->file_data[entry_point] << 24);
        if(tries > 50)
        {
            printf("Sync frame not found...\n");
            break;
        }
        tries++;
        printf("invalid sync sequence detected, might be a false sync... %06X / %d\n", audio_frame, entry_point);
        entry_point++;
    } while (!(audio_frame & 0xFFF00000));
    printf("frame found.\n\n");
    
    
    if(audio_frame & (uint32_t)(0b1000 << 16))
    {
        frame.version = Ver_1;

    }else if(audio_frame & (uint32_t)(0b1100 << 16))
    {
        frame.version = Ver_1;
    }

    if(audio_frame & ((uint32_t)(0b0010 << 16)))
    {
        frame.layer_des = L_III;
    }else if(audio_frame & ((uint32_t)(0b0100 << 16)))
    {
        frame.layer_des = L_II;
    }else if(audio_frame & ((uint32_t)(0b0110 << 16)))
    {
        frame.layer_des = L_I;
    }

    if(frame.version == Ver_2)
    {
        frame.MPEG_Sample_freq = 44;
        if(audio_frame & (0x00000200))
        {
            frame.MPEG_Sample_freq = 48;
        }
       
        
        if(audio_frame & (0x00006000))
        {
            frame.Bitrate = 224;
        }else if (audio_frame & (0x00001000)){
            frame.Bitrate = 40;
        }else if (audio_frame & (0x00001800)){
            frame.Bitrate = 48;
        }else if (audio_frame & (0x00000800)){
            frame.Bitrate = 16;
        }
    }

    frame.Frame_Length = Get_Frame_Length(frame);

    if(!(audio_frame & (1 << 16))) frame.is_protected = true;
    if(audio_frame & (1 << 9)) frame.is_padded = true;

    frame.channel_mode = _Stero;
    if(audio_frame & (0x00000040))
    {
        frame.channel_mode = _Dual_Channel_Stero;
    }

    if(audio_frame & (0x00000020))
    {
        frame.channel_mode = _Joint_Stero;
    }

    if(audio_frame & (0x00000030))
    {
        frame.channel_mode = _Single_Channel_Mono;
    }

    frame.main_data_pointer = 0;

    if(frame.is_protected)
    {
        printf("IT WORKED, entry point: %d\n", entry_point);

        for(uint8_t i = 0; i < 9; i++)
        {
            uint16_t pointer = song->file_data[entry_point + 6]|song->file_data[entry_point + 5] << 8;
            
            if((pointer & (1 << i)))
            {
                frame.main_data_pointer |= (1 << i); //calculates the side data entry point
            }
            else
            {
                frame.main_data_pointer &= ~(1 << i);
            }
        }

    }else{
        
        for(uint8_t i = 0; i < 9; i++)
        {
            uint16_t pointer = song->file_data[entry_point + 4]|song->file_data[entry_point + 3] << 8;
            if((pointer & (1 << i)))
            {
                frame.main_data_pointer |= (1 << i);
            }
            else
            {
                frame.main_data_pointer &= ~(1 << i);
            }
        }
    }

    printf("main information entry point (relative to header start probably): %d %s\n\n", frame.main_data_pointer, frame.main_data_pointer ? "...Jump to this pointer" : "...Samples should start after the side data");
    
    

    return frame;
}

MPEG_Header Get_First_Header(MP3_Song* song)
{
    /*
    
    uint8_t data[2]= {
        0x80,
        0x05
    };

    uint16_t crc;
    uint8_t aux = 0;

    crc = 0x0000;

    while(aux < 2)
    {
        crc = crc16(crc, data[aux]);
        aux++;
    }
    
    */

    return Decode_MPEG_Frame(song, song->ID3_size);
}

void Generate_Samples(MP3_Player player)
{
    
}

