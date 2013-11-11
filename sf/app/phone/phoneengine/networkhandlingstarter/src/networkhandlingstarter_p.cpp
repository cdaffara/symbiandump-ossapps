/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hbdevicemessagebox.h>
#include <hbaction.h>
#include <xqserviceutil.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <hbtranslator.h>

#include "networkhandlingstarter_p.h"
#include "networkhandlingstarterlogging.h"
#include "cnetworklistener.h"

/*!
    Constructor of NetworkHandlingStarterPrivate.
 */
NetworkHandlingStarterPrivate::NetworkHandlingStarterPrivate(QObject *parent) 
    : 
    QObject(parent), 
    m_networkListener(NULL), 
    m_note(NULL)
{
    DPRINT << ": IN";
    
    QT_TRAP_THROWING(m_networkListener = CNetworkListener::NewL(*this))
    
    DPRINT << ": OUT";
}

/*!
    Destructor of NetworkHandlingStarterPrivate.
 */
NetworkHandlingStarterPrivate::~NetworkHandlingStarterPrivate()
{
    DPRINT << ": IN";
    
    delete m_networkListener;
    if (m_note) {
        delete m_note;
    }
    
    DPRINT << ": OUT";
}
    
/*!
    NetworkHandlingStarterPrivate::ShowNote()
 */
void NetworkHandlingStarterPrivate::ShowNote()
{
    DPRINT << ": IN";
    
    if (m_note) {
        m_note->close();
        delete m_note;
        m_note = NULL;
    }
    HbTranslator commonTranslator("common");
    HbTranslator telephone_cpTranslator("telephone_cp");
    m_note = new HbDeviceMessageBox(
        hbTrId("txt_phone_info_network_lost_select_network"), 
        HbMessageBox::MessageTypeQuestion);
    HbAction *primaryAction = new HbAction(hbTrId("txt_common_button_yes"), m_note);
    m_note->setAction(primaryAction, HbDeviceMessageBox::AcceptButtonRole); 
    HbAction *secondaryAction = new HbAction(hbTrId("txt_common_button_no"), m_note);
    m_note->setAction(secondaryAction,HbDeviceMessageBox::RejectButtonRole); 
    QObject::connect(
        primaryAction, SIGNAL(triggered()),
        this, SLOT(LaunchCpNetworkPluginView()));
    m_note->setTimeout(0);
    m_note->show();
    
    DPRINT << ": OUT";
}

/*!
    NetworkHandlingStarterPrivate::RemoveNote()
 */

void NetworkHandlingStarterPrivate::RemoveNote()
{
    DPRINT << ": IN";
    
    if (m_note) {
        m_note->close();
        delete m_note;
        m_note = NULL;
    }
    
    DPRINT << ": OUT";
}

/*!
    NetworkHandlingStarterPrivate::LaunchCpNetworkPluginView()
 */
void NetworkHandlingStarterPrivate::LaunchCpNetworkPluginView()
{
    DPRINT << ": IN";
    
    XQApplicationManager appManager;
    QList<QVariant> args;
    QString service;
    QString interface;
    QString operation;
    QVariantHash hash;
    QVariantMap map;
    
    interface = "com.nokia.symbian.ICpPluginLauncher";
    operation = "launchSettingView(QString,QVariant)";
    args << QVariant("cpnetworkplugin.dll");            
    hash["command"] = "searchAvailableNetworks";
    args << hash;

    QScopedPointer<XQAiwRequest> request( service.isEmpty() ? 
        appManager.create(interface, operation, false):
        appManager.create(service, interface, operation, false));
    if (request){
        request->setArguments(args);
        request->send();
    } 
    
    DPRINT << ": OUT";
}

// End of File.
