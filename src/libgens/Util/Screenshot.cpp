/***************************************************************************
 * libgens: Gens Emulation Library.                                        *
 * Screenshot.hpp: Screenshot helper.                                      *
 *                                                                         *
 * Copyright (c) 2015 by David Korth.                                      *
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

#include "Screenshot.hpp"

// LibGens
#include "Util/MdFb.hpp"
#include "Rom.hpp"

// LibZomg
#include "libzomg/PngWriter.hpp"
#include "libzomg/Metadata.hpp"
#include "libzomg/img_data.h"
using LibZomg::PngWriter;
using LibZomg::Metadata;

// C includes. (C++ namespace)
#include <cerrno>
#include <cstring>

// C++ includes.
#include <string>
using std::string;

namespace LibGens {

/**
 * Save a screenshot to a file.
 * File will be in PNG format.
 * TODO: Metadata flags parameter.
 * @param fb		[in] MD framebuffer.
 * @param rom		[in] ROM object. (Needed for some metadata.)
 * @param filename	[in] Filename for the screenshot.
 * @return 0 on success; negative errno on error.
 */
int Screenshot::toFile(const MdFb *fb, const Rom *rom, const utf8_str *filename)
{
	if (!fb || !filename || !filename[0])
		return -EINVAL;

	// Take the screenshot.
	fb->ref();
	const int imgXStart = fb->imgXStart();
	const int imgYStart = fb->imgYStart();

	// TODO: Option to save the full framebuffer, not just active display?
	// NOTE: Zeroing the struct in case new stuff is added later.
	Zomg_Img_Data_t img_data;
	memset(&img_data, 0, sizeof(img_data));
	img_data.w = fb->imgWidth();
	img_data.h = fb->imgHeight();

	// Aspect ratio.
	// Vertical is always 4.
	// Horizontal is 4 for H40, 5 for H32.
	// TODO: Handle Interlaced mode 2x rendering?
	img_data.phys_y = 4;
	// TODO: Formula to automatically scale for any width?
	img_data.phys_x = (img_data.w == 256 ? 5 : 4);

	const MdFb::ColorDepth bpp = fb->bpp();
	if (bpp == MdFb::BPP_32) {
		img_data.data = (void*)(fb->lineBuf32(imgYStart) + imgXStart);
		img_data.pitch = (fb->pxPitch() * sizeof(uint32_t));
		img_data.bpp = 32;
	} else {
		img_data.data = (void*)(fb->lineBuf16(imgYStart) + imgXStart);
		img_data.pitch = (fb->pxPitch() * sizeof(uint16_t));
		img_data.bpp = (bpp == MdFb::BPP_16 ? 16 : 15);
	}

	// Set up metadata.
	Metadata metadata;

	// System ID.
	// TODO: Pass the MDP system ID directly.
	const char *sysId;
	switch (rom->sysId()) {
		case Rom::MDP_SYSTEM_UNKNOWN:
		default:
			sysId = nullptr;
			break;
		case Rom::MDP_SYSTEM_MD:
			sysId = "MD";
			break;
		case Rom::MDP_SYSTEM_MCD:
			sysId = "MCD";
			break;
		case Rom::MDP_SYSTEM_32X:
			sysId = "32X";
			break;
		case Rom::MDP_SYSTEM_MCD32X:
			// TODO: "MCD32X", or "MCD,32X"?
			// Note that "MCD" and "32X" imply MD.
			sysId = "MCD,32X";
			break;
		case Rom::MDP_SYSTEM_SMS:
			sysId = "SMS";
			break;
		case Rom::MDP_SYSTEM_GG:
			sysId = "GG";
			break;
		case Rom::MDP_SYSTEM_SG1000:
			sysId = "SG-1000";
			break;
		case Rom::MDP_SYSTEM_PICO:
			sysId = "Pico";
			break;
		/* TODO: ColecoVision.
		case Rom::MDP_SYSTEM_CV:
			sysId = "CV";
			break;
		*/
	}

	if (sysId != nullptr) {
		metadata.setSystemId(string(sysId));
	};

	//metadata.setRegion();		// TODO: Get region code.
	// TODO: Save ROM filename with extension; also, z_file?
	metadata.setRomFilename(rom->filenameBaseNoExt());
	metadata.setRomCrc32(rom->rom_crc32());
	//metadata.setRomSize(rom->romSize());	// TODO; also, include SMD header?
	// TODO: Add more metadata.

	// Write the PNG image.
	// TODO: Do UTF-8 filenames work with libpng on Windows?
	PngWriter pngWriter;
	return pngWriter.writeToFile(&img_data, filename,
				&metadata, Metadata::MF_Default);
}

}