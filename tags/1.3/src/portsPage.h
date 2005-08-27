/***************************************************************************
 *   Copyright (C) 2005 by Fred Emmott                                     *
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

#ifndef PORTSPAGE_H
#define PORTSPAGE_H

#include <qframe.h>
#include <qstring.h>
#include <qvaluevector.h>

#include <klistview.h>
#include <kpushbutton.h>

#include "newServiceDialog.h"
#include "kiptg.h"

namespace kiptg
{
	class portsPage : public QFrame
	{
		Q_OBJECT
		private:
  		newServiceDialog *m_newServiceDialog;
  		KListView *m_services;
  		
  		KPushButton *m_addService;
  		KPushButton *m_delService;
  		KPushButton *m_serviceUp;
  		KPushButton *m_serviceDown;
  		
		public:
			portsPage(QString text, QWidget* parent);
			QValueVector<struct Service> getServices();
    private slots:
    	void slotServicesChanged();
    	void slotAddService();
    	void slotMoveUp();
    	void slotMoveDown();
    	void slotDelService();
	};
}

#endif
