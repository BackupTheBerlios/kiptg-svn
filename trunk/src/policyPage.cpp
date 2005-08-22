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

#include "policyPage.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdialogbase.h>
#include <klocale.h>

#include "constants.h"

namespace kiptg
{
	policyPage::policyPage(QString text, QWidget* parent) : QFrame(parent)
	{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(KDialogBase::spacingHint());
    
    QLabel *intro = new QLabel(text, this);
    intro->show();
    layout->addWidget(intro);
    
    m_policy = new KComboBox(this);
    m_policy->insertItem(i18n("Accept"), ACCEPT);
    m_policy->insertItem(i18n("Drop"), DROP);
    m_policy->setCurrentItem(1);
    m_policy->show();
    layout->addWidget(m_policy);
    
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Ignored));
	};
	
	int policyPage::value()
	{
		return m_policy->currentItem();
	}
}
