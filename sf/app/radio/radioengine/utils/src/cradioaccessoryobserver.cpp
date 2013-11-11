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

// System includes
#include <ncp_feature_settings.hrh>

// User includes
#include "cradioaccessoryobserver.h"
#include "mradioheadseteventobserver.h"
#include "cradioenginelogger.h"

#ifdef ASW_CORE_AUDIO_PLATFORM_VERSION // Nokia specific adaptation
//#include <accpolpropgenericid.h>
#include <AccPolGenericIdDefinitions.h>

#ifndef __WINS__
// See accpolpropgenericid.h definitions KPCNokiaAV == 0x20000,
// Forced to use hard coded value here to avoid including from epoc32/include/internal
const TUint KPhysicalConnectionBitmask = 0x20000 | KPCWired;

#else
// See accpolpropgenericid.h definitions KPCNokiaAV == 0x20000, 
// Forced to use hard coded value here to avoid including from epoc32/include/internal
const TUint KPhysicalConnectionBitmask = 0x20000 | KPCWired;

#endif // __WINS__

#else
const TUint KPhysicalConnectionBitmask = KPCWired;
#endif // ASW_CORE_AUDIO_PLATFORM_VERSION

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::CRadioAccessoryObserver()
    : CActive( CActive::EPriorityStandard )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::ConstructL()
    {
    LEVEL2( LOG_METHOD_AUTO );
    User::LeaveIfError( iAccessoryServer.Connect() );

    // Creates a new sub-session within an existing session.
    User::LeaveIfError( iAccessoryConn.CreateSubSession( iAccessoryServer ) );
    User::LeaveIfError( iAccessoryConn.GetAccessoryConnectionStatus( iAccPolGenIdArr ) );

    TInt nroChangedAccessories( 0 );
    UpdateCurrentAccessoriesL( nroChangedAccessories );
    LOG_FORMAT( "Found %d wired/BT accessories ( %d accessories in total ).",
              nroChangedAccessories, iAccPolGenIdArr.Count() );
    CActiveScheduler::Add( this );
    // Accessory is always listened
    iAccessoryConn.NotifyAccessoryConnectionStatusChanged( iStatus, iAccPolGenIdArr );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver* CRadioAccessoryObserver::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioAccessoryObserver* self = new( ELeave ) CRadioAccessoryObserver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::~CRadioAccessoryObserver()
    {
    LEVEL3( LOG_METHOD_AUTO );
    Cancel();
    iAccessoryConn.CloseSubSession();
    iAccessoryServer.Close();
    iSavedAccessories.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::FindAccessoryL( TBool aAcceptOnlyHeadset, TAccAudioOutPutType& aOutputType ) const
    {
    LEVEL2( LOG_METHOD_AUTO );
#ifdef __WINS__
    TBool accessoryFound = ETrue;
    aAcceptOnlyHeadset = ETrue;
    aOutputType = EAccAudioOutPutTypePrivate;
#else
    TBool accessoryFound = EFalse;

    LOG_FORMAT( "Connected accessories count = %d", iAccPolGenIdArr.Count());
    for ( TInt i = 0; i < iAccPolGenIdArr.Count() && !accessoryFound; ++i )
        {
        TAccPolGenericID accPolGenId = iAccPolGenIdArr.GetGenericIDL( i );

        //Check if physical connection is of acceptable type
        TUint32 physicalConnectionCaps = accPolGenId.PhysicalConnectionCaps();
        if ( physicalConnectionCaps & KPhysicalConnectionBitmask || physicalConnectionCaps & KPCBluetooth )
            {
            LOG_FORMAT( "is wired or BT ( physicalConnectionCaps = %d )", physicalConnectionCaps );

            if ( !aAcceptOnlyHeadset )
                {
                accessoryFound = ETrue;
                }
            //Get device type ( check if is HeadSet )
            TUint32 deviceType = accPolGenId.DeviceTypeCaps();
            if ( deviceType & KDTHeadset )
                {
                LOG_FORMAT( "is HeadSet( deviceType = %d )", deviceType );
                if ( !IsLineoutConnectedL( accPolGenId ) )
                    {
                    LOG( "Is a normal headSet" );
                    accessoryFound = ETrue;
                    }
                }
            else if ( deviceType & KDTLoopset )
                {
                LOG_FORMAT( "is Loopset( deviceType = %d )", deviceType );
                accessoryFound = ETrue;
                }
            else
                {
                LOG_FORMAT( "is NOT HeadSet or Loopset( deviceType = %d )", deviceType );
                }

            if ( accessoryFound )
                {
                // get the public/private status
                TAccPolNameRecord nameRecord;
                nameRecord.SetNameL( KAccAudioOutputType );

                TAccValueTypeTInt audioOutputTypeValue;
                iAccessoryConn.GetValueL( accPolGenId, nameRecord, audioOutputTypeValue );

                aOutputType = static_cast<TAccAudioOutPutType>( audioOutputTypeValue.iValue );
                LOG_FORMAT( "AudioOutputType = %d", aOutputType );
                }
            }
        else
            {
            LOG_FORMAT( "is NOT wired or BT ( physicalConnectionCaps = %d )", physicalConnectionCaps );
            }
        }
#endif
    return accessoryFound;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::IsHeadsetConnectedL() const
    {
    TAccAudioOutPutType audioOutputType( EAccAudioOutPutTypePrivate );
    return FindAccessoryL( ETrue , audioOutputType );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::IsLineoutConnectedL( TAccPolGenericID& aAccPolGenId ) const
    {
    LEVEL2( LOG_METHOD_AUTO );
    TBool isLineOut( EFalse );

    CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( nameArray );
    iAccessoryConn.GetSubblockNameArrayL( aAccPolGenId, *nameArray );

    TBool isAudioOutValue = nameArray->HasName( KAccAudioOut );
    CleanupStack::PopAndDestroy( nameArray );
    if ( isAudioOutValue )
        {
        LOG_FORMAT( "isAudioOutValue = ( %d )", isAudioOutValue );

        TAccPolNameRecord nameRecord;
        nameRecord.SetNameL( KAccAudioOut );

        //Check if connected accessory is LineOut
        TAccValueTypeTInt audioOutValue;
        iAccessoryConn.GetValueL( aAccPolGenId, nameRecord, audioOutValue );

        LOG( "CRadioAccessoryObserver::IsLineoutConnectedL - GetValueL() - OK" );
        if ( TAccAudioOutLineout == audioOutValue.iValue )
            {
            //is lineout
            LOG_FORMAT( "Accessory is LineOut( audioOutValue = %d )", audioOutValue.iValue );
            isLineOut = ETrue;
            }
        }
    return isLineOut;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::UpdateCurrentAccessoriesL( TInt& aNroChangedAccessories )
    {
    LEVEL2( LOG_METHOD_AUTO );
    // first check, the removed accessories.
    TInt removedAccessories = CountRemovedAccessoriesL();
    LOG_FORMAT( "- %d accessories removed.", removedAccessories );
    aNroChangedAccessories += removedAccessories;
    TInt addedAccessories = CountAddedAccessoriesL();
    LOG_FORMAT( "- %d accessories added.", addedAccessories );
    aNroChangedAccessories += addedAccessories;

    iSavedAccessories.Reset();

    for ( TInt i = 0; i < iAccPolGenIdArr.Count(); ++i )
        {
        iSavedAccessories.AppendL( iAccPolGenIdArr.GetGenericIDL( i ) );
        LOG_FORMAT( "- accessory in slot %d: %d",
                  i, iAccPolGenIdArr.GetGenericIDL( i ).UniqueID() );
        }
    LOG_FORMAT( "- %d accessories in total.", iSavedAccessories.Count() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioAccessoryObserver::CountRemovedAccessoriesL() const
    {
    LEVEL2( LOG_METHOD_AUTO );
    TInt removedAccessories( 0 );

    for ( TInt i = 0; i < iSavedAccessories.Count(); ++i )
        {
        TAccPolGenericID savedAcc = iSavedAccessories[i];

        TUint32 physicalConnectionCaps = savedAcc.PhysicalConnectionCaps();

        if ( physicalConnectionCaps & KPhysicalConnectionBitmask || physicalConnectionCaps & KPCBluetooth )
            {
            TBool found( EFalse );

            for ( TInt j = 0; j < iAccPolGenIdArr.Count() && !found; j++ )
                {
                TAccPolGenericID freshAcc = iAccPolGenIdArr.GetGenericIDL( j );
                if ( freshAcc.UniqueID() == savedAcc.UniqueID() )
                    {
                    LOG_FORMAT( "- Accessory with id = %d found.", savedAcc.UniqueID() );
                    found = ETrue;
                    }
                }
            if ( !found )
                {
                LOG_FORMAT( "- Accessory with id = %d removed.", savedAcc.UniqueID() );
                removedAccessories++;
                }
            }
        }
    return removedAccessories;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioAccessoryObserver::CountAddedAccessoriesL() const
    {
    LEVEL2( LOG_METHOD_AUTO );
    TInt addedAccessories( 0 );

    for ( TInt i = 0; i < iAccPolGenIdArr.Count(); ++i )
        {
        TAccPolGenericID freshAcc = iAccPolGenIdArr.GetGenericIDL( i );

        TUint32 physicalConnectionCaps = freshAcc.PhysicalConnectionCaps();

        if ( physicalConnectionCaps & KPhysicalConnectionBitmask || physicalConnectionCaps & KPCBluetooth )
            {
            TBool found( EFalse );

            for ( TInt j = 0; j < iSavedAccessories.Count() && !found; j++ )
                {
                TAccPolGenericID savedAcc = iSavedAccessories[j];
                if ( savedAcc.UniqueID() == freshAcc.UniqueID() )
                    {
                    LOG_FORMAT( "- Accessory with id = %d found.", freshAcc.UniqueID() );
                    found = ETrue;
                    }
                }
            if ( !found )
                {
                LOG_FORMAT( "- Accessory with id = %d added.", freshAcc.UniqueID() );
                addedAccessories++;
                }
            }
        }
    return addedAccessories;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::SetObserver( MRadioHeadsetEventObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// From class CActive
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::RunL()
    {
    LEVEL2( LOG_METHOD_AUTO );
    TRequestStatus status = iStatus;
    iAccessoryConn.NotifyAccessoryConnectionStatusChanged( iStatus, iAccPolGenIdArr );

    SetActive();

    TInt nroChangedAccessories( 0 );
    UpdateCurrentAccessoriesL( nroChangedAccessories );

    if ( status == KErrNone && nroChangedAccessories != 0 )
        {
        TAccAudioOutPutType audioOutputType( EAccAudioOutPutTypePrivate );

        if ( FindAccessoryL( EFalse, audioOutputType ) )
            {
            // Accessory mode may change when combined connection status changes
            // or when audio routing status changes.
            if ( audioOutputType == EAccAudioOutPutTypePrivate )
                {
                LOG( "Headset connected" );
                iObserver->HeadsetConnectedCallbackL();
                }
            else
                {
                LOG( "Public wired or BT accessory connected." );
                iObserver->HeadsetDisconnectedCallbackL();
                }
            }
        else
            {
            LOG( "No wired or BT accessories available" );
            iObserver->HeadsetDisconnectedCallbackL();
            }
        }
    }


// ---------------------------------------------------------------------------
// From class CActive
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::DoCancel()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iAccessoryConn.CancelNotifyAccessoryConnectionStatusChanged();
    }
