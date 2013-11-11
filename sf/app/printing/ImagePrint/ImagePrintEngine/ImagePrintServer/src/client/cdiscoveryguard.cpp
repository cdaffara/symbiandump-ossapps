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


#include "cdiscoveryguard.h"
#include "rimageprintclient.h"
#include "imageprint.h"
#include "tdiscoveryguarddata.h"

EXPORT_C CDiscoveryGuard* CDiscoveryGuard::NewL( RImagePrintClient& aClient )
    {	
	CDiscoveryGuard* self = new ( ELeave ) CDiscoveryGuard( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CDiscoveryGuard::CDiscoveryGuard( RImagePrintClient& aClient ) : 
							CActive( CActive::EPriorityStandard ),
							iClient( aClient )
    {
    CActiveScheduler::Add( this );
    }

EXPORT_C CDiscoveryGuard::~CDiscoveryGuard()
    {  
    Cancel();
    delete iData;
    }

void CDiscoveryGuard::ConstructL()
    {
    iData = new (ELeave) TDiscoveryGuardData;
    }
	
EXPORT_C TInt CDiscoveryGuard::Guard( MPrinterDiscoveryObserver& aObserver, TUint aProtocols )
	{
	Cancel();	
	iObserver = &aObserver;
	TInt err = iClient.StartDiscovery( *iData, aProtocols, iStatus );
	if( !err )
		{
		SetActive();	
		}	
	return err;
	}	
	
void CDiscoveryGuard::RunL()
	{
	if( iStatus == KErrNone && iObserver )
		{
		switch( iData->iCb )
			{
			case TDiscoveryGuardData::EFoundCb:
				iObserver->FoundPrinterL( iData->iPrinterInfo );
				break;			
			case TDiscoveryGuardData::EStatusCb:
				iObserver->DiscoveryStatusL( iData->iStatus, iData->iErrorCode, iData->iErrorStringCode );
				break;				
			case TDiscoveryGuardData::ERemoveCb:
				iObserver->RemovePrinterL( iData->iPrinterInfo );
				break;				
			default:
				break;
			}
			
		iClient.ContinueDiscovery( *iData, iStatus );
		SetActive();
		}		
	}

void CDiscoveryGuard::DoCancel()
	{
	iClient.CancelStartDiscovery();		
	}

//  End of File
