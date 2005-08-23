/***************************************************************************
 *   Copyright (C) 2004 by Fred Emmott                                     *
 *   mail@fredemmott.co.uk                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef KIPTG_H
#define KIPTG_H

#include <qstring.h>

namespace kiptg 
{
	struct Service
	{
		QString portNumber, protocol, action, portName;
  };
	struct Host
	{
		bool accept;
		bool useIP;
		QString address;
  };
	enum direction {
		INCOMING,
		OUTGOING
  };
	enum ctstate {
		ESTABLISHED = 1,
		RELATED = 2,
		NEW = 4
  };
	enum policy {
		ACCEPT,
		DROP
  };
	enum os {
		LINUX,
		BSD
  };
	enum distros {
    GENERIC_LINUX,
    SLACKWARE,
    GENTOO,
    GENERIC_BSD,
    FREEBSD,
    NETBSD,
    OPENBSD
  };
}

#endif
