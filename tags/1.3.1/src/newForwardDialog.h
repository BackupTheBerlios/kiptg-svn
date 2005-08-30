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

#ifndef NEWFORWARDDIALOG_H
#define NEWFORWARDDIALOG_H

#include <qradiobutton.h>
#include <qstring.h>

#include <kdialogbase.h>
#include <klineedit.h>

#include "kiptg.h"

namespace kiptg
{
	class newForwardDialog : public KDialogBase
	{
		Q_OBJECT
		private:
			QRadioButton *m_incoming;
			QRadioButton *m_outgoing;
			
			KLineEdit *m_port;
			KLineEdit *m_to;
    public:
    	newForwardDialog::newForwardDialog(QString text, QWidget *parent);
    	struct Forward getForward();
    	void show();
	};
}

#endif
