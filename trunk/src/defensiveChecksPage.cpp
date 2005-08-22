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

#include "defensiveChecksPage.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kactivelabel.h>
#include <kdialogbase.h>
#include <klocale.h>

namespace kiptg
{
	bool defensiveChecksPage::blockSpoofed()
	{
		return m_spoofed->isChecked();
	}
	
	bool defensiveChecksPage::blockSynFlood()
	{
		return m_synFlood->isChecked();
  }
  
  bool defensiveChecksPage::checkNewSyn()
  {
  	return m_newSyn->isChecked();
  }
  
  bool defensiveChecksPage::blockSynFin()
  {
  	return m_synFin->isChecked();
  }
  
  bool defensiveChecksPage::blockEchoBroadcasts()
  {
  	return m_echoBroadcasts->isChecked();
  }
  
  bool defensiveChecksPage::ignoreRedirects()
  {
  	return m_ignoreRedirects->isChecked();
  }
  
  bool defensiveChecksPage::blockSourceRouted()
  {
  	return m_sourceRouting->isChecked();
  }
  
	defensiveChecksPage::defensiveChecksPage(QWidget *parent) : QFrame(parent)
	{
	  QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(KDialogBase::spacingHint());
    
    KActiveLabel *summary = new KActiveLabel(i18n(
      "<p>This page allows you to enable additional checks to help protect your system "
      "from common firewall evasion techniques, DOS attacks, and other malicious activities.</p>"
      "<p>If you don't understand the above, the defaults provided are suitable for most systems.</p>"),
      this);
    summary->show();
    layout->addWidget(summary);
    
    m_spoofed = new QCheckBox(i18n("&Block spoofed packets"), this);
    m_spoofed->setChecked(true);
    m_spoofed->show();
    layout->addWidget(m_spoofed);
    
    m_synFlood = new QCheckBox(i18n("&SYN-flood protection"), this);
    m_synFlood->setChecked(true);
    m_synFlood->show();
    layout->addWidget(m_synFlood);
    
    m_newSyn = new QCheckBox(i18n("&Check new connections start with a SYN packet"), this);
    m_newSyn->setChecked(true);
    m_newSyn->show();
    layout->addWidget(m_newSyn);
    
    m_synFin = new QCheckBox(i18n("&Drop packets with both SYN and FIN flags set"), this);
    m_synFin->setChecked(true);
    m_synFin->show();
    layout->addWidget(m_synFin);
    
    m_echoBroadcasts = new QCheckBox(i18n("&Ignore ICMP echo broadcasts (helps protect against smurf attacks)"), this);
    m_echoBroadcasts->setChecked(true);
    m_echoBroadcasts->show();
    layout->addWidget(m_echoBroadcasts);
    
    m_ignoreRedirects = new QCheckBox(i18n("I&gnore ICMP redirects"), this);
    m_ignoreRedirects->setChecked(true);
    m_ignoreRedirects->show();
    layout->addWidget(m_ignoreRedirects);
    
    m_sourceRouting = new QCheckBox(i18n("Ig&nore packets that use IP source routing"), this);
    m_sourceRouting->setChecked(true);
    m_sourceRouting->show();
    layout->addWidget(m_sourceRouting);
   
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Ignored));
  }
}
