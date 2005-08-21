/***************************************************************************
 *   Copyright (C) 2004-2005 by Fred Emmott                                *
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
#include "kiptablesgenerator.h"

#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <kactivelabel.h>
#include <kcombobox.h>
#include <kinputdialog.h>
#include <klineedit.h>
#include <klistbox.h>
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kseparator.h>

kiptablesgenerator::kiptablesgenerator(QWidget *parent, const char *name)
 : KWizard(parent, name)
{
	currentOS = KIPTG_LINUX;

  setupNewForwardDialog();
  setupNewServiceDialog();
  setupNewHostDialog();

	welcomePage = new kiptg::textPage(i18n(
	  "<p>Welcome to KIptablesGenerator.</p>"
    "<p>This wizard will help you create your firewall rules.</p>"
    "<p><i>Troubleshooting:</i> The generated script requires "
    "iptables installed, and the netfilter conntrack, TCP, UDP, "
    "and ICMP kernel modules loaded.</p>"), this);
  this->addPage(welcomePage, i18n("Welcome"));
  setupDistroPage();
  setupInterfacesPage();
  setupIncomingPage();
  setAppropriate(incomingPage, false); // don't show this page, but set it up so accept() can reference it's settings
  setupIPolicyPage();
  setupIConntrackPage();
  setupIPortsPage();
  setupIHostsPage();
  setupFForwardingPage();
  setupFMasqueradingPage();
  setupIDefensiveChecksPage();
  finishedPage = new kiptg::textPage(i18n(
    "<p><b>Congratulations!</b></p>"
    "<p>All the information required to create your firewall rules has been collected. "
    "Please finish the wizard to generate your firewall script.</p>"), this);
  this->addPage(finishedPage, i18n("Finished"));
  setFinishEnabled(finishedPage, true);
  helpButton()->hide();
}

void kiptablesgenerator::setupIHostsPage()
{
  iHostsPage = new QFrame(this);
  
  QGridLayout *layout = new QGridLayout(iHostsPage, 4, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  KActiveLabel *label = new KActiveLabel(i18n(
    "<p><i>Advanced users only</i> - If you wish to allow or block any specific hosts, "
    "ignoring all other rules, add them to this page.</p>"),
    iHostsPage);
  label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  label->show();
  layout->addMultiCellWidget(label, 0, 0, 0, 1);
  
  KListView *hosts = new KListView(iHostsPage);
  hosts->addColumn(i18n("Allow/Block"));
  hosts->addColumn(i18n("MAC/IP"));
  hosts->addColumn(i18n("Host"));
  hosts->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
  hosts->show();
  namedWidgets["hostsList"] = hosts;
  layout->addMultiCellWidget(hosts, 1, 3, 0, 0);
  
  KPushButton *addHost = new KPushButton(i18n("Add..."), iHostsPage);
  addHost->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  addHost->show();
  layout->addWidget(addHost, 1, 1);
  connect( addHost, SIGNAL(clicked()), this, SLOT(slotShowHostDialog()));
  
  KPushButton *delHost = new KPushButton(i18n("Remove"), iHostsPage);
  delHost->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  layout->addWidget(delHost, 2, 1);
  connect( delHost, SIGNAL(clicked()), this, SLOT(slotDelHost()));
  
  layout->addItem(new QSpacerItem(QSizePolicy::Minimum, QSizePolicy::Ignored), 3, 1);
  layout->setColStretch(0, 1);
  
  this->addPage(iHostsPage, i18n("Host Control"));
}

void kiptablesgenerator::slotDistroChanged(int distro)
{
	switch (distro)
	{
		case KIPTG_GENERIC_LINUX:
		case KIPTG_SLACKWARE:
		case KIPTG_GENTOO:
			if ( currentOS == KIPTG_LINUX )
				return;
      for ( unsigned int i = 0; i < linuxOnlyPages.count(); i++)
      	setAppropriate(linuxOnlyPages.at(i), true);
      for ( unsigned int i = 0; i < linuxOnlyWidgets.count(); i++)
      	linuxOnlyWidgets.at(i)->setEnabled(true);
      currentOS = KIPTG_LINUX;
			break;
    default: // BSD
    	if ( currentOS == KIPTG_BSD )
    		return;
      for ( unsigned int i = 0; i < linuxOnlyPages.count(); i++)
      	setAppropriate(linuxOnlyPages.at(i), false);
      for ( unsigned int i = 0; i < linuxOnlyWidgets.count(); i++)
      	linuxOnlyWidgets.at(i)->setEnabled(false);
      currentOS = KIPTG_BSD;
    	break;
	}
}

void kiptablesgenerator::setupFForwardingPage()
{
  fForwardingPage = new QFrame(this);
  
  QGridLayout *layout = new QGridLayout(fForwardingPage, 4, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *label = new QLabel(i18n(
    "<p>If you wish to enable TCP forwarding for any ports (incoming or outgoing) add them to this page.</p>"),
    fForwardingPage);
  label->show();
  layout->addMultiCellWidget(label, 0, 0, 0, 1);
  
  KListView *ports = new KListView(fForwardingPage);
  ports->addColumn(i18n("In/Out"));
  ports->addColumn(i18n("Port"));
  ports->addColumn(i18n("Destination"));
  ports->show();
  namedWidgets["forwardsList"] = ports;
  layout->addMultiCellWidget(ports, 1, 3, 0, 0);
  
  KPushButton *addForward = new KPushButton(i18n("Add..."), fForwardingPage);
  layout->addWidget(addForward, 1, 1);
  connect( addForward, SIGNAL(clicked()), this, SLOT(slotShowForwardDialog()));
  
  KPushButton *delForward = new KPushButton(i18n("Remove"), fForwardingPage);
  layout->addWidget(delForward, 2, 1);
  connect( delForward, SIGNAL(clicked()), this, SLOT(slotDelForward()));
  
  this->addPage(fForwardingPage, i18n("Port Forwarding"));
}

void kiptablesgenerator::setupFMasqueradingPage()
{
  char ifBuffer[IFNAMSIZ];
	fMasqueradingPage = new QFrame(this);
	
	QGridLayout *layout = new QGridLayout(fMasqueradingPage, 6,2);
	layout->setSpacing(KDialogBase::spacingHint());
	
	KActiveLabel *label = new KActiveLabel(i18n("<p>Do you wish to setup masquerading?</p>"
		"<p><i>Note: Some operating systems "
		"call masquerading 'internet connection sharing'.</i></p>"), fMasqueradingPage);
  label->show();
  layout->addMultiCellWidget(label, 0, 0, 0, 1);
  
  QButtonGroup *optYesNo = new QButtonGroup(fMasqueradingPage);
  optYesNo->hide();
  
  QRadioButton *optYes = new QRadioButton(i18n("&Yes"), fMasqueradingPage);
  optYes->show();
  layout->addWidget(optYes, 1, 0);
  namedWidgets["masqueradeBool"] = optYes;
  connect(optYes, SIGNAL(toggled(bool )), this, SLOT(slotMasqueradingEnabled(bool)));
  optYesNo->insert(optYes);
  
  QRadioButton *optNo = new QRadioButton(i18n("&No"), fMasqueradingPage);
  optNo->setChecked(true); // people will know if they want this on, secure defaults
  optNo->show();
  layout->addWidget(optNo, 1, 1);
  optYesNo->insert(optNo);
  
  label = new KActiveLabel(i18n("<p>Please select your external interface; this generally means the interface for your internet connection.</p>"),
  	fMasqueradingPage);
  label->show();
  layout->addMultiCellWidget(label, 2, 2, 0, 1);
  namedWidgets["masqueradeLabel1"] = label;
  
  KComboBox *extIfList = new KComboBox(fMasqueradingPage);
  for (unsigned int i = 1; if_indextoname(i, ifBuffer) != NULL; i++)
		if ((QString)ifBuffer != "lo") extIfList->insertItem((QString)ifBuffer);
  extIfList->show();
  layout->addMultiCellWidget(extIfList, 3, 3, 0, 1);
  namedWidgets["masqueradeExtIf"] = extIfList;
  
	label = new KActiveLabel(i18n("<p>Please select the interfaces which should have access to the external interface.</p>"),
		fMasqueradingPage);
  label->show();
  layout->addMultiCellWidget(label, 4, 4, 0, 1);
  namedWidgets["masqueradeLabel2"] = label;
  
  KListBox *intIfList = new KListBox(fMasqueradingPage);
  for (unsigned int i = 1; if_indextoname(i, ifBuffer) != NULL; i++)
  	if((QString)ifBuffer != "lo") intIfList->insertItem((QString)ifBuffer);
  intIfList->show();
  layout->addMultiCellWidget(intIfList, 5, 5, 0, 1);
  namedWidgets["masqueradeIntIfs"] = intIfList;
  intIfList->setSelectionMode(QListBox::Multi);
  
  slotMasqueradingEnabled(false);
  
  fMasqueradingPage->show();
  this->addPage(fMasqueradingPage, i18n("Masquerading"));
}

void kiptablesgenerator::setupDistroPage()
{
  distroPage = new QFrame(this);
  
  QVBoxLayout *layout = new QVBoxLayout(distroPage);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *label = new QLabel(i18n(
    "<p>Please select which distribution you wish to use the "
    "produced script on:</p>"), distroPage);
  label->show();
  layout->addWidget(label);
  
  KComboBox *distroList = new KComboBox(distroPage);
  distroList->insertItem(i18n("Generic Linux"), KIPTG_GENERIC_LINUX);
  distroList->insertItem(i18n("Slackware"), KIPTG_SLACKWARE);
  distroList->insertItem(i18n("Gentoo"), KIPTG_GENTOO);
  distroList->insertItem(i18n("Generic BSD"), KIPTG_GENERIC_BSD); // FIXME: these are unimplemented
  distroList->insertItem(i18n("FreeBSD"), KIPTG_FREEBSD);
  distroList->insertItem(i18n("NetBSD"), KIPTG_NETBSD);
  distroList->insertItem(i18n("OpenBSD"), KIPTG_OPENBSD);
  distroList->show();
  layout->addWidget(distroList);
  namedWidgets["distro"] = distroList;
  connect( distroList, SIGNAL(activated(int)), this, SLOT(slotDistroChanged(int)));
  
  label = new QLabel(i18n(
  	"<p><i>Note: If your distribution isn't listed, the 'Generic Linux' or 'Generic BSD' "
  	"options should work for you.</i></p>"), distroPage);
  label->show();
  layout->addWidget(label);
  
  layout->addItem(new QSpacerItem(0,0, QSizePolicy::Ignored, QSizePolicy::Ignored));
  
  distroPage->show();
  this->addPage(distroPage, i18n("Distribution"));
}

void kiptablesgenerator::makeScript(QString &rulesList, QString &undoList, int distro)
{
  QString output, copyrightText =
    "# Copyright (c) 2004-2005 Frederick Emmott\n"
    "# Produced by KIptablesGenerator, please see\n"
    "# http://fredemmott.co.uk/index.php?page=kitg\n"
    "# This script is under the terms of the GNU\n"
    "# General Public License, Version 2, or at your\n"
    "# option, any later version.\n";
  
  switch (distro)
  {
  	case KIPTG_GENERIC_LINUX:
  		output = "#!/bin/sh\n" + copyrightText + "IPTABLES=/usr/sbin/iptables\n\n" + rulesList;
  		break;
    case KIPTG_SLACKWARE:
      output = "#!/bin/sh\n" + copyrightText + "IPTABLES=/usr/sbin/iptables\n\n"
        "function start() {\n" + rulesList +
        "}\n"
        "function stop() {\n" + undoList +
        "$IPTABLES -P INPUT ACCEPT\n"
        "$IPTABLES -P FORWARD ACCEPT\n"
        "$IPTABLES -P OUTPUT ACCEPT\n"
        "$IPTABLES -F\n"
        "}\n"
        "case $1 in\n"
        "\tstop)\n"
        "\t\tstop;\n"
        "\t\t;;\n"
        "\trestart)\n"
        "\t\tstop;\n"
        "\t\tstart;\n"
        "\t\t;;\n"
        "\t*)\n"
        "\t\tstart;\n"
        "esac";
      break;
    case KIPTG_GENTOO:
      output = "#!/sbin/runscript\n" + copyrightText + 
        "IPTABLES=/sbin/iptables\n\n"
        "start() {\n"
        "ebegin \"Starting firewall\"\n" + rulesList +
        "eend 0\n}\n\n"
        "stop() {\n"
        "ebegin \"Stopping firewall\"\n" + undoList +
        "$IPTABLES -P INPUT ACCEPT\n"
        "$IPTABLES -P OUTPUT ACCEPT\n"
        "$IPTABLES -P FORWARD ACCEPT\n"
        "$IPTABLES -F\n"
        "eend 0\n"
        "}";
      break;
  }
  rulesDialog = new RulesDialog(this,(char*) 0, &output);
  rulesDialog->show();
  connect(rulesDialog, SIGNAL(closeClicked()), this, SLOT(slotShownRules()));
}

void kiptablesgenerator::setupIncomingPage()
{
  incomingPage = new QFrame(this);
  
  QGridLayout *layout = new QGridLayout(incomingPage, 2, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *intro = new QLabel(i18n(
    "<p>Do you want to filter incoming data? (recommended)</p>"
    "<p>This will allow you to control other computers access to "
    "your computer.</p>"), incomingPage);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);
  
  QButtonGroup *optYesNo = new QButtonGroup(incomingPage);
  optYesNo->hide();
  
  QRadioButton *optYes = new QRadioButton(i18n("&Yes"), incomingPage);
  optYes->setChecked(true);
  optYes->setName("yes");
  optYes->show();
  layout->addWidget(optYes, 1, 0);
  namedWidgets["incomingBool"] = optYes;
  optYesNo->insert(optYes);
  
  QRadioButton *optNo = new QRadioButton(i18n("N&o"), incomingPage);
  optNo->setName("no");
  optNo->show();
  layout->addWidget(optNo, 1, 1);
  optYesNo->insert(optNo);
  
  incomingPage->show();
  this->addPage(incomingPage, i18n("Incoming Data"));
}

void kiptablesgenerator::setupIPolicyPage()
{
  iPolicyPage = new QFrame(this);
  
  QVBoxLayout *layout = new QVBoxLayout(iPolicyPage);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *intro = new QLabel(i18n(
    "<p>What do you want your firewall to do to unmatched packets?</p>"
    "<p>Your firewall can either accept unmatched packets, or drop unmatched "
    "packets; the recommended option is to drop unmatched packets.</p>"
    "<p><i>Advanced users: This sets the policy for the INPUT chain.</i></p>"), iPolicyPage);
  intro->show();
  layout->addWidget(intro);
  
  KComboBox *options = new KComboBox(iPolicyPage);
  options->insertItem(i18n("Accept"));
  options->insertItem(i18n("Drop"));
  options->setCurrentItem(1);
  options->show();
  layout->addWidget(options);
  namedWidgets["incomingPolicy"] = options;
  
  layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Ignored));
  
  iPolicyPage->show();
  this->addPage(iPolicyPage, i18n("Default Action"));
}

void kiptablesgenerator::setupInterfacesPage()
{
  interfacesPage = new QFrame(this);
  
  QVBoxLayout *layout = new QVBoxLayout(interfacesPage);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *intro = new QLabel(i18n(
    "<p>Please select which of the following network interfaces do you want to filter.</p>"), interfacesPage);
  intro->show();
  layout->addWidget(intro);
  
  KListBox *interfaces = new KListBox(interfacesPage);

  char buffer[IFNAMSIZ];
  for(unsigned int i = 1; if_indextoname(i, buffer) != NULL; i++)
  	if ((QString)buffer != "lo") interfaces->insertItem((QString)buffer);
  
  interfaces->setSelectionMode(QListBox::Multi);
  for (unsigned short i = 0; i < interfaces->count(); i++)
    interfaces->setSelected(i, true);
  interfaces->show();
  layout->addWidget(interfaces);
  namedWidgets["iInterfaces"] = interfaces;
  
  KPushButton *newInterface = new KPushButton(i18n("A&dd Interface..."), interfacesPage);
  newInterface->show();
  layout->addWidget(newInterface);
  connect(newInterface, SIGNAL(clicked()), this, SLOT(slotNewInterface()));
  
  interfacesPage->show();
  this->addPage(interfacesPage, i18n("Interfaces"));
}

void kiptablesgenerator::setupIConntrackPage()
{
  iConntrackPage = new QFrame(this);
  QGridLayout *layout = new QGridLayout(iConntrackPage, 8, 4);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *intro = new QLabel(i18n( "<p><i>Advanced users only</i> - "
    "The defaults here are sensible, and will work for most users.</p>"
    "<p>If packets match one of the following connection states, they "
    "will be admitted by the firewall.</p>"), iConntrackPage);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 3);
  
  QCheckBox *allSame = new QCheckBox(i18n("Use same settings for all protocols"), iConntrackPage);
  allSame->show();
  layout->addMultiCellWidget(allSame, 1, 1, 0, 3);
  namedWidgets["iConntrackAllSame"] = allSame;
  
  // ALL CONNTRACK
  
  QLabel *allLabel = new QLabel(i18n("All: "), iConntrackPage);
  allLabel->show();
  layout->addWidget(allLabel, 2, 0);
  
  QCheckBox *allEstablished = new QCheckBox(i18n("Established"), iConntrackPage);
  allEstablished->setChecked(true);
  allEstablished->show();
  namedWidgets["iConntrackAllEstablished"] = allEstablished;
  layout->addWidget(allEstablished, 2, 1);
  
  QCheckBox *allRelated = new QCheckBox(i18n("Related"), iConntrackPage);
  allRelated->setChecked(true);
  allRelated->show();
  namedWidgets["iConntrackAllRelated"] = allRelated;
  layout->addWidget(allRelated, 2, 2);
  
  QCheckBox *allNew = new QCheckBox(i18n("New"), iConntrackPage);
  allNew->show();
  namedWidgets["iConntrackAllNew"] = allNew;
  layout->addWidget(allNew, 2, 3);
  
  KSeparator *separator = new KSeparator(iConntrackPage);
  layout->addMultiCellWidget(separator, 3, 3, 0, 3);
  
  // TCP CONNTRACK
    
  QLabel *tcpLabel = new QLabel(i18n("TCP: "), iConntrackPage);
  tcpLabel->show();
  layout->addWidget(tcpLabel, 4, 0);
  
  QCheckBox *tcpEstablished = new QCheckBox(i18n("Established"), iConntrackPage);
  tcpEstablished->setChecked(true);
  tcpEstablished->show();
  layout->addWidget(tcpEstablished, 4, 1);
  namedWidgets["iConntrackTcpEstablished"] = tcpEstablished;
  
  QCheckBox *tcpRelated = new QCheckBox(i18n("Related"), iConntrackPage);
  tcpRelated->setChecked(true);
  tcpRelated->show();
  layout->addWidget(tcpRelated, 4, 2);
  namedWidgets["iConntrackTcpRelated"] = tcpRelated;
  
  QCheckBox *tcpNew = new QCheckBox(i18n("New"), iConntrackPage);
  tcpNew->show();
  layout->addWidget(tcpNew, 4, 3);
  namedWidgets["iConntrackTcpNew"] = tcpNew;
  
  // UDP CONNTRACK
  
  QLabel *udpLabel = new QLabel(i18n("UDP: "), iConntrackPage);
  udpLabel->show();
  layout->addWidget(udpLabel, 5, 0);
  
  QCheckBox *udpEstablished = new QCheckBox(i18n("Established"), iConntrackPage);
  udpEstablished->setChecked(true);
  udpEstablished->show();
  layout->addWidget(udpEstablished, 5, 1);
  namedWidgets["iConntrackUdpEstablished"] = udpEstablished;
  
  QCheckBox *udpRelated = new QCheckBox(i18n("Related"), iConntrackPage);
  udpRelated->setChecked(true);
  udpRelated->show();
  layout->addWidget(udpRelated, 5, 2);
  namedWidgets["iConntrackUdpRelated"] = udpRelated;
  
  QCheckBox *udpNew = new QCheckBox(i18n("New"), iConntrackPage);
  udpNew->show();
  layout->addWidget(udpNew, 5, 3);
  namedWidgets["iConntrackUdpNew"] = udpNew;
  
  // ICMP CONNTRACK
  
  QLabel *icmpLabel = new QLabel(i18n("ICMP: "), iConntrackPage);
  icmpLabel->show();
  layout->addWidget(icmpLabel, 6, 0);
  
  QCheckBox *icmpEstablished = new QCheckBox(i18n("Established"), iConntrackPage);
  icmpEstablished->setChecked(true);
  icmpEstablished->show();
  layout->addWidget(icmpEstablished, 6, 1);
  namedWidgets["iConntrackICMPEstablished"] = icmpEstablished;
  
  QCheckBox *icmpRelated = new QCheckBox(i18n("Related"), iConntrackPage);
  icmpRelated->setChecked(true);
  icmpRelated->show();
  layout->addWidget(icmpRelated, 6, 2);
  namedWidgets["iConntrackICMPRelated"] = icmpRelated;
  
  QCheckBox *icmpNew = new QCheckBox(i18n("New"), iConntrackPage);
  icmpNew->show();
  layout->addWidget(icmpNew, 6, 3);
  namedWidgets["iConntrackICMPNew"] = icmpNew;

  connect(allSame, SIGNAL(toggled(bool )), allLabel, SLOT(setEnabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), allEstablished, SLOT(setEnabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), allNew, SLOT(setEnabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), allRelated, SLOT(setEnabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), tcpLabel, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), tcpEstablished, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), tcpNew, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), tcpRelated, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), udpLabel, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), udpEstablished, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), udpNew, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), udpRelated, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), icmpLabel, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), icmpEstablished, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), icmpNew, SLOT(setDisabled(bool )));
  connect(allSame, SIGNAL(toggled(bool )), icmpRelated, SLOT(setDisabled(bool )));
  
  allSame->setChecked(true);
  
  layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Ignored));
  
  iConntrackPage->show();
  this->addPage(iConntrackPage, i18n("Connection Tracking"));
}

void kiptablesgenerator::setupIPortsPage()
{
  iPortsPage = new QFrame(this);
  QGridLayout *layout = new QGridLayout(iPortsPage, 6, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  QLabel *intro = new QLabel(
    i18n("<p>Here you can select which services are available to the outside world.</p>"),
    iPortsPage);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);
  
  KListView *ports = new KListView(iPortsPage);
  ports->addColumn(i18n("Port"));
  ports->addColumn(i18n("Protocol"));
  ports->addColumn(i18n("Action"));
  ports->addColumn(i18n("Service"));
  ports->setSorting(-1);
  ports->show();
  layout->addMultiCellWidget(ports, 1, 5, 0, 0);
  namedWidgets["iPorts"] = ports;
  
  KPushButton *newService = new KPushButton(i18n("A&dd Service"), iPortsPage);
  newService->show();
  layout->addWidget(newService, 1, 1);
  connect(newService, SIGNAL(clicked()), this, SLOT(slotShowServiceDialog()));
  
  KPushButton *delService = new KPushButton(i18n("&Remove Service"), iPortsPage);
  delService->show();
  layout->addWidget(delService, 2, 1);
  connect(delService, SIGNAL(clicked()), this, SLOT(slotDelService()));
  
  KPushButton *upService = new KPushButton(i18n("&Move Up"), iPortsPage);
  upService->show();
  layout->addWidget(upService, 3, 1);
  connect(upService, SIGNAL(clicked()), this, SLOT(slotUpService()));
  
  KPushButton *downService = new KPushButton(i18n("M&ove Down"), iPortsPage);
  downService->show();
  layout->addWidget(downService, 4, 1);
  connect(downService, SIGNAL(clicked()), this, SLOT(slotDownService()));
  
  layout->setColStretch(0, 1);
  
  iPortsPage->show();
  this->addPage(iPortsPage, i18n("Incoming Ports"));
}

void kiptablesgenerator::setupNewHostDialog()
{
  newHostDialog = new KDialogBase(this, 0, true, i18n("Add Host"), KDialogBase::Ok | KDialogBase::Cancel);
  
  QFrame *dialogArea = new QFrame(newHostDialog);
  QGridLayout *layout = new QGridLayout(dialogArea, 7, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  KActiveLabel *intro = new KActiveLabel(i18n(
    "<p>Here you can tell netfilter to allow all connections from a given host regardless of other rules, "
    "or block all connections from a given host regardless of other rules.</p>"), dialogArea);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);

	QLabel *label = new QLabel(i18n(
		"<p>Do you wish to specify the host by IP address or MAC (hardware) address?</p>"), dialogArea);
  label->show();
  layout->addMultiCellWidget(label, 1, 1, 0, 1);

  QButtonGroup *ipOrMAC = new QButtonGroup(dialogArea);
  ipOrMAC->hide();
  
  QRadioButton *useIP = new QRadioButton(i18n("&Use IP"), dialogArea);
  useIP->setChecked(true);
  useIP->show();
  layout->addWidget(useIP, 2, 0);
  namedWidgets["newHost_useIP"] = useIP;
  ipOrMAC->insert(useIP);
  
  QRadioButton *useMAC = new QRadioButton(i18n("U&se MAC"), dialogArea);
  useMAC->show();
  layout->addWidget(useMAC, 2, 1);
  ipOrMAC->insert(useMAC);
  
  label = new QLabel(i18n("<p>Do you want to allow connections from this host, or block them?</p>"), dialogArea);
  label->show();
  layout->addMultiCellWidget(label, 3, 3, 0, 1);

  QButtonGroup *whitelistOrBlacklist = new QButtonGroup(dialogArea);
  whitelistOrBlacklist->hide();
  
  QRadioButton *whitelist = new QRadioButton(i18n("&Allow"), dialogArea);
  whitelist->setChecked(true);
  whitelist->show();
  layout->addWidget(whitelist, 4, 0);
  namedWidgets["newHost_allow"] = whitelist;
  whitelistOrBlacklist->insert(whitelist);
  
  QRadioButton *blacklist = new QRadioButton(i18n("&Block"), dialogArea);
  blacklist->setChecked(false);
  blacklist->show();
  layout->addWidget(blacklist, 4, 1);
  whitelistOrBlacklist->insert(blacklist);
  
 
  KActiveLabel *hostLabel = new KActiveLabel(i18n("Enter the IP or MAC address of the host:"), dialogArea);
  hostLabel->show();
  layout->addMultiCellWidget(hostLabel, 5, 5, 0, 1);
  
  KLineEdit *host = new KLineEdit(dialogArea);
  host->show();
  namedWidgets["newHost_address"] = host;
  layout->addMultiCellWidget(host, 6, 6, 0, 1);
  
  connect(newHostDialog, SIGNAL(okClicked()), this, SLOT(slotAddHost()));
  
  dialogArea->show();
  newHostDialog->setMainWidget(dialogArea);
}

void kiptablesgenerator::setupNewForwardDialog()
{
  newForwardDialog = new KDialogBase(this, 0, true, i18n("Add Forward"), KDialogBase::Ok | KDialogBase::Cancel);
  
  QFrame *dialogArea = new QFrame(newForwardDialog);
  QGridLayout *layout = new QGridLayout(dialogArea, 4, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  KActiveLabel *intro = new KActiveLabel(i18n(
      "<p><i>Advanced users only</i></p>"
      "<p>Here you can tell netfilter to forward connections to given ports to another address/port.</p>"
      "<p>This is using netfilter's DNAT functionality - incoming redirects go in the prerouting chain,"
      "outgoing redirects go in the output chain.</p>"
      "<p>The destination should be of the from destination.computer.ip.address:destinationPort</p>"), dialogArea);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);
  
  QButtonGroup *direction = new QButtonGroup(dialogArea);
  direction->hide();
  
  QRadioButton *incoming = new QRadioButton(i18n("&Incoming"), dialogArea);
  incoming->setChecked(true);
  incoming->show();
  layout->addWidget(incoming, 1, 0);
  namedWidgets["forward_incoming"] = incoming;
  direction->insert(incoming);
  
  QRadioButton *outgoing = new QRadioButton(i18n("&Outgoing"), dialogArea);
  outgoing->show();
  layout->addWidget(outgoing, 1, 1);
  direction->insert(outgoing);
  
  QLabel *label = new QLabel(i18n("Port:"), dialogArea);
  label->show();
  layout->addWidget(label, 2, 0);
  
  KLineEdit *port = new KLineEdit(dialogArea);
  port->show();
  layout->addWidget(port, 2, 1);
  namedWidgets["forward_port"] = port;
  
  label = new QLabel(i18n("Destination:"), dialogArea);
  label->show();
  layout->addWidget(label, 3, 0);
  
  KLineEdit *destination = new KLineEdit(dialogArea);
  destination->show();
  layout->addWidget(destination, 3, 1);
  namedWidgets["forward_destination"] = destination;
 

  connect(newForwardDialog, SIGNAL(okClicked()), this, SLOT(slotAddForward()));
  
  dialogArea->show();
  newForwardDialog->setMainWidget(dialogArea);
}

void kiptablesgenerator::slotShowServiceDialog()
{
  // Reset dialog
  ((KComboBox *) namedWidgets["newService_protocols"])->setCurrentItem(0);
  slotChangedProtocol(0);
  ((QRadioButton*) namedWidgets["newService_named"])->setChecked(true);
  ((KComboBox *) namedWidgets["newService_names"])->setCurrentItem(0);
  ((KLineEdit*) namedWidgets["newService_ports"])->setText("");
  ((QRadioButton*) namedWidgets["newService_allow"])->setChecked(true);
  slotServiceNamedChanged(true);

  newServiceDialog->show();
}

void kiptablesgenerator::setupNewServiceDialog()
{
  newServiceDialog = new KDialogBase(this, 0, true, i18n("Add Service"), KDialogBase::Ok | KDialogBase::Cancel);
  
  QFrame *dialogArea = new QFrame(newServiceDialog);
  QGridLayout *layout = new QGridLayout(dialogArea, 7, 2);
  layout->setSpacing(KDialogBase::spacingHint());
  
  KActiveLabel *intro = new KActiveLabel(i18n(
    "<p><i>Advanced users only</i></p>"
    "<p>Here you can allow or deny access to services through your firewall.<br />"
    "You can specify a port range in the box using this format: <tt>fromPort:toPort</tt>, "
    "or you can specify multiple ports by seperating them with commas.</p>"), dialogArea);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);
  
  QLabel *protocolLabel = new QLabel(i18n("&Protocol: "), dialogArea);
  protocolLabel->show();
  layout->addWidget(protocolLabel, 1, 0);
 
  KComboBox *protocols = new KComboBox(dialogArea);
  protocols->insertItem(i18n("TCP"));
  protocols->insertItem(i18n("UDP"));
  protocols->insertItem(i18n("TCP & UDP"));
  protocols->insertItem(i18n("ICMP"));
  protocols->setCurrentItem(2);
  protocols->show();
  layout->addWidget(protocols, 1, 1);
  protocolLabel->setBuddy(protocols);
  namedWidgets["newService_protocols"] = protocols;
  connect(protocols, SIGNAL(activated(int )), this, SLOT(slotChangedProtocol(int )));
  
  QLabel *selectByLabel = new QLabel(i18n("Select service by: "), dialogArea);
  selectByLabel->show();
  layout->addMultiCellWidget(selectByLabel, 2, 2, 0, 1);
  
  QButtonGroup *optNamedOrNumbered = new QButtonGroup(dialogArea);
  optNamedOrNumbered->hide();
  
  QRadioButton *optNamed = new QRadioButton(i18n("&Name: "), dialogArea);
  optNamed->setChecked(true);
  optNamed->setName("named");
  optNamedOrNumbered->insert(optNamed);
  optNamed->show();
  layout->addWidget(optNamed, 3, 0);
  namedWidgets["newService_named"] = optNamed;
  connect(optNamed, SIGNAL(toggled(bool )), this, SLOT(slotServiceNamedChanged(bool )));
  
  KComboBox *names = new KComboBox(dialogArea);
  names->show();
  layout->addWidget(names, 3, 1);
  namedWidgets["newService_names"] = names;
  
  QRadioButton *optNumbered = new QRadioButton(i18n("&Port number(s): "), dialogArea);
  optNumbered->setName("numbered");
  optNamedOrNumbered->insert(optNumbered);
  optNumbered->show();
  layout->addWidget(optNumbered, 4, 0);
  namedWidgets["newService_numbered"] = optNumbered;
  
  KLineEdit *ports = new KLineEdit(dialogArea);
  ports->show();
  layout->addWidget(ports, 4, 1);
  namedWidgets["newService_ports"] = ports;
  
  QButtonGroup *optAllowDeny = new QButtonGroup(dialogArea);
  optAllowDeny->hide();
  
  KSeparator *separator = new KSeparator(dialogArea);
  separator->show();
  layout->addMultiCellWidget(separator, 5, 5, 0, 1);
  
  QRadioButton *optAllow = new QRadioButton(i18n("&Accept"), dialogArea);
  optAllow->setName(i18n("Accept"));
  optAllow->setChecked(true);
  optAllow->show();
  optAllowDeny->insert(optAllow);
  layout->addWidget(optAllow, 6, 0);
  namedWidgets["newService_allow"] = optAllow;
  
  QRadioButton *optDeny = new QRadioButton(i18n("&Drop"), dialogArea);
  optDeny->setName(i18n("Drop"));
  optDeny->show();
  optAllowDeny->insert(optDeny);
  layout->addWidget(optDeny, 6, 1);
    
  dialogArea->show();
  newServiceDialog->setMainWidget(dialogArea);
  connect(newServiceDialog, SIGNAL(okClicked()), this, SLOT(slotAddService()));
  slotChangedProtocol(2); // TCP+UDP
}

void kiptablesgenerator::slotShowHostDialog()
{
  ((QRadioButton*) namedWidgets["newHost_allow"])->setChecked(true);
  ((QRadioButton*) namedWidgets["newHost_useIP"])->setChecked(true);
  ((KLineEdit*) namedWidgets["newHost_address"])->setText("");
  
  newHostDialog->show();
}

void kiptablesgenerator::slotAddHost()
{
  KListView *hosts = (KListView*) namedWidgets["hostsList"];
  
  QString allowOrBlock, ipOrMAC;
  ((QRadioButton*) namedWidgets["newHost_allow"])->isChecked()
    ? allowOrBlock = i18n("Allow")
    : allowOrBlock = i18n("Block");
  ((QRadioButton*) namedWidgets["newHost_useIP"])->isChecked()
    ? ipOrMAC = i18n("IP")
    : ipOrMAC = i18n("MAC");
  QString host = ((KLineEdit*) namedWidgets["newHost_address"])->text();
  
  KListViewItem *item = new KListViewItem(hosts,
    allowOrBlock,
    ipOrMAC,
    host);
  item = 0; //unused variable warnings  
}

void kiptablesgenerator::slotShowForwardDialog()
{
  ((KLineEdit*) namedWidgets["forward_port"])->setText("");
  ((KLineEdit*) namedWidgets["forward_destination"])->setText("");
  ((QRadioButton*) namedWidgets["forward_incoming"])->setChecked(true);
  
  newForwardDialog->show();
}

void kiptablesgenerator::slotMasqueradingEnabled(bool isEnabled)
{
	namedWidgets["masqueradeLabel1"]->setEnabled(isEnabled);
	namedWidgets["masqueradeLabel2"]->setEnabled(isEnabled);
	namedWidgets["masqueradeIntIfs"]->setEnabled(isEnabled);
	namedWidgets["masqueradeExtIf"]->setEnabled(isEnabled);
}

void kiptablesgenerator::slotAddForward()
{
  KListView *forwards = (KListView*) namedWidgets["forwardsList"];
  QString localPort = ((KLineEdit*) namedWidgets["forward_port"])->text();
  QString destination = ((KLineEdit*) namedWidgets["forward_destination"])->text();

  QString direction;
  ((QRadioButton*) namedWidgets["forward_incoming"])->isChecked()
    ? direction = i18n("Incoming")
    : direction = i18n("Outgoing");

  KListViewItem *item = new KListViewItem(forwards,
    direction,
    localPort,
    destination);
    item = 0; // stop unused variable warnings
}

void kiptablesgenerator::slotServiceNamedChanged(bool named)
{
  namedWidgets["newService_names"]->setEnabled(named);
  namedWidgets["newService_ports"]->setEnabled(!named);
}

void kiptablesgenerator::slotAddService()
{
  KListView *ports = (KListView*) namedWidgets["iPorts"];
  QString protoName = ((KComboBox*) namedWidgets["newService_protocols"])->currentText();
  QString portName, portNumber;
  QString action;
  ((QRadioButton*) namedWidgets["newService_allow"])->isChecked()
    ? action = i18n("Allow")
    : action = i18n("Deny");
  
  if (protoName != i18n("ICMP"))
  {
    if (((QRadioButton*) namedWidgets["newService_named"])->isChecked())
    {
      portName = ((KComboBox*) namedWidgets["newService_names"])->currentText();
      struct servent *service;
      service = getservbyname(portName.lower().ascii(), NULL);
      portNumber = QString::number(ntohs(service->s_port));
    }
    else
    {
      portName = "";
      portNumber = ((KLineEdit*) namedWidgets["newService_ports"])->text();
    }
  }
  else
  {
    portNumber = "";
    portName = ((KComboBox*) namedWidgets["newService_names"])->currentText().stripWhiteSpace();
  }
  
  KListViewItem *item = new KListViewItem(ports,
    portNumber,
    protoName,
    action,
    portName);
  item = 0; // stop unused variable warnings
}

void kiptablesgenerator::setupIDefensiveChecksPage()
{
  iDefensiveChecksPage = new QFrame(this);
  
  QVBoxLayout *layout = new QVBoxLayout(iDefensiveChecksPage);
  layout->setSpacing(KDialogBase::spacingHint());
  
  KActiveLabel *summary = new KActiveLabel(i18n(
    "<p>This page allows you to enable additional checks to help protect your system "
    "from common firewall evasion techniques, DOS attacks, and other malicious activities.</p>"
    "<p>If you don't understand the above, the defaults provided are suitable for most systems.</p>"),
    iDefensiveChecksPage);
  summary->show();
  layout->addWidget(summary);
  
  QCheckBox *localSpoof = new QCheckBox(i18n("&Block spoofed packets"), iDefensiveChecksPage);
  localSpoof->setChecked(true);
  localSpoof->show();
  layout->addWidget(localSpoof);
  namedWidgets["iCheckLocalSpoof"] = localSpoof;
  
  QCheckBox *synFlood = new QCheckBox(i18n("&SYN-flood protection"), iDefensiveChecksPage);
  synFlood->setChecked(true);
  synFlood->show();
  layout->addWidget(synFlood);
  namedWidgets["iSynFloodProtect"] = synFlood;
  
  QCheckBox *checkSyn = new QCheckBox(i18n("&Check new connections start with a SYN packet"), iDefensiveChecksPage);
  checkSyn->setChecked(true);
  checkSyn->show();
  layout->addWidget(checkSyn);
  namedWidgets["iCheckSyn"] = checkSyn;
  
  QCheckBox *checkSynFin = new QCheckBox(i18n("&Drop packets with both SYN and FIN flags set"), iDefensiveChecksPage);
  checkSynFin->setChecked(true);
  checkSynFin->show();
  layout->addWidget(checkSynFin);
  namedWidgets["iCheckSynFin"] = checkSynFin;
  
  QCheckBox *sysctlSmurf = new QCheckBox(i18n("&Ignore ICMP echo broadcasts (helps protect against smurf attacks)"), iDefensiveChecksPage);
  sysctlSmurf->setChecked(true);
  sysctlSmurf->show();
  layout->addWidget(sysctlSmurf);
  namedWidgets["sysctlSmurf"] = sysctlSmurf;
  
  QCheckBox *sysctlIgnoreRedirects = new QCheckBox(i18n("I&gnore ICMP redirects"), iDefensiveChecksPage);
  sysctlIgnoreRedirects->setChecked(true);
  sysctlIgnoreRedirects->show();
  layout->addWidget(sysctlIgnoreRedirects);
  namedWidgets["sysctlIgnoreRedirects"] = sysctlIgnoreRedirects;
  
  QCheckBox *sysctlIgnoreSourceRouted = new QCheckBox(i18n("Ig&nore packets that use IP source routing"), iDefensiveChecksPage);
  sysctlIgnoreSourceRouted->setChecked(true);
  sysctlIgnoreSourceRouted->show();
  layout->addWidget(sysctlIgnoreSourceRouted);
  namedWidgets["sysctlIgnoreSourceRouted"] = sysctlIgnoreSourceRouted;
  
  layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Ignored));
  
  iDefensiveChecksPage->show();
  this->addPage(iDefensiveChecksPage, i18n("Defensive Checks"));
}

void kiptablesgenerator::slotNewInterface()
{
  QString interface;
  KListBox *interfaces = (KListBox*) namedWidgets["iInterfaces"];
  
  interface = KInputDialog::getText(i18n("Add Interface"),
    i18n("Enter the name of the interface, eg eth1"));
  if (interface == "")
    return;
  
  interfaces->insertItem(interface);
  interfaces->setSelected(interfaces->count() - 1, true);
}

void kiptablesgenerator::slotUpService()
{
  QListView* ports = (QListView*) namedWidgets["iPorts"];
  QListViewItem *sel = ports->selectedItem(), *iabove = 0;
  
  if (sel)
    iabove=sel->itemAbove(); // Only check itemAbove() if it exists...

  if (sel  && iabove)
    iabove->moveItem(sel); //Move the Item
}

void kiptablesgenerator::slotDownService()
{
  QListView* ports = (QListView*) namedWidgets["iPorts"];
  QListViewItem *sel = ports->selectedItem(), *ibelow = 0;
  
  if (sel)
    ibelow=sel->itemBelow(); // Only check itemAbove() if it exists...

  if (sel  && ibelow)
    sel->moveItem(ibelow); //Move the Item
}

void kiptablesgenerator::slotDelService()
{
  QListView* ports = (QListView*) namedWidgets["iPorts"];
  QListViewItem *sel = ports->selectedItem();
  if (sel) delete sel;
}

void kiptablesgenerator::slotDelForward()
{
  QListView* forwards = (QListView*) namedWidgets["forwardsList"];
  QListViewItem *sel = forwards->selectedItem();
  if (sel) delete sel;
}

void kiptablesgenerator::slotDelHost()
{
  QListView* hosts = (QListView*) namedWidgets["hostsList"];
  QListViewItem *sel = hosts->selectedItem();
  if (sel) delete sel;
}

void kiptablesgenerator::accept()
{
  QString rulesList, undoList;

	if ( currentOS == KIPTG_LINUX ) linuxOutput(rulesList, undoList);
 
  this->hide();
  makeScript(rulesList, undoList, ((KComboBox*) namedWidgets["distro"])->currentItem());
}

void kiptablesgenerator::linuxOutput(QString& rulesList, QString& undoList)
{
	QStringList sections;

  if (((QCheckBox*) namedWidgets["incomingBool"])->isChecked())
  {
  	rulesList = "##### Set the incoming policy - this decides what happens with unmatches packets #####\n";
    if ( ((KComboBox*) namedWidgets["incomingPolicy"])->currentItem() == 0)
      rulesList += "$IPTABLES -P INPUT ACCEPT\n";
    else
      rulesList += "$IPTABLES -P INPUT DROP\n";
    
    sections.append(rulesList);
      
    rulesList = "##### Interfaces whitelist #####\n";
    KListBox* interfaces = (KListBox*) namedWidgets["iInterfaces"];
    for (unsigned int i = 0; i < interfaces->count(); i++)
    {
      QListBoxItem* interface = interfaces->item(i);
      if (! interface->isSelected())
          rulesList += QString("$IPTABLES -A INPUT -i %1 -j ACCEPT\n").arg(interface->text());
    }
    sections.append(rulesList);
    
    rulesList = "##### Hosts whitelist #####\n";
    
    KListView* hosts = (KListView*) namedWidgets["hostsList"];
    QListViewItem* host = hosts->firstChild();
    while (host)
    {
      QString
        accept = host->text(0),
        ipOrMAC = host->text(1),
        address = host->text(2),
        action;
      accept == i18n("Allow")
        ? action = "ACCEPT"
        : action = "DROP";
      ipOrMAC == i18n("IP")
        ? rulesList += QString("$IPTABLES -A INPUT -s %1 -j %2\n").arg(address).arg(action)
        : rulesList += QString("$IPTABLES -A INPUT -m mac --mac-source %1 -j %2\n").arg(address).arg(action);
      host = host->nextSibling();
    }
    
    sections.append(rulesList);
    
    rulesList = "##### Assorted defensive checks #####\n";
 
    if (((QCheckBox *) namedWidgets["iCheckLocalSpoof"])->isChecked())
      rulesList += "$IPTABLES -A INPUT ! -i lo -d 127.0.0.0/8 -j DROP\n";
    if (((QCheckBox *) namedWidgets["iSynFloodProtect"])->isChecked())
    {
      rulesList += "$IPTABLES -N Flood-Scan\n";
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp --syn -j Flood-Scan\n";
      rulesList += "$IPTABLES -A Flood-Scan -m limit --limit 1/s --limit-burst 20 -j RETURN\n";
      rulesList += "$IPTABLES -A Flood-Scan -j DROP\n";
      undoList += "$IPTABLES -X Flood-Scan\n";
    }
    if (((QCheckBox *) namedWidgets["iCheckSyn"])->isChecked())
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp ! --syn -m conntrack --ctstate NEW -j DROP\n";
    if (((QCheckBox *) namedWidgets["iCheckSynFin"])->isChecked())
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp --tcp-flags SYN,FIN SYN,FIN -j DROP\n";
      
  	sections.append(rulesList);
  	
  	rulesList = "##### sysctl-based defenses #####\n";
  	
  	if (((QCheckBox *) namedWidgets["iCheckLocalSpoof"])->isChecked())
  	{
    	rulesList += "# Help protect against spoofing\n"
    		"for i in /proc/sys/net/ipv4/conf/*/rp_filter; do\n"
        "\techo 1 > $i;\n"
      	"done\n";
      undoList +=
      	"for i in /proc/sys/net/ipv4/conf/*/rp_filter; do\n"
        "\t echo 0 > $i; \n"
        "done\n";
  	}
    if (((QCheckBox *) namedWidgets["sysctlSmurf"])->isChecked())
    {
    	rulesList +=
    		"# don't participate in smurf attacks\n"
    		"echo 1 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts\n";
    	undoList += "echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts\n";
    }
    if (((QCheckBox *) namedWidgets["sysctlIgnoreRedirects"])->isChecked())
    {
    	rulesList +=
    		"# Ignore ICMP redirects\n"
    		"for i in /proc/sys/net/ipv4/conf/*/accept_redirects ; do\n"
    		"\techo 0 > $i;\n"
    		"done\n";
      undoList +=
      	"for i in /proc/sys/net/ipv4/conf/*/accept_redirects ; do\n"
        "\techo 1 > $i;\n"
        "done\n";
    }
    if (((QCheckBox *) namedWidgets["sysctlIgnoreSourceRouted"])->isChecked())
    {
    	rulesList +=
    		"# Ignore packets with source routing\n"
    		"for i in /proc/sys/net/ipv4/conf/*/accept_source_route ; do\n"
        "\techo 0 > $i;\n"
        "done\n";
      undoList +=
      	"for i in /proc/sys/net/ipv4/conf/*/accept_source_route ; do\n"
      	"\techo 1 > $i;\n"
      	"done\n";
    }
    
    sections.append(rulesList);
    
    rulesList = "##### Connection tracking rules #####\n";
    
    if ( ((QCheckBox*) namedWidgets["iConntrackAllSame"])->isChecked() )
    {
      if ( ((QCheckBox*) namedWidgets["iConntrackAllEstablished"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackAllRelated"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackAllNew"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -m conntrack --ctstate NEW -j ACCEPT\n";
    }
    else
    {
      if ( ((QCheckBox*) namedWidgets["iConntrackTcpEstablished"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p tcp -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackTcpRelated"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p tcp -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackTcpNew"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p tcp -m conntrack --ctstate NEW -j ACCEPT\n";
        
      if ( ((QCheckBox*) namedWidgets["iConntrackUdpEstablished"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p udp -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackUdpRelated"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p udp -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackUdpNew"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p udp -m conntrack --ctstate NEW -j ACCEPT\n";
      
      if ( ((QCheckBox*) namedWidgets["iConntrackICMPEstablished"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p icmp -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackICMPRelated"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p icmp -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( ((QCheckBox*) namedWidgets["iConntrackICMPNew"])->isChecked() )
        rulesList += "$IPTABLES -A INPUT -p icmp -m conntrack --ctstate NEW -j ACCEPT\n";
    }
    
    sections.append(rulesList);
    
    rulesList = "##### Rules to allow by ports and/or ICMP type #####\n";

    KListView* services = (KListView*) namedWidgets["iPorts"];
    QListViewItem* service = services->firstChild();
    while (service)
    {
      // columns: portNumber, protoName, action, portName
      QString
        portNumber = service->text(0),
        protocol = service->text(1),
        action = service->text(2),
        portName = service->text(3);
      
      action == i18n("Accept") ? action = "ACCEPT" : action = "DROP";
      if ( ! portNumber.contains(",") )
      {
        if (protocol == i18n("TCP & UDP") || protocol == i18n("TCP"))
          rulesList += QString("$IPTABLES -A INPUT -p tcp -m tcp --dport %1 -j %2\n").arg(portNumber).arg(action);
        if (protocol == i18n("TCP & UDP") || protocol == i18n("UDP"))
          rulesList += QString("$IPTABLES -A INPUT -p udp -m udp --dport %1 -j %2\n").arg(portNumber).arg(action);
      } else {
        if (protocol == i18n("TCP & UDP") || protocol == i18n("TCP"))
          rulesList += QString("$IPTABLES -A INPUT -p tcp -m multiport --dports %1 -j %2\n").arg(portNumber).arg(action);
        if (protocol == i18n("TCP & UDP") || protocol == i18n("UDP"))
          rulesList += QString("$IPTABLES -A INPUT -p udp -m multiport --dports %1 -j %2\n").arg(portNumber).arg(action);
      }
      if (protocol == i18n("ICMP"))
        rulesList += QString("$IPTABLES -A INPUT -p icmp -m icmp --icmp-type %1 -j %2\n").arg(portName).arg(action);
      service = service->nextSibling();
    }
    
    sections.append(rulesList);
    
    rulesList = "##### Port forwarding #####\n";

    KListView* forwards = (KListView*) namedWidgets["forwardsList"];
    QListViewItem* forward = forwards->firstChild();
    while (forward)
    {
      QString
        direction = forward->text(0),
        localPort = forward->text(1),
        destination = forward->text(2);
      direction == i18n("Incoming")
        ? rulesList += QString("$IPTABLES -t nat -A PREROUTING -p tcp -m tcp --dport %1 -j DNAT --to %2\n").arg(localPort).arg(destination)
        : rulesList += QString("$IPTABLES -t nat -A OUTPUT -p tcp -m tcp --dport %1 -j DNAT --to %2\n").arg(localPort).arg(destination);
      forward = forward->nextSibling();
    }
  }
  
  sections.append(rulesList);
  
  if ( ((QRadioButton*)namedWidgets["masqueradeBool"])->isChecked() )
  {
  	rulesList = "##### IP Masquerading (internet connection sharing) #####\n";
  	KListBox *mIntIfs = ((KListBox*)namedWidgets["masqueradeIntIfs"]);
    QStringList qslIntIfs;
    
    for (unsigned int i = 0; i < mIntIfs->count(); i++)
    	if ( mIntIfs->isSelected(i) )
    		qslIntIfs.append(mIntIfs->item(i)->text());
    	
  	rulesList += QString(
    	"# external interface (connected to the internet)\n"
    	"EXT_IF=\"%1\"\n"
    	"# internal interfaces (connected to your local network)\n"
      "INT_IFS=\"%2\"\n"
      "# Don't just allow everything into the internal network\n"
      "$IPTABLES -F FORWARD\n"
      "$IPTABLES -P FORWARD DROP\n"
      "for INT_IF in $INT_IFS; do\n"
      "\t$IPTABLES -A FORWARD -i $INT_IF -o $EXT_IF -j ACCEPT\n"
      "\t$IPTABLES -A FORWARD -i $EXT_IF -o $INT_IF -m state --state ESTABLISHED,RELATED -j ACCEPT\n"
      "done\n"
      "$IPTABLES -t nat -F POSTROUTING\n"
      "$IPTABLES -t nat -A POSTROUTING -o $EXT_IF -j MASQUERADE\n"
      "# Enable IP packet forwarding\n"
      "echo 1 > /proc/sys/net/ipv4/ip_forward\n").arg(
      	((KComboBox*)namedWidgets["masqueradeExtIf"])->currentText()).arg(
      	qslIntIfs.join(" ") );
    undoList += QString(
    	"$IPTABLES -t nat -F POSTROUTING\n"
    	"echo 0 > /proc/sys/net/ipv4/ip_forward\n");
    sections.append(rulesList);
  }
  rulesList = sections.join("\n");
}

void kiptablesgenerator::slotShownRules()
{
  QDialog::accept();
}

void kiptablesgenerator::slotChangedProtocol(int newProtocol)
{
  // 0 = TCP
  // 1 = UDP
  // 2 = TCP+UDP
  // 3 = ICMP
  KComboBox *names = (KComboBox*) namedWidgets["newService_names"];
  names->clear();
  if (newProtocol != 3)
  {
    names->insertItem("SSH");
    names->insertItem("Telnet");
    names->insertItem("HTTP");
    names->insertItem("HTTPS");
    names->insertItem("SMTP");
    names->insertItem("SMTPS");
    names->insertItem("POP3");
    names->insertItem("POP3S");
    names->insertItem("IMAP");
    names->insertItem("IMAPS");
    ((QRadioButton *) namedWidgets["newService_numbered"])->setEnabled(true);
    ((QLineEdit *) namedWidgets["newService_ports"])->setEnabled(true);
    return;
  }
  ((QRadioButton *) namedWidgets["newService_numbered"])->setEnabled(false);
  ((QLineEdit *) namedWidgets["newService_ports"])->setEnabled(false);
  ((QRadioButton *) namedWidgets["newService_named"])->setChecked(true);
  names->insertItem("any");
  names->insertItem("echo-reply");
  names->insertItem("destination-unreachable");
  names->insertItem(" network-unreachable");
  names->insertItem(" host-unreachable");
  names->insertItem(" protocol-unreachable");
  names->insertItem(" port-unreachable");
  names->insertItem(" fragmentation-needed");
  names->insertItem(" source-route-failed");
  names->insertItem(" network-unknown");
  names->insertItem(" host-unknown");
  names->insertItem(" network-prohibited");
  names->insertItem(" host-prohibited");
  names->insertItem(" TOS-network-unreachable");
  names->insertItem(" TOS-host-unreachable");
  names->insertItem(" communication-prohibited");
  names->insertItem(" host-precedence-violation");
  names->insertItem(" precedence-cutoff");
  names->insertItem("source-quench");
  names->insertItem("redirect");
  names->insertItem(" network-redirect");
  names->insertItem(" host-redirect");
  names->insertItem(" TOS-network-redirect");
  names->insertItem(" TOS-host-redirect");
  names->insertItem("echo-request");
  names->insertItem("router-advertisement");
  names->insertItem("router-solicitation");
  names->insertItem("time-exceeded");
  names->insertItem(" ttl-zero-during-transit");
  names->insertItem(" ttl-zero-during-reassembly");
  names->insertItem("parameter-problem");
  names->insertItem(" ip-header-bad");
  names->insertItem(" required-option-missing");
  names->insertItem("timestamp-request");
  names->insertItem("timestamp-reply");
  names->insertItem("address-mask-request");
  names->insertItem("address-mask-reply");
}

kiptablesgenerator::~kiptablesgenerator()
{
  delete newServiceDialog;

  delete welcomePage;
  delete interfacesPage;
  delete incomingPage;
  delete iPolicyPage;
  delete iPortsPage;
  delete iDefensiveChecksPage;
  delete finishedPage;
}


#include "kiptablesgenerator.moc"
