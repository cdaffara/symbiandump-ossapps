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
#ifndef HBAPPLICATION_H
#define HBAPPLICATION_H
 
#include <qobject.h>
#include <qglobal.h> 
#include <qapplication.h>
#include <hbnamespace.h>

class HbActivityManager;

class HbApplication : public QApplication
{
public:
    HbApplication(int &/*argc*/, char */*argv*/[], Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags);
    ~HbApplication();
    
    HbActivityManager *activityManager();
    Hb::ActivationReason activateReason() const;
    QVariantHash activateParams() const;
    QString activateId() const; 
    QVariant activateData();    
    
    static void quit();

public:
    Hb::ApplicationFlags mTestFlags;
};

#endif//HBAPPLICATION_H
