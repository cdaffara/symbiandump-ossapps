/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <hbaction.h>
#include <hbdialog.h>

#include "devicemanagementnotifierwidget_p.h"


// Constructor
devicemanagementnotifierwidget::devicemanagementnotifierwidget(const QVariantMap &parameters):devicemanagementnotifierutils(this, parameters)
{
    // Do translation
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
    
}

// Set parameters
bool devicemanagementnotifierwidget::setDeviceDialogParameters(
    const QVariantMap &parameters)
{
    
    return true;
}

// Get error
int devicemanagementnotifierwidget::deviceDialogError() const
{
    
    return 0;
}

// Close device dialog
void devicemanagementnotifierwidget::closeDeviceDialog(bool byClient)
{
    close();
    //emit deviceDialogClosed();
    
}

// Return display widget
HbDialog *devicemanagementnotifierwidget::deviceDialogWidget() const
{
   
    return const_cast<devicemanagementnotifierwidget*>(this);
}

devicemanagementnotifierwidget::~devicemanagementnotifierwidget()
    {
    qDebug("devicemanagementnotifierutils::~devicemanagementnotifierutils");
    }


void devicemanagementnotifierwidget::dmDevdialogDismissed(ENotfierType notifiertype, int response)
    {
    QVariantMap resultMap;
    resultMap.insert("notifiertype",notifiertype);
    resultMap.insert("keypress", response);
    emit deviceDialogData(resultMap);    
    }	
