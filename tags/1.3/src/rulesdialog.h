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
#ifndef RULESDIALOG_H
#define RULESDIALOG_H

#include <kdialogbase.h>
#include <kpushbutton.h>
#include <ktextedit.h>

/**
@author Fred Emmott
*/
class RulesDialog : public KDialogBase
{
Q_OBJECT
private:
  KTextEdit *mRules;
public:
  RulesDialog(QWidget *parent = 0, const char *name = 0, const QString *rules = 0);

  ~RulesDialog();
protected slots:
  virtual void slotUser1();
  virtual void slotUser2();
};

#endif
