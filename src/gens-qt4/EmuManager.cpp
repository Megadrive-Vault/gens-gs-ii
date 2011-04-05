/******************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                     *
 * EmuManager.cpp: Emulation manager.                                         *
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

#include <config.h>

#include "EmuManager.hpp"
#include "gqt4_main.hpp"

// LibGens includes.
#include "libgens/Util/Timing.hpp"
#include "libgens/MD/EmuMD.hpp"

// LibGens Sound Manager.
// Needed for LibGens::SoundMgr::MAX_SAMPLING_RATE.
#include "libgens/sound/SoundMgr.hpp"

// Audio backend.
#include "Audio/GensPortAudio.hpp"

// LibGens video includes.
#include "libgens/MD/VdpPalette.hpp"
#include "libgens/MD/VdpRend.hpp"
#include "libgens/MD/VdpIo.hpp"
#include "libgens/MD/TAB336.h"

// M68K_Mem.hpp has SysRegion.
#include "libgens/cpu/M68K_Mem.hpp"

// Multi-File Archive Selection Dialog.
#include "ZipSelectDialog.hpp"

// libzomg. Needed for savestate preview images.
#include "libzomg/Zomg.hpp"

// Qt includes.
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>

// Qt key handler.
#include "Input/KeyHandlerQt.hpp"

// Filename case-sensitivity.
// TODO: Determine this from QAbstractFileEngine instead of hard-coding it!
// TODO: Share this with the RecentRoms class.
#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
#define FILENAME_CASE_SENSITIVE Qt::CaseInsensitive
#else
#define FILENAME_CASE_SENSITIVE Qt::CaseSensitive
#endif


namespace GensQt4
{

EmuManager::EmuManager(QObject *parent)
	: QObject(parent)
{
	// Initialize timing information.
	m_lastTime = 0.0;
	m_lastTime_fps = 0.0;
	m_frames = 0;
	
	// No ROM is loaded at startup.
	m_rom = NULL;
	m_paused.data = 0;
	
	// Save slot.
	m_saveSlot = gqt4_config->saveSlot();
	
	// TODO: Load initial VdpPalette settings.
	
	// Create the Audio Backend.
	// TODO: Allow selection of all available audio backend classes.
	// NOTE: Audio backends are NOT QWidgets!
	m_audio = new GensPortAudio();
	
	// Connect the configuration slots.
	connect(gqt4_config, SIGNAL(saveSlot_changed(int)),
		this, SLOT(saveSlot_changed_slot(int)));
	connect(gqt4_config, SIGNAL(autoFixChecksum_changed(bool)),
		this, SLOT(autoFixChecksum_changed_slot(bool)));
	
	// Graphics settings.
	connect(gqt4_config, SIGNAL(contrast_changed(int)),
		this, SLOT(contrast_changed_slot(int)));
	connect(gqt4_config, SIGNAL(brightness_changed(int)),
		this, SLOT(brightness_changed_slot(int)));
	connect(gqt4_config, SIGNAL(grayscale_changed(bool)),
		this, SLOT(grayscale_changed_slot(bool)));
	connect(gqt4_config, SIGNAL(inverted_changed(bool)),
		this, SLOT(inverted_changed_slot(bool)));
	connect(gqt4_config, SIGNAL(colorScaleMethod_changed(int)),
		this, SLOT(colorScaleMethod_changed_slot(int)));
	connect(gqt4_config, SIGNAL(interlacedMode_changed(GensConfig::InterlacedMode_t)),
		this, SLOT(interlacedMode_changed_slot(GensConfig::InterlacedMode_t)));
	
	// Region code settings.
	connect(gqt4_config, SIGNAL(regionCode_changed(GensConfig::ConfRegionCode_t)),
		this, SLOT(regionCode_changed_slot(GensConfig::ConfRegionCode_t)));
	connect(gqt4_config, SIGNAL(regionCodeOrder_changed(uint16_t)),
		this, SLOT(regionCodeOrder_changed_slot(uint16_t)));
}

EmuManager::~EmuManager()
{
	// Delete the audio backend.
	m_audio->close();
	delete m_audio;
	m_audio = NULL;
	
	// Delete the ROM.
	// TODO
	
	// TODO: Do we really need to clear this?
	m_paused.data = 0;
}


/**
 * openRom(): Open a ROM file.
 * Prompts the user to select a ROM file, then opens it.
 * @return 0 on success; non-zero on error.
 */
int EmuManager::openRom(void)
{
	// TODO: Proper compressed file support.
	#define ZLIB_EXT " *.zip *.zsg *.gz"
	#define LZMA_EXT " *.7z"
	#define RAR_EXT " *.rar"
	
	// TODO: Set the default filename.
	QString filename = QFileDialog::getOpenFileName(NULL,
			tr("Open ROM"),		// Dialog title
			QString(),		// Default filename.
			tr("Sega Genesis ROM images") +
			QLatin1String(
				" (*.bin *.gen *.md *.smd"
#ifdef HAVE_ZLIB
				ZLIB_EXT
#endif /* HAVE_ZLIB */
#ifdef HAVE_LZMA
				LZMA_EXT
#endif /* HAVE_LZMA */
				RAR_EXT
				");;"
				) +
#if 0
			tr("Sega Genesis / 32X ROMs; Sega CD disc images") +
			"(*.bin *.smd *.gen *.32x *.cue *.iso *.raw" ZLIB_EXT LZMA_EXT RAR_EXT ");;" +
#endif
			tr("All Files") + QLatin1String(" (*.*)"));
	
	if (filename.isEmpty())
		return -1;
	
	// Convert the filename to native separators.
	filename = QDir::toNativeSeparators(filename);
	
	// Open the ROM file.
	return openRom(filename);
}


/**
 * openRom(): Open a ROM file.
 * @param filename ROM filename. (Must have native separators!)
 * @param z_filename Internal archive filename. (If not specified, user will be prompted.)
 * @return 0 on success; non-zero on error.
 */
int EmuManager::openRom(const QString& filename, QString z_filename)
{
	// Open the file using the LibGens::Rom class.
	// TODO: This won't work for KIO...
	LibGens::Rom *rom = new LibGens::Rom(filename.toUtf8().constData());
	if (!rom->isOpen())
	{
		// Couldn't open the ROM file.
		fprintf(stderr, "Error opening ROM file. (TODO: Get error information.)\n");
		delete rom;
		return -2;
	}
	
	// Check if this is a multi-file ROM archive.
	if (rom->isMultiFile())
	{
		// Multi-file ROM archive.
		const mdp_z_entry_t *z_entry;
		
		if (z_filename.isEmpty())
		{
			// Prompt the user to select a file.
			ZipSelectDialog *zipsel = new ZipSelectDialog();
			zipsel->setFileList(rom->get_z_entry_list());
			int ret = zipsel->exec();
			if (ret != QDialog::Accepted || zipsel->selectedFile() == NULL)
			{
				// Dialog was rejected.
				delete rom;
				delete zipsel;
				return -3;
			}
			
			// Get the selected file.
			z_entry = zipsel->selectedFile();
			delete zipsel;
		}
		else
		{
			// Search for the specified filename in the z_entry list.
			z_entry = rom->get_z_entry_list();
			for (; z_entry; z_entry = z_entry->next)
			{
				const QString z_entry_filename = QString::fromUtf8(z_entry->filename);
				if (!z_filename.compare(z_entry_filename, FILENAME_CASE_SENSITIVE))
					break;
			}
			
			if (!z_entry)
			{
				// z_filename not found.
				// TODO: Show an error message.
				fprintf(stderr, "Error opening ROM file: z_filename not found.\n");
				delete rom;
				return -4;
			}
		}
		
		// Get the selected file.
		z_filename = QString::fromUtf8(z_entry->filename);
		rom->select_z_entry(z_entry);
	}
	
	// Add the Load ROM request to the emulation queue.
	EmuRequest_t rq;
	rq.rqType = EmuRequest_t::RQT_LOAD_ROM;
	rq.loadRom.rom = rom;
	rq.loadRom.filename = new QString(filename);
	rq.loadRom.z_filename = new QString(z_filename);
	m_qEmuRequest.enqueue(rq);
	
	// If there's no ROM running or emulation is paused,
	// process the Load ROM request immediately.
	if (!m_rom || m_paused.data)
		processQEmuRequest();
	
	// Load ROM request has been queued.
	return 0;
}


/**
 * GetLgRegionCode(): Determine the LibGens region code to use.
 * @param confRegionCode Current GensConfig region code.
 * @param mdHexRegionCode ROM region code, in MD hex format.
 * @param regionCodeOrder Region code order for auto-detection. (MSN == highest priority)
 * @return LibGens region code to use.
 */
LibGens::SysVersion::RegionCode_t EmuManager::GetLgRegionCode(
		GensConfig::ConfRegionCode_t confRegionCode,
		int mdHexRegionCode, uint16_t regionCodeOrder)
{
	// TODO: Consolidate LibGens::SysVersion::RegionCode_t and GensConfig::ConfRegion_t.
	LibGens::SysVersion::RegionCode_t lg_region;
	
	switch (confRegionCode)
	{
		case GensConfig::CONFREGION_JP_NTSC:
			lg_region = LibGens::SysVersion::REGION_JP_NTSC;
			break;
		case GensConfig::CONFREGION_ASIA_PAL:
			lg_region = LibGens::SysVersion::REGION_ASIA_PAL;
			break;
		case GensConfig::CONFREGION_US_NTSC:
			lg_region = LibGens::SysVersion::REGION_US_NTSC;
			break;
		case GensConfig::CONFREGION_EU_PAL:
			lg_region = LibGens::SysVersion::REGION_EU_PAL;
			break;
		case GensConfig::CONFREGION_AUTODETECT:
		default:
		{
			// Use the detected region from the ROM image.
			int regionMatch = 0;
			int orderTmp = regionCodeOrder;
			for (int i = 0; i < 4; i++, orderTmp <<= 4)
			{
				int orderN = ((orderTmp >> 12) & 0xF);
				if (mdHexRegionCode & orderN)
				{
					// Found a match.
					regionMatch = orderN;
					break;
				}
			}
			
			if (regionMatch == 0)
			{
				// No region matched.
				// Use the highest-priority region.
				regionMatch = ((regionCodeOrder >> 12) & 0xF);
			}
			
			switch (regionMatch & 0xF)
			{
				default:
				case 0x4:	lg_region = LibGens::SysVersion::REGION_US_NTSC; break;
				case 0x8:	lg_region = LibGens::SysVersion::REGION_EU_PAL; break;
				case 0x1:	lg_region = LibGens::SysVersion::REGION_JP_NTSC; break;
				case 0x2:	lg_region = LibGens::SysVersion::REGION_ASIA_PAL; break;
			}
		}
	}
	
	// Return the region code.
	return lg_region;
}


/**
 * closeRom(): Close the open ROM file and stop emulation.
 * @param emitStateChanged If true, emits the stateChanged() signal after the ROM is closed.
 */
int EmuManager::closeRom(bool emitStateChanged)
{
	if (!isRomOpen())
		return 0;
	
	if (gqt4_emuThread)
	{
		// Disconnect the emuThread's signals.
		gqt4_emuThread->disconnect();
		
		// Stop and delete the emulation thread.
		gqt4_emuThread->stop();
		gqt4_emuThread->wait();
		delete gqt4_emuThread;
		gqt4_emuThread = NULL;
	}
	
	if (gqt4_emuContext)
	{
		// Make sure SRam/EEPRom data is saved.
		// (SaveData() will call the LibGens OSD handler if necessary.)
		gqt4_emuContext->saveData();
		
		// Delete the emulation context.
		delete gqt4_emuContext;
		gqt4_emuContext = NULL;
		
		// Delete the Rom instance.
		// TODO: Handle this in gqt4_emuContext.
		delete m_rom;
		m_rom = NULL;
	}
	
	// Close audio.
	m_audio->close();
	m_paused.data = 0;
	
	// Only clear the screen if we're emitting stateChanged().
	// If we're not emitting stateChanged(), this usually means
	// we're loading a new ROM immediately afterwards, so
	// clearing the screen is a waste of time.
	if (emitStateChanged)
	{
		if (gqt4_config->introStyle() == 0)
		{
			// Intro Effect is disabled.
			// Clear the screen.
			LibGens::VdpIo::Reset();
			emit updateVideo();
		}
		
		// Update the Gens title.
		emit stateChanged();
	}
	
	return 0;
}


/**
 * romName(): Get the ROM name.
 * @return ROM name, or empty string if no ROM is loaded.
 */
QString EmuManager::romName(void)
{
	if (!m_rom)
		return QString();
	
	// TODO: This is MD/MCD/32X only!
	
	// Check the active system region.
	const char *s_romName;
	if (LibGens::M68K_Mem::ms_SysVersion.isEast())
	{
		// East (JP). Return the domestic ROM name.
		s_romName = m_rom->romNameJP();
		if (!s_romName || s_romName[0] == 0x00)
		{
			// Domestic ROM name is empty.
			// Return the overseas ROM name.
			s_romName = m_rom->romNameUS();
		}
	}
	else
	{
		// West (US/EU). Return the overseas ROM name.
		s_romName = m_rom->romNameUS();
		if (!s_romName || s_romName[0] == 0x00)
		{
			// Overseas ROM name is empty.
			// Return the domestic ROM name.
			s_romName = m_rom->romNameJP();
		}
	}
	
	// Return the ROM name.
	if (!s_romName)
		return QString();
	return QString::fromUtf8(s_romName);
}


/**
 * sysName(): Get the system name for the active ROM, based on ROM region.
 * @return System name, or empty string if unknown or no ROM is loaded.
 */
QString EmuManager::sysName(void)
{
	if (!m_rom)
		return QString();
	
	return SysName(m_rom->sysId(), LibGens::M68K_Mem::ms_SysVersion.region());
}


/**
 * getSaveStateFilename(): Get the savestate filename.
 * TODO: Move savestate code to another file?
 * @return Savestate filename, or empty string if no ROM is loaded.
 */
QString EmuManager::getSaveStateFilename(void)
{
	if (!m_rom)
		return QString();
	
	const QString filename =
		gqt4_config->userPath(GensConfig::GCPATH_SAVESTATES) +
		QString::fromUtf8(m_rom->filenameBaseNoExt()) +
		QChar(L'.') + QString::number(m_saveSlot) +
		QLatin1String(".zomg");
	return filename;
}


/**
 * emuFrameDone(): Emulation thread is finished rendering a frame.
 * @param wasFastFrame The frame was rendered "fast", i.e. no VDP updates.
 */
void EmuManager::emuFrameDone(bool wasFastFrame)
{
	// Make sure the emulation thread is still running.
	if (!gqt4_emuThread || gqt4_emuThread->isStopRequested())
		return;
	
	if (!wasFastFrame)
		m_frames++;
	
	double timeDiff;
	double thisTime = LibGens::Timing::GetTimeD();
	
	if (m_lastTime < 0.001)
	{
		m_lastTime = thisTime;
		m_lastTime_fps = m_lastTime;
		timeDiff = 0.0;
	}
	else
	{
		// Get the time difference.
		timeDiff = (thisTime - m_lastTime);
		
		// Check the FPS counter.
		double timeDiff_fps = (thisTime - m_lastTime_fps);
		if (timeDiff_fps >= 0.250)
		{
			// Push the current fps.
			// (Updated four times per second.)
			double fps = ((double)m_frames / timeDiff_fps);
			emit updateFps(fps);
			
			// Reset the timer and frame counter.
			m_lastTime_fps = thisTime;
			m_frames = 0;
		}
	}
	
	// Check for SRam/EEPRom autosave.
	// TODO: Frames elapsed.
	gqt4_emuContext->autoSaveData(1);
	
	// Check for requests in the emulation queue.
	if (!m_qEmuRequest.isEmpty())
		processQEmuRequest();
	
	// Check for controller configuration updates.
	if (gqt4_config->m_ctrlConfig->isDirty())
	{
		// Update the controller ports.
		gqt4_config->m_ctrlConfig->updateSysPort(&gqt4_emuContext->m_port1, CtrlConfig::PORT_1);
		gqt4_config->m_ctrlConfig->updateSysPort(&gqt4_emuContext->m_port2, CtrlConfig::PORT_2);
		gqt4_config->m_ctrlConfig->clearDirty();
	}
	
	// Update the GensQGLWidget.
	if (!wasFastFrame)
		emit updateVideo();
	
	// If emulation is paused, don't resume the emulation thread.
	if (m_paused.data)
		return;
	
	/** Auto Frame Skip **/
	// TODO: Figure out how to properly implement the old Gens method of synchronizing to audio.
#if 0
	// Check if we should do a fast frame.
	// TODO: Audio stutters a bit if the video drops below 60.0 fps.
	m_audio->wpSegWait();	// Wait for the buffer to empty.
	m_audio->write();	// Write audio.
	bool doFastFrame = !m_audio->isBufferEmpty();
#else
	// TODO: Remove the ring buffer and just use the classic SDL-esque method.
	m_audio->write();	// Write audio.
#endif
	
	// Check if we're higher or lower than the required framerate.
	bool doFastFrame = false;
	const double frameRate = (1.0 / (LibGens::M68K_Mem::ms_SysVersion.isPal() ? 50.0 : 60.0));
	const double threshold = 0.001;
	if (timeDiff > (frameRate + threshold))
	{
		// Lower than the required framerate.
		// Do a fast frame.
		//printf("doing fast frame; ");
		doFastFrame = true;
	}
	else if (timeDiff < (frameRate - threshold))
	{
		// Higher than the required framerate.
		// Wait for the required amount of time.
		do
		{
			thisTime = LibGens::Timing::GetTimeD();
			timeDiff = (thisTime - m_lastTime);
		} while (timeDiff < frameRate);
		
		// TODO: This causes some issues...
		if (timeDiff > (frameRate + threshold))
			doFastFrame = true;
	}
	// Update the last time value.
	m_lastTime = thisTime;
	
	// Tell the emulation thread that we're ready for another frame.
	if (gqt4_emuThread)
		gqt4_emuThread->resume(doFastFrame);
}

}
