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
#include "rulesdialog.h"

#include <qtextstream.h>
#include <qfile.h>
#include <qframe.h>
#include <qlayout.h>

#include <kfiledialog.h>
#include <kguiitem.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstdguiitem.h>
#include <ktempfile.h>
#include <kurl.h>
#include <kio/netaccess.h>

RulesDialog::RulesDialog(QWidget *parent, const char *name, const QString* rules)
 : KDialogBase(parent, name, true, i18n("IPTables Script"),
  KDialogBase::Close | KDialogBase::User1 | KDialogBase::User2, KDialogBase::User1, false,
    KGuiItem(i18n("C&opy")), KStdGuiItem::saveAs())
{
  mRules = new KTextEdit(this);
  setMainWidget(mRules);
  mRules->setReadOnly(true);
  mRules->setText(*rules);
  mRules->setWordWrap(QTextEdit::NoWrap);
  mRules->setMinimumSize(QSize(mRules->contentsWidth() + mRules->verticalScrollBar()->width(), mRules->height()));
  mRules->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  mRules->show();
}

void RulesDialog::slotUser1()
{
  mRules->selectAll();
  mRules->copy();
  mRules->selectAll(false);
}

void RulesDialog::slotUser2()
{
  KURL kurl = KFileDialog::getSaveURL(0, i18n("*.sh|Shell Scripts (*.sh)"), this);
  if (kurl.path() == "")
    return;

  KTempFile temp;
  QString fileName = kurl.path();

  if (fileName == "")
    return;

  if (!kurl.isLocalFile())
  {
    fileName = temp.name();
  }
  
  QFile file(fileName);
  file.open(IO_WriteOnly);
  QTextStream stream(&file);
  stream << mRules->text();
  file.close();
  if (!kurl.isLocalFile())
  {
    if (!KIO::NetAccess::upload(fileName, kurl, this))
      KMessageBox::error(this, i18n("Failed to upload file."));
  }

  temp.unlink();
}

RulesDialog::~RulesDialog()
{
}


#include "rulesdialog.moc"
