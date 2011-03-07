/******************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                     *
 * EmuManager_str.cpp: Emulation manager. (Translatable string functions.)    *
 *                                                                            *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                         *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                                *
 * Copyright (c) 2008-2011 by David Korth.                                    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#include "EmuManager.hpp"

namespace GensQt4
{

/**
 * LgRegionCodeStr(): Get a string identifying a given LibGens region code.
 * @param region Region code. (1, 2, 4, 8)
 * @return Region code string, or empty string on error.
 */
QString EmuManager::LgRegionCodeStr(LibGens::SysVersion::RegionCode_t region)
{
	switch (region)
	{
		case LibGens::SysVersion::REGION_JP_NTSC:	return tr("Japan (NTSC)");
		case LibGens::SysVersion::REGION_ASIA_PAL:	return tr("Asia (PAL)");
		case LibGens::SysVersion::REGION_US_NTSC:	return tr("USA (NTSC)");
		case LibGens::SysVersion::REGION_EU_PAL:	return tr("Europe (PAL)");
		default:	return QString();
	}
	
	// Should not get here...
	return QString();
}


/**
 * LgRegionCodeStr(): Get a string identifying a given GensConfig region code.
 * TODO: Combine ConfRegionCode_t with RegionCode_t.
 * @param region Region code.
 * @return Region code string, or empty string on error.
 */
QString EmuManager::GcRegionCodeStr(GensConfig::ConfRegionCode_t region)
{
	switch (region)
	{
		case GensConfig::CONFREGION_AUTODETECT:	return tr("Auto-Detect");
		case GensConfig::CONFREGION_JP_NTSC:	return tr("Japan (NTSC)");
		case GensConfig::CONFREGION_ASIA_PAL:	return tr("Asia (PAL)");
		case GensConfig::CONFREGION_US_NTSC:	return tr("USA (NTSC)");
		case GensConfig::CONFREGION_EU_PAL:	return tr("Europe (PAL)");
		default:	return QString();
	}
	
	// Should not get here...
	return QString();
}


/**
 * LgRegionCodeStrMD(): Get a string identifying a given region code. (MD hex code)
 * @param region Region. (1, 2, 4, 8)
 * @return Region code string, or empty string on error.
 */
QString EmuManager::LgRegionCodeStrMD(int region)
{
	switch (region & 0xF)
	{
		case 0x1:	return tr("Japan (NTSC)");
		case 0x2:	return tr("Asia (PAL)");
		case 0x4:	return tr("USA (NTSC)");
		case 0x8:	return tr("Europe (PAL)");
		default:	return QString();
	}
	
	// Should not get here...
	return QString();
}


/**
 * SysName(): Get the system name for the specified system ID and region.
 * @param sysID System ID.
 * @param region Region.
 * @return System name, or empty string on error.
 */
QString EmuManager::SysName(LibGens::Rom::MDP_SYSTEM_ID sysId, LibGens::SysVersion::RegionCode_t region)
{
	switch (sysId)
	{
		case LibGens::Rom::MDP_SYSTEM_MD:
			// Genesis / Mega Drive.
			if (region == LibGens::SysVersion::REGION_US_NTSC)
			{
				//: MD ROM region is US/NTSC. System name should be the equivalent of "Genesis".
				return tr("Genesis", "rom-region");
			}
			else
			{
				//: MD ROM region is not US/NTSC. System name should be the equivalent of "Mega Drive".
				return tr("Mega Drive", "rom-region");
			}
		
		case LibGens::Rom::MDP_SYSTEM_MCD:
			if (region == LibGens::SysVersion::REGION_US_NTSC)
			{
				//: MCD disc region is US/NTSC. System name should be the equivalent of "Sega CD".
				return tr("Sega CD", "rom-region");
			}
			else
			{
				//: MCD disc region is not US/NTSC. System name should be the equivalent of "Mega CD".
				return tr("Mega CD", "rom-region");
			}
		
		case LibGens::Rom::MDP_SYSTEM_32X:
			switch (region)
			{
				default:
				case LibGens::SysVersion::REGION_US_NTSC:
					//: 32X ROM is US/NTSC. System name should be the equivalent of "Sega 32X".
					return tr("Sega 32X", "rom-region");
				case LibGens::SysVersion::REGION_EU_PAL:
					//: 32X ROM is EU/PAL. System name should be the equivalent of "Mega Drive 32X".
					return tr("Mega Drive 32X", "rom-region");
				case LibGens::SysVersion::REGION_JP_NTSC:
				case LibGens::SysVersion::REGION_ASIA_PAL:
					//: 32X ROM is JP or ASIA. System name should be the equivalent of "Super 32X".
					return tr("Super 32X", "rom-region");
			}
		
		case LibGens::Rom::MDP_SYSTEM_MCD32X:
			if (region == LibGens::SysVersion::REGION_US_NTSC)
			{
				//: Sega CD 32X disc region is US/NTSC. System name should be the equivalent of "Sega CD 32X".
				return tr("Sega CD 32X", "rom-region");
			}
			else
			{
				//: Sega CD 32X disc region is US/NTSC. System name should be the equivalent of "Mega CD 32X".
				return tr("Mega CD 32X", "rom-region");
			}
		
		case LibGens::Rom::MDP_SYSTEM_SMS:
			//: Master System. (No localized names yet...)
			return tr("Master System", "rom-region");
		
		case LibGens::Rom::MDP_SYSTEM_GG:
			//: Game Gear. (No localized names yet...)
			return tr("Game Gear", "rom-region");
		
		case LibGens::Rom::MDP_SYSTEM_SG1000:
			//: SG-1000. (No localized names yet...)
			return tr("SG-1000", "rom-region");
		
		case LibGens::Rom::MDP_SYSTEM_PICO:
			//: Pico. (No localized names yet...)
			return tr("Pico", "rom-region");
		
		default:
			return QString();
	}
	
	// Should not get here...
	return QString();
}


/**
 * SysName_l(): Get the localized system name for the specified system ID.
 * @param sysID System ID.
 * @return Localized system name, or empty string on error.
 */
QString EmuManager::SysName_l(LibGens::Rom::MDP_SYSTEM_ID sysId)
{
	switch (sysId)
	{
		case LibGens::Rom::MDP_SYSTEM_MD:
			//: Localized name of Sega Genesis.
			return tr("Genesis", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_MCD:
			//: Localized name of Sega CD.
			return tr("Sega CD", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_32X:
			//: Localized name of Sega 32X.
			return tr("Sega 32X", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_MCD32X:
			//: Localized name of Sega CD 32X.
			return tr("Sega CD 32X", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_SMS:
			//: Localized name of Master System.
			return tr("Master System", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_GG:
			//: Localized name of Game Gear.
			return tr("Game Gear", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_SG1000:
			//: Localized name of SG-1000.
			return tr("SG-1000", "local-region");
		
		case LibGens::Rom::MDP_SYSTEM_PICO:
			//: Localized name of Pico.
			return tr("Pico", "local-region");
		
		default:
			return QString();
	}
	
	// Should not get here...
	return QString();
}


/**
 * RomFormat(): Get the ROM format name for the specified ROM format ID.
 * @param romFormat ROM format ID.
 * @return ROM format name, or empty string on error.
 */
QString EmuManager::RomFormat(LibGens::Rom::RomFormat romFormat)
{
	switch (romFormat)
	{
		case LibGens::Rom::RFMT_BINARY:
			//: Plain binary ROM dump.
			return tr("Binary", "rom-format");
		
		case LibGens::Rom::RFMT_SMD:
			//: Interleaved ROM dump from a Super Magic Drive.
			return tr("Super Magic Drive", "rom-format");
		
		case LibGens::Rom::RFMT_SMD_SPLIT:
			//: Split interleaved ROM dump from a Super Magic Drive.
			return tr("Super Magic Drive (split)", "rom-format");
		
		case LibGens::Rom::RFMT_MGD:
			//: Interleaved ROM dump from a Multi Game Doctor.
			return tr("Multi Game Doctor", "rom-format");
		
		case LibGens::Rom::RFMT_CD_CUE:
			//: CD-ROM cue sheet. (Used for BIN/CUE.)
			return tr("CD-ROM cue sheet", "rom-format");
		
		case LibGens::Rom::RFMT_CD_ISO_2048:
		case LibGens::Rom::RFMT_CD_ISO_2352:
			//: Standard ISO-9660 CD-ROM disc image.
			return tr("ISO-9660 CD-ROM image (%1-byte sectors)", "rom-format")
				.arg(romFormat == LibGens::Rom::RFMT_CD_ISO_2048 ? 2048 : 2352);
		
		case LibGens::Rom::RFMT_CD_BIN_2048:
		case LibGens::Rom::RFMT_CD_BIN_2352:
			//: BIN portion of BIN/CUE CD-ROM disc image.
			return tr("Raw CD-ROM image (%1-byte sectors)", "rom-format")
				.arg(romFormat == LibGens::Rom::RFMT_CD_BIN_2048 ? 2048 : 2352);
		
		default:
			return QString();
	}
	
	// Should not get here...
	return QString();
}

}