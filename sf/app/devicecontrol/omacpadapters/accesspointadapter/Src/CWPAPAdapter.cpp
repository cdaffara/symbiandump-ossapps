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
* Description:   Main class for handling provisioning accesspoint data
*
*/



// INCLUDE FILES

#include "CWPAPAdapter.h"			// Own header

#include <CWPCharacteristic.h>
#include <commdb.h>
#include <wpapadapterresource.rsg>
#include <featmgr.h>					// Feature manager
#include "WPAPUtil.h"
#include "CWPAPAccesspointItem.h"
#include "CWPAPNapdef.h"
#include "CWPAPLogicalProxy.h"
#include "WPAPAdapter.pan"
#include "CWPLog.h"
#include "ProvisioningDebug.h"
#include "WPAPDefs.h"
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <WPAdapterUtil.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAPAdapter::CWPAPAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAPAdapter::CWPAPAdapter() : CWPAdapter(), 
                               iLinks( KInitialArraySize ), 
                               iDataItems( KInitialArraySize ), 
                               iSaveItems( KInitialArraySize )
	{
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::ConstructL()
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::ConstructL:" ) );
	
	iTitle = WPAPUtil::ReadHBufCL( R_QTN_SM_WML_ACCESSPOINTS );
	iDefaultName = WPAPUtil::ReadHBufCL( R_QTN_SM_WAPAP_NAME );

	FeatureManager::InitializeLibL();
	
	iCmManager = new RCmManagerExt;
	iCmManager->OpenL();
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPAPAdapter* CWPAPAdapter::NewL()
	{
	CWPAPAdapter* self = new(ELeave) CWPAPAdapter; 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPAPAdapter::~CWPAPAdapter()
	{
	// Contents of iLinks array are not owned
    iLinks.Close();

    iDataItems.ResetAndDestroy();
    iDataItems.Close();

    iSaveItems.ResetAndDestroy();
    iSaveItems.Close();
    iAPValue.Reset();

	delete iTitle;
    delete iDefaultName;
	delete iCommsDb;
	
	if ( iCmManager !=  NULL )
	    {
	    iCmManager->Close();
	    delete iCmManager;
	    }
	
    FeatureManager::UnInitializeLib();
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::ItemCount
// -----------------------------------------------------------------------------
//
TInt CWPAPAdapter::ItemCount() const
	{
	LOG1("Number of Access points: %d", iSaveItems.Count() );
	return iSaveItems.Count();
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CWPAPAdapter::SummaryTitle(TInt aIndex) const
	{
	__ASSERT_DEBUG( aIndex >= 0 && aIndex < iSaveItems.Count(), 
		Panic( EIndexOutOfBounds ));
	// The title is always the same.
	return *iTitle;
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CWPAPAdapter::SummaryText(TInt aIndex) const
	{
	__ASSERT_DEBUG( aIndex >= 0 && aIndex < iSaveItems.Count(), 
		Panic( EIndexOutOfBounds ));
	return iSaveItems[ aIndex ]->Name();
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CWPAPAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/)
	{
	// Detail view is a feature for later release.
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::SaveL( TInt aItem )
	{
    FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::SaveL:" ) );

	__ASSERT_DEBUG(aItem >= 0 &&
		aItem < iSaveItems.Count(), 
		Panic( EPreConditionFail ));

    TBool internetparmfound = EFalse;
    TInt err1;

    for (TInt i = 0; i < iAPValue.Count(); i++)
        {
        if (aItem == iAPValue[i])
            {
            internetparmfound = ETrue;
            break;
            }

        }

	// Create the commsdb when the save is initiated first time.
	// This stays alive until the adapter is destroyed.
	if ( !iCommsDb )
		{
		iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
		}

    if( iAlreadySavedOnce && aItem == 0 )
        {
        for( TInt i( 0 ); i < iSaveItems.Count(); i++ )
            {
            iSaveItems[i]->DeleteUidData();
            }
        }
    
    // ********************************************************************************** //            	
    // ***** check that the napdefs with a certain id are only saved once SKSS-79REKH *** //
    // ********************************************************************************** //
    TBool found = EFalse;
    if (iSaveItems[ aItem ]->Characteristic() == KWPNapDef)
    	{
    	
    	// Get the napdef pointer from the saved items and check that it is not NULL
    	CWPAPNapdef* napdef = static_cast<CWPAPNapdef*>(iSaveItems[ aItem ]->NapDef());

    	if ( napdef )
        	{
        	// Store the id of the NAP that is being saved now
        	TBufC<KNameMaxLength> napId = napdef->NapId();
        
        	// check whether napdef with same id has been already saved.
        	for ( TInt j( 0 ); j < aItem; j++ )
        	    {
        	    // If a napdef is not in question we can move to the next item
        	    if ( iSaveItems[ j ]->Characteristic() != KWPNapDef)
        	   		{
        	    	continue;
        	    	}
        	    	
        	    // Store the pointer to to comparison napdef and make the nap id check if
        	    // the pointer is other than NULL
        	    CWPAPNapdef* tmpNap = static_cast<CWPAPNapdef*>(iSaveItems[ j ]->NapDef());
        	    
        	    if ( tmpNap != NULL )
        	        {
        	        // If the id was found, the napdef with this id is already stored
        	        if ( tmpNap->NapId() == napId )
        	        	{
        	        	found = ETrue;
        	        	break;
        	 	       	}
       	         	}
        	    }
        	}
    	}
        
    // if access point is not already stored, then we store it
    if ( !found )
        {
        TUint32 iap = 0;
        if (internetparmfound)
            {    
            // Give pointer to cm manager to access point item
            iSaveItems[ aItem ]->SetCMManager(iCmManager);
            TRAPD(err,iap = iSaveItems[ aItem ]->SaveL( *iCommsDb));
            if(err!=KErrNone)
			          {
			          User::LeaveIfError(err);	
			          }
			          
			     TRAP(err1, WPAdapterUtil::SetAPDetailsL(iap));
			     if (err1 == KErrNone)
               {
               iAlreadySavedOnce = ETrue;
               }
           }
			   
			   if (!internetparmfound || (internetparmfound && err1 != KErrNone))
             {     
             // Give pointer to cm manager to access point item
            iSaveItems[ aItem ]->SetCMManager(iCmManager);
            TRAPD(err,iap = iSaveItems[ aItem ]->SaveL( *iCommsDb));
            if(err!=KErrNone)
			          {
			          User::LeaveIfError(err);	
			          }
			      iAlreadySavedOnce = ETrue;
            }
        }
  FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::SaveL: done" ) );
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPAPAdapter::CanSetAsDefault( TInt /*aItem*/ ) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::SetAsDefault
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::SetAsDefaultL( TInt /*aItem*/ )
	{
	// This shouldn't be called because CanSetAsDefault
	// always returns EFalse.
	Panic( EPreConditionFail );
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::VisitL( CWPCharacteristic& aCharacteristic )
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::VisitL:" ) );
	
	TInt type = aCharacteristic.Type();

	if ( type == KWPApplication )
		{
		FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::VisitL: KWPApplication" ) );
		aCharacteristic.AcceptL( *this );
		}
	// Check every NAPDEF characteristic. If it contains INTERNET
	// parameter then new access point is created from NAPDEF data.
	else if ( type == KWPNapDef && !IsAlreadyLinked( aCharacteristic ) )
		{
		FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::VisitL: KWPNapDef" ) );
        CWPAPNapdef* item = CWPAPNapdef::NewLC( EFalse, 
            *iDefaultName, 
            aCharacteristic,
            aCharacteristic );
        if( item->AddItemsL( iSaveItems, NULL, NULL ) > 0 )
            {
            User::LeaveIfError( iLinks.InsertInAddressOrder( &aCharacteristic ) );
            }
        User::LeaveIfError( iDataItems.Append( item ) );
        
       
    TInt val = item->GetInternetIndicator();
    if (val != -1)
        {
        iAPValue.Append(val);
        }
    CleanupStack::Pop( item );
		}
		FLOG( _L( "[AccesspointAdapter] CWPAPAdapter::VisitL:done" ) );

	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::VisitL( CWPParameter& /*aParameter*/ ) 
	{
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::VisitLinkL
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::VisitLinkL( CWPCharacteristic& aCharacteristic )
	{
    if( !IsAlreadyLinked( aCharacteristic ) )
        {
        switch( aCharacteristic.Type() )
            {
            // This is link in APPLICATION characteristic to certain NAPDEF or LOGICAL proxy
            case KWPPxLogical:
                {
                CWPAPLogicalProxy* item = CWPAPLogicalProxy::NewLC( 
                    *iDefaultName, aCharacteristic );
                item->AddItemsL( iSaveItems, NULL, NULL );
                User::LeaveIfError( iLinks.InsertInAddressOrder( &aCharacteristic ) );
                User::LeaveIfError( iDataItems.Append( item ) );
                CleanupStack::Pop( item );
                break;
                }
            case KWPNapDef:
                {
                CWPAPNapdef* item = CWPAPNapdef::NewLC( ETrue, 
                    *iDefaultName, 
                    aCharacteristic,
                    aCharacteristic );
                item->AddItemsL( iSaveItems, NULL, NULL );
                User::LeaveIfError( iLinks.InsertInAddressOrder( &aCharacteristic ) );
                User::LeaveIfError( iDataItems.Append( item ) );
                CleanupStack::Pop( item );
                break;
                }
            }
        }
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CWPAPAdapter::ContextExtension( MWPContextExtension*& aContextExtension )
	{
	aContextExtension = this;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CWPAPAdapter::SaveDataL( TInt aIndex ) const
	{
	return iSaveItems[ aIndex ]->Uid();
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CWPAPAdapter::DeleteL( const TDesC8& aSaveData )
	{
	if ( !iCommsDb )
		{
		iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
		}

    TPckgBuf<TUint32> uid;
    if( aSaveData.Length() == uid.MaxLength() )
        {
        LOG1( "DeleteL: Deleting %08x", uid() );
        uid.Copy( aSaveData );

        // CMManager
        TRAPD( err, iCmManager->ConnectionMethodL( uid()).DeleteL() );
        
        LOG1( "DeleteL: Result: %d", err );
        if( err == KErrLocked )
            {
            User::Leave( EWPCommsDBLocked );
            }
        else if ( err != KErrNone && err != KErrNotFound )
            {
            User::Leave( err );
            }
        }

	//CleanupStack::PopAndDestroy( dataHandler );
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::Uid
// -----------------------------------------------------------------------------
//
TUint32 CWPAPAdapter::Uid() const
	{
    return iDtor_ID_Key.iUid;
	}

// -----------------------------------------------------------------------------
// CWPAPAdapter::IsAlreadyLinked
// -----------------------------------------------------------------------------
//
TBool CWPAPAdapter::IsAlreadyLinked( CWPCharacteristic& aCharacteristic )
	{
    return iLinks.FindInAddressOrder( &aCharacteristic ) >= 0;
	}

//  End of File  
