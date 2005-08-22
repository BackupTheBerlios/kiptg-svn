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

#ifndef CONNTRACKPAGE_H
#define CONNTRACKPAGE_H

#include <qcheckbox.h>
#include <qframe.h>

#include "constants.h"

namespace kiptg 
{
	class conntrackPage : public QFrame
	{
  Q_OBJECT
  private:
  	QCheckBox *m_allSame;
  	QCheckBox *m_allEST, *m_allREL, *m_allNEW;
  	QCheckBox *m_tcpEST, *m_tcpREL, *m_tcpNEW;
  	QCheckBox *m_udpEST, *m_udpREL, *m_udpNEW;
  	QCheckBox *m_icmpEST, *m_icmpREL, *m_icmpNEW;
  public:
  	conntrackPage::conntrackPage(QWidget *parent);
  	bool allSame();
  	int getAll(); // these return the kiptg::ctstate enum
  	int getTCP();
  	int getUDP();
  	int getICMP();
	};
}

#endif
