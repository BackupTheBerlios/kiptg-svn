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
#include <kpushbutton.h>

namespace kiptg
{
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
		setupNewForwardDialog(newText);
		
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
    
    KPushButton *addForward = new KPushButton(i18n("Add..."), this);
    layout->addWidget(addForward, 1, 1);
    connect( addForward, SIGNAL(clicked()), this, SLOT(slotShowForwardDialog()));
    
    KPushButton *delForward = new KPushButton(i18n("Remove"), this);
    layout->addWidget(delForward, 2, 1);
    connect( delForward, SIGNAL(clicked()), this, SLOT(slotDel()));
	}
	
  void forwardingPage::slotDel()
  {
    QListViewItem *sel = m_forwards->selectedItem();
    if (sel) delete sel;
  }
  
  void forwardingPage::slotAdd()
  {
    QString direction;
    m_forwardIncoming->isChecked()
      ? direction = i18n("Incoming")
      : direction = i18n("Outgoing");
  
    QListViewItem *item = new KListViewItem(m_forwards,
      direction,
      m_forwardPort->text(),
      m_forwardTo->text());
      item = 0; // stop unused variable warnings
  }
  
	void forwardingPage::slotShowForwardDialog()
  {
    m_forwardPort->setText("");
    m_forwardTo->setText("");
    m_forwardIncoming->setChecked(true);

    m_newForwardDialog->show();
  }

	void forwardingPage::setupNewForwardDialog(QString text)
	{
    m_newForwardDialog = new KDialogBase(this, 0, true, i18n("Add Forward"), KDialogBase::Ok | KDialogBase::Cancel);
    
    QFrame *dialogArea = new QFrame(m_newForwardDialog);
    QGridLayout *layout = new QGridLayout(dialogArea, 4, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    KActiveLabel *intro = new KActiveLabel(text, dialogArea);
    intro->show();
    layout->addMultiCellWidget(intro, 0, 0, 0, 1);
    
    QButtonGroup *direction = new QButtonGroup(dialogArea);
    direction->hide();
    
    m_forwardIncoming = new QRadioButton(i18n("&Incoming"), dialogArea);
    m_forwardIncoming->setChecked(true);
    m_forwardIncoming->show();
    layout->addWidget(m_forwardIncoming, 1, 0);
    direction->insert(m_forwardIncoming);
    
    m_forwardOutgoing = new QRadioButton(i18n("&Outgoing"), dialogArea);
    m_forwardOutgoing->show();
    layout->addWidget(m_forwardOutgoing, 1, 1);
    direction->insert(m_forwardOutgoing);
    
    QLabel *label = new QLabel(i18n("Port:"), dialogArea);
    label->show();
    layout->addWidget(label, 2, 0);
    
    m_forwardPort = new KLineEdit(dialogArea);
    m_forwardPort->show();
    layout->addWidget(m_forwardPort, 2, 1);
    
    label = new QLabel(i18n("Destination:"), dialogArea);
    label->show();
    layout->addWidget(label, 3, 0);
    
    m_forwardTo = new KLineEdit(dialogArea);
    m_forwardTo->show();
    layout->addWidget(m_forwardTo, 3, 1);
  
    connect(m_newForwardDialog, SIGNAL(okClicked()), this, SLOT(slotAdd()));
    
    dialogArea->show();
    m_newForwardDialog->setMainWidget(dialogArea);
	}
}
