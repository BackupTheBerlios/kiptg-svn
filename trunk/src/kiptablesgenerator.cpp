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
  setupNewForwardDialog();
  setupNewServiceDialog();
  setupNewHostDialog();

  setupWelcomePage();
  setupInterfacesPage();
  setupIncomingPage();
  setAppropriate(incomingPage, false); // don't show this page
  setupIPolicyPage();
  setupIHostsPage();
  setupIConntrackPage();
  setupIPortsPage();
  setupFForwardingPage();
  setupIDefensiveChecksPage();
  setupFinishedPage();
  helpButton()->hide();
}

void kiptablesgenerator::setupIHostsPage()
{
  iHostsPage = new QFrame(this);
  
  QGridLayout *layout = new QGridLayout(iHostsPage, 4, 2);
  QLabel *label = new QLabel(i18n(
    "<p><i>Advanced users only</i> - If you wish to allow or block any specific hosts, "
    "ignoring all other rules, add them to this page.</p>"),
    iHostsPage);
  label->show();
  layout->addMultiCellWidget(label, 0, 0, 0, 1);
  
  KListView *hosts = new KListView(iHostsPage);
  hosts->addColumn(i18n("Allow/Block"));
  hosts->addColumn(i18n("MAC/IP"));
  hosts->addColumn(i18n("Host"));
  hosts->show();
  namedWidgets["hostsList"] = hosts;
  layout->addMultiCellWidget(hosts, 1, 3, 0, 0);
  
  KPushButton *addHost = new KPushButton(i18n("Add..."), iHostsPage);
  addHost->show();
  layout->addWidget(addHost, 1, 1);
  connect( addHost, SIGNAL(clicked()), newHostDialog, SLOT(show()));
  
  KPushButton *delHost = new KPushButton(i18n("Remove"), iHostsPage);
  layout->addWidget(delHost, 2, 1);
  connect( delHost, SIGNAL(clicked()), this, SLOT(slotDelHost()));
  
  this->addPage(iHostsPage, i18n("Host Control"));
}

void kiptablesgenerator::setupFForwardingPage()
{
  fForwardingPage = new QFrame(this);
  
  QGridLayout *layout = new QGridLayout(fForwardingPage, 4, 2);
  QLabel *label = new QLabel(i18n(
    "<p>If you wish to enable TCP forwardind for any ports (incoming or outgoing) add them to this page.</p>"),
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
  connect( addForward, SIGNAL(clicked()), newForwardDialog, SLOT(show()));
  
  KPushButton *delForward = new KPushButton(i18n("Remove"), fForwardingPage);
  layout->addWidget(delForward, 2, 1);
  connect( delForward, SIGNAL(clicked()), this, SLOT(slotDelForward()));
  
  this->addPage(fForwardingPage, i18n("Port Forwarding"));
}

void kiptablesgenerator::setupWelcomePage()
{
  welcomePage = new QFrame(this);

  QVBoxLayout *layout = new QVBoxLayout(welcomePage);

  QLabel *label = new QLabel(i18n(
    "<p>Welcome to KIptablesGenerator.</p>"
    "<p>This wizard will help you create your firewall rules.</p>"
    "<p><i>Troubleshooting:</i> The generated script requires "
    "iptables installed, and the netfilter conntrack, TCP, UDP, "
    "and ICMP kernel modules loaded.</p>"), welcomePage);
  label->show();  
  layout->addWidget(label);

  welcomePage->show();
  this->addPage(welcomePage, i18n("Welcome"));
}

void kiptablesgenerator::setupIncomingPage()
{
  incomingPage = new QFrame(this);
  
  QGridLayout *layout = new QGridLayout(incomingPage, 2, 2);
  
  QLabel *intro = new QLabel(i18n(
    "<p>Do you want to filter incoming data? (recommended)</p>"
    "<p>This will allow you to control other computers access to "
    "your computer.</p>"), incomingPage);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);
  
  QButtonGroup *optYesNo = new QButtonGroup(incomingPage);
  namedWidgets["incomingYesNo"] = optYesNo;
  optYesNo->hide();
  
  QRadioButton *optYes = new QRadioButton(i18n("&Yes"), incomingPage);
  optYes->setChecked(true);
  optYes->setName("yes");
  optYes->show();
  layout->addWidget(optYes, 1, 0);
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
  
  iPolicyPage->show();
  this->addPage(iPolicyPage, i18n("Default Action"));
}

void kiptablesgenerator::setupInterfacesPage()
{
  interfacesPage = new QFrame(this);
  
  QVBoxLayout *layout = new QVBoxLayout(interfacesPage);
  
  QLabel *intro = new QLabel(i18n(
    "<p>Which of the following interfaces do you want to filter?</p>"
    "<p>It is strongly advised <b>not</b> to filter '<tt>lo</tt>'.</p>"), interfacesPage);
  intro->show();
  layout->addWidget(intro);
  
  KListBox *interfaces = new KListBox(interfacesPage);

  char buffer[IFNAMSIZ];
  for(unsigned int i = 1; if_indextoname(i, buffer) != NULL; i++)
  {
          interfaces->insertItem((QString)buffer);
  }
  
  interfaces->setSelectionMode(QListBox::Multi);
  for (unsigned short i = 0; i < interfaces->count(); i++)
    if (interfaces->item(i)->text() != "lo")
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
  QGridLayout *layout = new QGridLayout(iConntrackPage, 7, 4);
  
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
  
  iConntrackPage->show();
  this->addPage(iConntrackPage, i18n("Connection Tracking"));
}

void kiptablesgenerator::setupIPortsPage()
{
  iPortsPage = new QFrame(this);
  QGridLayout *layout = new QGridLayout(iPortsPage, 6, 2);
  
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
  connect(newService, SIGNAL(clicked()), newServiceDialog, SLOT(show()));
  
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
  
  iPortsPage->show();
  this->addPage(iPortsPage, i18n("Incoming Ports"));
}

void kiptablesgenerator::setupNewHostDialog()
{
  newHostDialog = new KDialogBase(this, 0, true, i18n("Add Host"), KDialogBase::Ok | KDialogBase::Cancel);
  
  QFrame *dialogArea = new QFrame(newHostDialog);
  QGridLayout *layout = new QGridLayout(dialogArea, 5, 2);
  
  QLabel *intro = new QLabel(i18n(
    "<p>Here you can tell netfilter to allow all connections from a given host regardless of other rules, "
    "or block all connections from a given host regardless of other rules.</p>"
    "<p>You can specify a host either by IP address or MAC address.</p>"), dialogArea);
  intro->show();
  layout->addMultiCellWidget(intro, 0, 0, 0, 1);

  QButtonGroup *whitelistOrBlacklist = new QButtonGroup(dialogArea);
  whitelistOrBlacklist->hide();
  
  QRadioButton *whitelist = new QRadioButton(i18n("&Allow"), dialogArea);
  whitelist->setChecked(true);
  whitelist->show();
  layout->addWidget(whitelist, 1, 0);
  namedWidgets["newHost_allow"] = whitelist;
  whitelistOrBlacklist->insert(whitelist);
  
  QRadioButton *blacklist = new QRadioButton(i18n("&Block"), dialogArea);
  blacklist->setChecked(false);
  blacklist->show();
  layout->addWidget(blacklist, 1, 1);
  whitelistOrBlacklist->insert(blacklist);
  
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
  
  QLabel *hostLabel = new QLabel(i18n("Host:"), dialogArea);
  hostLabel->show();
  layout->addMultiCellWidget(hostLabel, 3, 3, 0, 1);
  
  KLineEdit *host = new KLineEdit(dialogArea);
  host->show();
  namedWidgets["newHost_address"] = host;
  layout->addMultiCellWidget(host, 4, 4, 0, 1);
  
  connect(newHostDialog, SIGNAL(okClicked()), this, SLOT(slotAddHost()));
  
  dialogArea->show();
  newHostDialog->setMainWidget(dialogArea);
}

void kiptablesgenerator::setupNewForwardDialog()
{
  newForwardDialog = new KDialogBase(this, 0, true, i18n("Add Forward"), KDialogBase::Ok | KDialogBase::Cancel);
  
  QFrame *dialogArea = new QFrame(newForwardDialog);
  QGridLayout *layout = new QGridLayout(dialogArea, 4, 2);
  
  QLabel *intro = new QLabel(i18n(
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

void kiptablesgenerator::setupNewServiceDialog()
{
  newServiceDialog = new KDialogBase(this, 0, true, i18n("Add Service"), KDialogBase::Ok | KDialogBase::Cancel);
  
  QFrame *dialogArea = new QFrame(newServiceDialog);
  QGridLayout *layout = new QGridLayout(dialogArea, 7, 2);
  
  QLabel *intro = new QLabel(i18n(
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
  namedWidgets["newService_namedOrNumbered"] = optNamedOrNumbered;
  
  QRadioButton *optNamed = new QRadioButton(i18n("&Name: "), dialogArea);
  optNamed->setChecked(true);
  optNamed->setName("named");
  optNamedOrNumbered->insert(optNamed);
  optNamed->show();
  layout->addWidget(optNamed, 3, 0);
  namedWidgets["newService_named"] = optNamed;
  
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
  namedWidgets["newService_allowDeny"] = optAllowDeny;
  
  KSeparator *separator = new KSeparator(dialogArea);
  separator->show();
  layout->addMultiCellWidget(separator, 5, 5, 0, 1);
  
  QRadioButton *optAllow = new QRadioButton(i18n("&Accept"), dialogArea);
  optAllow->setName(i18n("Accept"));
  optAllow->setChecked(true);
  optAllow->show();
  optAllowDeny->insert(optAllow);
  layout->addWidget(optAllow, 6, 0);
  
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

void kiptablesgenerator::slotAddService()
{
  KListView *ports = (KListView*) namedWidgets["iPorts"];
  QString protoName = ((KComboBox*) namedWidgets["newService_protocols"])->currentText();
  QString action = ((QButtonGroup*) namedWidgets["newService_allowDeny"])->selected()->name();
  QString portName, portNumber;
  
  if (protoName != i18n("ICMP"))
  {
    if (((QButtonGroup*) namedWidgets["newService_namedOrNumbered"])->selected()->name() == (QString) "named")
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
  
  QLabel *summary = new QLabel(i18n(
    "<p>This page allows you to enable additional checks to help protect your system "
    "from common firewall evasion techniques, DOS attacks, and other malicious activities.</p>"
    "<p>If you don't understand the above, the defaults provided are suitable for most systems.</p>"),
    iDefensiveChecksPage);
  summary->show();
  layout->addWidget(summary);
  
  QCheckBox *localSpoof = new QCheckBox(i18n("&Block spoofed local packets"), iDefensiveChecksPage);
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
  
  iDefensiveChecksPage->show();
  this->addPage(iDefensiveChecksPage, i18n("Defensive Checks"));
}

void kiptablesgenerator::setupFinishedPage()
{
  finishedPage = new QFrame(this);
  
  QVBoxLayout *layout = new QVBoxLayout(finishedPage);
  
  QLabel *finishedLabel = new QLabel(i18n(
    "<p><b>Congratulations!</b></p>"
    "<p>All the information required to create your firewall rules has been collected. "
    "Please finish the wizard to generate your firewall script.</p>"), finishedPage);
  finishedLabel->show();
  
  layout->addWidget(finishedLabel);
  
  finishedPage->show();
  this->addPage(finishedPage, i18n("Finished"));
  setFinishEnabled(finishedPage, true);
}

void kiptablesgenerator::slotNewInterface()
{
  QString interface;
  KListBox *interfaces = (KListBox*) namedWidgets["interfaces"];
  
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
  QString rulesList;
  rulesList =
    "#!/bin/sh\n" +
    i18n("# Generated by KIptablesGenerator\n") +
    i18n("# Copyright (c) 2004 Fred Emmott <mail@fredemmott.co.uk>\n") +
    i18n("# See KIptablesGenerator for license information.\n") + 
    i18n("# You probably want to make this a startup script, eg on\n") +
    i18n("# slackware you probably want to save this as /etc/rc.d/rc.firewall\n")+
    "IPTABLES=/usr/sbin/iptables\n";
  if (((QButtonGroup*) namedWidgets["incomingYesNo"])->selected()->name() == (QString) "yes")
  {
    if ( ((KComboBox*) namedWidgets["incomingPolicy"])->currentItem() == 0)
      rulesList += "$IPTABLES -P INPUT ACCEPT\n";
    else
      rulesList += "$IPTABLES -P INPUT DROP\n";
      
    KListBox* interfaces = (KListBox*) namedWidgets["iInterfaces"];
    for (unsigned int i = 0; i < interfaces->count(); i++)
    {
      QListBoxItem* interface = interfaces->item(i);
      if (! interface->isSelected())
          rulesList += QString("$IPTABLES -A INPUT -i %1 -j ACCEPT\n").arg(interface->text());
    }
    
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
 
    if (((QCheckBox *) namedWidgets["iCheckLocalSpoof"])->isChecked())
      rulesList += "$IPTABLES -A INPUT ! -i lo -d 127.0.0.0/8 -j DROP\n";
    if (((QCheckBox *) namedWidgets["iSynFloodProtect"])->isChecked())
    {
      rulesList += "$IPTABLES -N Flood-Scan\n";
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp --syn -j Flood-Scan\n";
      rulesList += "$IPTABLES -A Flood-Scan -m limit --limit 1/s --limit-burst 20 -j RETURN\n";
      rulesList += "$IPTABLES -A Flood-Scan -j DROP\n";
    }
    if (((QCheckBox *) namedWidgets["iCheckSyn"])->isChecked())
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp ! --syn -m conntrack --ctstate NEW -j DROP\n";
    if (((QCheckBox *) namedWidgets["iCheckSynFin"])->isChecked())
      rulesList += "$IPTABLES -A INPUT -p tcp -m tcp --tcp-flags SYN,FIN SYN,FIN -j DROP\n";
         
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
 
  this->hide();
  rulesDialog = new RulesDialog(this,(char*) 0, &rulesList);
  rulesDialog->show();
  connect(rulesDialog, SIGNAL(closeClicked()), this, SLOT(slotShownRules()));
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
