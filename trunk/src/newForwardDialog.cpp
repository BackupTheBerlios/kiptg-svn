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

#include "newForwardDialog.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstring.h>

#include <kactivelabel.h>
#include <klocale.h>

namespace kiptg
{
	struct Forward newForwardDialog::getForward()
	{
		struct Forward forward;
		m_incoming->isChecked()
			? forward.direction = kiptg::INCOMING
			: forward.direction = kiptg::OUTGOING;
    forward.from = m_port->text();
    forward.to = m_to->text();
    return forward;
	}
	
	void newForwardDialog::show()
	{
		m_incoming->setChecked(true);
		m_port->setText("");
		m_to->setText("");
		KDialogBase::show();
	}
	
	newForwardDialog::newForwardDialog(QString text, QWidget *parent)
		: KDialogBase(parent, 0, true, i18n("Add Forward"), KDialogBase::Ok | KDialogBase::Cancel)
  {
    QFrame *dialogArea = new QFrame(this);
    QGridLayout *layout = new QGridLayout(dialogArea, 4, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    KActiveLabel *intro = new KActiveLabel(text, dialogArea);
    intro->show();
    layout->addMultiCellWidget(intro, 0, 0, 0, 1);
    
    QButtonGroup *direction = new QButtonGroup(dialogArea);
    direction->hide();
    
    m_incoming = new QRadioButton(i18n("Incoming"), dialogArea);
    m_incoming->setChecked(true);
    m_incoming->show();
    layout->addWidget(m_incoming, 1, 0);
    direction->insert(m_incoming);
    
    m_outgoing = new QRadioButton(i18n("Outgoing"), dialogArea);
    m_outgoing->show();
    layout->addWidget(m_outgoing, 1, 1);
    direction->insert(m_outgoing);
    
    QLabel *label = new QLabel(i18n("Port:"), dialogArea);
    label->show();
    layout->addWidget(label, 2, 0);
    
    m_port = new KLineEdit(dialogArea);
    m_port->show();
    layout->addWidget(m_port, 2, 1);
    
    label = new QLabel(i18n("Destination:"), dialogArea);
    label->show();
    layout->addWidget(label, 3, 0);
    
    m_to = new KLineEdit(dialogArea);
    m_to->show();
    layout->addWidget(m_to, 3, 1);
  
    dialogArea->show();
    this->setMainWidget(dialogArea);
  };
}
