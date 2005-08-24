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

#include "forwardingPage.h"

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <klocale.h>

namespace kiptg
{
	void forwardingPage::slotSelectionChanged()
	{
		QListViewItem *item = m_forwards->selectedItem();
		if ( item )
			m_del->setEnabled(true);
    else
    	m_del->setEnabled(false);	
	}
	
	QValueVector<struct Forward> forwardingPage::getForwards()
	{
		QValueVector<struct Forward> forwards;
		QListViewItem *item = m_forwards->firstChild();
		while (item)
		{
    	struct Forward _item;
    	item->text(0) == i18n("Incoming")
    		? _item.direction = kiptg::INCOMING
    		: _item.direction = kiptg::OUTGOING;
      _item.from = item->text(1);
      _item.to = item->text(2);
			forwards.append(_item);
			item = item->nextSibling();
		}
		return forwards;
	}
	
	forwardingPage::forwardingPage(QString text, QString newText, QWidget* parent) : QFrame(parent)
	{
		m_newForwardDialog = new newForwardDialog(newText, this);
		connect(m_newForwardDialog, SIGNAL(okClicked()), this, SLOT(slotAdd()));
		
    QGridLayout *layout = new QGridLayout(this, 4, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    QLabel *label = new QLabel(text, this);
    label->show();
    layout->addMultiCellWidget(label, 0, 0, 0, 1);
    
    m_forwards = new KListView(this);
    m_forwards->addColumn(i18n("In/Out"));
    m_forwards->addColumn(i18n("Port"));
    m_forwards->addColumn(i18n("Destination"));
    m_forwards->show();
    layout->addMultiCellWidget(m_forwards, 1, 3, 0, 0);
    connect(m_forwards, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    
    m_add = new KPushButton(i18n("Add Port"), this);
    m_add->show();
    layout->addWidget(m_add, 1, 1);
    connect(m_add, SIGNAL(clicked()), m_newForwardDialog, SLOT(show()));
    
    m_del = new KPushButton(i18n("Remove Port"), this);
    m_del->show();
    m_del->setEnabled(false);
    layout->addWidget(m_del, 2, 1);
    connect(m_del, SIGNAL(clicked()), this, SLOT(slotDel()));
    
    layout->setColStretch(0, 1);
	}
	
  void forwardingPage::slotDel()
  {
    QListViewItem *sel = m_forwards->selectedItem();
    if (sel) delete sel;
  }
  
  void forwardingPage::slotAdd()
  {
  	struct Forward forward = m_newForwardDialog->getForward();
    QString direction;
    forward.direction == kiptg::INCOMING
      ? direction = i18n("Incoming")
      : direction = i18n("Outgoing");
  
    QListViewItem *item = new KListViewItem(m_forwards,
      direction,
      forward.from,
      forward.to);
      item = 0; // stop unused variable warnings
  }
  
}
