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
#ifndef KIPTABLESGENERATOR_H
#define KIPTABLESGENERATOR_H

#include <qframe.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

#include <kdialogbase.h>
#include <kwizard.h>

#include "rulesdialog.h"

/**
@author Fred Emmott
*/
class kiptablesgenerator : public KWizard
{
Q_OBJECT
private:
  QStringList mRules;
  QMap<QString, QWidget*> namedWidgets;
  QMap<QString, unsigned int> namesToPorts;

  KDialogBase *newServiceDialog;
  KDialogBase *newForwardDialog;
	KDialogBase *newHostDialog;

  QFrame *welcomePage;
  QFrame *interfacesPage;
  QFrame *incomingPage;
  QFrame *iPolicyPage;
  QFrame *iHostsPage;
  QFrame *iConntrackPage;
  QFrame *iPortsPage;
  QFrame *fForwardingPage;
  QFrame *iDefensiveChecksPage;
  QFrame *finishedPage;

  RulesDialog* rulesDialog;

  void setupNewServiceDialog();
  void setupNewForwardDialog();
  void setupNewHostDialog();

  void setupWelcomePage();
  void setupInterfacesPage();
  void setupIncomingPage();
  void setupIPolicyPage();
  void setupIHostsPage();
  void setupIConntrackPage();
  void setupIPortsPage();
  void setupIDefensiveChecksPage();
  void setupFForwardingPage();
  void setupFinishedPage();
public:
  kiptablesgenerator(QWidget *parent = 0, const char *name = 0);

  ~kiptablesgenerator();
protected slots:
  void slotNewInterface();
  void slotChangedProtocol(int);
  void slotShowServiceDialog();
  void slotAddService();
  void slotDelService();
  void slotShowForwardDialog();
  void slotAddForward();
  void slotDelForward();
  void slotAddHost();
  void slotDelHost();
  void slotUpService();
  void slotDownService();
  void slotShownRules();
  virtual void accept();
};

#endif
