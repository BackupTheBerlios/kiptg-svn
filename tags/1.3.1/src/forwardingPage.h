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
#include <qstring.h>
#include <qvaluevector.h>

#include <klistview.h>
#include <kpushbutton.h>

#include "newForwardDialog.h"
#include "kiptg.h"

namespace kiptg
{
	class forwardingPage : public QFrame
	{
		Q_OBJECT
		private:
			
			KListView *m_forwards;
			
			KPushButton *m_add;
			KPushButton *m_del;
			newForwardDialog *m_newForwardDialog;
    private slots:
    	void slotSelectionChanged();
    	void slotAdd();
    	void slotDel();
    public:
    	forwardingPage(QString text, QString newText, QWidget *parent);
    	QValueVector<struct Forward> getForwards();
  };
}

#endif
