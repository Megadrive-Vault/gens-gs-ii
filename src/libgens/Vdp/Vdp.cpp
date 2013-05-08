/***************************************************************************
 * libgens: Gens Emulation Library.                                        *
 * Vdp.cpp: VDP class: General functions.                                  *
 *                                                                         *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                      *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                             *
 * Copyright (c) 2008-2011 by David Korth.                                 *
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

#include "Vdp.hpp"

// C includes.
#include <string.h>

// ARRAY_SIZE(x)
#include "macros/common.h"

namespace LibGens
{
	
// VdpRend_Err private class.
#include "VdpRend_Err_p.hpp"

/** Static member initialization. (VDP global items) **/
// TODO: Make these non-static?
VdpTypes::VdpEmuOptions_t Vdp::VdpEmuOptions =
{
	VdpTypes::INTREND_FLICKER,	// intRendMode
	true,				// borderColorEmulation
	true,				// ntscV30Rolling
	false,				// zeroLengthDMA
	true,				// spriteLimits
	true,				// vscrollBug
};


/**
 * Initialize the VDP subsystem.
 * @param fb Existing MdFb to use. (If nullptr, allocate a new MdFb.)
 */
Vdp::Vdp(MdFb *fb)
	: MD_Screen(fb ? fb->ref() : new MdFb())
	, d_err(new VdpRend_Err_Private(this))
{
	// Initialize the Horizontal Counter table.
	unsigned int hc_val;
	for (unsigned int hc = 0; hc < 512; hc++)
	{
		// H32
		hc_val = ((hc * 170) / 488) - 0x18;
		H_Counter_Table[hc][0] = (uint8_t)hc_val;
		
		// H40
		hc_val = ((hc * 205) / 488) - 0x1C;
		H_Counter_Table[hc][1] = (uint8_t)hc_val;
	}
	
	// Clear VDP_Reg before initializing the VDP.
	// Valgrind complains if we don't do this.
	memset(&VDP_Reg.reg[0], 0x00, sizeof(VDP_Reg.reg));
	VDP_Mode = 0;
	
	// Initialize system status.
	// TODO: Move SysStatus somewhere else?
	SysStatus.data = 0;
	
	// Initialize the VDP rendering subsystem.
	rend_init();
	
	// Reset the VDP.
	reset();
}


/**
 * Vdp::~Vdp(): Shut down the VDP subsystem.
 */
Vdp::~Vdp(void)
{
	// Shut down the VDP rendering subsystem.
	rend_end();
	
	// Unreference the framebuffer.
	MD_Screen->unref();
}


/**
 * Vdp::reset(): Reset the VDP.
 */
void Vdp::reset(void)
{
	// Reset the VDP rendering arrays.
	rend_reset();

	// Clear VRam and VSRam.
	memset(&VRam, 0x00, sizeof(VRam));
	memset(&VSRam, 0x00, sizeof(VSRam));

	// Reset the palette. (Includes CRam.)
	m_palette.reset();

	/**
	 * VDP registers.
	 * Default register values: (Mode 5)
	 * - 0x01 (Mode1):   0x04 (H_Int off, Mode 5 [MD])
	 * - 0x0A (H_Int):   0xFF (disabled).
	 * - 0x0C (Mode4):   0x81 (H40, S/H off, no interlace)
	 * - 0x0F (AutoInc): 0x02 (auto-increment by 2 on memory access)
	 * All other registers are set to 0x00 by default.
	 */
	static const uint8_t vdp_reg_init_m5[24] =
	{
		0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00, 0x81, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	for (int i = 0; i < ARRAY_SIZE(vdp_reg_init_m5); i++) {
		setReg(i, vdp_reg_init_m5[i]);
	}

	// Reset the DMA variables.
	DMAT_Length = 0;
	DMAT_Type = 0;

	// VDP status register.
	// (Maintain the status of the PAL/NTSC bit.)
	const bool isPal = Reg_Status.isPal();
	Reg_Status.reset(isPal);

	// Other variables.
	VDP_Int = 0;

	// VDP control struct.
	VDP_Ctrl.Data.d = 0;
	VDP_Ctrl.Access = 0;
	VDP_Ctrl.Address = 0;
	VDP_Ctrl.DMA_Mode = 0;
	VDP_Ctrl.DMA = 0;
	VDP_Ctrl.ctrl_latch = false;

	// Set the VDP update flags.
	MarkVRamDirty();

	// Initialize the Horizontal Interrupt counter.
	HInt_Counter = VDP_Reg.m5.H_Int;
}

}
