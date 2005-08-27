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

#include "interfacesPage.h"

#include <net/if.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qstring.h>

#include <kdialogbase.h>
#include <kinputdialog.h>
#include <klistbox.h>
#include <klocale.h>
#include <kpushbutton.h>

namespace kiptg
{
	interfacesPage::interfacesPage(QWidget* parent) : QFrame (parent)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->setSpacing(KDialogBase::spacingHint());
		
    QLabel *intro = new QLabel(i18n(
      "<p>Please select which of the following network interfaces do you want to filter.</p>"), this);
    intro->show();
    layout->addWidget(intro);
    
    m_interfaces = new KListBox(this);
  
    struct if_nameindex *interfaces = if_nameindex();
    while (true)
    {
    	static int i = -1;
    	if (interfaces[++i].if_index != 0)
      {
      	if ((QString) interfaces[i].if_name != "lo")
      		m_interfaces->insertItem(interfaces[i].if_name);
      }
      else
      	break;
    }
    if_freenameindex(interfaces);
    
    m_interfaces->setSelectionMode(QListBox::Multi);
    for (unsigned short i = 0; i < m_interfaces->count(); i++)
      m_interfaces->setSelected(i, true);
    m_interfaces->show();
    layout->addWidget(m_interfaces);
    
    KPushButton *newInterface = new KPushButton(i18n("A&dd Interface..."), this);
    newInterface->show();
    layout->addWidget(newInterface);
    connect(newInterface, SIGNAL(clicked()), this, SLOT(slotNewInterface()));
	}
	
  void interfacesPage::slotNewInterface()
  {
    QString interface;
    
    interface = KInputDialog::getText(i18n("Add Interface"),
      i18n("Enter the name of the interface, eg eth1"));
    if (interface == "")
      return;
    
    m_interfaces->insertItem(interface);
    m_interfaces->setSelected(m_interfaces->count() - 1, true);
  }
  
  QStringList interfacesPage::getWhitelistInterfaces()
  {
  	QStringList interfaces;
  	
  	for (unsigned int i = 0; i < m_interfaces->count(); i++)
  		if (! m_interfaces->item(i)->isSelected())
  			interfaces.append(m_interfaces->item(i)->text());
    
    return interfaces;
  }
}
