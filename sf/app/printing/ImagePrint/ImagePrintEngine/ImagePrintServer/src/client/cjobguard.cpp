/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cjobguard.h"
#include "rimageprintclient.h"
#include "imageprint.h"
#include "clog.h"
#include "tjobguarddata.h"

EXPORT_C CJobGuard* CJobGuard::NewL( RImagePrintClient& aClient )
    {	
	CJobGuard* self = new ( ELeave ) CJobGuard( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CJobGuard::CJobGuard( RImagePrintClient& aClient ) : 
							CActive( CActive::EPriorityHigh ),
							iClient( aClient )
    {
    CActiveScheduler::Add( this );
    }

EXPORT_C CJobGuard::~CJobGuard()
    {  
    Cancel();
    delete iData;
    }

void CJobGuard::ConstructL()
    {
    iData = new (ELeave) TJobGuardData;
    }

EXPORT_C TInt CJobGuard::Guard( TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver )	
	{
	LOG1("CJobGuard::Guard begin with aPrinterID: %d", aPrinterID);
	Cancel();	
	iObserver = &aObserver;			
	TInt err = iClient.CreateJob( aPrinterID, *iData, aImages, iStatus );
	if( !err ) 
		{
		SetActive();	
		}
	LOG1("CJobGuard::Guard end with err: %d", err);				
	return err;
	}	
	
void CJobGuard::RunL()
	{
	LOG1("CJobGuard::RunL begin with iStatus.Int(): %d", iStatus.Int());
	if( iStatus == KErrNone && iObserver )
		{
		switch( iData->iCb )
			{
			case TJobGuardData::EProgressEventCb:
				LOG("CJobGuard::RunL TJobGuardData::EProgressEventCb");
				LOG1("CJobGuard::RunL iData->iStatus: %d", iData->iStatus);
				LOG1("CJobGuard::RunL iData->iPercentCompletion: %d", iData->iPercentCompletion);
				LOG1("CJobGuard::RunL iData->iJobStateCode: %d", iData->iJobStateCode);
				iObserver->PrintJobProgressEvent( iData->iStatus, iData->iPercentCompletion, iData->iJobStateCode );
				break;			
			case TJobGuardData::EErrorEventCb:
				LOG("CJobGuard::RunL TJobGuardData::EErrorEventCb");
				LOG1("CJobGuard::RunL iData->iError: %d", iData->iError);
				LOG1("CJobGuard::RunL iData->iErrorStringCode: %d", iData->iErrorStringCode);
				iObserver->PrintJobErrorEvent( iData->iError, iData->iErrorStringCode );
				break;			
			case TJobGuardData::EStatusEventCb:
				LOG("CJobGuard::RunL TJobGuardData::EStatusEventCb");
				LOG1("CJobGuard::RunL iData->iError: %d", iData->iError);
				LOG1("CJobGuard::RunL iData->iErrorStringCode: %d", iData->iErrorStringCode);
				iObserver->PrintJobErrorEvent( iData->iError, iData->iErrorStringCode );
				break;				
			case TJobGuardData::EImageEventCb:
				LOG("CJobGuard::RunL TJobGuardData::EImageEventCb");
				LOG1("CJobGuard::RunL iData->iFsBitmapHandle: %d", iData->iFsBitmapHandle);
				iObserver->PreviewImageEvent( iData->iFsBitmapHandle );
				break;				
			default:
				break;
			}
			
		iClient.ContinueCreateJob( *iData, iStatus );
		SetActive();
		}
	LOG("CJobGuard::RunL end");		
	}
			
void CJobGuard::DoCancel()
	{
	iClient.CancelCreateJob();			
	}

//  End of File
