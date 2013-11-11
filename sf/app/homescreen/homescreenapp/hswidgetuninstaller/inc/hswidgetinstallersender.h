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
#ifndef HSWIDGETINSTALLERSENDER_H
#define HSWIDGETINSTALLERSENDER_H

#include <QObject>

class HsWidgetComponentDescriptor;

class HsWidgetInstallerSender : public QObject
    {
    Q_OBJECT        
public:
    HsWidgetInstallerSender( QObject *parent=0 );
    ~HsWidgetInstallerSender();
 
public slots:
    void widgetChanged(QString functionSignature, HsWidgetComponentDescriptor &widgetDescriptor);

    };

#endif
