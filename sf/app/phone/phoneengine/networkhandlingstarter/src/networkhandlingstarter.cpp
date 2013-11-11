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

#include <tstasksettings.h>
#include <networkhandlingstarter.h>
#include <networkhandlingstarterlogging.h>
#include <networkhandlingstarter_p.h>

/*!
    Constructor of NetworkHandlingStarter.
 */
NetworkHandlingStarter::NetworkHandlingStarter(QObject *parent) :
    XQServiceProvider(QLatin1String("networkhandlingstarter.com.nokia.symbian.IStart"), parent),
    m_privateImpl(NULL)
{
    // Install plugin specific msg handler
    INSTALL_TRACE_MSG_HANDLER;
    
    publishAll();
    
    DPRINT;
}

/*!
    Destructor of NetworkHandlingStarter.
 */
NetworkHandlingStarter::~NetworkHandlingStarter()
{
    DPRINT;
    delete m_privateImpl;
    // Uninstall plugin specific msg handler
    UNINSTALL_TRACE_MSG_HANDLER;
}

void NetworkHandlingStarter::start()
{
    DPRINT;
    
    if(!m_privateImpl) {
        m_privateImpl = new NetworkHandlingStarterPrivate();
    }

    // Networkhandlingstarter to be invisible in taskswitcher 
    TsTaskSettings taskSettings;
    taskSettings.setVisibility(false);
}

// End of File.
