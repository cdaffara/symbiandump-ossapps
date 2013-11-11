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
#include <ut_networkhandlingstarter.h>
#include <qtestmains60.h>
#include <networkhandlingstarter.h>

/*!
  ut_networkhandlingstarter::ut_networkhandlingstarter
 */
ut_networkhandlingstarter::ut_networkhandlingstarter() 
    : m_NetworkHandlingStarter(0)
{
}

/*!
  ut_networkhandlingstarter::~ut_networkhandlingstarter
 */
ut_networkhandlingstarter::~ut_networkhandlingstarter()
{
}

/*!
  ut_networkhandlingstarter::init
 */
void ut_networkhandlingstarter::init()
{
    initialize();
    
    m_NetworkHandlingStarter = new NetworkHandlingStarter();
    
    QVERIFY(verify());
}

/*!
  ut_networkhandlingstarter::cleanup
 */
void ut_networkhandlingstarter::cleanup()
{
    delete m_NetworkHandlingStarter;
    m_NetworkHandlingStarter = 0;
    
    reset();
}

/*!
  ut_networkhandlingstarter::t_test
 */
void ut_networkhandlingstarter::t_test()
{
    m_NetworkHandlingStarter->start();
    m_NetworkHandlingStarter->start();
}

QTEST_MAIN_S60(ut_networkhandlingstarter)
