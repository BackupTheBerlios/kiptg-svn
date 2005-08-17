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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("A KDE IPTables Script Generator");

static const char version[] = "1.0";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("KIptablesGenerator", I18N_NOOP("KIptablesGenerator"), version, description,
		     KAboutData::License_GPL, "(C) %{YEAR} Fred Emmott", 0, 0, "mail@fredemmott.co.uk");
    about.addAuthor( "Fred Emmott", 0, "mail@fredemmott.co.uk" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    kiptablesgenerator *mainWin = 0;

    // no session.. just start up normally
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    /// @todo do something with the command line args here

    mainWin = new kiptablesgenerator();
    app.setMainWidget( mainWin );
    mainWin->show();

    args->clear();

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

