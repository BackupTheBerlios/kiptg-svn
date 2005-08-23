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

#include "newServiceDialog.h"

#include <netdb.h>

#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <klocale.h>
#include <kseparator.h>

namespace kiptg
{
	struct Service newServiceDialog::getService()
	{
		struct Service service;
		m_accept->isChecked()
			? service.action = i18n("Allow")
			: service.action = i18n("Deny");
    service.protocol = m_protocols->currentText();
    if (m_protocols->currentText() != i18n("ICMP"))
    {
      if (m_named->isChecked())
      {
        service.portName = m_name->currentText();
        struct servent *_service;
        _service = getservbyname(service.portName.lower().ascii(), NULL);
        service.portNumber = QString::number(ntohs(_service->s_port));
      }
      else
      {
        service.portName = "";
        service.portNumber = m_number->text();
      }
    }
    else
    {
      service.portNumber = "";
      service.portName = m_name->currentText().stripWhiteSpace();
    }
    return service;
	}
	
	void newServiceDialog::show()
	{
		m_protocols->setCurrentItem(0);
		slotChangedProtocol(0);
		m_named->setChecked(true);
		m_name->setCurrentItem(0);
		m_number->setText("");
		m_accept->setChecked(true);
		slotNamedChanged(true);
		KDialogBase::show();
	}
	
  void newServiceDialog::slotNamedChanged(bool named)
  {
    m_name->setEnabled(named);
    m_number->setEnabled(!named);
  }
  
  void newServiceDialog::slotChangedProtocol(int newProtocol)
  {
    // 0 = TCP
    // 1 = UDP
    // 2 = TCP+UDP
    // 3 = ICMP
    m_name->clear();
    if (newProtocol != 3)
    {
      m_name->insertItem("SSH");
      m_name->insertItem("Telnet");
      m_name->insertItem("HTTP");
      m_name->insertItem("HTTPS");
      m_name->insertItem("SMTP");
      m_name->insertItem("SMTPS");
      m_name->insertItem("POP3");
      m_name->insertItem("POP3S");
      m_name->insertItem("IMAP");
      m_name->insertItem("IMAPS");
      m_numbered->setEnabled(true);
      if ( m_numbered->isChecked() )
      	m_number->setEnabled(true);
      return;
    }
    m_numbered->setEnabled(false);
    m_number->setEnabled(false);
    m_named->setChecked(true);
    m_name->insertItem("any");
    m_name->insertItem("echo-reply");
    m_name->insertItem("destination-unreachable");
    m_name->insertItem(" network-unreachable");
    m_name->insertItem(" host-unreachable");
    m_name->insertItem(" protocol-unreachable");
    m_name->insertItem(" port-unreachable");
    m_name->insertItem(" fragmentation-needed");
    m_name->insertItem(" source-route-failed");
    m_name->insertItem(" network-unknown");
    m_name->insertItem(" host-unknown");
    m_name->insertItem(" network-prohibited");
    m_name->insertItem(" host-prohibited");
    m_name->insertItem(" TOS-network-unreachable");
    m_name->insertItem(" TOS-host-unreachable");
    m_name->insertItem(" communication-prohibited");
    m_name->insertItem(" host-precedence-violation");
    m_name->insertItem(" precedence-cutoff");
    m_name->insertItem("source-quench");
    m_name->insertItem("redirect");
    m_name->insertItem(" network-redirect");
    m_name->insertItem(" host-redirect");
    m_name->insertItem(" TOS-network-redirect");
    m_name->insertItem(" TOS-host-redirect");
    m_name->insertItem("echo-request");
    m_name->insertItem("router-advertisement");
    m_name->insertItem("router-solicitation");
    m_name->insertItem("time-exceeded");
    m_name->insertItem(" ttl-zero-during-transit");
    m_name->insertItem(" ttl-zero-during-reassembly");
    m_name->insertItem("parameter-problem");
    m_name->insertItem(" ip-header-bad");
    m_name->insertItem(" required-option-missing");
    m_name->insertItem("timestamp-request");
    m_name->insertItem("timestamp-reply");
    m_name->insertItem("address-mask-request");
    m_name->insertItem("address-mask-reply");
  }
  
	newServiceDialog::newServiceDialog(QWidget *parent) : KDialogBase(parent, 0, true, i18n("Add Service"), KDialogBase::Ok | KDialogBase::Cancel)
	{
    QFrame *dialogArea = new QFrame(this);
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
  
    m_protocols = new KComboBox(dialogArea);
    m_protocols->insertItem(i18n("TCP"));
    m_protocols->insertItem(i18n("UDP"));
    m_protocols->insertItem(i18n("TCP & UDP"));
    m_protocols->insertItem(i18n("ICMP"));
    m_protocols->setCurrentItem(2);
    m_protocols->show();
    layout->addWidget(m_protocols, 1, 1);
    protocolLabel->setBuddy(m_protocols);
    connect(m_protocols, SIGNAL(activated(int )), this, SLOT(slotChangedProtocol(int )));
    
    QLabel *selectByLabel = new QLabel(i18n("Select service by: "), dialogArea);
    selectByLabel->show();
    layout->addMultiCellWidget(selectByLabel, 2, 2, 0, 1);
    
    QButtonGroup *optNamedOrNumbered = new QButtonGroup(dialogArea);
    optNamedOrNumbered->hide();
    
    m_named = new QRadioButton(i18n("&Name: "), dialogArea);
    m_named->setChecked(true);
    m_named->setName("named");
    optNamedOrNumbered->insert(m_named);
    m_named->show();
    layout->addWidget(m_named, 3, 0);
    connect(m_named, SIGNAL(toggled(bool )), this, SLOT(slotNamedChanged(bool )));
    
    m_name = new KComboBox(dialogArea);
    m_name->show();
    layout->addWidget(m_name, 3, 1);
    
    m_numbered = new QRadioButton(i18n("&Port number(s): "), dialogArea);
    m_numbered->setName("numbered");
    optNamedOrNumbered->insert(m_numbered);
    m_numbered->show();
    layout->addWidget(m_numbered, 4, 0);
    
    m_number = new KLineEdit(dialogArea);
    m_number->show();
    layout->addWidget(m_number, 4, 1);
    
    QButtonGroup *optAllowDeny = new QButtonGroup(dialogArea);
    optAllowDeny->hide();
    
    KSeparator *separator = new KSeparator(dialogArea);
    separator->show();
    layout->addMultiCellWidget(separator, 5, 5, 0, 1);
    
    m_accept = new QRadioButton(i18n("&Accept"), dialogArea);
    m_accept->setName(i18n("Accept"));
    m_accept->setChecked(true);
    m_accept->show();
    optAllowDeny->insert(m_accept);
    layout->addWidget(m_accept, 6, 0);
    
    m_drop = new QRadioButton(i18n("&Drop"), dialogArea);
    m_drop->setName(i18n("Drop"));
    m_drop->show();
    optAllowDeny->insert(m_drop);
    layout->addWidget(m_drop, 6, 1);
      
    dialogArea->show();
    this->setMainWidget(dialogArea);
    slotChangedProtocol(2); // TCP + UDP
	}
}
