/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include <QtTest/QtTest>

#include "pt_logscntfinder.h"
#include "pt_logscntfinderqwerty.h"



int main(int argc, char *argv[]) //on HW
{
    int samples = 100;
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-s") {
            samples = QString(argv[i+1]).toInt();
        }
    }

    QApplication app(argc, argv);
    
    PT_LogsCntFinder pt_logscntfinder( samples );
    QString resultFileName = "c:/data/others/pt_logscntfinder.txt";
    QStringList args_logsCntFinder( "PT_logscntfinder");
    args_logsCntFinder << "-o" << resultFileName;
    QTest::qExec(&pt_logscntfinder, args_logsCntFinder);
    
    PT_LogsCntFinderQwerty pt_LogsCntFinderQwerty( samples );
    resultFileName = "c:/data/others/pt_logscntfinderqwerty.txt";
    QStringList args_logsCntFinderQwerty( "PT_LogsCntFinderQwerty");
    args_logsCntFinderQwerty << "-o" << resultFileName;
    QTest::qExec(&pt_LogsCntFinderQwerty, args_logsCntFinderQwerty);
    
    return 0;   
}




 
