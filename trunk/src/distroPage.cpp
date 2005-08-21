/***************************************************************************
 *   Copyright (C) 2004 by Fred Emmott                                     *
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

#include "distroPage.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdialogbase.h>
#include <klocale.h>

#include "distros.h"

namespace kiptg
{
	distroPage::distroPage(QWidget* parent) : QFrame(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
  	layout->setSpacing(KDialogBase::spacingHint());
  	
  	QLabel *label = new QLabel(i18n(
    	"<p>Please select which distribution you wish to use the "
    	"produced script on:</p>"), this);
    label->show();
    layout->addWidget(label);
		
		m_distroList = new KComboBox(this);
    m_distroList->insertItem(i18n("Generic Linux"), KIPTG_GENERIC_LINUX);
    m_distroList->insertItem(i18n("Slackware"), KIPTG_SLACKWARE);
    m_distroList->insertItem(i18n("Gentoo"), KIPTG_GENTOO);
    m_distroList->insertItem(i18n("Generic BSD"), KIPTG_GENERIC_BSD); // FIXME: these are unimplemented
    m_distroList->insertItem(i18n("FreeBSD"), KIPTG_FREEBSD);
    m_distroList->insertItem(i18n("NetBSD"), KIPTG_NETBSD);
    m_distroList->insertItem(i18n("OpenBSD"), KIPTG_OPENBSD);
    m_distroList->show();
    layout->addWidget(m_distroList);
    connect(m_distroList, SIGNAL(activated(int )), this, SIGNAL(distroChanged(int )));
    
    label = new QLabel(i18n(
      "<p><i>Note: If your distribution isn't listed, the 'Generic Linux' or 'Generic BSD' "
      "options should work for you.</i></p>"), this);
    label->show();
    layout->addWidget(label);
    
    layout->addItem(new QSpacerItem(0,0, QSizePolicy::Ignored, QSizePolicy::Ignored));
	}
	
	int distroPage::getDistro()
	{
		return m_distroList->currentItem();
  }
}
