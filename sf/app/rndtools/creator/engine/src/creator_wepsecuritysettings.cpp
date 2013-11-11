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
* Description: 
*
*/



#include "creator_traces.h"
#include "creator_wepsecuritysettings.h"
#include <commdb.h>
//#include <featmgr.h>
#include <WlanCdbCols.h>

//#include <commsdattypesv1_1.h>
//#include <cmmanagertablefields.h>
//#include <wlancontainer.h>



//----------------------------------------------------------------------------

CCreatorWEPSecuritySettings* CCreatorWEPSecuritySettings::NewL()
    {
    CCreatorWEPSecuritySettings* self = CCreatorWEPSecuritySettings::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CCreatorWEPSecuritySettings* CCreatorWEPSecuritySettings::NewLC()
    {
    CCreatorWEPSecuritySettings* self = new (ELeave) CCreatorWEPSecuritySettings;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCreatorWEPSecuritySettings::CCreatorWEPSecuritySettings()
    {
    }

void CCreatorWEPSecuritySettings::ConstructL()
    {
    LOGSTRING("Creator: CCreatorWEPSecuritySettings::ConstructL");
    }

CCreatorWEPSecuritySettings::~CCreatorWEPSecuritySettings()
    {
    LOGSTRING("Creator: CCreatorWEPSecuritySettings::~CCreatorWEPSecuritySettings");

    
    }

    

//----------------------------------------------------------------------------



// ---------------------------------------------------------
// CWEPSecuritySettingsImpl::SaveL
// ---------------------------------------------------------
//
void CCreatorWEPSecuritySettings::SaveL( TUint32 /*aIapId*/, 
                                      CCommsDatabase& /*aCommsDb*/ ) const
    {
    /*
    // Data of the key
    TBuf8<KMaxLengthOfKeyData> iKeyData[KMaxNumberofKeys];    
    
    CCommsDbTableView* wLanServiceTable;

    // Caller MUST initiate a transaction, WE WILL NOT.

    wLanServiceTable = aCommsDb.OpenViewMatchingUintLC( 
                    TPtrC( WLAN_SERVICE ), TPtrC( WLAN_SERVICE_ID ), aIapId );
    TInt errorCode = wLanServiceTable->GotoFirstRecord();

    if ( errorCode == KErrNone )
        {
        wLanServiceTable->UpdateRecord();
        }
    else
        {
        TUint32 dummyUid( 0 ); //was KUidNone
        User::LeaveIfError( wLanServiceTable->InsertRecord( dummyUid ) );

        // Save link to LAN service
        wLanServiceTable->WriteUintL( TPtrC( WLAN_SERVICE_ID ), aIapId );
        }

    // Save index of key in use
    wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_INDEX ), 
                                 ( TUint32& ) iKeyInUse );

    // Save index of key in use
    wLanServiceTable->WriteUintL( TPtrC( NU_WLAN_AUTHENTICATION_MODE ), 
                                 ( TUint32& ) iAuthentication );

    // Save first WEP key
    wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY1 ), 
                                  iKeyData[KFirstKey] );

    // Save second WEP key
    wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY2 ), 
                                  iKeyData[KSecondKey] );

    // Save third WEP key
    wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY3 ), 
                                  iKeyData[KThirdKey] );

    // Save fourth WEP key
    wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY4 ), 
                                  iKeyData[KFourthKey] );

    // Save the format of the keys
    wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY1_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[KFirstKey] );

    wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY2_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[KSecondKey] );

    wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY3_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[KThirdKey] );

    wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY4_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[KFourthKey] );

    wLanServiceTable->PutRecordChanges();

    CleanupStack::PopAndDestroy( wLanServiceTable );  // wLanServiceTable
    */
    }

