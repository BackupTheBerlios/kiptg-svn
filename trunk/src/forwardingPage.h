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

#ifndef FORWARDINGPAGE_H
#define FORWARDINGPAGE_H

#include <qframe.h>
#include <qradiobutton.h>
#include <qstring.h>
#include <qvaluevector.h>

#include <kdialogbase.h>
#include <klineedit.h>
#include <klistview.h>
#include <kpushbutton.h>

#include "constants.h"

namespace kiptg
{
	struct Forward
	{
		int direction; // contains kiptg::direction enum
		QString from;
		QString to;
  };
	class forwardingPage : public QFrame
	{
		Q_OBJECT
		private:
			void setupNewForwardDialog(QString text);
			
			KListView *m_forwards;
			
			KDialogBase *m_newForwardDialog;
			
			QRadioButton *m_forwardIncoming;
			QRadioButton *m_forwardOutgoing;
			
			KPushButton *m_add;
			KPushButton *m_del;
			
			KLineEdit *m_forwardPort;
			KLineEdit *m_forwardTo;
    private slots:
    	void slotSelectionChanged();
    	void slotShowForwardDialog();
    	void slotAdd();
    	void slotDel();
    public:
    	forwardingPage(QString text, QString newText, QWidget *parent);
    	QValueVector<struct Forward> getForwards();
  };
}

#endif
