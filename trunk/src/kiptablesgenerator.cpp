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

#include "constants.h"
using namespace kiptg;

kiptablesgenerator::kiptablesgenerator(QWidget *parent, const char *name)
 : KWizard(parent, name)
{
	currentOS = LINUX;

	m_welcomePage = new textPage(i18n(
	  "<p>Welcome to KIptablesGenerator.</p>"
    "<p>This wizard will help you create your firewall rules.</p>"
    "<p><i>Troubleshooting:</i> The generated script requires "
    "iptables installed, and the netfilter conntrack, TCP, UDP, "
    "and ICMP kernel modules loaded.</p>"), this);
  m_welcomePage->show();
  this->addPage(m_welcomePage, i18n("Welcome"));
  
  m_distroPage = new distroPage(this);
  m_distroPage->show();
  this->addPage(m_distroPage, i18n("Distribution"));
  connect(m_distroPage, SIGNAL(distroChanged(int )), this, SLOT(slotDistroChanged(int)));
  
  m_interfacesPage = new interfacesPage(this);
  m_interfacesPage->show();
  this->addPage(m_interfacesPage, i18n("Interfaces"));
  
  m_incomingPage = new yesNoPage(i18n(
    "<p>Do you want to filter incoming data? (recommended)</p>"
    "<p>This will allow you to control other computer's access to "
    "your computer.</p>"), this);
  //m_incomingPage->show();
  this->addPage(m_incomingPage, i18n("Incoming Packets"));
  setAppropriate(m_incomingPage, false); // don't show this page, but create it so accept() can reference it's settings
  
  m_policyPage = new policyPage(i18n(
    "<p>What do you want your firewall to do to unmatched packets?</p>"
    "<p>Your firewall can either accept unmatched packets, or drop unmatched "
    "packets; the recommended option is to drop unmatched packets.</p>"
    "<p><i>Advanced users: This sets the policy for the INPUT chain.</i></p>"), this);
  m_policyPage->show();
  this->addPage(m_policyPage, i18n("Default Action"));
  
  m_conntrackPage = new conntrackPage(this);
  m_conntrackPage->show();
  this->addPage(m_conntrackPage, i18n("Connection Tracking"));
  
  m_portsPage = new portsPage(i18n("<p>Here you can select which services are available to the outside world.</p>"), this);
  m_portsPage->show();
  this->addPage(m_portsPage, i18n("Incoming Services"));
  
  m_hostsPage = new hostsPage(i18n(
      "<p><i>Advanced users only</i> - If you wish to allow or block any specific hosts, "
      "ignoring all other rules, add them to this page.</p>"), i18n(
    "<p>Here you can tell netfilter to allow all connections from a given host regardless of other rules, "
    "or block all connections from a given host regardless of other rules.</p>"), this);
  m_hostsPage->show();
  this->addPage(m_hostsPage, i18n("Host Control"));
  
  m_forwardsPage = new forwardingPage(i18n(
    "<p>If you wish to enable TCP forwarding for any ports (incoming or outgoing) add them to this page.</p>"),
    i18n(
    "<p><i>Advanced users only</i></p>"
    "<p>Here you can tell netfilter to forward connections to given ports to another address/port.</p>"
    "<p>This is using netfilter's DNAT functionality - incoming redirects go in the prerouting chain,"
    "outgoing redirects go in the output chain.</p>"
    "<p>The destination should be of the from destination.computer.ip.address:destinationPort</p>"), this);
  m_forwardsPage->show();
  this->addPage(m_forwardsPage, i18n("Port Forwarding"));
  
  setupFMasqueradingPage();
  setupIDefensiveChecksPage();
  finishedPage = new textPage(i18n(
    "<p><b>Congratulations!</b></p>"
    "<p>All the information required to create your firewall rules has been collected. "
    "Please finish the wizard to generate your firewall script.</p>"), this);
  this->addPage(finishedPage, i18n("Finished"));
  setFinishEnabled(finishedPage, true);
  helpButton()->hide();
}

void kiptablesgenerator::slotDistroChanged(int distro)
{
	switch (distro)
	{
		case GENERIC_LINUX:
		case SLACKWARE:
		case GENTOO:
			if ( currentOS == LINUX )
				return;
      for ( unsigned int i = 0; i < linuxOnlyPages.count(); i++)
      	setAppropriate(linuxOnlyPages.at(i), true);
      for ( unsigned int i = 0; i < linuxOnlyWidgets.count(); i++)
      	linuxOnlyWidgets.at(i)->setEnabled(true);
      currentOS = LINUX;
			break;
    default: // BSD
    	if ( currentOS == BSD )
    		return;
      for ( unsigned int i = 0; i < linuxOnlyPages.count(); i++)
      	setAppropriate(linuxOnlyPages.at(i), false);
      for ( unsigned int i = 0; i < linuxOnlyWidgets.count(); i++)
      	linuxOnlyWidgets.at(i)->setEnabled(false);
      currentOS = BSD;
    	break;
	}
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
  	case GENERIC_LINUX:
  		output = "#!/bin/sh\n" + copyrightText + "IPTABLES=/usr/sbin/iptables\n\n" + rulesList;
  		break;
    case SLACKWARE:
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
    case GENTOO:
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

void kiptablesgenerator::slotMasqueradingEnabled(bool isEnabled)
{
	namedWidgets["masqueradeLabel1"]->setEnabled(isEnabled);
	namedWidgets["masqueradeLabel2"]->setEnabled(isEnabled);
	namedWidgets["masqueradeIntIfs"]->setEnabled(isEnabled);
	namedWidgets["masqueradeExtIf"]->setEnabled(isEnabled);
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

void kiptablesgenerator::accept()
{
  QString rulesList, undoList;

	if ( currentOS == LINUX ) linuxOutput(rulesList, undoList);
 
  this->hide();
  makeScript(rulesList, undoList, m_distroPage->getDistro());
}

void kiptablesgenerator::linuxOutput(QString& rulesList, QString& undoList)
{
	QStringList sections;

  if (m_incomingPage->value())
  {
  	rulesList = "##### Set the incoming policy - this decides what happens with unmatches packets #####\n";
    if (m_policyPage->value() == kiptg::ACCEPT)
      rulesList += "$IPTABLES -P INPUT ACCEPT\n";
    else
      rulesList += "$IPTABLES -P INPUT DROP\n";
    
    sections.append(rulesList);
      
    rulesList = "##### Interfaces whitelist #####\n";
    QStringList interfaces = m_interfacesPage->getWhitelistInterfaces();
    for (unsigned int i = 0; i < interfaces.count(); i++)
	    rulesList += QString("$IPTABLES -A INPUT -i %1 -j ACCEPT\n").arg(interfaces[i]);
    sections.append(rulesList);
    
    rulesList = "##### Hosts whitelist #####\n";
    
    QValueVector<struct Host> hosts = m_hostsPage->getHosts();
    for (unsigned int i = 0; i < hosts.count(); i++)
    {
    	struct Host host = hosts[i];
      QString action;
      host.accept
        ? action = "ACCEPT"
        : action = "DROP";
      host.useIP
        ? rulesList += QString("$IPTABLES -A INPUT -s %1 -j %2\n").arg(host.address).arg(action)
        : rulesList += QString("$IPTABLES -A INPUT -m mac --mac-source %1 -j %2\n").arg(host.address).arg(action);
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
    
    if ( m_conntrackPage->allSame() )
    {
      if ( (m_conntrackPage->getAll() & kiptg::ESTABLISHED) != 0 )
        rulesList += "$IPTABLES -A INPUT -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( (m_conntrackPage->getAll() & kiptg::RELATED) != 0 )
        rulesList += "$IPTABLES -A INPUT -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( (m_conntrackPage->getAll() & kiptg::NEW) != 0)
        rulesList += "$IPTABLES -A INPUT -m conntrack --ctstate NEW -j ACCEPT\n";
    }
    else
    {
      if ( (m_conntrackPage->getTCP() & kiptg::ESTABLISHED) != 0 )
        rulesList += "$IPTABLES -A INPUT -p tcp -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( (m_conntrackPage->getTCP() & kiptg::RELATED) != 0)
        rulesList += "$IPTABLES -A INPUT -p tcp -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( (m_conntrackPage->getTCP() & kiptg::NEW) != 0)
        rulesList += "$IPTABLES -A INPUT -p tcp -m conntrack --ctstate NEW -j ACCEPT\n";
      
      if ( (m_conntrackPage->getUDP() & kiptg::ESTABLISHED) != 0 )
        rulesList += "$IPTABLES -A INPUT -p udp -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( (m_conntrackPage->getUDP() & kiptg::RELATED) != 0)
        rulesList += "$IPTABLES -A INPUT -p udp -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( (m_conntrackPage->getUDP() & kiptg::NEW) != 0)
        rulesList += "$IPTABLES -A INPUT -p udp -m conntrack --ctstate NEW -j ACCEPT\n";

      if ( (m_conntrackPage->getICMP() & kiptg::ESTABLISHED) != 0 )
        rulesList += "$IPTABLES -A INPUT -p icmp -m conntrack --ctstate ESTABLISHED -j ACCEPT\n";
      if ( (m_conntrackPage->getICMP() & kiptg::RELATED) != 0 )
        rulesList += "$IPTABLES -A INPUT -p icmp -m conntrack --ctstate RELATED -j ACCEPT\n";
      if ( (m_conntrackPage->getICMP() & kiptg::NEW) != 0 )
        rulesList += "$IPTABLES -A INPUT -p icmp -m conntrack --ctstate NEW -j ACCEPT\n";
    }
    
    sections.append(rulesList);
    
    rulesList = "##### Rules to allow by ports and/or ICMP type #####\n";

    QValueVector<struct kiptg::Service> services = m_portsPage->getServices();

    for (unsigned int i = 0; i < services.count(); i++)
    {
    	struct Service service = services[i];
    	QString action;
      service.action == i18n("Allow") ? action = "ACCEPT" : action = "DROP";
      if ( ! service.portNumber.contains(",") )
      {
        if (service.protocol == i18n("TCP & UDP") || service.protocol == i18n("TCP"))
          rulesList += QString("$IPTABLES -A INPUT -p tcp -m tcp --dport %1 -j %2\n").arg(service.portNumber).arg(action);
        if (service.protocol == i18n("TCP & UDP") || service.protocol == i18n("UDP"))
          rulesList += QString("$IPTABLES -A INPUT -p udp -m udp --dport %1 -j %2\n").arg(service.portNumber).arg(action);
      } else {
        if (service.protocol == i18n("TCP & UDP") || service.protocol == i18n("TCP"))
          rulesList += QString("$IPTABLES -A INPUT -p tcp -m multiport --dports %1 -j %2\n").arg(service.portNumber).arg(action);
        if (service.protocol == i18n("TCP & UDP") || service.protocol == i18n("UDP"))
          rulesList += QString("$IPTABLES -A INPUT -p udp -m multiport --dports %1 -j %2\n").arg(service.portNumber).arg(action);
      }
      if (service.protocol == i18n("ICMP"))
        rulesList += QString("$IPTABLES -A INPUT -p icmp -m icmp --icmp-type %1 -j %2\n").arg(service.portName).arg(action);
    }
    
    sections.append(rulesList);
    
    rulesList = "##### Port forwarding #####\n";

    QValueVector<struct kiptg::Forward> forwards = m_forwardsPage->getForwards();
    
    for(unsigned int i = 0; i < forwards.count(); i++)
    {
    	struct kiptg::Forward forward = forwards[i];
      forward.direction == kiptg::INCOMING
        ? rulesList += QString(
        	"$IPTABLES -t nat -A PREROUTING -p tcp -m tcp --dport %1 -j DNAT --to %2\n").arg(forward.from).arg(forward.to)
        : rulesList += QString(
        	"$IPTABLES -t nat -A OUTPUT -p tcp -m tcp --dport %1 -j DNAT --to %2\n").arg(forward.from).arg(forward.to);
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

kiptablesgenerator::~kiptablesgenerator()
{
  delete iDefensiveChecksPage;
  delete finishedPage;
}


#include "kiptablesgenerator.moc"
