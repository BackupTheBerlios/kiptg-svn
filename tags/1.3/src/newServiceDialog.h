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

#ifndef NEWSERVICEDIALOG_H
#define NEWSERVICEDIALOG_H

#include <qradiobutton.h>

#include <kcombobox.h>
#include <kdialogbase.h>
#include <klineedit.h>

#include "kiptg.h"

namespace kiptg
{
	class newServiceDialog : public KDialogBase
	{
		Q_OBJECT
		private:
			KComboBox *m_protocols;
			
			QRadioButton *m_named;
			QRadioButton *m_numbered;
			
			KComboBox *m_name;
			KLineEdit *m_number;
			
			QRadioButton *m_accept;
			QRadioButton *m_drop;
    public:
    	newServiceDialog(QWidget *parent);
    	struct Service getService();
    	void show();
    private slots:
    	void slotChangedProtocol(int newProtocol);
      void slotNamedChanged(bool);
	};
}

#endif
