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

#include "portsPage.h"

#include <netdb.h>
#include <netinet/in.h>

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <klocale.h>
#include <kseparator.h>

namespace kiptg
{
	QValueVector<struct Service> portsPage::getServices()
	{
		QValueVector<struct Service> services;
		QListViewItem *item = m_services->firstChild();
		while (item)
		{
    	struct Service _item;
    	_item.portNumber = item->text(0);
    	_item.protocol = item->text(1);
    	_item.action = item->text(2);
    	_item.portName = item->text(3);
			services.append(_item);
			item = item->nextSibling();
		}
		return services;
	}
	
	void portsPage::slotDelService()
	{
    QListViewItem *sel = m_services->selectedItem();
    if (sel) delete sel;
	}
	
	void portsPage::slotMoveDown()
	{
    QListViewItem *sel = m_services->selectedItem(), *ibelow = 0;
    
    if (sel)
      ibelow=sel->itemBelow(); // Only check itemAbove() if it exists...
  
    if (sel  && ibelow)
      sel->moveItem(ibelow); //Move the Item
	}
	
	void portsPage::slotMoveUp()
	{
    QListViewItem *sel = m_services->selectedItem(), *iabove = 0;
    
    if (sel)
      iabove=sel->itemAbove(); // Only check itemAbove() if it exists...
  
    if (sel  && iabove)
      iabove->moveItem(sel); //Move the Item
  }
  
  void portsPage::slotAddService()
  {
    QString protoName = m_serviceProtocols->currentText();
    QString portName, portNumber;
    QString action;
    m_serviceAccept->isChecked()
      ? action = i18n("Allow")
      : action = i18n("Deny");
    
    if (protoName != i18n("ICMP"))
    {
      if (m_serviceNamed->isChecked())
      {
        portName = m_serviceNames->currentText();
        struct servent *service;
        service = getservbyname(portName.lower().ascii(), NULL);
        portNumber = QString::number(ntohs(service->s_port));
      }
      else
      {
        portName = "";
        portNumber = m_serviceNumbers->text();
      }
    }
    else
    {
      portNumber = "";
      portName = m_serviceNames->currentText().stripWhiteSpace();
    }
    
    QListViewItem *item = new QListViewItem(m_services,
      portNumber,
      protoName,
      action,
      portName);
    item = 0; // stop unused variable warnings
  }

	portsPage::portsPage(QString text, QWidget *parent) : QFrame(parent)
	{
		setupNewServiceDialog();
		
    QGridLayout *layout = new QGridLayout(this, 6, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    QLabel *intro = new QLabel(text, this);
    intro->show();
    layout->addMultiCellWidget(intro, 0, 0, 0, 1);
    
    m_services = new KListView(this);
    m_services->addColumn(i18n("Port"));
    m_services->addColumn(i18n("Protocol"));
    m_services->addColumn(i18n("Action"));
    m_services->addColumn(i18n("Service"));
    m_services->setSorting(-1);
    m_services->show();
    layout->addMultiCellWidget(m_services, 1, 5, 0, 0);
    
    m_addService = new KPushButton(i18n("A&dd Service"), this);
    m_addService->show();
    layout->addWidget(m_addService, 1, 1);
    connect(m_addService, SIGNAL(clicked()), this, SLOT(slotShowServiceDialog()));
    
    m_delService = new KPushButton(i18n("&Remove Service"), this);
    m_delService->show();
    layout->addWidget(m_delService, 2, 1);
    connect(m_delService, SIGNAL(clicked()), this, SLOT(slotDelService()));
    
    m_serviceUp = new KPushButton(i18n("&Move Up"), this);
    m_serviceUp->show();
    layout->addWidget(m_serviceUp, 3, 1);
    connect(m_serviceUp, SIGNAL(clicked()), this, SLOT(slotMoveUp()));
    
    m_serviceDown = new KPushButton(i18n("M&ove Down"), this);
    m_serviceDown->show();
    layout->addWidget(m_serviceDown, 4, 1);
    connect(m_serviceDown, SIGNAL(clicked()), this, SLOT(slotMoveDown()));
    
    layout->setColStretch(0, 1);
	}
	
  void portsPage::slotShowServiceDialog()
  {
    // Reset dialog
    m_serviceProtocols->setCurrentItem(0);
    slotChangedProtocol(0);
    m_serviceNamed->setChecked(true);
    m_serviceNames->setCurrentItem(0);
    m_serviceNumbers->setText("");
    m_serviceAccept->setChecked(true);
    slotServiceNamedChanged(true);
  
    m_newServiceDialog->show();
  }

  void portsPage::slotServiceNamedChanged(bool named)
  {
    m_serviceNames->setEnabled(named);
    m_serviceNumbers->setEnabled(!named);
  }

	void portsPage::setupNewServiceDialog()
	{
    m_newServiceDialog = new KDialogBase(this, 0, true, i18n("Add Service"), KDialogBase::Ok | KDialogBase::Cancel);
    
    QFrame *dialogArea = new QFrame(m_newServiceDialog);
    QGridLayout *layout = new QGridLayout(dialogArea, 7, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    KActiveLabel *intro = new KActiveLabel(i18n(
      "<p><i>Advanced users only</i></p>"
      "<p>Here you can allow or deny access to services through your firewall.<br />"
      "You can specify a port range in the box using this format: <tt>fromPort:toPort</tt>, "
      "or you can specify multiple ports by seperating them with commas.</p>"), dialogArea);
    intro->show();
    layout->addMultiCellWidget(intro, 0, 0, 0, 1);
    
    QLabel *protocolLabel = new QLabel(i18n("&Protocol: "), dialogArea);
    protocolLabel->show();
    layout->addWidget(protocolLabel, 1, 0);
  
    m_serviceProtocols = new KComboBox(dialogArea);
    m_serviceProtocols->insertItem(i18n("TCP"));
    m_serviceProtocols->insertItem(i18n("UDP"));
    m_serviceProtocols->insertItem(i18n("TCP & UDP"));
    m_serviceProtocols->insertItem(i18n("ICMP"));
    m_serviceProtocols->setCurrentItem(2);
    m_serviceProtocols->show();
    layout->addWidget(m_serviceProtocols, 1, 1);
    protocolLabel->setBuddy(m_serviceProtocols);
    connect(m_serviceProtocols, SIGNAL(activated(int )), this, SLOT(slotChangedProtocol(int )));
    
    QLabel *selectByLabel = new QLabel(i18n("Select service by: "), dialogArea);
    selectByLabel->show();
    layout->addMultiCellWidget(selectByLabel, 2, 2, 0, 1);
    
    QButtonGroup *optNamedOrNumbered = new QButtonGroup(dialogArea);
    optNamedOrNumbered->hide();
    
    m_serviceNamed = new QRadioButton(i18n("&Name: "), dialogArea);
    m_serviceNamed->setChecked(true);
    m_serviceNamed->setName("named");
    optNamedOrNumbered->insert(m_serviceNamed);
    m_serviceNamed->show();
    layout->addWidget(m_serviceNamed, 3, 0);
    connect(m_serviceNamed, SIGNAL(toggled(bool )), this, SLOT(slotServiceNamedChanged(bool )));
    
    m_serviceNames = new KComboBox(dialogArea);
    m_serviceNames->show();
    layout->addWidget(m_serviceNames, 3, 1);
    
    m_serviceNumbered = new QRadioButton(i18n("&Port number(s): "), dialogArea);
    m_serviceNumbered->setName("numbered");
    optNamedOrNumbered->insert(m_serviceNumbered);
    m_serviceNumbered->show();
    layout->addWidget(m_serviceNumbered, 4, 0);
    
    m_serviceNumbers = new KLineEdit(dialogArea);
    m_serviceNumbers->show();
    layout->addWidget(m_serviceNumbers, 4, 1);
    
    QButtonGroup *optAllowDeny = new QButtonGroup(dialogArea);
    optAllowDeny->hide();
    
    KSeparator *separator = new KSeparator(dialogArea);
    separator->show();
    layout->addMultiCellWidget(separator, 5, 5, 0, 1);
    
    m_serviceAccept = new QRadioButton(i18n("&Accept"), dialogArea);
    m_serviceAccept->setName(i18n("Accept"));
    m_serviceAccept->setChecked(true);
    m_serviceAccept->show();
    optAllowDeny->insert(m_serviceAccept);
    layout->addWidget(m_serviceAccept, 6, 0);
    
    m_serviceDrop = new QRadioButton(i18n("&Drop"), dialogArea);
    m_serviceDrop->setName(i18n("Drop"));
    m_serviceDrop->show();
    optAllowDeny->insert(m_serviceDrop);
    layout->addWidget(m_serviceDrop, 6, 1);
      
    dialogArea->show();
    m_newServiceDialog->setMainWidget(dialogArea);
    connect(m_newServiceDialog, SIGNAL(okClicked()), this, SLOT(slotAddService()));
    slotChangedProtocol(2); // TCP+UDP
	}
	
  void portsPage::slotChangedProtocol(int newProtocol)
  {
    // 0 = TCP
    // 1 = UDP
    // 2 = TCP+UDP
    // 3 = ICMP
    m_serviceNames->clear();
    if (newProtocol != 3)
    {
      m_serviceNames->insertItem("SSH");
      m_serviceNames->insertItem("Telnet");
      m_serviceNames->insertItem("HTTP");
      m_serviceNames->insertItem("HTTPS");
      m_serviceNames->insertItem("SMTP");
      m_serviceNames->insertItem("SMTPS");
      m_serviceNames->insertItem("POP3");
      m_serviceNames->insertItem("POP3S");
      m_serviceNames->insertItem("IMAP");
      m_serviceNames->insertItem("IMAPS");
      m_serviceNumbered->setEnabled(true);
      if ( m_serviceNumbered->isChecked() )
      	m_serviceNumbers->setEnabled(true);
      return;
    }
    m_serviceNumbered->setEnabled(false);
    m_serviceNumbers->setEnabled(false);
    m_serviceNamed->setChecked(true);
    m_serviceNames->insertItem("any");
    m_serviceNames->insertItem("echo-reply");
    m_serviceNames->insertItem("destination-unreachable");
    m_serviceNames->insertItem(" network-unreachable");
    m_serviceNames->insertItem(" host-unreachable");
    m_serviceNames->insertItem(" protocol-unreachable");
    m_serviceNames->insertItem(" port-unreachable");
    m_serviceNames->insertItem(" fragmentation-needed");
    m_serviceNames->insertItem(" source-route-failed");
    m_serviceNames->insertItem(" network-unknown");
    m_serviceNames->insertItem(" host-unknown");
    m_serviceNames->insertItem(" network-prohibited");
    m_serviceNames->insertItem(" host-prohibited");
    m_serviceNames->insertItem(" TOS-network-unreachable");
    m_serviceNames->insertItem(" TOS-host-unreachable");
    m_serviceNames->insertItem(" communication-prohibited");
    m_serviceNames->insertItem(" host-precedence-violation");
    m_serviceNames->insertItem(" precedence-cutoff");
    m_serviceNames->insertItem("source-quench");
    m_serviceNames->insertItem("redirect");
    m_serviceNames->insertItem(" network-redirect");
    m_serviceNames->insertItem(" host-redirect");
    m_serviceNames->insertItem(" TOS-network-redirect");
    m_serviceNames->insertItem(" TOS-host-redirect");
    m_serviceNames->insertItem("echo-request");
    m_serviceNames->insertItem("router-advertisement");
    m_serviceNames->insertItem("router-solicitation");
    m_serviceNames->insertItem("time-exceeded");
    m_serviceNames->insertItem(" ttl-zero-during-transit");
    m_serviceNames->insertItem(" ttl-zero-during-reassembly");
    m_serviceNames->insertItem("parameter-problem");
    m_serviceNames->insertItem(" ip-header-bad");
    m_serviceNames->insertItem(" required-option-missing");
    m_serviceNames->insertItem("timestamp-request");
    m_serviceNames->insertItem("timestamp-reply");
    m_serviceNames->insertItem("address-mask-request");
    m_serviceNames->insertItem("address-mask-reply");
  }
}
