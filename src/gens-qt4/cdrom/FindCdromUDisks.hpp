/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * FindCdromUDisks.hpp: Find CD-ROM drives: UDisks1 version.               *
 *                                                                         *
 * Copyright (c) 2011-2015 by David Korth.                                 *
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

#ifndef __GENS_QT4_CDROM_FINDCDROMUDISKS_HPP__
#define __GENS_QT4_CDROM_FINDCDROMUDISKS_HPP__

#include "FindCdromBase.hpp"

// QtDBus includes.
#include <QtDBus/QDBusObjectPath>

namespace GensQt4 {

class FindCdromUDisksPrivate;

class FindCdromUDisks : public FindCdromBase
{
	Q_OBJECT
	
	public:
		FindCdromUDisks(QObject *parent = 0);
		virtual ~FindCdromUDisks();

	private:
		typedef FindCdromBase super;
		friend class FindCdromUDisksPrivate;
		FindCdromUDisksPrivate *const d;
	private:
		Q_DISABLE_COPY(FindCdromUDisks)

	public:
		/**
		 * Determine if this CD-ROM backend is usable.
		 * @return True if this CD-ROM backend is usable; false if not.
		 */
		bool isUsable(void) const;

		/**
		 * Scan the system for CD-ROM devices.
		 * @return QStringList with all detected CD-ROM device names.
		 */
		QStringList scanDeviceNames(void);

		// TODO: LibGensCD support for notifications.
#if 0
	private slots:
		void deviceChanged(const QDBusObjectPath& objectPath);
		void deviceRemoved(const QDBusObjectPath& objectPath);
#endif
};

}

#endif /* __GENS_QT4_CDROM_FINDCDROMUDISKS_HPP__ */
