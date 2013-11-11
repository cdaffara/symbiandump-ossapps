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
#include <networkhandlingproxy.h>
#include <cnwsession.h>
#include <BTSapDomainPSKeys.h>
#include <startupdomainpskeys.h>
#include <PsetContainer.h>
#include <PsetNetwork.h>
#include <PsetSAObserver.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>

#include "cnetworklistener.h"
#include "mnetworklistenerobserver.h"
#include "networkhandlingstarterlogging.h"

// Constants

// 1-minute timeout before showing soft notification
const TInt KNetworkLostTimeout = 60*1000000;
const TInt KSimRefreshAsynronationTimeout = 1000000;

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of CNetworkListener.
 */
CNetworkListener::CNetworkListener( MNetworkListenerObserver& aObserver )
    :
    iRegistered(ETrue),
    iObserver(aObserver)
{
    DPRINT << ": IN";
    
    DPRINT << ": OUT";
}

/*!
    Constructor of CNetworkListener.
 */
void CNetworkListener::ConstructL()
{
    DPRINT << ": IN";
    
    //Create network handling engine session.
    iSession = CreateL(*this, iInfo);
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    QT_TRYCATCH_LEAVING(iDeviceInfo = new QSystemDeviceInfo())
    
    iContainer = CPsetContainer::NewL();
    iRefreshHandler = iContainer->CreateRefreshHandlerL();
    iRefreshHandler->NotifyFileChangeL(
            *this,
            KCsp1Ef,
            EFileChangeNotification );
    iSettingsEngine = iContainer->CreateNetworkObjectL(*this);
    
    DPRINT << ": OUT";
}

/*!
    Constructor of CNetworkListener.
 */
CNetworkListener* CNetworkListener::NewL( MNetworkListenerObserver& aObserver )
{
    DPRINT << ": IN";
    
    CNetworkListener* self = new(ELeave) CNetworkListener(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    DPRINT << ": OUT";
    return self;
 }
    
/*!
    Destructor of CNetworkListener.
 */
CNetworkListener::~CNetworkListener()
{
    DPRINT << ": IN";
    
    delete iSession;
    delete iTimer;
    delete iDeviceInfo;
    delete iSettingsEngine;
    delete iRefreshHandler;
    delete iContainer;    
    
    DPRINT << ": OUT";
}

/*!
    CNetworkListener::HandleNetworkMessage.
 */
void CNetworkListener::HandleNetworkMessage(const TNWMessages aMessage)
{
    DPRINT << ": IN";
    
    if(aMessage == ENWMessageNetworkRegistrationStatusChange) {
        switch(iInfo.iRegistrationStatus) {
            case ENWNotRegisteredNoService:
                // Fall-through
            case ENWNotRegisteredEmergencyOnly:
                // Fall-through
            case ENWNotRegisteredSearching:
                // Fall-through
            case ENWRegistrationDenied:
                HandleNetworkLost();
                break;
            
            case ENWRegisteredBusy:
                // Fall-through
            case ENWRegisteredOnHomeNetwork:
                // Fall-through
            case ENWRegisteredRoaming:
                HandleNetworkFound();
                break;
            
            case ENWRegistrationUnknown:
                // Take no action
            default:
                break;
        }
    }
    
    DPRINT << ": OUT";
}

/*!
    CNetworkListener::HandleNetworkError.
 */
void CNetworkListener::HandleNetworkError( 
    const TNWOperation /*aOperation*/, TInt /*aErrorCode*/)
    {
    DPRINT << ": IN";
    }

/*!
    CNetworkListener::NWLostDelayCallBack.
 */
TInt CNetworkListener::NWLostDelayCallBack(TAny* aParam)
{
    DPRINT << ": IN";
    
    CNetworkListener* self = 
        reinterpret_cast< CNetworkListener* >( aParam );
    
    if ( self ) {
        self->iTimer->Cancel();
        self->iObserver.ShowNote();
    }
    
    DPRINT << ": OUT";
    return KErrNone;
}
    
/*!
    CNetworkListener::HandleNetworkFound
 */
void CNetworkListener::HandleNetworkFound()
{
    DPRINT << ": IN";
    
    if (!iRegistered) {
        iTimer->Cancel();
        iObserver.RemoveNote();
    }
    iRegistered = ETrue;
    
    DPRINT << ": OUT";
}

/*!
    CNetworkListener::HandleNetworkLost
 */
void CNetworkListener::HandleNetworkLost()
{
    DPRINT << ": IN";
    
    if (iInfo.iSelectionSetting == ENWNetworkSelectionManual) {
        // See if we were registered before
        if (iRegistered) {
            iRegistered = EFalse;

            if (IsNetworkConnectionAllowed() &&
                !IsBluetoothSAPConnected() &&
                IsSimOk()) {
                // Start 60 second timer and when expired show the
                // notifier
                iTimer->Cancel();
                iTimer->Start(KNetworkLostTimeout,
                              KNetworkLostTimeout,
                              TCallBack( NWLostDelayCallBack, this ));
             }
        }
    }
    
    DPRINT << ": OUT";
}

/*!
    CNetworkListener::IsNetworkConnectionAllowed
 */
TBool CNetworkListener::IsNetworkConnectionAllowed() const
{
    DPRINT << ": IN";
    // Key indicates whether network connections are allowed.
    // E.g. is Offline Mode active or not.
    TInt nwConnectionAllowed( ECoreAppUIsNetworkConnectionAllowed );
    CRepository* repository = CRepository::NewL( KCRUidCoreApplicationUIs );
    repository->Get(KCoreAppUIsNetworkConnectionAllowed,nwConnectionAllowed);
    delete repository;
    repository = NULL;

    DPRINT << ": OUT, nwConnectionAllowed: " << nwConnectionAllowed;
    return nwConnectionAllowed == ECoreAppUIsNetworkConnectionAllowed;
}

/*!
    CNetworkListener::IsBluetoothSAPConnected
 */
TBool CNetworkListener::IsBluetoothSAPConnected() const
{
    DPRINT << ": IN";
    
    TInt btSapState( EBTSapNotConnected );
    TInt err = RProperty::Get(KPSUidBluetoothSapConnectionState,
                              KBTSapConnectionState,
                              btSapState );
    
    DPRINT << ": OUT";
    
    return btSapState != EBTSapNotConnected && err == KErrNone;
}
    
/*!
    CNetworkListener::IsSimOk
 */
TBool CNetworkListener::IsSimOk() const
{
    DPRINT << ": IN";
    
    TInt simState(ESimStatusUninitialized);
    TInt err = RProperty::Get(KPSUidStartup,
                              KPSSimStatus,
                              simState );
    
    DPRINT << ": OUT";
    
    return simState == ESimUsable && err == KErrNone;
}

/*!
    CNetworkListener::AllowRefresh
 */
TBool CNetworkListener::AllowRefresh(
                const TSatRefreshType aType,
                const TSatElementaryFiles aFiles )
{
    DPRINT << "aType: " << aType;
    DPRINT << "aFiles: " << aFiles;
    
    TBool allowRefresh = ETrue;
    if ( iSettingsEngine->IsCallActive() == CPsetSAObserver::EPSetCallActive )
        {
        allowRefresh = EFalse;
        }

    DPRINT << ": OUT allowRefresh " << allowRefresh;
    
    return allowRefresh;
}

/*!
    CNetworkListener::Refresh
 */
void CNetworkListener::Refresh(
                const TSatRefreshType aType,
                const TSatElementaryFiles aFiles )
{
    DPRINT << ": IN";
    
    if ((aType != EFileChangeNotification) ||
        ((aType == EFileChangeNotification) &&
        (aFiles & KCsp1Ef)))
        {
        MPsetNetworkSelect::TSelectMode mode;
        iSettingsEngine->GetNetworkSelectMode(mode);
        DPRINT << "mode " << mode;
        if (mode == MPsetNetworkSelect::ENetSelectModeManual) {
            TPSetChangedCspSetting changedCspSettings;
            TPSetChangedCspSetting newValues;
            TInt err = iRefreshHandler->ChangedCspSettings(changedCspSettings, newValues);
            if ((changedCspSettings & EPSetNetSelSup) && 
                !(newValues & EPSetNetSelSup) &&
                (err == KErrNone)) {
                DPRINT << "Network selection not supported ";
                // Network selection mode can't change during SIM refresh,
                // start 1 second timer.
                iTimer->Cancel();
                iTimer->Start(KSimRefreshAsynronationTimeout,
                              KSimRefreshAsynronationTimeout,
                              TCallBack( NWSimRefreshCallBack, this ));
            }
        }
    }
    
    DPRINT << ": OUT";
}

/*!
    CNetworkListener::HandleNetworkInfoReceivedL
 */
void CNetworkListener::HandleNetworkInfoReceivedL( 
        const CNetworkInfoArray* /*aInfoArray*/, const TInt /*aResult*/ )
{
    DPRINT;
}

/*!
    CNetworkListener::HandleCurrentNetworkInfoL
 */
void CNetworkListener::HandleCurrentNetworkInfoL( 
        const MPsetNetworkSelect::TCurrentNetworkInfo& /*aCurrentInfo*/, 
        const TInt /*aResult*/ )
{
    DPRINT;
}

/*!
    CNetworkListener::HandleNetworkChangedL
 */
void CNetworkListener::HandleNetworkChangedL( 
        const MPsetNetworkSelect::TNetworkInfo& /*aCurrentInfo*/,
        const MPsetNetworkSelect::TCurrentNetworkStatus /*aStatus*/, 
        const TInt /*aResult*/ )
{
    /**
    *  DEPRECATED.
    */
}

/*!
    CNetworkListener::HandleNetworkChangedL
 */
void CNetworkListener::HandleNetworkChangedL( 
        const MPsetNetworkSelect::TNetworkInfo& /*aCurrentInfo*/,
        const RMobilePhone::TMobilePhoneRegistrationStatus& /*aStatus*/, 
        const TInt /*aResult*/ )
{
    DPRINT;
}

/*!
    CNetworkListener::HandleSearchingNetworksL
 */
void CNetworkListener::HandleSearchingNetworksL( TServiceRequest /*aRequest*/ )
{
    DPRINT;
}

/*!
    CNetworkListener::HandleRequestingSelectedNetworkL
 */
void CNetworkListener::HandleRequestingSelectedNetworkL( TBool /*aOngoing*/ )
{
    DPRINT; 
}

/*!
    CNetworkListener::HandleCallActivatedL
 */
void CNetworkListener::HandleCallActivatedL()
{
    DPRINT;
}

/*!
    CNetworkListener::HandleNetworkErrorL
 */
void CNetworkListener::HandleNetworkErrorL( 
        const MPsetNetworkInfoObserver::TServiceRequest /*aRequest*/,
        const TInt /*aError*/ )
{
    DPRINT;
}

/*!
    CNetworkListener::NWSimRefreshCallBack.
 */
TInt CNetworkListener::NWSimRefreshCallBack(TAny* aParam)
{
    DPRINT << ": IN";
    
    CNetworkListener* self = 
        reinterpret_cast< CNetworkListener* >( aParam );
    
    if ( self ) {
        self->iTimer->Cancel();
        //Change the network mode to Automatic.
        MPsetNetworkSelect::TNetworkInfo info;
        info.iMode = MPsetNetworkSelect::ENetSelectModeAutomatic;
        DPRINT << ": set network mode to automatic";
        TRAP_IGNORE(self->iSettingsEngine->SelectNetworkL(info));
    }
    
    DPRINT << ": OUT";
    return KErrNone;
}

// End of file
    
