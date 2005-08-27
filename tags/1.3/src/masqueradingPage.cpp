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

#include "masqueradingPage.h"

#include <net/if.h>

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <kdialogbase.h>
#include <klocale.h>

namespace kiptg
{
	QStringList masqueradingPage::intIfs()
	{
    QStringList qslIntIfs;

    for (unsigned int i = 0; i < m_intIfs->count(); i++)
      if ( m_intIfs->isSelected(i) )
        qslIntIfs.append(m_intIfs->item(i)->text());
		return qslIntIfs;
	}

	QString masqueradingPage::extIf()
	{
		return m_extIf->currentText();
	}
	
	bool masqueradingPage::masqEnabled()
	{
		return m_yes->isChecked();
	}
	
	masqueradingPage::masqueradingPage(QWidget *parent) : QFrame(parent)
	{
		struct if_nameindex *interfaces = if_nameindex();
		
    QGridLayout *layout = new QGridLayout(this, 6,2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    QLabel *label = (QLabel*) new KActiveLabel(i18n("<p>Do you wish to setup masquerading?</p>"
      "<p><i>Note: Some operating systems "
      "call masquerading 'internet connection sharing'.</i></p>"), this);
    label->show();
    layout->addMultiCellWidget(label, 0, 0, 0, 1);
    
    QButtonGroup *optYesNo = new QButtonGroup(this);
    optYesNo->hide();
    
    m_yes = new QRadioButton(i18n("&Yes"), this);
    m_yes->show();
    layout->addWidget(m_yes, 1, 0);
    optYesNo->insert(m_yes);
    
    m_no = new QRadioButton(i18n("&No"), this);
    m_no->show();
    layout->addWidget(m_no, 1, 1);
    optYesNo->insert(m_no);
    
    label = new QLabel(i18n("<p>Please select your external interface; this generally means the interface for your internet connection.</p>"),
      this);
    label->show();
    layout->addMultiCellWidget(label, 2, 2, 0, 1);
    connect(m_no, SIGNAL(toggled(bool )), label, SLOT(setDisabled(bool )));
    
    m_extIf = new KComboBox(this);
    while (true)
    {
    	static int i = -1;
    	if (interfaces[++i].if_index != 0)
      {
      	if ((QString) interfaces[i].if_name != "lo")
      		m_extIf->insertItem(interfaces[i].if_name);
      }
      else
      	break;
    }
    m_extIf->show();
    layout->addMultiCellWidget(m_extIf, 3, 3, 0, 1);
    connect(m_no, SIGNAL(toggled(bool )), m_extIf, SLOT(setDisabled(bool )));
    
    label = new QLabel(i18n("<p>Please select the interfaces which should have access to the external interface.</p>"),
      this);
    label->show();
    layout->addMultiCellWidget(label, 4, 4, 0, 1);
    connect(m_no, SIGNAL(toggled(bool )), label, SLOT(setDisabled(bool )));
    
    m_intIfs = new KListBox(this);
    while (true)
    {
    	static int i = -1;
    	if (interfaces[++i].if_index != 0)
      {
      	if ((QString) interfaces[i].if_name != "lo")
      		m_intIfs->insertItem(interfaces[i].if_name);
      }
      else
      	break;
    }
    m_intIfs->show();
    layout->addMultiCellWidget(m_intIfs, 5, 5, 0, 1);
    connect(m_no, SIGNAL(toggled(bool )), m_intIfs, SLOT(setDisabled(bool )));
    
    m_intIfs->setSelectionMode(QListBox::Multi);
    m_no->setChecked(true);
    if_freenameindex(interfaces);
	};
}
