/***************************************************************************
 *   Copyright (C) 2004-2005 by Fred Emmott                                     *
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
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

#include <kdialogbase.h>
#include <kwizard.h>

#include "constants.h"
#include "conntrackPage.h"
#include "distroPage.h"
#include "hostsPage.h"
#include "interfacesPage.h"
#include "policyPage.h"
#include "portsPage.h"
#include "textPage.h"
#include "yesNoPage.h"

#include "rulesdialog.h"

/**
@author Fred Emmott
*/
class kiptablesgenerator : public KWizard
{
Q_OBJECT
private:
  kiptg::os currentOS;
  
  QStringList mRules;
  QMap<QString, QWidget*> namedWidgets;
    
  KDialogBase *newForwardDialog;
  
  kiptg::textPage *m_welcomePage;
  kiptg::distroPage *m_distroPage;
  kiptg::interfacesPage *m_interfacesPage;
  kiptg::yesNoPage *m_incomingPage;
  kiptg::policyPage *m_policyPage;
  kiptg::conntrackPage *m_conntrackPage;
  kiptg::portsPage *m_portsPage;
  kiptg::hostsPage *m_hostsPage;
  QFrame *fForwardingPage;
  QFrame *fMasqueradingPage;
  QFrame *iDefensiveChecksPage;
  kiptg::textPage *finishedPage;

  RulesDialog* rulesDialog;

  void setupNewForwardDialog();

  void setupIHostsPage();
  void setupIDefensiveChecksPage();
  void setupFForwardingPage();
  void setupFMasqueradingPage();
  
public:
  kiptablesgenerator(QWidget *parent = 0, const char *name = 0);

  ~kiptablesgenerator();
  void makeScript(QString &rulesList, QString &undoList, int distro);
protected:
	QPtrList<QWidget> linuxOnlyWidgets;
	QPtrList<QFrame> linuxOnlyPages;
	void linuxOutput(QString &rulesList, QString &undoList);
protected slots:
  void slotShowForwardDialog();
  void slotAddForward();
  void slotDelForward();
  void slotShownRules();
  void slotMasqueradingEnabled(bool);
  void slotDistroChanged(int);
  virtual void accept();
};

#endif
