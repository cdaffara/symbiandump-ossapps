/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Bio control for Provisioning documents.
*
*/

#include <hbapplication.h>
#include <CpQtSpMainWindow.h>
#include <QTranslator>
#include <QString>
#include <QLocale>

int main(int argc, char **argv)
{ 
    HbApplication a( argc, argv );    
    
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_en.qm", path);
    if (fine)
        qApp->installTranslator(translator);

    QTranslator *commontranslator = new QTranslator();
    fine = commontranslator->load("common_" + lang, path);
    if (fine)
        qApp->installTranslator(commontranslator);
    
    CpQtSpMainWindow mainWindow;    
    mainWindow.show();    
    return a.exec();     
}

