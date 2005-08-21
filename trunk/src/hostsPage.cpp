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
		setupNewHostDialog(newHostText);
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
    
    KPushButton *addHost = new KPushButton(i18n("Add..."), this);
    addHost->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    addHost->show();
    layout->addWidget(addHost, 1, 1);
    connect( addHost, SIGNAL(clicked()), this, SLOT(slotShowHostDialog()));
    
    KPushButton *delHost = new KPushButton(i18n("Remove"), this);
    delHost->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout->addWidget(delHost, 2, 1);
    connect( delHost, SIGNAL(clicked()), this, SLOT(slotDel()));
    
    layout->addItem(new QSpacerItem(QSizePolicy::Minimum, QSizePolicy::Ignored), 3, 1);
    layout->setColStretch(0, 1);
	}
	
	void hostsPage::slotShowHostDialog()
	{
    m_hostAllow->setChecked(true);
    m_hostUseIP->setChecked(true);
    m_hostAddress->setText("");
    
    m_newHostDialog->show();
	}
	
	void hostsPage::slotDel()
	{
    QListViewItem *sel = m_hosts->selectedItem();
    if (sel) delete sel;
	}
	
	void hostsPage::slotAdd()
	{
    QString allowOrBlock, ipOrMAC;
    m_hostAllow->isChecked()
      ? allowOrBlock = i18n("Allow")
      : allowOrBlock = i18n("Block");
    m_hostUseIP->isChecked()
      ? ipOrMAC = i18n("IP")
      : ipOrMAC = i18n("MAC");
    QString host = m_hostAddress->text();
    
    KListViewItem *item = new KListViewItem(m_hosts,
      allowOrBlock,
      ipOrMAC,
      host);
    item = 0; //unused variable warnings  
	}
	
	void hostsPage::setupNewHostDialog(QString text)
	{
    m_newHostDialog = new KDialogBase(this, 0, true, i18n("Add Host"), KDialogBase::Ok | KDialogBase::Cancel);
    
    QFrame *dialogArea = new QFrame(m_newHostDialog);
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
    
    m_hostUseIP = new QRadioButton(i18n("&Use IP"), dialogArea);
    m_hostUseIP->setChecked(true);
    m_hostUseIP->show();
    layout->addWidget(m_hostUseIP, 2, 0);
    ipOrMAC->insert(m_hostUseIP);
    
    m_hostUseMAC = new QRadioButton(i18n("U&se MAC"), dialogArea);
    m_hostUseMAC->show();
    layout->addWidget(m_hostUseMAC, 2, 1);
    ipOrMAC->insert(m_hostUseMAC);
    
    label = new QLabel(i18n("<p>Do you want to allow connections from this host, or block them?</p>"), dialogArea);
    label->show();
    layout->addMultiCellWidget(label, 3, 3, 0, 1);
  
    QButtonGroup *whitelistOrBlacklist = new QButtonGroup(dialogArea);
    whitelistOrBlacklist->hide();
    
    m_hostAllow = new QRadioButton(i18n("&Allow"), dialogArea);
    m_hostAllow->setChecked(true);
    m_hostAllow->show();
    layout->addWidget(m_hostAllow, 4, 0);
    whitelistOrBlacklist->insert(m_hostAllow);
    
    m_hostBlock = new QRadioButton(i18n("&Block"), dialogArea);
    m_hostBlock->setChecked(false);
    m_hostBlock->show();
    layout->addWidget(m_hostBlock, 4, 1);
    whitelistOrBlacklist->insert(m_hostBlock);
    
  
    KActiveLabel *hostLabel = new KActiveLabel(i18n("Enter the IP or MAC address of the host:"), dialogArea);
    hostLabel->show();
    layout->addMultiCellWidget(hostLabel, 5, 5, 0, 1);
    
    m_hostAddress = new KLineEdit(dialogArea);
    m_hostAddress->show();
    layout->addMultiCellWidget(m_hostAddress, 6, 6, 0, 1);
    
    connect(m_newHostDialog, SIGNAL(okClicked()), this, SLOT(slotAdd()));
    
    dialogArea->show();
    m_newHostDialog->setMainWidget(dialogArea);		
	}
}