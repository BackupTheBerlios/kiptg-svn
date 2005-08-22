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

#include <klocale.h>

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
  
  m_masqueradingPage = new masqueradingPage(this);
  m_masqueradingPage->show();
  this->addPage(m_masqueradingPage, i18n("Masquerading"));
  
  m_defensiveChecksPage = new defensiveChecksPage(this);
  m_defensiveChecksPage->show();
  this->addPage(m_defensiveChecksPage, i18n("Defensive Checks"));

  m_finishedPage = new textPage(i18n(
    "<p><b>Congratulations!</b></p>"
    "<p>All the information required to create your firewall rules has been collected. "
    "Please finish the wizard to generate your firewall script.</p>"), this);
  this->addPage(m_finishedPage, i18n("Finished"));
  setFinishEnabled(m_finishedPage, true);
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
      currentOS = LINUX;
			break;
    default: // BSD
    	if ( currentOS == BSD )
    		return;
      currentOS = BSD;
    	break;
	}
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
 
    if (m_defensiveChecksPage->blockSpoofed())
      rulesList += "$IPTABLES -A INPUT ! -i lo -d 127.0.0.0/8 -j DROP\n";
    if (m_defensiveChecksPage->blockSynFlood())
    {
      rulesList += "$IPTABLES -N Flood-Scan\n";
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp --syn -j Flood-Scan\n";
      rulesList += "$IPTABLES -A Flood-Scan -m limit --limit 1/s --limit-burst 20 -j RETURN\n";
      rulesList += "$IPTABLES -A Flood-Scan -j DROP\n";
      undoList += "$IPTABLES -X Flood-Scan\n";
    }
    if (m_defensiveChecksPage->checkNewSyn())
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp ! --syn -m conntrack --ctstate NEW -j DROP\n";
    if (m_defensiveChecksPage->blockSynFin())
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp --tcp-flags SYN,FIN SYN,FIN -j DROP\n";
      
  	sections.append(rulesList);
  	
  	rulesList = "##### sysctl-based defenses #####\n";
  	
  	if (m_defensiveChecksPage->blockSpoofed())
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
    if (m_defensiveChecksPage->blockEchoBroadcasts())
    {
    	rulesList +=
    		"# don't participate in smurf attacks\n"
    		"echo 1 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts\n";
    	undoList += "echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts\n";
    }
    if (m_defensiveChecksPage->ignoreRedirects())
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
    if (m_defensiveChecksPage->blockSourceRouted())
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
  
  if ( m_masqueradingPage->masqEnabled() )
  {
  	rulesList = "##### IP Masquerading (internet connection sharing) #####\n";

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
      "echo 1 > /proc/sys/net/ipv4/ip_forward\n").arg(m_masqueradingPage->extIf()).arg(m_masqueradingPage->intIfs().join(" ") );
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

#include "kiptablesgenerator.moc"
