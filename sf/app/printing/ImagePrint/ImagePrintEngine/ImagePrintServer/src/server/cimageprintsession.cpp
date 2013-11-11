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


#include <usbman.h>
#include <usbstates.h>
#include <usbpersonalityids.h>

#include "cimageprintsession.h"
#include "imageprintclientserver.h"
#include "cimageprintserver.h"
#include "clog.h"
#include "cimageprintbody.h"
#include "cserveridleguard.h"
#include "cserverdiscoveryguard.h"
#include "cserverjobguard.h"
#include "cnumofcopies.h"
 
namespace
	{
	// panic
	_LIT( KImagePrintSessionPanic, "CIPSes" );
	enum TImagePrintSessionPanic
		{
		ENotSupported
		};
	
	void PanicClient( const TMessageWrp2& aMessage, TImagePrintSessionPanic aCategory )
		{
		aMessage.Panic( KImagePrintSessionPanic, aCategory );
		}
	}

CImagePrintSession::~CImagePrintSession()
    { 
    LOG("CImagePrintSession::~CImagePrintSession begin");
    delete iNumOfCopies;   
    delete iIdleGuard;
    delete iDiscoveryGuard;
    delete iJobGuard;
    iCapabilityIDs.Close();
    iMsgWrappers.ResetAndDestroy();
    iMsgWrappers.Close();
    Server().ReleaseEngine( this );
	Server().RemoveSession();
	LOG("CImagePrintSession::~CImagePrintSession end");
    }

CImagePrintServer& CImagePrintSession::Server()
    {	
	return *static_cast<CImagePrintServer*>( const_cast<CServer2*>( CSession2::Server() ) );
    }

void CImagePrintSession::CreateL()
    {
    LOG("CImagePrintSession::CreateL begin");
	Server().AddSession();
	iIdleGuard =  CServerIdleGuard::NewL( Server().Engine() ); 
	iDiscoveryGuard = CServerDiscoveryGuard::NewL( Server().Engine() );
	iJobGuard = CServerJobGuard::NewL( Server().Engine() );
	iNumOfCopies = CNumOfCopies::NewL();
	iConnected = ETrue;
	LOG("CImagePrintSession::CreateL end");
    }

void CImagePrintSession::Disconnect(const RMessage2 &aMessage)
	{
	LOG("CImagePrintSession::Disconnect begin");
	Server().Engine().HandleSessionDisconnect( iIdleGuard, iDiscoveryGuard, iJobGuard );
	iConnected = EFalse;	
	CSession2::Disconnect( aMessage );
	LOG("CImagePrintSession::Disconnect end");
	}

void CImagePrintSession::ServiceL( const RMessage2& aMessage )
    {
    LOG1("CImagePrintSession::ServiceL aMessage.Function(): %d", aMessage.Function());
    
    // wrapper array clean begin
	TInt index = iMsgWrappers.Count();
	LOG1("CImagePrintSession::ServiceL index: %d", index);
	TBool result( EFalse );
	while( --index >= 0 && !result )
		{
		if( ( iMsgWrappers[index] )->Disposable() )
			{
			TMessageWrp2* toDispose = iMsgWrappers[index];
			iMsgWrappers.Remove( index );
			delete toDispose;
			toDispose = NULL;
			result = ETrue;
			}
		}	
	// wrapper array clean end
    
    TMessageWrp2* msgWrapper = new (ELeave) TMessageWrp2( aMessage, iConnected );    
    TInt err = iMsgWrappers.Insert( msgWrapper, 0 );
    if( err )
    	{
    	delete msgWrapper;
    	User::Leave( err );
    	}
    else
    	{
    	msgWrapper = NULL;
    	}    
      
    TInt messageIndex = aMessage.Function();
    
    if ( messageIndex <= EContinuePrintJob )
		{
	    ServiceFirstPartL( messageIndex );
		}
    else if ( messageIndex >= ERemoveCachedPrinter && messageIndex <= EGetPrinterCapHigh )
    	{
		ServiceSecondPartL( messageIndex );				
    	}		
    else if ( messageIndex >= EGetPrinterCapEnumCount && messageIndex <= ESetNumberOfCopies )
    	{
		ServiceThirdPartL ( messageIndex );
    	}
    else 
    	{		
    	PanicClient( *iMsgWrappers[0], ENotSupported );    
    	}	

	LOG("CImagePrintSession::ServiceL end");
    }
void CImagePrintSession::ServiceFirstPartL( TInt aIndex )
	{
    switch( aIndex )
		{
		case ECountConnections:
			CountConnectionsL( *iMsgWrappers[0] );
			break;	
			
		case ESetForeground:
			SetForegroundL( *iMsgWrappers[0] );
			break;	
			
		case EReserveEngine:
			ReserveEngine( *iMsgWrappers[0] );
			break;

		case EReleaseEngine:
			ReleaseEngine( *iMsgWrappers[0] );
			break;	

		case ESupportedProtocols:
			SupportedProtocols( *iMsgWrappers[0] );
			break;	
			
		case EGetNumPrintPages:
			GetNumPrintPages( *iMsgWrappers[0] );
			break;	
			
		case EGetJobStatus:
			GetJobStatus( *iMsgWrappers[0] );
			break;	
			
		case EGetPrinterStatus:
			GetPrinterStatus( *iMsgWrappers[0] );
			break;	
			
		case ECancelDiscovery:
			CancelDiscovery( *iMsgWrappers[0] );
			break;	
			
		case ESubmitPrintJob:
			SubmitPrintJobL( *iMsgWrappers[0] );
			break;	
			
		case ECancelPrintJob:
			CancelPrintJob( *iMsgWrappers[0] );
			break;
			
		case EContinuePrintJob:
			ContinuePrintJobL( *iMsgWrappers[0] );
			break;
			
		default:
		LOG1("[CImagePrintSession::ServiceFirstPartL]\t Method reached to an end with index: %d", aIndex);
			break;
		}
	}
            
void CImagePrintSession::ServiceSecondPartL( TInt aIndex )
	{
    switch( aIndex )
		{
		case ERemoveCachedPrinter:
			RemoveCachedPrinterL( *iMsgWrappers[0] );
			break;
		
		case EGetJobTemplateIcon:
			GetJobTemplateIconL( *iMsgWrappers[0] );
			break;
			
		case EGetNumPreviewPages:
			GetNumPreviewPages( *iMsgWrappers[0] );
			break;
		
		case ESetJobSetting:
			SetJobSettingL( *iMsgWrappers[0] );
			break;	
		
		case EGetJobSetting:
			GetJobSettingL( *iMsgWrappers[0] );
			break;
			
		case EGetPrinterCapability:
			GetPrinterCapabilityL( *iMsgWrappers[0] );
			break;
			
		case EGetPrinterCapId:
			GetPrinterCapIdL( *iMsgWrappers[0] );
			break;
					
		case EGetPrinterCapType:
			GetPrinterCapTypeL( *iMsgWrappers[0] );
			break;
		
    	case EGetPrinterCapDef:
			GetPrinterCapDefL( *iMsgWrappers[0] );
			break;
    				
		case EGetPrinterCapLow:
			GetPrinterCapLowL( *iMsgWrappers[0] );
			break;
			
		case EGetPrinterCapHigh:
			GetPrinterCapHighL( *iMsgWrappers[0] );
			break;	
			
		default:
			LOG1("[CImagePrintSession::ServiceSecondPartL]\t Method reached to an end with index: %d", aIndex);
			break;
		}
	}

void CImagePrintSession::ServiceThirdPartL( TInt aIndex )
	{
    switch( aIndex )
		{
		case EGetPrinterCapEnumCount:
			GetPrinterCapEnumCountL( *iMsgWrappers[0] );
			break;
			
		case EGetPrinterCapEnum:
			GetPrinterCapEnumL( *iMsgWrappers[0] );
			break;	
																										
		case EGetPrinterCapabilityIDsCount:
			GetPrinterCapabilityIDsCountL( *iMsgWrappers[0] );
			break;
			
		case EGetPrinterCapabilityID:
			GetPrinterCapabilityIDL( *iMsgWrappers[0] );
			break;
				
		case ERegisterIdleObserver:
			RegisterIdleObserverL( *iMsgWrappers[0] );
			break;
			
		case ECancelRegisterIdleObserver:
			CancelRegisterIdleObserverL( *iMsgWrappers[0] );
			break;
				
		case EStartDiscovery:
			StartDiscoveryL( *iMsgWrappers[0] );
			break;
			
		case EContinueDiscovery:
			ContinueDiscoveryL( *iMsgWrappers[0] );
			break;
			
		case EPrepareJob:
			PrepareJobL( *iMsgWrappers[0] );
			break;
			
		case ECreateJob:
			CreateJobL( *iMsgWrappers[0] );
			break;			
		
		case EContinueCreateJob:
			ContinueCreateJobL( *iMsgWrappers[0] );
			break;
			
		case ECancelStartDiscovery:
			CancelStartDiscoveryL( *iMsgWrappers[0] );
			break;
			
		case ECancelCreateJob:
			CancelCreateJobL( *iMsgWrappers[0] );
			break;
			
		case EIsPictBridgeMode:
			IsPictBridgeModeL( *iMsgWrappers[0] );
			break;
			
		case ESetNumberOfCopiesCount:
			SetNumberOfCopiesCountL( *iMsgWrappers[0] );
			break;
			
		case ESetNumberOfCopies:
			SetNumberOfCopiesL( *iMsgWrappers[0] );
			break;
			
		default:
			LOG1("[CImagePrintSession::ServiceThirdPartL]\t Method reached to an end with index: %d", aIndex);
			break;
		}
	}


void CImagePrintSession::CountConnectionsL( TMessageWrp2& aMessage )
	{	
	TInt count = Server().SessionCount();
	TPckgC<TInt> countDes( count );
	aMessage.WriteL( 0, countDes );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNone );
	}
		
void CImagePrintSession::SetForegroundL( TMessageWrp2& aMessage )
	{
	TInt fg = aMessage.Int0();
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNone );
	}
	
void CImagePrintSession::ReserveEngine( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}

void CImagePrintSession::ReleaseEngine( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReleaseEngine( this );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::SupportedProtocols( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	if( !err )	
		{
		TInt protocols = Server().Engine().SupportedProtocols();
		aMessage.Complete( protocols );	
		}
	else
		{
		aMessage.Complete( err );	
		}
	}
	
void CImagePrintSession::GetNumPrintPages( TMessageWrp2& aMessage )
{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	if( !err )	
		{
		TInt pages = Server().Engine().GetNumPrintPages();
		aMessage.Complete( pages );	
		}
	else
		{
		aMessage.Complete( err );	
		}
	}	

void CImagePrintSession::GetJobStatus( TMessageWrp2& aMessage )
{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	if( !err )	
		{
		TInt status = Server().Engine().GetJobStatus();
		aMessage.Complete( status );	
		}
	else
		{
		aMessage.Complete( err );	
		}
	}	
	
void CImagePrintSession::GetPrinterStatus( TMessageWrp2& aMessage )
{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	if( !err )	
		{
		TInt id = aMessage.Int0();
		TInt status = Server().Engine().GetPrinterStatus( id );
		aMessage.Complete( status );	
		}
	else
		{
		aMessage.Complete( err );	
		}
	}
	
void CImagePrintSession::CancelDiscovery( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		Server().Engine().CancelDiscovery();
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CImagePrintSession::SubmitPrintJobL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		Server().Engine().SubmitPrintJobL();			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CImagePrintSession::CancelPrintJob( TMessageWrp2& aMessage )
	{	
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		err = Server().Engine().CancelPrintJob();
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CImagePrintSession::ContinuePrintJobL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		err = Server().Engine().ContinuePrintJobL();			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}			

void CImagePrintSession::RemoveCachedPrinterL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		TInt id = aMessage.Int0();
		err = Server().Engine().RemoveCachedPrinterL( id );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}				

void CImagePrintSession::GetJobTemplateIconL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		TInt id = aMessage.Int0();
		TInt handle;
		err = Server().Engine().GetJobTemplateIconL( id, handle );
		TPckgC<TInt> handleDes( handle );
		aMessage.WriteL( 1, handleDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}

void CImagePrintSession::GetNumPreviewPages( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	if( !err )	
		{
		TInt num = Server().Engine().GetNumPreviewPages();
		aMessage.Complete( num );	
		}
	else
		{
		aMessage.Complete( err );	
		}
	}
	
void CImagePrintSession::SetJobSettingL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		TInt capabilityID = aMessage.Int0();
		TInt value = aMessage.Int1();		
		TInt affectedCapability;
		
		LOG2("CImagePrintSession::SetJobSettingL || capabilityID = %d, value = %d", capabilityID, value );
		
		err = Server().Engine().SetJobSettingL( capabilityID, value, affectedCapability );
		TPckgC<TInt> affectedCapabilityDes( affectedCapability );
		aMessage.WriteL( 2, affectedCapabilityDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CImagePrintSession::GetJobSettingL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		TInt capabilityID = aMessage.Int0();
		TInt value;		
		err = Server().Engine().GetJobSetting( capabilityID, value );
		TPckgC<TInt> valueDes( value );
		aMessage.WriteL( 1, valueDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}			
	
void CImagePrintSession::GetPrinterCapabilityL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		TInt printerID = aMessage.Int0();
		TInt capabilityID = aMessage.Int1();	
		err = Server().Engine().GetPrinterCapabilityL( printerID, capabilityID, iCapability );
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}
	
void CImagePrintSession::GetPrinterCapIdL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{		
		TPckgC<TInt> capIdDes( iCapability.iCapabilityID );
		aMessage.WriteL( 0, capIdDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapTypeL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{		
		TPckgC<TInt> typeDes( iCapability.iType );
		aMessage.WriteL( 0, typeDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapDefL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{		
		TPckgC<TInt> defDes( iCapability.iDefaultValue );
		aMessage.WriteL( 0, defDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapLowL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{		
		TPckgC<TInt> lowDes( iCapability.iLow );
		aMessage.WriteL( 0, lowDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapHighL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{		
		TPckgC<TInt> highDes( iCapability.iHigh );
		aMessage.WriteL( 0, highDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapEnumCountL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{		
		TPckgC<TInt> countDes( iCapability.iEnumCount );
		aMessage.WriteL( 0, countDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapEnumL( TMessageWrp2& aMessage )	
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		TInt index = aMessage.Int0();		
		TPckgC<TInt> enumDes( iCapability.iEnumCodes[index] );
		aMessage.WriteL( 1, enumDes );			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}
	

void CImagePrintSession::GetPrinterCapabilityIDsCountL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )
		{
		iCapabilityIDs.Reset();
		TInt printerID = aMessage.Int0();	
		err = Server().Engine().GetPrinterCapabilityIDsL( printerID, iCapabilityIDs );
		if( !err )
			{
			TInt count = iCapabilityIDs.Count();
			TPckgC<TInt> countDes( count );
			aMessage.WriteL( 1, countDes );	
			}
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::GetPrinterCapabilityIDL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )
		{
		TInt index = aMessage.Int0();
		TInt id = iCapabilityIDs[index];
		TPckgC<TInt> idDes( id );
		aMessage.WriteL( 1, idDes );
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CImagePrintSession::RegisterIdleObserverL( TMessageWrp2& aMessage )
	{
	iIdleGuard->Guard( aMessage );	
	}
	
void CImagePrintSession::CancelRegisterIdleObserverL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		iIdleGuard->Stop();					
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );			
	}	
	
void CImagePrintSession::StartDiscoveryL( TMessageWrp2& aMessage )
	{
	TInt err = iDiscoveryGuard->StartL( aMessage );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::ContinueDiscoveryL( TMessageWrp2& aMessage )
	{
	iDiscoveryGuard->ContinueL( aMessage );	
	}
	
void CImagePrintSession::PrepareJobL( TMessageWrp2& aMessage )
	{
	iJobGuard->PrepareL( aMessage );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNone );
	}

void CImagePrintSession::CreateJobL( TMessageWrp2& aMessage )
	{
	TInt err = iJobGuard->CreateL( aMessage );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CImagePrintSession::ContinueCreateJobL( TMessageWrp2& aMessage )
	{
	iJobGuard->ContinueCreateL( aMessage );
	}
	
void CImagePrintSession::CancelStartDiscoveryL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		iDiscoveryGuard->Stop();					
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CImagePrintSession::CancelCreateJobL( TMessageWrp2& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		iJobGuard->Stop();					
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}
	
void CImagePrintSession::IsPictBridgeModeL( TMessageWrp2& aMessage )
	{
		RUsb usbManager;
		TUsbDeviceState dState;
		TInt personality;
		TInt err = EFalse;
		User::LeaveIfError( usbManager.Connect());
		User::LeaveIfError( usbManager.GetDeviceState(dState));
		User::LeaveIfError( usbManager.GetCurrentPersonalityId(personality));
		usbManager.Close();
		
		/* 
		*	If there is some problems with pictbridge, check from usbpersonalityids.h that constants haven't been changed.
		*/
        if( dState == EUsbDeviceStateConfigured && 
                ((personality == KUsbPersonalityIdMTP)||(personality == KUsbPersonalityIdPCSuiteMTP)))
			err = ETrue;		

		aMessage.SetDisposable( ETrue );
		aMessage.Complete( err );
	}	

void CImagePrintSession::SetNumberOfCopiesCountL( TMessageWrp2& aMessage )
	{
	LOG("CImagePrintSession::CheckNumberOfCopiesL begin");
	TInt count = aMessage.Int0();
	LOG1("CImagePrintSession::CheckNumberOfCopiesL count: %d", count);
	TInt err = ( count > 0 ) ? KErrNone : KErrInvalidData;
	if( !err )
		{
		iNumOfCopies->Reset( count );
		}					
	LOG1("CImagePrintSession::CheckNumberOfCopiesL return: %d", err);
	aMessage.SetDisposable( ETrue );	
	aMessage.Complete( err );
	}

void CImagePrintSession::SetNumberOfCopiesL( TMessageWrp2& aMessage )
	{
	LOG("CImagePrintSession::SetNumberOfCopiesL begin");
	TInt copies = aMessage.Int0();
	LOG1("CImagePrintSession::SetNumberOfCopiesL copies: %d", copies);	
	TInt err = ( copies < 0 ) ? KErrInvalidData : KErrNone;
	if( !err )
		{
		TBool full;
		iNumOfCopies->AddNumOfCopyL( copies, full );
		LOG1("CImagePrintSession::SetNumberOfCopiesL full: %d", full);
		if( full )
			{
			Server().Engine().SetNumsOfCopiesL( iNumOfCopies->NumsOfCopies(), err );	
			}						
		}	
	LOG1("CImagePrintSession::SetNumberOfCopiesL return: %d", err);
	aMessage.SetDisposable( ETrue );	
	aMessage.Complete( err );
	}

//  End of File
