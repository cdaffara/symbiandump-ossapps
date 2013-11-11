/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Class stores data for one accesspoint
*
*/



// INCLUDE FILES

#include "CWPAPAccesspointItem.h"		// Own header

#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <CWPAdapter.h>


#include <bldvariant.hrh>
#include <featmgr.h>					// Feature manager
#include <CWPAdapter.h>					// Leave code for protected	
#include "CWPAPItemBase.h"
#include "CWPAPNapdef.h"
#include "WPAPAdapter.pan"
#include "ProvisioningDebug.h"
#include <cmconnectionmethodext.h>
#include <cmmanagerext.h>
#include <cmpluginpacketdatadef.h> 
#include <cmpluginwlandef.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::NewLC
// -----------------------------------------------------------------------------
CWPAPAccesspointItem* CWPAPAccesspointItem::NewLC(
                                            const TDesC& aDefaultName,
                                            CWPAPItemBase* aLogicalProxy,
                                            CWPAPItemBase* aPhysicalProxy,
                                            CWPAPItemBase* aNapdef,
                                            CWPCharacteristic& aCharacteristic )
	{
	CWPAPAccesspointItem* self = 
                        new(ELeave) CWPAPAccesspointItem( aDefaultName, 
                                                          aLogicalProxy,
                                                          aPhysicalProxy,
                                                          aNapdef,
                                                          aCharacteristic ); 
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPAPAccesspointItem::~CWPAPAccesspointItem()
	{
	}

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPAPAccesspointItem::Name() const
	{
	if ( iLogicalProxy && iLogicalProxy->Name() != KNullDesC )
        {
        return iLogicalProxy->Name();
		}
    else if( iPhysicalProxy && iPhysicalProxy->Name() != KNullDesC )
        {
        return iPhysicalProxy->Name();
        }
    else if( iNapdef && iNapdef->Name() != KNullDesC )
        {
        return iNapdef->Name();
        }
	else
		{
		return iDefaultName;
		}
	}
// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::NapDef
// -----------------------------------------------------------------------------
//
CWPAPItemBase* CWPAPAccesspointItem::NapDef()
	{
	return iNapdef;
	}
	

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::SetCMManager
// -----------------------------------------------------------------------------
//
void CWPAPAccesspointItem::SetCMManager( RCmManagerExt* aCm )
	{
	iCmManager = aCm;
	}

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::Characteristic
// -----------------------------------------------------------------------------
//
TInt CWPAPAccesspointItem::Characteristic()
	{
	return iCharacteristic.Type();
	}
	

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::SaveL
// -----------------------------------------------------------------------------
//
TUint32 CWPAPAccesspointItem::SaveL( CCommsDatabase& aCommsDb )
	{
	FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::SaveL:" ) );
	
	if ( FeatureManager::FeatureSupported( KFeatureIdSettingsProtection ) )
		{
		// This protection check is no longer needed since CMManager 
		// initiates the transactions to the database
		/*if( CApProtHandler::IsTableProtectedL( &aCommsDb ) )
			{
			// Table protected -> leave with specific code.
			User::Leave( EWPAccessPointsProtected );
			}*/	
		}

	  TUint32 iap = 0;
    TRAPD( err,iap = StoreL( aCommsDb ) );
    if( err == KErrLocked )
        {
        err = EWPCommsDBLocked;
        }
    User::LeaveIfError( err );
    return iap;
	}

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::StoreL
// -----------------------------------------------------------------------------
//
TUint32 CWPAPAccesspointItem::StoreL( CCommsDatabase& aCommsDb )
	{
	FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL:" ) );
    
    // CMManager
    // Create the cmIten, give the bearer as parameter if it is available
    RCmConnectionMethodExt cmItem;
    TBool created = EFalse;
    TUint bearer;
    TInt err(KErrNone);
    
    // If the NapDef object exists and the bearer is defined, then we can
    // create the connection method using the bearer
    if ( iNapdef != NULL )
    	{
    	bearer = static_cast<CWPAPNapdef*>(iNapdef)->Bearer();
    	if ( bearer != 0 )
    		{
    		cmItem = iCmManager->CreateConnectionMethodL(bearer);
    		created = ETrue;
    		}
    	}
    	
    // If the bearer id is not known we will use packet data
    if ( !created)
    	{
    	cmItem = iCmManager->CreateConnectionMethodL(KUidPacketDataBearerType);
    	}
    // Create the connection method into the DB
    TRAP( err,cmItem.UpdateL() );
    // Leave if the creation fails for some reason
    if(err!=KErrNone)
		{
       	User::Leave(err);
       	}
       
    if( iNapdef )
        {
        FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL: napdef" ) );
        
        // CMManager 
        TRAP (err, iNapdef->AddDataL( cmItem ));
       	TRAP ( err,cmItem.UpdateL() );
       	// Leave if the update of the parameters fails
    	if(err!=KErrNone)
			{
       		User::Leave(err);
       		}
        }
        
    if( iLogicalProxy )
        {
        FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL: logicalproxy" ) );
        // CMManager 
        iLogicalProxy->AddDataL( cmItem );
        TRAP( err,cmItem.UpdateL() );
        // Leave if the update of the parameters fails
    	if(err!=KErrNone)
			{
       		User::Leave(err);
       		}
        }

    if( iPhysicalProxy )
        {
        FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL: physicalproxy" ) );
        // CMManager 
        iPhysicalProxy->AddDataL( cmItem );
        TRAP( err,cmItem.UpdateL() );
        // Leave if the update of the parameters fails
    	if(err!=KErrNone)
			{
       		User::Leave(err);
       		}
        }

    FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL: cmItem.UpdateL" ) );
    
    // CMManager
    iUid = cmItem.GetIntAttributeL( CMManager::ECmWapId );
    
    TInt test( KErrNone );
    // Update parameter data to CM
    TRAP( test,cmItem.UpdateL() );
    FTRACE(RDebug::Print(_L("[AccessPointAdapter] CWPAPAccesspointItem::StoreL: UpdateL err: %d"), test ));
    
    TUint32 cmId = cmItem.GetIntAttributeL( CMManager::ECmId ); 
    cmItem.Close();
    
	if( iNapdef )
    	{
		iNapdef->UpdateLingerL(iUid());
    	}
       

    FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL: SaveWlanDataL" ) );
    if( iNapdef )
        { 
        // Give CMManager pointer and save WLAN data
		static_cast<CWPAPNapdef*>(iNapdef)->SetCMManager( iCmManager );
        iNapdef->SaveWlanDataL( cmId, aCommsDb );
        }
    
    
    TInt i( 0 );
    TPtrC8 data( iCharacteristic.Data(i) );
    while( data != KNullDesC8 )
        {
        i++;
        data.Set( iCharacteristic.Data(i) );
        }
        
    iCharacteristic.SetDataL( iUid, i );
    
    FLOG( _L( "[AccessPointAdapter] CWPAPAccesspointItem::StoreL: done" ) );
    return iUid();
    }

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::Uid
// -----------------------------------------------------------------------------
//
const TDesC8& CWPAPAccesspointItem::Uid() const
	{
    return iUid;
    }

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::DeleteUidData
// -----------------------------------------------------------------------------
//
void CWPAPAccesspointItem::DeleteUidData() const
	{
    iCharacteristic.DeleteAllData();
    }

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::CWPAPAccesspointItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAPAccesspointItem::CWPAPAccesspointItem( const TDesC& aDefaultName,
                                            CWPAPItemBase* aLogicalProxy,
                                            CWPAPItemBase* aPhysicalProxy,
                                            CWPAPItemBase* aNapdef,
                                            CWPCharacteristic& aCharacteristic )
                                        : iDefaultName( aDefaultName ),
                                          iNapdef( aNapdef ),
                                          iPhysicalProxy( aPhysicalProxy ),
                                          iLogicalProxy( aLogicalProxy ),
                                          iCharacteristic( aCharacteristic )
	{
	}

// -----------------------------------------------------------------------------
// CWPAPAccesspointItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAPAccesspointItem::ConstructL()
	{
	}

//  End of File
