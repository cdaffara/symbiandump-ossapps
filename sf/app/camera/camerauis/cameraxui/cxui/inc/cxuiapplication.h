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

#ifndef CXUIAPPLICATION_H
#define CXUIAPPLICATION_H

#include <QObject>
#include <HbApplication>

class CxuiApplication : public HbApplication
{
    Q_OBJECT

public:
    CxuiApplication(int &argc, char *argv[]);
    ~CxuiApplication();

public: // from HbApplication
#if defined(Q_WS_S60)
    bool symbianEventFilter(const QSymbianEvent *e);
#endif // Q_WS_S60

signals:
    /*!
    * Testing framework support.
    * Application ready signal is emitted after startup when UI is fully loaded and functional.
    */
    void applicationReady();
#if defined(Q_WS_S60)
    void symbianEvent(const QSymbianEvent *e);
#endif // Q_WS_S60
};

#endif // CXUIAPPLICATION_H
