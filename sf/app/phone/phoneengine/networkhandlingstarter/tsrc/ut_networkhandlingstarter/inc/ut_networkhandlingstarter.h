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

#ifndef UT_NETWORKHANDLINGSTARTER_H
#define UT_NETWORKHANDLINGSTARTER_H

#include <QtTest/QtTest>
#include <mockservice.h>

class NetworkHandlingStarter;

class ut_networkhandlingstarter : public QObject, public MockService
{
    Q_OBJECT

public:
    
    ut_networkhandlingstarter();
    ~ut_networkhandlingstarter();
    
private slots:

    void init();
    void cleanup();
    
    void t_test();

    
private:
    
    NetworkHandlingStarter *m_NetworkHandlingStarter;

};

#endif // UT_NETWORKHANDLINGSTARTER_H
