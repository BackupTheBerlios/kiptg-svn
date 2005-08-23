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

#include "newHostDialog.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <klocale.h>

namespace kiptg
{
	void newHostDialog::show()
	{
		m_allow->setChecked(true);
		m_useIP->setChecked(true);
		m_address->setText("");
		KDialogBase::show();
	}
	
	newHostDialog::newHostDialog(QString text, QWidget *parent)
		: KDialogBase(parent, 0, true, i18n("Add Host"), KDialogBase::Ok | KDialogBase::Cancel)
	{
    QFrame *dialogArea = new QFrame(this);
    QGridLayout *layout = new QGridLayout(dialogArea, 7, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    KActiveLabel *intro = new KActiveLabel(text, dialogArea);
    intro->show();
    layout->addMultiCellWidget(intro, 0, 0, 0, 1);
  
    QLabel *label = new QLabel(i18n(
      "<p>Do you wish to specify the host by IP address or MAC (hardware) address?</p>"), dialogArea);
    label->show();
    layout->addMultiCellWidget(label, 1, 1, 0, 1);
  
    QButtonGroup *ipOrMAC = new QButtonGroup(dialogArea);
    ipOrMAC->hide();
    
    m_useIP = new QRadioButton(i18n("&Use IP"), dialogArea);
    m_useIP->setChecked(true);
    m_useIP->show();
    layout->addWidget(m_useIP, 2, 0);
    ipOrMAC->insert(m_useIP);
    
    m_useMAC = new QRadioButton(i18n("U&se MAC"), dialogArea);
    m_useMAC->show();
    layout->addWidget(m_useMAC, 2, 1);
    ipOrMAC->insert(m_useMAC);
    
    label = new QLabel(i18n("<p>Do you want to allow connections from this host, or block them?</p>"), dialogArea);
    label->show();
    layout->addMultiCellWidget(label, 3, 3, 0, 1);
    QButtonGroup *whitelistOrBlacklist = new QButtonGroup(dialogArea);
    whitelistOrBlacklist->hide();
    
    m_allow = new QRadioButton(i18n("&Allow"), dialogArea);
    m_allow->setChecked(true);
    m_allow->show();
    layout->addWidget(m_allow, 4, 0);
    whitelistOrBlacklist->insert(m_allow);
    
    m_block = new QRadioButton(i18n("&Block"), dialogArea);
    m_block->setChecked(false);
    m_block->show();
    layout->addWidget(m_block, 4, 1);
    whitelistOrBlacklist->insert(m_block);
    
  
    KActiveLabel *hostLabel = new KActiveLabel(i18n("Enter the IP or MAC address of the host:"), dialogArea);
    hostLabel->show();
    layout->addMultiCellWidget(hostLabel, 5, 5, 0, 1);
    
    m_address = new KLineEdit(dialogArea);
    m_address->show();
    layout->addMultiCellWidget(m_address, 6, 6, 0, 1);
    
    dialogArea->show();
    this->setMainWidget(dialogArea);		
	}
	struct Host newHostDialog::getHost()
	{
		struct Host host;
		host.accept = m_allow->isChecked();
		host.useIP = m_useIP->isChecked();
		host.address = m_address->text();
		return host;
	}
}
