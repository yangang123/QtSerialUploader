#include  <stdbool.h>
#include  <string.h>

#include "packet.h"
#include "crc16.h"

#define PACKET_DEBUG_EN 1

#if PACKET_DEBUG_EN
#define PACKET_DEBUG(format,...)   printf(format, ##__VA_ARGS__)
#else 
#define PACKET_DEBUG(format,...)   
#endif 

uint8_t address_src =  0 ;
uint8_t address_dest = 0 ;

uint8_t packet_buf[IAP_CONFIG_PACKET_BUFSIZE];
uint16_t packet_buf_index = 0;
fw_update_t fw_update;

int packet_creat_address(uint8_t src, uint8_t dest)
{
	address_src = src;
	address_dest = dest;

	return 0;
}

void pakect_send(uint8_t cmd, uint8_t *playload, uint16_t length, uint8_t *buf, uint16_t *new_len)
{   	
    buf[FW_START1_POS] = FW_HEAD_BYTE_1;
    buf[FW_START2_POS] = FW_HEAD_BYTE_2;
	buf[FW_DST_POS]    = address_dest;
    buf[FW_SRC_POS]    = address_src;

	buf[FW_LEN_L_POS]  = (length+1) & 0xff;  
    buf[FW_LEN_H_POS]  = ((length+1) >> 8) & 0xff;
    buf[FW_CMD_POS]    = cmd;

	if (length > 0) {
    	memcpy(&buf[FW_DATA_POS], playload, length);
	}
	uint16_t cksum_pos = FW_PAKET_HEAD_LEN+length;
	uint16_t cksum;
	cksum = calc_crc((const uint8_t *)buf, FW_PAKET_HEAD_LEN+length);
	buf[cksum_pos]   = cksum & 0x00ff;
	buf[cksum_pos+1] = (cksum >> 8) & 0x00ff;
	buf[cksum_pos+2] = FW_END_BYTE_1;
    buf[cksum_pos+3] = FW_END_BYTE_2;

	*new_len = FW_PAKET_HEAD_LEN + length + FW_PAKET_TAIL_LEN;
}

bool packet_parse_data_callback(char data_char, packet_desc_t *packet)
{   
	static uint16_t packet_buf_len = 0;
	static uint16_t index  = 0;
	
    packet_buf[packet_buf_index++] = data_char; //先加1
 	switch (fw_update.parse_packet_step)
	{
		case FW_HEAD:							
			if (packet_buf_index >= 2)
			{
		        if ((packet_buf[FW_START1_POS] == FW_HEAD_BYTE_1) && (packet_buf[FW_START2_POS] == FW_HEAD_BYTE_2))
		     	{   
					fw_update.parse_packet_step = FW_LEN;
					packet->head0 = FW_HEAD_BYTE_1;
					packet->head1 = FW_HEAD_BYTE_2;	
		     	}
				else if (packet_buf[FW_START2_POS] == FW_HEAD_BYTE_1)
				{   
			        /* 保留�?1一�?�? */
				    packet_buf[FW_START1_POS] = FW_HEAD_BYTE_1;
					packet_buf_index = 1;
				}
				else
				{
					packet_buf_index = 0;
					
				}
			}
			break;	
			
		case FW_LEN:	
			if (packet_buf_index >= 6)
			{  
			   packet->dest    = packet_buf[FW_DST_POS];
			   packet->src     = packet_buf[FW_SRC_POS];
			   packet->length  = *((uint16_t *)&packet_buf[FW_LEN_L_POS]);
			   packet_buf_len  = packet->length + 10;
			   if (packet_buf_len <= IAP_CONFIG_PACKET_BUFSIZE)
			   {
		      		fw_update.parse_packet_step = FW_DATA_CRC_TAIL; 
					index = 0;
			   }
			   else
			   {
					fw_update.parse_packet_step = FW_HEAD;
					packet_buf_index            = 0; 
					memset(packet_buf, 0x00, sizeof(packet_buf));
			   }
			}
			break;
		
		case FW_DATA_CRC_TAIL:
			packet->data[index++] = data_char;
			if(packet_buf_index == packet_buf_len)
			{    
				uint16_t RxCrcCheckSum_A = 0;
				uint16_t RxCrcCheckSum_B = 0;
	            RxCrcCheckSum_A =  calc_crc((const uint8_t *)&packet_buf[0], packet_buf_len-4);
			    RxCrcCheckSum_B =  *((uint16_t *)&packet_buf[packet_buf_len-4]);
				fw_update.parse_packet_step = FW_HEAD;
				packet_buf_index                = 0; 
				
				if (RxCrcCheckSum_A == RxCrcCheckSum_B) // only check crc?
				{   
				   return true;
				}
				else
				{   
				    fw_update.crc_error++;
				}
				
			}							
			break;
						
		default:
			break;
	}


	return false;
}

