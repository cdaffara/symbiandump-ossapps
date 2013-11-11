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
#include <etelmm.h>
#include <commsdattypesv1_1.h>

// User includes
#include "cradionetworkinfolistener.h"
#include "mradiosettingssetter.h"
#include "cradioenginelogger.h"

using namespace CommsDat;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioNetworkInfoListener* CRadioNetworkInfoListener::NewL( MRadioSettingsSetter& aSetter,
                                                            MRadioNetworkChangeObserver* aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioNetworkInfoListener* self = new ( ELeave ) CRadioNetworkInfoListener( aSetter, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioNetworkInfoListener::CRadioNetworkInfoListener( MRadioSettingsSetter& aSetter,
                                                      MRadioNetworkChangeObserver* aObserver )
    : CActive( CActive::EPriorityHigh )
    , iSetter( aSetter )
    , iObserver( aObserver )
    , iSubscriberIdPckg( iSubscriberId )
    , iNetworkInfoPckg( iNetworkInfo )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioNetworkInfoListener::ConstructL()
    {
    LOG_METHOD_AUTO;
#ifndef __WINS__
    iTelephony = CTelephony::NewL();

    // Initialize the default network id and country code directly from CommsDat, as CTelephony cannot be used in synchronous mode
    // and using it in async mode will cause problems in core initialisation, and too much complexity!

    // Get default TSY from database.

    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion());
    CleanupStack::PushL( db );

    CMDBRecordSet<CCDModemBearerRecord>* set = new ( ELeave ) CMDBRecordSet<CCDModemBearerRecord>( KCDTIdModemBearerRecord );
    CleanupStack::PushL( set );

    set->LoadL( *db );

    TInt index = 0;
    CCDModemBearerRecord* record = static_cast<CCDModemBearerRecord*>( set->iRecords[index++]);

    if ( record->iTsyName.IsNull() && set->iRecords.Count() > index ) // the first item is normally only a template
        {
        record = static_cast<CCDModemBearerRecord*>( set->iRecords[index++]);
        }

    TPtrC name( static_cast<const TDesC&>( record->iTsyName ));
    TBuf<50> tsyName;
    tsyName.Copy( name );

    LOG_FORMAT( "CRadioNetworkInfoListener::ConstructL, TSY name: %S", &tsyName );
    CleanupStack::PopAndDestroy( set );
    CleanupStack::PopAndDestroy( db );

    RTelServer telServer;
    CleanupClosePushL( telServer );
    RMobilePhone mobilePhone;
    CleanupClosePushL( mobilePhone );

    //  Open phone
    User::LeaveIfError( telServer.Connect());

    // TSY module gets automatically unloaded when tel.Close() is called ( and tel is is CU-stack ),
    // so loaded TSY is also leave-safe
    User::LeaveIfError( telServer.LoadPhoneModule( tsyName ));

    // Get number of phones.
    TInt phones( 0 );
    User::LeaveIfError( telServer.EnumeratePhones( phones ));
    LOG_FORMAT( "CRadioNetworkInfoListener::ConstructL, Number of phones=%d", phones );

    // Get phone info of first legal phone.
    TInt legalPhoneIndex = KErrNotFound;
    RTelServer::TPhoneInfo phoneInfo;
    for ( TInt i=0; i<phones && legalPhoneIndex == KErrNotFound; ++i )
        {
        if ( telServer.GetPhoneInfo( i, phoneInfo ) == KErrNone )
            {
            if ( phoneInfo.iNetworkType == RTelServer::ENetworkTypeMobileDigital )
                {
                legalPhoneIndex = i;
                }
            }
        }
    User::LeaveIfError( legalPhoneIndex );

    // Open legal phone.
    User::LeaveIfError( mobilePhone.Open( telServer, phoneInfo.iName ));

    TUint32 networkCaps;
    User::LeaveIfError( mobilePhone.GetNetworkCaps( networkCaps ));
    TUint32 identityCaps;
    User::LeaveIfError( mobilePhone.GetIdentityCaps( identityCaps ));
    // Check if we are allowed to get network info.
    if ( networkCaps & RMobilePhone::KCapsGetCurrentNetwork )
        {
        // Gather initial information synchronically.
        RMobilePhone::TMobilePhoneLocationAreaV1 location;

        RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkInfoPckg( networkInfo );

        mobilePhone.GetCurrentNetwork( iStatus, networkInfoPckg, location );
        User::WaitForRequest( iStatus );

        CTelephony::TNetworkInfoV1 telephonyNetworkInfo;
        if ( iStatus == KErrNone )
            {
            telephonyNetworkInfo.iMode = static_cast<CTelephony::TNetworkMode>( networkInfo.iMode );
            telephonyNetworkInfo.iCountryCode = networkInfo.iCountryCode;
            telephonyNetworkInfo.iCdmaSID = networkInfo.iCdmaSID;
            telephonyNetworkInfo.iNetworkId = networkInfo.iNetworkId;
            }
        else
            {
            telephonyNetworkInfo.iCountryCode = iSetter.CountryCode();
            telephonyNetworkInfo.iNetworkId = iSetter.NetworkId();
            }
            iNetworkInfoPckg() = telephonyNetworkInfo;
        }
    else
        {
        // Leave if we are not allowed to get network info.
        // Other action could also be considered!!!!
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( 2, &telServer );
#endif
    // At the end, update the local variables by simulating the "netid changed" event.
    RunL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioNetworkInfoListener::~CRadioNetworkInfoListener()
    {
    LEVEL3( LOG_METHOD_AUTO );
    Cancel();

#ifndef __WINS__
    delete iTelephony;
#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioNetworkInfoListener::CompoundNetworkId() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    LOG_FORMAT( "Returning iCompoundNetworkId: %d", iCompoundNetworkId );
    return iCompoundNetworkId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioNetworkInfoListener::SubscriberId() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TPtrC id = iSubscriberId.iSubscriberId;
    LOG_FORMAT( "SubscriberId: %S", &id );
    return id;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioNetworkInfoListener::CountryCode() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TPtrC countryCode = iSetter.CountryCode();
    LOG_FORMAT( "CountryCode: %S", &countryCode );
    return countryCode;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioNetworkInfoListener::SaveNetworkInfoL()
    {
    LEVEL3( LOG_METHOD_AUTO );
#ifdef __WINS__
    iNetworkInfo.iCountryCode.Copy( _L("244") );
    iNetworkInfo.iNetworkId.Zero();
#endif

    LOG_FORMAT( "iNetworkInfo.iCountryCode = %S",
                  &iNetworkInfo.iCountryCode );

    User::LeaveIfError( iSetter.SetCountryCode( iNetworkInfo.iCountryCode ) );

    // The compound network identifier is composed of the current network identifier and country code.
    TBuf<KDefaultRealWidth> compoundNetworkId;
    compoundNetworkId.Append( iSetter.CountryCode() );
    if ( iNetworkInfo.iNetworkId.Length() < 2 )
        {
        // Network identifier has only one digit, in which case it must be prepended with a zero.
        compoundNetworkId.AppendNum( 0 );
        }

    compoundNetworkId.Append( iNetworkInfo.iNetworkId );

    TLex lex( compoundNetworkId );
    User::LeaveIfError( lex.Val( iCompoundNetworkId ) );

    if ( iCompoundNetworkId != 0 && iPreviousCompoundNetworkId != iCompoundNetworkId )
        {
        if ( iPreviousCompoundNetworkId != 0 && iObserver )
            {
            iObserver->NetworkIdChanged();
            }
        iPreviousCompoundNetworkId = iCompoundNetworkId;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioNetworkInfoListener::DoCancel()
    {
    LEVEL3( LOG_METHOD_AUTO );
#ifndef __WINS__
    iTelephony->CancelAsync( CTelephony::ECurrentNetworkInfoChangeCancel );
#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioNetworkInfoListener::RunL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "iStatus.Int() = %d", iStatus.Int() ) );
#ifndef __WINS__
    iTelephony->NotifyChange( iStatus, CTelephony::ECurrentNetworkInfoChange, iNetworkInfoPckg );
    SetActive();
#endif
    SaveNetworkInfoL();
    }
