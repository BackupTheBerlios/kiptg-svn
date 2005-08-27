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

#include "yesNoPage.h"

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kdialogbase.h>
#include <klocale.h>

namespace kiptg
{
	yesNoPage::yesNoPage(QString text, QWidget* parent) : QFrame(parent)
	{
    QGridLayout *layout = new QGridLayout(this, 2, 2);
    layout->setSpacing(KDialogBase::spacingHint());
    
    QLabel *label = new QLabel(text, this);
    label->show();
    layout->addMultiCellWidget(label, 0, 0, 0, 1);
    
    QButtonGroup *optYesNo = new QButtonGroup(this);
    optYesNo->hide();
    
    m_yes = new QRadioButton(i18n("&Yes"), this);
    m_yes->setChecked(true);
    m_yes->show();
    layout->addWidget(m_yes, 1, 0);
    optYesNo->insert(m_yes);
    
    m_no = new QRadioButton(i18n("N&o"), this);
    m_no->show();
    layout->addWidget(m_no, 1, 1);
    optYesNo->insert(m_no);
  }
  
  bool yesNoPage::value()
  {
  	return m_yes->isChecked();
  }
}
