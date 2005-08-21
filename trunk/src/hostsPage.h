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

#ifndef HOSTSPAGE_H
#define HOSTSPAGE_H

#include <qframe.h>
#include <qradiobutton.h>
#include <qstring.h>
#include <qvaluevector.h>

#include <kdialogbase.h>
#include <klineedit.h>
#include <klistview.h>

namespace kiptg
{
	struct Host
	{
		bool accept;
		bool useIP;
		QString address;
  };
	class hostsPage : public QFrame
	{
  	Q_OBJECT
  	private:
  		void setupNewHostDialog(QString text);
  		
  		KListView *m_hosts;
  		
  		KDialogBase *m_newHostDialog;
  		
  		QRadioButton *m_hostUseIP;
  		QRadioButton *m_hostUseMAC;
  		QRadioButton *m_hostAllow;
  		QRadioButton *m_hostBlock;
  		KLineEdit *m_hostAddress;
  	private slots:
  		void slotShowHostDialog();
  		void slotAdd();
  		void slotDel();
    public:
    	hostsPage(QString text, QString newHostText, QWidget *parent);
    	QValueVector<struct Host> getHosts();
	};
}

#endif
