/***************************************************************************
 * libzomg: Zipped Original Memory from Genesis.                           *
 * zomg_vdp.h: ZOMG save definitions for the Video Display Processor.      *
 *                                                                         *
 * Copyright (c) 2008-2010 by David Korth                                  *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#ifndef __LIBGENS_SAVE_ZOMG_ZOMG_VDP_H__
#define __LIBGENS_SAVE_ZOMG_ZOMG_VDP_H__

#include "zomg_common.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * vdp_reg: VDP registers.
 * File: common/vdp_reg.bin
 * NOTE: Byteswapping is done in Zomg.cpp when saving/loading.
 */
#pragma pack(1)
typedef union _vdp_reg
{
	uint8_t tms9918[8];	// TMS9918: 0x00 - 0x07
	uint8_t sms[11];	// SMS/GG: 0x00 - 0x0A
	uint8_t md[24];		// MD: 0x00 - 0x17
} vdp_reg;
#pragma pack()

/**
 * VdpCtrl_8: VDP control registers. (8-bit)
 * File: common/vdp_ctrl.bin
 * NOTE: Byteswapping is done in Zomg.cpp when saving/loading.
 */
#define ZOMG_VDPCTRL_8_HEADER	0x534D5320
#pragma pack(1)
typedef struct _Zomg_VdpCtrl_8_t
{
	uint32_t header;		// 32-bit BE: Should be "SMS " (0x534D5320)
	
	uint8_t ctrl_byte[2];		// 8-bit: VDP control bytes.
	uint16_t reserved1;
	uint8_t ctrl_latch;		// 8-bit: Control latch. 0 == first word; 1 == second.
	uint8_t dest;			// 8-bit: Read/Write location. (TODO)
	
	uint16_t address;		// 16-bit BE: VDP address counter.
	uint8_t status;			// 8-bit: VDP status register.
	uint8_t reserved2;
	
	uint8_t data_read_buffer;	// 8-bit: Data read buffer.
} Zomg_VdpCtrl_8_t;
#pragma pack()

/**
 * VRam: Video RAM.
 * File: common/VRam.bin
 * NOTE: Byteswapping is done in Zomg.cpp when saving/loading.
 */
#pragma pack(1)
typedef union _Zomg_VRam_t
{
	uint8_t sms[16384];	// TMS9918/SMS/GG
	uint16_t md[32768];	// MD
} _Zomg_VRam_t;
#pragma pack()

/**
 * CRam: Color RAM.
 * File: common/CRam.bin
 * NOTE: Byteswapping is done in Zomg.cpp when saving/loading.
 */
#pragma pack(1)
typedef union _Zomg_CRam_t
{
	uint8_t sms[32];	// SMS only
	uint16_t gg[32];	// GG (little-endian)
	uint16_t md[64];	// MD (big-endian)
} Zomg_CRam_t;
#pragma pack()

// TODO: Other common VDP stuff.

/** MD-specific. **/

/**
 * VdpCtrl_16: VDP control registers. (16-bit)
 * File: common/vdp_ctrl.bin
 * NOTE: Byteswapping is done in Zomg.cpp when saving/loading.
 */
#define ZOMG_VDPCTRL_16_HEADER	0x4D442020
#pragma pack(1)
typedef struct _Zomg_VdpCtrl_16_t
{
	uint32_t header;		// 32-bit BE: Should be "MD  " (0x4D442020)
	
	uint16_t ctrl_word[2];		// 16-bit BE: VDP control words.
	uint8_t ctrl_latch;		// 8-bit: Control latch. 0 == first word; 1 == second.
	uint8_t dest;			// 8-bit: Read/Write location. (TODO)
	
	uint16_t address;		// 16-bit BE: VDP address counter.
	uint16_t status;		// 16-bit BE: VDP status register.
	
	uint16_t data_fifo[4];		// 16-bit BE: Data FIFO. (4 words)
	uint8_t data_fifo_count;	// 8-bit: Number of words currently in the FIFO.
	uint8_t reserved2;
	
	// TODO: Finalize DMA registers.
	uint32_t dma_src_address;	// 32-bit BE: DMA source address.
	uint16_t dma_length;		// 16-bit BE: DMA length.
} Zomg_VdpCtrl_16_t;
#pragma pack()

#pragma pack(1)
/**
 * MD_VSRam: Vertical Scroll RAM.
 * File: MD/VSRam.bin
 */
typedef struct _Zomg_MD_VSRam_t
{
	uint16_t MD_VSRam[40];
} Zomg_MD_VSRam_t;
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* __LIBGENS_SAVE_ZOMG_ZOMG_VDP_H__ */
