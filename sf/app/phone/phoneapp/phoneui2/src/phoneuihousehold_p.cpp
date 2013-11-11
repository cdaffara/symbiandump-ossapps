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
* Description:  Household private class.
*
*/

#include <exception> // must be before e32base.h so uncaught_exception gets defined
#include <starterclient.h>
#include <telinternalpskeys.h>
#include <QLocale>
#include <HbTranslator>
#include <telremotepartyinformationpskeys.h>
#include <telinformationpskeys.h>
#include <startupdomainpskeys.h>
#include <featmgr.h>        // for FeatureManager
#include <hbmainwindow.h>
#include "phoneconstants.h"
#include "cphonelogger.h"
#include "cphonepubsubproxy.h"
#include "cphoneuicontroller.h"
#include "phoneuihousehold_p.h"
#include "phoneuiqtviewadapter.h"
#include "phoneuiqtview.h"
#include "phoneuikeyeventadapter.h"
#include "phoneuicommandadapter.h"
#include "phonenotecontroller.h"
#include "qtphonelog.h"
#include "cphoneclearblacklist.h"
#include "hbinstance.h"

PhoneUiHouseHoldPrivate::PhoneUiHouseHoldPrivate(HbMainWindow &window) :
    iAppsReady (0), iStartupSignalRecoveryId (0), iLightIdleReached (EFalse), 
    iPhoneUIController (0), m_window (window)
{
    PHONE_DEBUG("phoneui - Start phoneapp");
    
    TRAPD( error, ConstructL() );
    qt_symbian_throwIfError(error);
}

PhoneUiHouseHoldPrivate::~PhoneUiHouseHoldPrivate()
{
    while (!m_translators.isEmpty()) {
        HbTranslator *translator = m_translators.takeFirst();
        delete translator;
        translator = 0;
    }
    
    delete iKeyEventAdapter;
    delete iCommandAdapter;
    delete iPhoneUIController;
    delete iViewAdapter;
}

TInt PhoneUiHouseHoldPrivate::DoStartupSignalL( TAny* aAny )
{
    PhoneUiHouseHoldPrivate* appUi =
        static_cast< PhoneUiHouseHoldPrivate* >( aAny );
    
    return
        !appUi->iLightIdleReached ?
            appUi->DoStartupSignalIdleL() :
            appUi->DoStartupSignalSecurityL();
}


TInt PhoneUiHouseHoldPrivate::DoStartupSignalIdleL()
    {
    PHONE_DEBUG("phoneui::DoStartupSignalIdleL()");
    // Idle application has already started
    if ( !( iAppsReady & EPhoneIdleStartedUp ) )
        {
        PHONE_DEBUG("phoneui::DoStartupSignalIdleL() Idle App started ");
        iAppsReady += EPhoneIdleStartedUp;
        
        // Remove Phone application from Fast Swap Window.
        //    iPhoneViewController->SetHiddenL( ETrue );
        
        // Avkon removal            
        // hack to make sure EPhonePhase1Ok is set - to be fixed properly
       
        PHONE_DEBUG("phoneui::DoStartupSignalIdleL() Phone and Idle apps have both been started");    
        CPhonePubSubProxy::Instance()->ChangePropertyValue(
            KPSUidStartup,
            KPSPhonePhase1Ok,
            EPhonePhase1Ok );
        
        CPhoneRecoverySystem::Instance()->EnablePreconditionL();
        // Update the Phone light idle flag
        iLightIdleReached = ETrue;
        // Notify the UI controller that the phone is ready
        iPhoneUIController->HandlePhoneStartupL();
        }
    else 
        {
        PHONE_DEBUG("phoneui::DoStartupSignalIdleL() Idle is not reached yet!");
        // Idle has not been reached yet. Indicate to the recovery
        // system that this iteration has failed so that it will
        // try again at a later time.
        User::Leave( KErrNotReady );
        }
    return KErrNone;
    }

TInt PhoneUiHouseHoldPrivate::DoStartupSignalSecurityL()
    {
    TInt err = KErrNone;    
    return err;
    }

void PhoneUiHouseHoldPrivate::HandlePropertyChangedL(
        const TUid& aCategory, const TUint /*aKey*/, const TInt /*aValue*/)
{
    PHONE_DEBUG("phoneui::HandlePropertyChangedL()");
    
    if ( aCategory == KPSUidStartup )
        {
        
        const TInt startupState = CPhonePubSubProxy::Instance()->Value(
            KPSUidStartup,
            KPSGlobalSystemState );
        
        PHONE_DEBUG2("phoneui::HandlePropertyChangedL() StartupState value=%d", startupState );
       
        if ( startupState == ESwStateCriticalPhaseOK ||
            startupState == ESwStateEmergencyCallsOnly ||
            startupState == ESwStateNormalRfOn ||
            startupState == ESwStateNormalRfOff ||
            startupState == ESwStateNormalBTSap )
            {
            PHONE_DEBUG("phoneui::HandlePropertyChangedL() Try to update the startup signal again..." );
            // Try to update the startup signal again
            CPhoneRecoverySystem::Instance()->RecoverNow(
                iStartupSignalRecoveryId, 
                CTeleRecoverySystem::EPhonePriorityHigh );    
            }
        }
}

void PhoneUiHouseHoldPrivate::ConstructL()
{
    new( ELeave ) CPhoneLogger( KUidPhoneUILoggerSingleton );
    FeatureManager::InitializeLibL();
    
    HbTranslator *translator = new HbTranslator(QString("telephone"));

	if (translator) {
        translator->loadCommon();
        m_translators.append(translator);
	}
    
	HbTranslator *translator2 = new HbTranslator(QString("telephone_cp"));
    if (translator2) {
        m_translators.append(translator2);
    }

    PhoneUIQtView *view = new PhoneUIQtView(m_window);
    iViewAdapter = new PhoneUIQtViewAdapter(*view);
    iPhoneUIController = CPhoneUIController::NewL(iViewAdapter);
    iViewAdapter->setEngineInfo(iPhoneUIController->EngineInfo());
    
    m_window.addView (view);
    m_window.setCurrentView (view);
    m_window.scene ()->setFocusItem (view);
    iKeyEventAdapter = new PhoneUIKeyEventAdapter (*iPhoneUIController);
    iCommandAdapter = new PhoneUiCommandAdapter (*iPhoneUIController);
    QObject::connect(view, SIGNAL(dialpadIsAboutToClose()), iViewAdapter, SLOT(dialpadClosed()));
    QObject::connect(view, SIGNAL(keyReleased(QKeyEvent *)), iViewAdapter, SLOT(keyReleased (QKeyEvent *)));
    QObject::connect(view, SIGNAL(keyPressed (QKeyEvent *)), iKeyEventAdapter, SLOT(keyPressed (QKeyEvent *)));
    QObject::connect(view, SIGNAL(keyReleased (QKeyEvent *)), iKeyEventAdapter, SLOT(keyReleased (QKeyEvent *)));
    QObject::connect(view, SIGNAL(command (int)), iCommandAdapter, SLOT(handleCommand (int)),
                     Qt::QueuedConnection); // async to enable deletion of widget during signal handling
    QObject::connect(view, SIGNAL(windowActivated()), iViewAdapter, SLOT(handleWindowActivated()));
    QObject::connect(view, SIGNAL(windowDeactivated()), iViewAdapter, SLOT(handleWindowDeactivated()));
    QObject::connect(&m_window, SIGNAL(focusLost()),iViewAdapter, SLOT(onFocusLost()));
    QObject::connect(&m_window, SIGNAL(focusGained()),iViewAdapter, SLOT(onFocusGained()));
    QObject::connect(iViewAdapter->noteController(), SIGNAL(command (int)), 
                     iCommandAdapter, SLOT(handleCommand (int))); 
    
    // Disable default Send key functionality in application framework 
    // avkon removal
//    CAknAppUi *appUi = static_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi());
//    appUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort | 
//                             CAknAppUiBase::EDisableSendKeyLong );
    
    // CLI Name.
    TInt err = RProperty::Define( 
        KPSUidTelRemotePartyInformation,
        KTelCLIName,
        RProperty::EText,
        KPhoneReadPolicy,
        KPhoneWritePolicy );
      
    // CLI Number.
    err = RProperty::Define( 
        KPSUidTelRemotePartyInformation,
        KTelCLINumber,
        RProperty::EText,
        KPhoneReadPolicy,
        KPhoneWritePolicy );

    // Startup event signalling
    // Define the telephony application system property 
 
     err = RProperty::Define( 
        KPSUidTelInformation,
        KTelPhoneUid,
        RProperty::EInt,
        KPhoneReadPolicy,
        KPhoneWritePolicy );  

// DefineNewPSKeys:

    // UID: KPSUidTelInternalInformation:
    err = RProperty::Define( 
        KPSUidTelInternalInformation,
        KTelRemotePartyContactInfo,
        RProperty::EByteArray,
        KPhoneReadPolicy,
        KPhoneWritePolicy );    
               
    // VT UID.
    err = RProperty::Define( 
        KPSUidTelInternalInformation,
        KTelVideoCallUid,
        RProperty::EInt,
        KPhoneReadPolicy,
        KPhoneWritePolicy );

    // TELINTERNALPSKEY CHANGE 
    // Move to AudioHandling
    // UID: KPSUidTelAudioPreference
    // Call Audio Routing
    err = RProperty::Define( 
        KPSUidTelAudioPreference, 
        KTelAudioOutput, 
        RProperty::EInt,
        KPhoneReadPolicyAlwaysPass,
        KPhoneWritePolicy );

    // UID: KPSUidNEInformation
    err = RProperty::Define( 
        KPSUidNEInformation, 
        KTelNumberEntryInfo, 
        RProperty::EInt,
        KPhoneReadPolicyAlwaysPass,
        KPhoneWritePolicy );


    // Add DoStartupSignalL to the recovery system 
    iStartupSignalRecoveryId = 
        CPhoneRecoverySystem::Instance()->AddL( 
            TCallBack( DoStartupSignalL, this ),
            CTeleRecoverySystem::EPhonePriorityHigh,
            CTeleRecoverySystem::EPhoneStateIdle );

    // Call DoStartupSignalL within recovery system
    const TInt startupState = CPhonePubSubProxy::Instance()->Value(
        KPSUidStartup,
        KPSGlobalSystemState );

    PHONE_DEBUG2("phoneui::main() startupState value=", startupState );

    if ( startupState == ESwStateCriticalPhaseOK ||
        startupState == ESwStateEmergencyCallsOnly ||
        startupState == ESwStateNormalRfOn ||
        startupState == ESwStateNormalRfOff ||
        startupState == ESwStateNormalBTSap )
        {
        PHONE_DEBUG("phoneui::main() Notify Starter that phone is ready...");    
        CPhoneRecoverySystem::Instance()->RecoverNow(
            iStartupSignalRecoveryId, 
            CTeleRecoverySystem::EPhonePriorityHigh );
        }
    else
        {        
        PHONE_DEBUG("phoneui::main() Startup not ready yet. Start listening...");
        CPhonePubSubProxy::Instance()->NotifyChangeL(
            KPSUidStartup,
            KPSGlobalSystemState,
            this );
        }

    if ( err == KErrNone || err == KErrAlreadyExists )
        {
        // Indicate to the system that the Phone has been started
        CPhonePubSubProxy::Instance()->ChangePropertyValue(
            KPSUidTelInformation,
            KTelPhoneUid,
            KUidPhoneApplication.iUid );
        }

    // Update the Apps ready flag
    iAppsReady += EPhoneStartedUp;

    HbMainWindow *main = hbInstance->allMainWindows().at(0);
    RWindow *win = static_cast<RWindow *>(main->effectiveWinId()->DrawableWindow());

    CEikonEnv* env = CEikonEnv::Static();
    if ( env ) {
        env->SetSystem(ETrue);
        // Blacklist singleton is initialized here
        CPhoneClearBlacklist::CreateL( env->WsSession(), *win);
    }
}

