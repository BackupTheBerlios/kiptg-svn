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

#include "conntrackPage.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdialogbase.h>
#include <klocale.h>
#include <kseparator.h>

namespace kiptg
{
	conntrackPage::conntrackPage(QWidget *parent) : QFrame(parent)
	{
    QGridLayout *layout = new QGridLayout(this, 8, 4);
    layout->setSpacing(KDialogBase::spacingHint());
    
    QLabel *intro = new QLabel(i18n( "<p><i>Advanced users only</i> - "
      "The defaults here are sensible, and will work for most users.</p>"
      "<p>If packets match one of the following connection states, they "
      "will be admitted by the firewall.</p>"), this);
    intro->show();
    layout->addMultiCellWidget(intro, 0, 0, 0, 3);
    
    m_allSame = new QCheckBox(i18n("Use same settings for all protocols"), this);
    m_allSame->show();
    layout->addMultiCellWidget(m_allSame, 1, 1, 0, 3);
    
    // ALL CONNTRACK
    
    QLabel *allLabel = new QLabel(i18n("All: "), this);
    allLabel->show();
    layout->addWidget(allLabel, 2, 0);
    
    m_allEST = new QCheckBox(i18n("Established"), this);
    m_allEST->setChecked(true);
    m_allEST->show();
    layout->addWidget(m_allEST, 2, 1);
    
    m_allREL = new QCheckBox(i18n("Related"), this);
    m_allREL->setChecked(true);
    m_allREL->show();
    layout->addWidget(m_allREL, 2, 2);
    
    m_allNEW = new QCheckBox(i18n("New"), this);
    m_allNEW->show();
    layout->addWidget(m_allNEW, 2, 3);
    
    KSeparator *separator = new KSeparator(this);
    layout->addMultiCellWidget(separator, 3, 3, 0, 3);
    
    // TCP CONNTRACK
    
    QLabel *tcpLabel = new QLabel(i18n("TCP: "), this);
    tcpLabel->show();
    layout->addWidget(tcpLabel, 4, 0);
    
    m_tcpEST = new QCheckBox(i18n("Established"), this);
    m_tcpEST->setChecked(true);
    m_tcpEST->show();
    layout->addWidget(m_tcpEST, 4, 1);
    
    m_tcpREL = new QCheckBox(i18n("Related"), this);
    m_tcpREL->setChecked(true);
    m_tcpREL->show();
    layout->addWidget(m_tcpREL, 4, 2);
    
    m_tcpNEW = new QCheckBox(i18n("New"), this);
    m_tcpNEW->show();
    layout->addWidget(m_tcpNEW, 4, 3);
    
    // UDP CONNTRACK

    QLabel *udpLabel = new QLabel(i18n("UDP: "), this);
    udpLabel->show();
    layout->addWidget(udpLabel, 5, 0);
    
    m_udpEST = new QCheckBox(i18n("Established"), this);
    m_udpEST->setChecked(true);
    m_udpEST->show();
    layout->addWidget(m_udpEST, 5, 1);
    
    m_udpREL = new QCheckBox(i18n("Related"), this);
    m_udpREL->setChecked(true);
    m_udpREL->show();
    layout->addWidget(m_udpREL, 5, 2);
    
    m_udpNEW = new QCheckBox(i18n("New"), this);
    m_udpNEW->show();
    layout->addWidget(m_udpNEW, 5, 3);
    
    // ICMP CONNTRACK

    QLabel *icmpLabel = new QLabel(i18n("ICMP: "), this);
    icmpLabel->show();
    layout->addWidget(icmpLabel, 6, 0);
    
    m_icmpEST = new QCheckBox(i18n("Established"), this);
    m_icmpEST->setChecked(true);
    m_icmpEST->show();
    layout->addWidget(m_icmpEST, 6, 1);
    
    m_icmpREL = new QCheckBox(i18n("Related"), this);
    m_icmpREL->setChecked(true);
    m_icmpREL->show();
    layout->addWidget(m_icmpREL, 6, 2);
    
    m_icmpNEW = new QCheckBox(i18n("New"), this);
    m_icmpNEW->show();
    layout->addWidget(m_icmpNEW, 6, 3);
    
    // SLOTS
    
    connect(m_allSame, SIGNAL(toggled(bool )), allLabel, SLOT(setEnabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_allEST, SLOT(setEnabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_allREL, SLOT(setEnabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_allNEW, SLOT(setEnabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), tcpLabel, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_tcpEST, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_tcpREL, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_tcpNEW, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), udpLabel, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_udpEST, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_udpREL, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_udpNEW, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), icmpLabel, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_icmpEST, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_icmpREL, SLOT(setDisabled(bool )));
    connect(m_allSame, SIGNAL(toggled(bool )), m_icmpNEW, SLOT(setDisabled(bool )));

		m_allSame->setChecked(true);
		
		layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Ignored));
	};
	
	bool conntrackPage::allSame()
	{
		return m_allSame->isChecked();
  }
  
  int conntrackPage::getAll()
  {
  	int _est, _rel, _new;
  	m_allEST->isChecked()
  		? _est = kiptg::ESTABLISHED
  		: _est = 0;
  	m_allREL->isChecked()
  		? _rel = kiptg::RELATED
  		: _rel = 0;
  	m_allNEW->isChecked()
  		? _new = kiptg::NEW
  		: _new = 0;
  	return (_est | _rel | _new);
  }
  
  int conntrackPage::getTCP()
  {
  	int _est, _rel, _new;
  	m_tcpEST->isChecked()
  		? _est = kiptg::ESTABLISHED
  		: _est = 0;
  	m_tcpREL->isChecked()
  		? _rel = kiptg::RELATED
  		: _rel = 0;
  	m_tcpNEW->isChecked()
  		? _new = kiptg::NEW
  		: _new = 0;
  	return (_est | _rel | _new);
  }
  
  int conntrackPage::getUDP()
  {
  	int _est, _rel, _new;
  	m_udpEST->isChecked()
  		? _est = kiptg::ESTABLISHED
  		: _est = 0;
  	m_udpREL->isChecked()
  		? _rel = kiptg::RELATED
  		: _rel = 0;
  	m_udpNEW->isChecked()
  		? _new = kiptg::NEW
  		: _new = 0;
  	return (_est | _rel | _new);
  }
  
  int conntrackPage::getICMP()
  {
  	int _est, _rel, _new;
  	m_icmpEST->isChecked()
  		? _est = kiptg::ESTABLISHED
  		: _est = 0;
  	m_icmpREL->isChecked()
  		? _rel = kiptg::RELATED
  		: _rel = 0;
  	m_icmpNEW->isChecked()
  		? _new = kiptg::NEW
  		: _new = 0;
  	return (_est | _rel | _new);
  }
}
