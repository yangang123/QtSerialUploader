#ifndef _CRC16_H
#define _CRC16_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

uint16_t calc_crc(const uint8_t *buf, const uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC16_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

