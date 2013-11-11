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
#include <ut_networkhandlingstarter_p.h>
#include <qtestmains60.h>
#include <networkhandlingstarter_p.h>

/*!
  ut_networkhandlingstarter_p::ut_networkhandlingstarter_p
 */
ut_networkhandlingstarter_p::ut_networkhandlingstarter_p() 
    : m_NetworkHandlingStarterPrivate(0),
      m_XQAiwRequest(NULL)
{
    
}

/*!
  ut_networkhandlingstarter_p::~ut_networkhandlingstarter_p
 */
ut_networkhandlingstarter_p::~ut_networkhandlingstarter_p()
{
    delete m_NetworkHandlingStarterPrivate;
}

/*!
  ut_networkhandlingstarter_p::init
 */
void ut_networkhandlingstarter_p::init()
{
    initialize();
    
    m_NetworkHandlingStarterPrivate = new NetworkHandlingStarterPrivate();
    
    QVERIFY(verify());
}

/*!
  ut_networkhandlingstarter_p::cleanup
 */
void ut_networkhandlingstarter_p::cleanup()
{
    delete m_NetworkHandlingStarterPrivate;
    m_NetworkHandlingStarterPrivate = 0;    
    reset();
}

/*!
  ut_networkhandlingstarter_p::t_LaunchNetworksettingsPlugin
 */
void ut_networkhandlingstarter_p::t_LaunchNetworksettingsPlugin()
{
    m_NetworkHandlingStarterPrivate->LaunchCpNetworkPluginView();
    XQAiwInterfaceDescriptor descriptor;
    m_XQAiwRequest = new XQAiwRequest(descriptor, "test", false);
    SmcDefaultValue<XQAiwRequest *>::SetL(m_XQAiwRequest);
    m_NetworkHandlingStarterPrivate->LaunchCpNetworkPluginView();
}

/*!
  ut_networkhandlingstarter_p::t_ShowNote
 */
void ut_networkhandlingstarter_p::t_ShowNote()
{
    m_NetworkHandlingStarterPrivate->ShowNote();
    m_NetworkHandlingStarterPrivate->ShowNote();
}

/*!
  ut_networkhandlingstarter_p::t_RemoveNote
 */
void ut_networkhandlingstarter_p::t_RemoveNote()
{
    // Create note
    m_NetworkHandlingStarterPrivate->ShowNote();
    // Test
    m_NetworkHandlingStarterPrivate->RemoveNote();
    m_NetworkHandlingStarterPrivate->RemoveNote();
}

QTEST_MAIN_S60(ut_networkhandlingstarter_p)
