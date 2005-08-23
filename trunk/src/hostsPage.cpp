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

#include "hostsPage.h"

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <klocale.h>
#include <kpushbutton.h>

namespace kiptg
{
	void hostsPage::slotSelectionChanged()
	{
		QListViewItem *item = m_hosts->selectedItem();
		if (item)
			m_del->setEnabled(true);
    else
    	m_del->setEnabled(false);
	}
	
	QValueVector<struct Host> hostsPage::getHosts()
	{
  	QValueVector<struct Host> hosts;
  	QListViewItem* item = m_hosts->firstChild();
  	while (item)
  	{
  		struct Host _item;
  		item->text(0) == i18n("Allow")
  			? _item.accept = true
  			: _item.accept = false;
      item->text(1) == i18n("IP")
      	? _item.useIP = true
      	: _item.useIP = false;
      _item.address = item->text(2);
      hosts.append(_item);
			item = item->nextSibling();
    }
    return hosts;
	}
	
	hostsPage::hostsPage(QString text, QString newHostText, QWidget *parent) : QFrame(parent)
	{
		m_newHostDialog = new newHostDialog(newHostText, this);
    connect(m_newHostDialog, SIGNAL(okClicked()), this, SLOT(slotAdd()));
		
    QGridLayout *layout = new QGridLayout(this, 4, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    KActiveLabel *label = new KActiveLabel(text, this);
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    label->show();
    layout->addMultiCellWidget(label, 0, 0, 0, 1);
    
    m_hosts = new KListView(this);
    m_hosts->addColumn(i18n("Allow/Block"));
    m_hosts->addColumn(i18n("MAC/IP"));
    m_hosts->addColumn(i18n("Host"));
    m_hosts->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
    m_hosts->show();
    layout->addMultiCellWidget(m_hosts, 1, 3, 0, 0);
    connect(m_hosts, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged())); 
    
    m_add = new KPushButton(i18n("Add Host"), this);
    m_add->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_add->show();
    layout->addWidget(m_add, 1, 1);
    connect(m_add, SIGNAL(clicked()), this, SLOT(slotShowHostDialog()));
    
    m_del = new KPushButton(i18n("Remove Host"), this);
    m_del->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_del->setEnabled(false);
    m_del->show();
    layout->addWidget(m_del, 2, 1);
    connect(m_del, SIGNAL(clicked()), this, SLOT(slotDel()));
    
    layout->addItem(new QSpacerItem(QSizePolicy::Minimum, QSizePolicy::Ignored), 3, 1);
    layout->setColStretch(0, 1);
	}
	
	void hostsPage::slotShowHostDialog()
	{
    m_newHostDialog->show();
	}
	
	void hostsPage::slotDel()
	{
    QListViewItem *sel = m_hosts->selectedItem();
    if (sel) delete sel;
	}
	
	void hostsPage::slotAdd()
	{
		struct Host host = m_newHostDialog->getHost();
    QString allowOrBlock, ipOrMAC;
    host.accept
      ? allowOrBlock = i18n("Allow")
      : allowOrBlock = i18n("Block");
    host.useIP
      ? ipOrMAC = i18n("IP")
      : ipOrMAC = i18n("MAC");
    
    KListViewItem *item = new KListViewItem(m_hosts,
      allowOrBlock,
      ipOrMAC,
      host.address);
    item = 0; //unused variable warnings  
	}
}
