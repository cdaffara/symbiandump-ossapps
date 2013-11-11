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
* Description: Adjusts visibility of Phone app in device lock situations.
*
*/
#include <eikenv.h>
#include <xqserviceutil.h>
#include <telinformationpskeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <apgtask.h>
#include <ccallinformation.h>
#include <ccallinfoiter.h>
#include <keyguardaccessapi.h>
#include "phonevisibilityhandler.h"
#include "cphonepubsubproxy.h"
#include "phoneconstants.h"
#include "phoneuiqtviewif.h"
#include "qtphonelog.h"

/*!
    PhoneVisibilityHandler::PhoneVisibilityHandler.
 */
PhoneVisibilityHandler::PhoneVisibilityHandler(PhoneUIQtViewIF &view, QObject *parent)
    : 
    QObject(parent),
    m_view(view),
    m_eikonEnv(CEikonEnv::Static()),
    m_carModeEnabled(false),
    m_hideDeviceDialogs(false),
    m_deviceLockEnabled(false),
    m_carModeSubscriber(0),
    m_carModePublisher(0)
{
    PHONE_TRACE;
    sendToBackground(false); // Send phone to background

    m_carModePublisher = new QValueSpacePublisher("/phone",this);
    m_carModePublisher->setValue(QString("/carmode"),QVariant(false));
    
    m_carModeSubscriber = new QValueSpaceSubscriber("/phone/carmode", this);
    connect(m_carModeSubscriber, SIGNAL(contentsChanged()), this, SLOT(carModeChanged()));
    m_carModeEnabled = m_carModeSubscriber->value().toBool();

    int autlockValue = CPhonePubSubProxy::Instance()->Value(
                KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus);
    m_deviceLockEnabled = (EAutolockOff < autlockValue);

    
    // Start listening to device lock changes
    QT_TRAP_THROWING(CPhonePubSubProxy::Instance()->NotifyChangeL(
            KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, this));
    
    QT_TRAP_THROWING(CPhonePubSubProxy::Instance()->NotifyChangeL(
        KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, this));

}


/*!
    PhoneVisibilityHandler::~PhoneVisibilityHandler.
 */
PhoneVisibilityHandler::~PhoneVisibilityHandler()
{
    PHONE_TRACE;
    CPhonePubSubProxy::Instance()->CancelAllNotifications(this);
}

/*!
    PhoneVisibilityHandler::bringToForeground.
 */
void PhoneVisibilityHandler::bringToForeground()
{
    PHONE_TRACE2("m_carModeEnabled=", m_carModeEnabled);
    
    if (!m_carModeEnabled) {
        disableKeyGuard();
        m_view.bringToForeground();
        adjustVisibility(BringForwards);
    }
}

/*!
    PhoneVisibilityHandler::hideDeviceDialogs.
 */
void PhoneVisibilityHandler::hideDeviceDialogs(bool hide)
{
    PHONE_TRACE2(": hide =", hide);
    m_hideDeviceDialogs = hide;
    adjustVisibility(KeepCurrentPos);
}

/*!
    PhoneVisibilityHandler::phoneVisible.
 */
bool PhoneVisibilityHandler::phoneVisible()
{
    PHONE_TRACE
    // Should we check if there is phone's devicedialogs visible?
    return (m_eikonEnv->RootWin().OrdinalPosition() == 0);
}

/*!
    PhoneVisibilityHandler::sendToBackground.
 */
void PhoneVisibilityHandler::sendToBackground(bool homeScreenForeground)
{
    PHONE_TRACE4(": homeScreenForeground =", homeScreenForeground, 
        ", m_carModeEnabled =", m_carModeEnabled);
    
    if(m_carModeEnabled) {
        // Don't bring homescreen to foreground
        return;
    }
    
    enableKeyGuard();
    
    bool setHsToForeground = homeScreenForeground && phoneVisible();
    // Send phone back on WSERV stack
    adjustVisibility(SendToBack);
    
    // Fetch homescreen to foreground if needed
    if (setHsToForeground) {
        _LIT(KPhoneHsAppName,"hsapplication");
        TApaTaskList taskList(m_eikonEnv->WsSession());
        TApaTask task = taskList.FindApp(KPhoneHsAppName);
        task.BringToForeground();
    }
}

/*!
    PhoneVisibilityHandler::HandlePropertyChangedL.
 */
void PhoneVisibilityHandler::HandlePropertyChangedL(const TUid& aCategory, 
    const TUint aKey, const TInt aValue)
{
    if((aCategory == KPSUidCoreApplicationUIs) &&
            (aKey == KCoreAppUIsAutolockStatus)) {
        // Adjust visibility according mode
        m_deviceLockEnabled = (EAutolockOff < aValue);
        PHONE_TRACE2(": m_deviceLockEnabled=", m_deviceLockEnabled);
        m_view.setRestrictedMode(m_deviceLockEnabled);
        adjustVisibility(KeepCurrentPos);

    }
}

/*!
 *  PhoneVisibilityHandler::disableKeyGuard().
 */
bool PhoneVisibilityHandler::disableKeyGuard()
{
    PHONE_TRACE
    TRAP_IGNORE(
        CKeyguardAccessApi* keyguardAccess = CKeyguardAccessApi::NewL( );
        if (!m_keyguardOnBeforeForeground) {
            // Check if keyguard previous status only when it is not set
            // Keyguard status will be restored when phone is ordered to background
            m_keyguardOnBeforeForeground = keyguardAccess->IsKeyguardEnabled();
        }
        keyguardAccess->DisableKeyguard( EFalse );
        delete keyguardAccess;
        );
    
    return m_keyguardOnBeforeForeground;
}

/*!
 *  PhoneVisibilityHandler::enableKeyGuard().
 */
void PhoneVisibilityHandler::enableKeyGuard()
{
    PHONE_TRACE
    if (phoneVisible() && m_keyguardOnBeforeForeground) {
        // If phone is visible return to previous keyguard status
        TRAP_IGNORE(
            CKeyguardAccessApi* keyguardAccess = CKeyguardAccessApi::NewL( );
            keyguardAccess->EnableKeyguard( EFalse );
            delete keyguardAccess;
            );
    }
    m_keyguardOnBeforeForeground = false;
}

/*!
 *  PhoneVisibilityHandler::ongoingCalls().
 */
int PhoneVisibilityHandler::ongoingCalls()
{
    int amountOfCalls=0;
    TRAP_IGNORE(
        CCallInformation* callInfos = CCallInformation::NewL();
        CleanupStack::PushL(callInfos);
        Q_ASSERT(callInfos != 0);
        amountOfCalls = callInfos->GetCallsL().Count();
        CleanupStack::PopAndDestroy(callInfos);
        );
    
    return amountOfCalls;
}

/*!
    PhoneVisibilityHandler::adjustVisibility.
	 -1 Ordinal position is lowest ( not visible )
	  0 Ordinal position is highest ( visible )
	  1 - ... Ordinal postition under one or more window group
	  
	It is agreed with devicedialog that:
     - Incoming call, ECoeWinPriorityAlwaysAtFront + 100
     - Ongoing call + security, ECoeWinPriorityAlwaysAtFront + 1
     - Ongoing call, ECoeWinPriorityNormal
 */
void PhoneVisibilityHandler::adjustVisibility(AdjustAction action)
{
    PHONE_TRACE
    if (m_carModeEnabled || (action == SendToBack)) {
        PHONE_TRACE1(": SendPhoneToBackground");
        m_eikonEnv->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNeverAtFront);
        
    } else if ((KeepCurrentPos == action) &&
        (m_eikonEnv->RootWin().OrdinalPriority() == ECoeWinPriorityNeverAtFront)) {
        // Skip situations where phone is put to back
        // and action is not to bring it up
        // Execution must come here if there is no calls
        PHONE_TRACE1(": Skip");
        
    } else if (m_hideDeviceDialogs) {
        PHONE_TRACE1(": Hide dialogs");
        m_eikonEnv->RootWin().SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront + 100);
        
    } else if (m_deviceLockEnabled) {
        // critical notes are allowed to show on top of Phone application
        PHONE_TRACE1(": Devicelock");
        m_eikonEnv->RootWin().SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront + 1);
        
    } else if (BringForwards == action) {
        // Try to show phone with normal priority
        PHONE_TRACE1(": Bring forward");
        m_eikonEnv->RootWin().SetOrdinalPosition(0, ECoeWinPriorityNormal);
        
    } else {
        // Normalize visiblity after ie. device lock
        PHONE_TRACE1(": Normalize");
        int ordinalPos = m_eikonEnv->RootWin().OrdinalPosition();
        
        m_eikonEnv->RootWin().SetOrdinalPosition(ordinalPos, ECoeWinPriorityNormal);
        // Flush is needed here, because otherwise launching an application may fail
        // if done same time with normalization.
        m_eikonEnv->WsSession().Flush();        
    }
}

void PhoneVisibilityHandler::carModeChanged()
{
    PHONE_TRACE;
    m_carModeEnabled = m_carModeSubscriber->value().toBool();
    
    if(!m_carModeEnabled && (ongoingCalls() > 0)) {
        bringToForeground();
    }

    // Adjust visibility according mode (TODO!)
    PHONE_TRACE2(": m_carModeEnabled=", m_carModeEnabled);
    //adjustVisibility(KeepCurrentPos);	
}
