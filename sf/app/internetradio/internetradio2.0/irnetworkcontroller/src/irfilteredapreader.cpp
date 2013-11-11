/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used to read in only access points with the desired characteristics.
*
*/


#include <commdb.h>
#include <e32cmn.h>

#include "irdebug.h"
#include "irfilteredapreader.h"

_LIT( KDynIpAddress, "0.0.0.0" );
_LIT( KEasyWLAN, "Easy WLAN" );


// ---------------------------------------------------------------------------
// CIRFilteredApReader::NewL
// ---------------------------------------------------------------------------
//
CIRFilteredApReader* CIRFilteredApReader::NewL( TInt aFilterFlags )
    {
    IRLOG_DEBUG2( "CIRFilteredApReader::NewL - Filter flags = %d", aFilterFlags );
    CIRFilteredApReader* self = CIRFilteredApReader::NewLC( aFilterFlags );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRFilteredApReader::NewL - Exiting." );    
    return self;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::NewLC
// ---------------------------------------------------------------------------
//
CIRFilteredApReader* CIRFilteredApReader::NewLC( TInt aFilterFlags )
    {
    IRLOG_DEBUG2( "CIRFilteredApReader::NewLC - Filter flags = %d", aFilterFlags );
    CIRFilteredApReader* self = new( ELeave ) CIRFilteredApReader( aFilterFlags );
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRFilteredApReader::NewL - Exiting." );    
    return self;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::FirstRecordL
// ---------------------------------------------------------------------------
//
CCDIAPRecord* CIRFilteredApReader::FirstRecordL()
    {
    IRLOG_DEBUG( "CIRFilteredApReader::FirstRecordL - Entering" );    
    CCDIAPRecord* record( NULL );
    iWapTableIndex = 0;
    if ( iWapTable->iRecords.Count() > 0 )
        {
        TBool loaded( EFalse );
        TRAPD( err, loaded = LoadCurrentApDetailsL() ); // may leave because of access rights etc.
        if ( err == KErrNone && loaded && FilterL() )
            {
            record = static_cast<CCDIAPRecord*>( CCDRecordBase::CreateCopyRecordL( *iIapRecord ) );
            record->SetRecordId( iIapRecord->RecordId() );
            IRLOG_INFO2( "CIRFilteredApReader::FirstRecordL - %S accepted.", &record->iRecordName.GetL() );
            }
        else if ( err != KErrNone || !loaded )
            {
            IRLOG_ERROR3( "CIRFilteredApReader::FirstRecordL - Loading AP details failed ( err=%d, loaded=%d ).",
                          err, loaded );            
            record = NextRecordL();
            }
        else
            {
            IRLOG_INFO2( "CIRFilteredApReader::FirstRecordL - %S filtered out.", &iIapRecord->iRecordName.GetL() );
            record = NextRecordL();            
            }
        }        
    IRLOG_DEBUG( "CIRFilteredApReader::FirstRecordL - Exiting." );    
    return record;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::NextRecordL
// ---------------------------------------------------------------------------
//
CCDIAPRecord* CIRFilteredApReader::NextRecordL()
    {
    IRLOG_DEBUG( "CIRFilteredApReader::NextRecordL - Entering" );    
    CCDIAPRecord* record( NULL );
    
    while ( !record && MoveToNextRecord() )
        {
        TBool loaded( EFalse );
        TRAPD( err, loaded = LoadCurrentApDetailsL() ); // may leave because of access rights etc.
        if ( err == KErrNone && loaded && FilterL() )
            {
            record = static_cast<CCDIAPRecord*>( CCDRecordBase::CreateCopyRecordL( *iIapRecord ) );
            record->SetRecordId( iIapRecord->RecordId() );            
            IRLOG_INFO2( "CIRFilteredApReader::FirstRecordL - %S accepted.", &record->iRecordName.GetL() );            
            }
        else if ( err != KErrNone || !loaded )
            {
            IRLOG_ERROR3( "CIRFilteredApReader::NextRecordL - Loading AP details failed ( err=%d, loaded=%d ).",
                          err, loaded );            
            }
        else
            {
            IRLOG_INFO2( "CIRFilteredApReader::NextRecordL - %S filtered out.", &iIapRecord->iRecordName.GetL() );
            }
        }
    IRLOG_DEBUG( "CIRFilteredApReader::NextRecordL - Exiting." );    
    return record;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::CIRFilteredApReader
// ---------------------------------------------------------------------------
//
CIRFilteredApReader::CIRFilteredApReader( TInt aFilterFlags ) 
    : iFilterFlags( aFilterFlags )  
    {
    IRLOG_DEBUG( "CIRFilteredApReader::CIRFilteredApReader" );                
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::ConstructL
// ---------------------------------------------------------------------------
//
void CIRFilteredApReader::ConstructL()
    {
    IRLOG_DEBUG( "CIRFilteredApReader::ConstructL - Entering" );                
    iDbSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    
    iWapTable = new ( ELeave ) CMDBRecordSet<CCDWAPAccessPointRecord>( KCDTIdWAPAccessPointRecord );
    iWapTable->LoadL( *iDbSession );
    IRLOG_DEBUG( "CIRFilteredApReader::ConstructL - Exiting." );    
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::~CIRFilteredApReader
// ---------------------------------------------------------------------------
//
CIRFilteredApReader::~CIRFilteredApReader()
    {
    IRLOG_DEBUG( "CIRFilteredApReader::~CIRFilteredApReader - Entering" );    
    delete iDbSession;
    delete iWapTable;
    delete iWapIpBearerRecord;
    delete iIapRecord;
    IRLOG_DEBUG( "CIRFilteredApReader::~CIRFilteredApReader - Exiting." );    
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::CurrentRecordL
// ---------------------------------------------------------------------------
//
CCDWAPAccessPointRecord* CIRFilteredApReader::CurrentRecordL() const
    {
    IRLOG_DEBUG( "CIRFilteredApReader::CurrentRecordL - Entering" );    
    if ( iWapTableIndex >= iWapTable->iRecords.Count() )
        {
        IRLOG_FATAL( "CIRFilteredApReader::CurrentRecordL - WAP table overflow!" );
        User::Leave( KErrCorrupt );
        }
    IRLOG_DEBUG( "CIRFilteredApReader::CurrentRecordL - Exiting" );    
    return static_cast<CCDWAPAccessPointRecord*>( iWapTable->iRecords[iWapTableIndex] );
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::MoveToNextRecord
// ---------------------------------------------------------------------------
//
TBool CIRFilteredApReader::MoveToNextRecord()
    {
    IRLOG_DEBUG( "CIRFilteredApReader::MoveToNextRecord - Entering" );    
    TBool retMe( EFalse );
    if ( ++iWapTableIndex < iWapTable->iRecords.Count() )
        {
        IRLOG_INFO2( "CIRFilteredApReader::MoveToNextRecord - Current record %d", iWapTableIndex );        
        retMe = ETrue;
        }
    else
        {
        IRLOG_INFO( "CIRFilteredApReader::MoveToNextRecord - No more records" );        
        }
    IRLOG_DEBUG( "CIRFilteredApReader::MoveToNextRecord - Exiting" );    
    return retMe;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::LoadCurrentApDetailsL
// ---------------------------------------------------------------------------
//
TBool CIRFilteredApReader::LoadCurrentApDetailsL()
    {
    IRLOG_DEBUG( "CIRFilteredApReader::LoadCurrentApDetailsL - Entering" );    
    delete iWapIpBearerRecord;
    iWapIpBearerRecord = NULL;
    
    delete iIapRecord;
    iIapRecord = NULL;
    
    // Load record from WAPIPBearer
    iWapIpBearerRecord = static_cast<CCDWAPIPBearerRecord*>( 
            CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );

    iWapIpBearerRecord->iWAPAccessPointId.SetL( CurrentRecordL()->RecordId() );
    TBool res = iWapIpBearerRecord->FindL( *iDbSession );
    
    if ( res )
        {
        iIapRecord = static_cast<CCDIAPRecord*>(
                 CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        iIapRecord->SetRecordId( iWapIpBearerRecord->iWAPIAP );
        iIapRecord->LoadL( *iDbSession );
        }
    IRLOG_DEBUG2( "CIRFilteredApReader::LoadCurrentApDetailsL - returning %d", res );    
    return res;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::FilterL
// ---------------------------------------------------------------------------
//
TBool CIRFilteredApReader::FilterL() const
    {
    IRLOG_DEBUG2( "CIRFilteredApReader::FilterL - Service type %S", &iIapRecord->iServiceType.GetL() );    
    TBool retMe( ETrue );
    
    // The WAP check needs to be done only for CSD, GPRS, CDMA and LAN (  because of Easy WLAN )
    // APs. 
    if ( iIapRecord->iServiceType.GetL().Compare( TPtrC( KCDTypeNameDialOutISP ) ) == 0 ||
         iIapRecord->iServiceType.GetL().Compare( TPtrC( KCDTypeNameOutgoingWCDMA ) ) == 0 ||
//         iIapRecord->iServiceType.GetL().Compare( 
//         TPtrC( KCDTypeNameCDMA2000PacketService ) ) == 0 ||
         iIapRecord->iServiceType.GetL().Compare( TPtrC( KCDTypeNameLANService) ) == 0 )
        {
        if ( iFilterFlags & KIRFilterWAPMandatory )
            {
            retMe = FilterWAPMandatoryL();
            }
        if ( retMe && iFilterFlags & KIRFilterWAPOnly )
            {
            retMe = FilterWAPOnlyL();
            }
        if ( retMe && iFilterFlags & KIRFilterEasyWLAN )
            {
            retMe = FilterEasyWlanL();            
            }
        }
    IRLOG_DEBUG2( "CIRFilteredApReader::FilterL - Returning %d", retMe );    
    return retMe;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::FilterWAPMandatoryL
// ---------------------------------------------------------------------------
//
TBool CIRFilteredApReader::FilterWAPMandatoryL() const
    {
    IRLOG_DEBUG( "CIRFilteredApReader::FilterWAPMandatoryL - Entering" );    
    TBool retMe( EFalse );
    
    // First check the WAP_GATEWAY_ADDRESS of WAP_IP_BEARER table.
    if ( iWapIpBearerRecord->iWAPGatewayAddress.IsNull() || 
         iWapIpBearerRecord->iWAPGatewayAddress.GetL().Length() == 0 ||
         iWapIpBearerRecord->iWAPGatewayAddress.GetL().Compare( KDynIpAddress ) == 0 )
        {
        retMe = ETrue;
        }
    IRLOG_INFO3( "CIRFilteredApReader::FilterWAPMandatoryL - Returning %d for %S", retMe, &iIapRecord->iRecordName.GetL() );
    return retMe;
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::FilterWAPOnlyL
// ---------------------------------------------------------------------------
//
TBool CIRFilteredApReader::FilterWAPOnlyL() const
    {
    IRLOG_DEBUG( "CIRFilteredApReader::FilterWAPOnlyL - Entering" );    
    TBool retMe( EFalse );

    IRLOG_DEBUG2( "CIRFilteredApReader::FilterWAPOnlyL - Service type %S", &iIapRecord->iServiceType.GetL() );
    if ( iIapRecord->iServiceType.GetL().Compare( TPtrC( KCDTypeNameDialOutISP ) ) == 0 )
        {
        CCDDialOutISPRecord* dialOutRecord = static_cast<CCDDialOutISPRecord*>( 
                    CCDRecordBase::RecordFactoryL( KCDTIdDialOutISPRecord ) );
        CleanupStack::PushL( dialOutRecord );
        dialOutRecord->SetRecordId( iIapRecord->iService );
        dialOutRecord->LoadL( *iDbSession );
        if ( dialOutRecord->iType != EIspTypeWAPOnly )
            {
            retMe = ETrue;
            }
        CleanupStack::PopAndDestroy( dialOutRecord );
        }
    else if ( iIapRecord->iServiceType.GetL().Compare( TPtrC( KCDTypeNameOutgoingWCDMA ) ) == 0 )
        {
        CCDWCDMAPacketServiceRecord* gprsRecord = static_cast<CCDWCDMAPacketServiceRecord*>( 
                CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        CleanupStack::PushL( gprsRecord );
        gprsRecord->SetRecordId( iIapRecord->iService );
        gprsRecord->LoadL( *iDbSession );
        if ( gprsRecord->iGPRSAPType != EIspTypeWAPOnly )
            {
            retMe = ETrue;
            }
        CleanupStack::PopAndDestroy( gprsRecord );
        }
//    else if ( iIapRecord->iServiceType.GetL().Compare( 
//    TPtrC( KCDTypeNameCDMA2000PacketService ) ) == 0 )
//        {
//        CCDCDMA2000PacketServiceRecord* cdmaRecord = static_cast<CCDCDMA2000PacketServiceRecord*>(
//        CCDRecordBase::RecordFactoryL( KCDTIdCDMA2000PacketServiceRecord ) );
//        CleanupStack::PushL( cdmaRecord );
//        cdmaRecord->SetRecordId( iIapRecord->iService );
//        cdmaRecord->LoadL( *iDbSession );
//        if ( cdmaRecord->iApType != EIspTypeWAPOnly )
//            {
//            retMe = ETrue;
//            }
//        CleanupStack::PopAndDestroy( cdmaRecord );
//        }
    else
        {
        retMe = ETrue;
        }
    
    IRLOG_INFO3( "CIRFilteredApReader::FilterWAPOnlyL - Returning %d for %S", retMe, &iIapRecord->iRecordName.GetL() );    
    return retMe; 
    }

// ---------------------------------------------------------------------------
// CIRFilteredApReader::FilterEasyWlanL
// ---------------------------------------------------------------------------
//
TBool CIRFilteredApReader::FilterEasyWlanL() const
    {
    IRLOG_DEBUG( "CIRFilteredApReader::FilterEasyWlanL - Entering" );    
    TBool retMe( EFalse );
    if ( iIapRecord->iRecordName.GetL().Compare( KEasyWLAN ) != 0 )
        {
        retMe = ETrue;
        }
    IRLOG_INFO3( "CIRFilteredApReader::FilterEasyWlanL - Returning %d for %S", retMe, &iIapRecord->iRecordName.GetL() );    
    return retMe;
    }
