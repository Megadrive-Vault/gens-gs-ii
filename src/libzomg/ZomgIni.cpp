/***************************************************************************
 * libzomg: Zipped Original Memory from Genesis.                           *
 * ZomgIni.cpp: ZOMG.ini handler.                                          *
 *                                                                         *
 * Copyright (c) 2013-2015 by David Korth.                                 *
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

#include "ZomgIni.hpp"

// C++ includes.
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
using std::string;
using std::swap;
using std::ostringstream;

// Platform-dependent newline constant.
#ifdef _WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif
		
namespace LibZomg
{

class ZomgIniPrivate
{
	public:
		ZomgIniPrivate();

	private:
		// Q_DISABLE_COPY() equivalent.
		// TODO: Add LibZomg-specific version of Q_DISABLE_COPY().
		ZomgIniPrivate(const ZomgIniPrivate &);
		ZomgIniPrivate &operator=(const ZomgIniPrivate &);

	public:
		// Useful functions.
		static void WriteValue(ostringstream& oss, const string &key, const string &value);
		static void WriteValue(ostringstream& oss, const string &key, uint32_t value, int width = 0, bool hex = false);

	public:
		string systemId;
		string creator;
		string creatorVersion;
		string creatorVcsVersion;
		string author;
		string romFilename;
		uint32_t romCrc32;
		string region;
		string description;
		string extensions;
};

/*****************************
 * ZomgIniPrivate functions. *
 *****************************/

ZomgIniPrivate::ZomgIniPrivate()
	: romCrc32(0)
{ }

/**
 * Write an INI value to an ostringstream.
 * @param key Key.
 * @param value Value.
 */
void ZomgIniPrivate::WriteValue(ostringstream& oss, const string &key, const string &value)
{
	oss << key << "=";
	for (size_t i = 0; i < value.length(); i++) {
		char chr = value.at(i);
		switch (chr) {
			case '\r':
				// Ignore '\r' here.
				// It'll be reinserted if the INI is loaded
				// on a Windows system.
				break;
			case '\n':
				// Escape newlines.
				oss << "\\n";
				break;
			case '\\':
				// Escape backslashes.
				oss << "\\\\";
				break;
			default:
				// Other character.
				oss << chr;
				break;
		}
	}
	oss << NL;
}

/**
 * Write an INI value to an ostringstream.
 * @param key Key.
 * @param value Value.
 * @param width Minimum width of the field.
 * @param hex If true, print in hex (with leading "0x").
 */
void ZomgIniPrivate::WriteValue(ostringstream& oss, const string &key, uint32_t value, int width, bool hex)
{
	std::ios_base::fmtflags old_flags = oss.flags();
	std::streamsize old_width = oss.width();
	char old_fill = oss.fill();

	oss << key << "=";
	if (hex) {
		oss << "0x";
		oss << std::hex << std::uppercase;
		// std::showbase doesn't work properly.
		// If value == 0, it doesn't do anything.
		// if value == 1, it prints 000000x1.
	}
	oss.width(width);
	oss.fill('0');
	oss << value;
	oss.flags(old_flags);
	oss.fill(old_fill);
	oss.width(old_width);
	oss << NL;
}

/**********************
 * ZomgIni functions. *
 **********************/

ZomgIni::ZomgIni()
	: d(new ZomgIniPrivate())
{ }

ZomgIni::~ZomgIni()
{
	delete d;
}

/**
 * Clear the loaded ZOMG.ini data.
 */
void ZomgIni::clear(void)
{
	// Swap the private class with a new instance.
	ZomgIni empty;
	swap(d, empty.d);
}

/**
 * Save the ZOMG.ini file.
 * @return String representation of ZOMG.ini.
 */
std::string ZomgIni::save(void) const
{
	ostringstream oss;

	// Write the ZOMG section header.
	oss << "[ZOMG]" << NL;

	// Write the ZOMG properties.
	d->WriteValue(oss, "FileType", "Zipped Original Memory from Genesis");
	// TODO: Get the ZomgVersion from somewhere.
	d->WriteValue(oss, "Version", "0.1-DEV-UNSTABLE");
	d->WriteValue(oss, "System", d->systemId);
	d->WriteValue(oss, "Creator", d->creator);
	d->WriteValue(oss, "CreatorVersion", d->creatorVersion);
	d->WriteValue(oss, "CreatorVcsVersion", d->creatorVcsVersion);
	d->WriteValue(oss, "Author", d->author);
	d->WriteValue(oss, "ROM", d->romFilename);
	d->WriteValue(oss, "ROM_CRC32", d->romCrc32, 8, true);
	d->WriteValue(oss, "Region", d->region);
	d->WriteValue(oss, "Description", d->description);
	d->WriteValue(oss, "Extensions", d->extensions);

	// Return the ZOMG.ini file.
	return oss.str();
}

/** Property get/set functions. **/
// TODO: Use macros?

// TODO: Use bitfield with system IDs instead of a string.
string ZomgIni::systemId(void) const
	{ return d->systemId; }
void ZomgIni::setSystemId(const string &systemId)
	{ d->systemId = systemId; }

string ZomgIni::creator(void) const
	{ return d->creator; }
void ZomgIni::setCreator(const string &creator)
	{ d->creator = creator; }

string ZomgIni::creatorVersion(void) const
	{ return d->creatorVersion; }
void ZomgIni::setCreatorVersion(const string &creatorVersion)
	{ d->creatorVersion = creatorVersion; }

string ZomgIni::creatorVcsVersion(void) const
	{ return d->creatorVcsVersion; }
void ZomgIni::setCreatorVcsVersion(const string &creatorVcsVersion)
	{ d->creatorVcsVersion = creatorVcsVersion; }

string ZomgIni::author(void) const
	{ return d->author; }
void ZomgIni::setAuthor(const string &author)
	{ d->author = author; }

string ZomgIni::romFilename(void) const
	{ return d->romFilename; }
void ZomgIni::setRomFilename(const string &romFilename)
	{ d->romFilename = romFilename; }

uint32_t ZomgIni::romCrc32(void) const
	{ return d->romCrc32; }
void ZomgIni::setRomCrc32(uint32_t romCrc32)
	{ d->romCrc32 = romCrc32; }

string ZomgIni::region(void) const
	{ return d->region; }
void ZomgIni::setRegion(const string &region)
	{ d->region = region; }

string ZomgIni::description(void) const
	{ return d->description; }
void ZomgIni::setDescription(const string &description)
	{ d->description = description; }

string ZomgIni::extensions(void) const
	{ return d->extensions; }
void ZomgIni::setExtensions(const string &extensions)
	{ d->extensions = extensions; }

}
