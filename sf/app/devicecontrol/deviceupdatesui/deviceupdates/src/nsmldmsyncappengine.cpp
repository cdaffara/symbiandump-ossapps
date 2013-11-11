/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods for CNSmlDMSyncAppEngine
*
*/


#include <collate.h>
#include <e32property.h>
#include "nsmldmsyncappengine.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmsynchandler.h"
#include "nsmldmsyncinternalpskeys.h"
#include "dminforetrieval.h"

// ============================ MEMBER FUNCTIONS ==============================
//

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::CNSmlDMSyncAppEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncAppEngine::CNSmlDMSyncAppEngine() : iProgressShown( ETrue )
	{
	FLOG( "[OMADM] CNSmlDMSyncAppEngine::CNSmlDMSyncAppEngine:" );
	
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::ConstructL(DmInfo* dmcallback)
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::ConstructL:" );
    
    iSyncMLSession.OpenL();  
    iDmcallback = dmcallback;
    iSyncHandler = CNSmlDMSyncHandler::NewL( &iSyncMLSession,
                                             this );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncAppEngine* CNSmlDMSyncAppEngine::NewL(DmInfo* dmcallback )
	{
	FLOG( "[OMADM] CNSmlDMSyncAppEngine::NewL:" );
	
	CNSmlDMSyncAppEngine* self = new( ELeave ) CNSmlDMSyncAppEngine();
    CleanupStack::PushL( self );
    self->ConstructL( dmcallback );
    CleanupStack::Pop();

    return self;
	}

// ------------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------------
//
CNSmlDMSyncAppEngine::~CNSmlDMSyncAppEngine()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::~CNSmlDMSyncAppEngine:" );
    
    delete iSyncHandler;
    delete iProfile;        
    iSyncMLSession.Close();
	}

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::DeleteProfileL
// ------------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::DeleteProfileL( TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::DeleteProfileL:" );
    
    delete iProfile;
    iProfile = NULL;
    iSyncMLSession.DeleteProfileL( aProfileId );
	}

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::CreateProfileL
// ------------------------------------------------------------------------------
//
TInt CNSmlDMSyncAppEngine::CreateProfileL()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::CreateProfileL:" );
    
    delete iProfile;
    iProfile = NULL;
    iProfile = CNSmlDMSyncProfile::NewL( KUidSmlSyncApp.iUid, &iSyncMLSession );
    return iProfile->CreateL();
	}

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::Profile
// ------------------------------------------------------------------------------
//
CNSmlDMSyncProfile* CNSmlDMSyncAppEngine::Profile( )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::Profile:" );
    
    return iProfile;
	}
	
// ------------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::CreateCopyProfileL
// ------------------------------------------------------------------------------
//
CNSmlDMSyncProfile* CNSmlDMSyncAppEngine::CreateCopyProfileL( TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::CreateCopyProfileL:" );
    
    delete iProfile;
    iProfile = NULL;
    iProfile = CNSmlDMSyncProfile::NewL( KUidSmlSyncApp.iUid, &iSyncMLSession );
    
    iProfile->CreateCopyL( aProfileId );
    return iProfile;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession* CNSmlDMSyncAppEngine::Session()
	{
	FLOG( "[OMADM] CNSmlDMSyncAppEngine::Session:" );
	
	return &iSyncMLSession;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::OpenL
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfile* CNSmlDMSyncAppEngine::OpenProfileL( TInt aProfileId,
                                                        TInt aOpenMode )
    {
	FLOG( "[OMADM] CNSmlDMSyncAppEngine::OpenL:" );
	
    delete iProfile;
    iProfile = NULL;
    iProfile = CNSmlDMSyncProfile::NewL( KUidSmlSyncApp.iUid, &iSyncMLSession );
   	iProfile->OpenL( aProfileId, aOpenMode );
   	return iProfile;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::Close
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::CloseProfile()
    {
    FLOG(  "[OMADM] CNSmlDMSyncAppEngine::Close:" );

    delete iProfile;
    iProfile = NULL;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::SynchronizeL( TDesC& aServerName,
                                         TInt aProfileId,
                                         TInt aConnectionBearer,
                                         const TBool aUseFotaProgressNote )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::SynchronizeL:" );
    	
    iProgressShown = ETrue;
    iSyncHandler->SynchronizeL( aServerName,
                                aProfileId,
                                aConnectionBearer,
                                aUseFotaProgressNote );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::SynchronizeL( TDesC& aServerName,
                                         TInt aProfileId,
                                         TInt aJobId,
                                         TInt aConnectionBearer,
                                         const TBool aUseFotaProgressNote )
    {
   	FLOG( "[OMADM] CNSmlDMSyncAppEngine::SynchronizeL:" );

    iProgressShown = ETrue;
  TInt err = RProperty::Set( KPSUidNSmlDMSyncApp,
    				KDmJobCancel, KErrNone );
    FTRACE(FPrint(
    	_L("CNSmlDMSyncAppEngine::SynchronizeL()KDmJobCancel reset to 0 err = %d"),
    										err ) );
    iSyncHandler->SynchronizeL( aServerName, 
                                aProfileId, 
                                aJobId, 
                                aConnectionBearer,
                                aUseFotaProgressNote );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::ShowProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::ShowProgressDialogL( TBool aShow )
    {
 	FLOG( "[OMADM] CNSmlDMSyncAppEngine::ShowProgressDialog()" );
    
    if ( aShow != iProgressShown )
        {
        if ( aShow )
            {
            iSyncHandler->ShowProgressDialogL();
            }
        else
            {
            iSyncHandler->HideProgressDialogL();
            }
        iProgressShown = aShow;
        }
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::ServerIdFoundL
// -----------------------------------------------------------------------------
//    
TBool CNSmlDMSyncAppEngine::ServerIdFoundL( const TDesC& aServerId,
                                            const TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::ServerIdFoundL:" );
    
	RArray<TSmlProfileId> arr;
    iSyncMLSession.ListProfilesL( arr, ESmlDevMan );
    TBool ret = EFalse;
   	CleanupClosePushL(arr);
   	
	for ( TInt index = 0; index < arr.Count(); index++ )
		{
	    TRAPD( error, OpenProfileL( arr[index], ESmlOpenRead ) );
	    if ( error == KErrNone )
	        {
	        if ( aProfileId != Profile()->ProfileId() )
	            {	        
    	        TBuf<KNSmlMaxItemLength> buf;
    	        Profile()->GetServerId( buf );
    	        if ( buf.Compare( aServerId ) == 0 )
    	            {
    	            FLOG( "[OMADM] CNSmlDMSyncAppEngine::ServerIdFoundL: Match" );
                    ret = ETrue;
                    CloseProfile();
                    break;
                    }
	            }
		    CloseProfile();
	        }
		}
    CleanupStack::PopAndDestroy( &arr );
    return ret;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSyncAppEngine::Compare
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncAppEngine::Compare( const TDesC& aLeft, const TDesC& aRight )
	{
	FLOG( "[OMADM] CNSmlDMSyncAppEngine::Compare:" );
	
	// Empty Name() is always greater than non-empty Name()
    if (aLeft.Length() == 0 && aRight.Length() == 0)
        {
        return 0;
        }
    if (aLeft.Length() == 0)
        {
        return 1;
        }
    if (aRight.Length() == 0)
        {
        return -1;
        }

    // None of the Name()s was empty, use TDesC::CompareC to do the comparison.
    // This is from cntmodel.
    TCollationMethod collateMethod;
    // get the standard method
	collateMethod = *Mem::CollationMethodByIndex(0);
    // dont ignore punctuation and spaces
	collateMethod.iFlags |= TCollationMethod::EIgnoreNone; 
	TInt comparison( aLeft.CompareC(aRight, 3, &collateMethod) );
	return comparison;	
	}
	
// -----------------------------------------------------------------------------
//  CNSmlDMSyncAppEngine::SyncRunning
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncAppEngine::SyncRunning()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::SyncRunning:" );
    
    return iSyncHandler->SyncRunning();
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSyncAppEngine::CancelSyncL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::CancelSyncL()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::CancelSyncL()" );
    iSyncHandler->CancelSynchronizeL();
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSyncAppEngine::SyncCompleted
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppEngine::SyncCompleted( TNSmlStatus aStatus )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::SyncCompleted:" );
    iDmcallback->synccomplete(aStatus);
    if( iSyncObserver )
        {
        FLOG( "[OMADM] CNSmlDMSyncAppEngine::SyncCompleted: inform observer" );        
        iSyncObserver->SyncComplete( aStatus );
        }    
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppEngine::RequestSyncStatus
// -----------------------------------------------------------------------------
//	
void CNSmlDMSyncAppEngine::RequestSyncStatus( 
                                           MNSmlDMSyncObserver* aSyncObserver )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppEngine::RequestSyncStatus:" );    
    if( aSyncObserver )
        {
        FLOG( "[OMADM] CNSmlDMSyncAppEngine::RequestSyncStatus: added observer" );
        iSyncObserver = aSyncObserver;
        }
    }

// End of File
