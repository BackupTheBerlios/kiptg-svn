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

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <klocale.h>
#include <kseparator.h>

namespace kiptg
{
	void portsPage::slotServicesChanged()
	{
		QListViewItem *item = m_services->selectedItem();
		if ( item )
		{
			m_delService->setEnabled(true);
      item->itemAbove()
      	? m_serviceUp->setEnabled(true)
      	: m_serviceUp->setEnabled(false);
			item->itemBelow()
				? m_serviceDown->setEnabled(true)
				: m_serviceDown->setEnabled(false);
    }
    else
    {
    	m_delService->setEnabled(false);
    	m_serviceUp->setEnabled(false);
    	m_serviceDown->setEnabled(false);
    }
				
	}
	
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
    slotServicesChanged();
	}
	
	void portsPage::slotMoveUp()
	{
    QListViewItem *sel = m_services->selectedItem();
    
    if (sel->itemAbove())
    	sel->itemAbove()->moveItem(sel);
    slotServicesChanged();
  }
  
  void portsPage::slotAddService()
  {
    struct kiptg::Service service = m_newServiceDialog->getService();
    
    QListViewItem *item = new QListViewItem(m_services,
      service.portNumber,
      service.protocol,
      service.action,
      service.portName);
    item = 0; // stop unused variable warnings
    
  }

	portsPage::portsPage(QString text, QWidget *parent) : QFrame(parent)
	{
		m_newServiceDialog = new newServiceDialog(this);
		connect(m_newServiceDialog, SIGNAL(okClicked()), this, SLOT(slotAddService()));
		
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
    connect(m_services, SIGNAL(selectionChanged()), this, SLOT(slotServicesChanged()));
    
    m_addService = new KPushButton(i18n("Add Service"), this);
    m_addService->show();
    layout->addWidget(m_addService, 1, 1);
    connect(m_addService, SIGNAL(clicked()), m_newServiceDialog, SLOT(show()));
    
    m_delService = new KPushButton(i18n("Remove Service"), this);
    m_delService->setEnabled(false);
    m_delService->show();
    layout->addWidget(m_delService, 2, 1);
    connect(m_delService, SIGNAL(clicked()), this, SLOT(slotDelService()));
    
    m_serviceUp = new KPushButton(i18n("Move Up"), this);
    m_serviceUp->setEnabled(false);
    m_serviceUp->show();
    layout->addWidget(m_serviceUp, 3, 1);
    connect(m_serviceUp, SIGNAL(clicked()), this, SLOT(slotMoveUp()));
    
    m_serviceDown = new KPushButton(i18n("Move Down"), this);
    m_serviceDown->setEnabled(false);
    m_serviceDown->show();
    layout->addWidget(m_serviceDown, 4, 1);
    connect(m_serviceDown, SIGNAL(clicked()), this, SLOT(slotMoveDown()));
    
    layout->setColStretch(0, 1);
	}

}	

