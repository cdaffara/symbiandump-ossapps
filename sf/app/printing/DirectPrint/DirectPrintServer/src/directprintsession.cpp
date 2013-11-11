/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


#ifdef _DIRECTPRINT_USE_USB
#include <usbman.h>
#include <usbstates.h>
#include <usbpersonalityids.h>
#endif // _DIRECTPRINT_USE_USB

#include "directprintsession.h"
#include "directprintclientserver.h"
#include "directprintserver.h"
#include "clog.h"
#include "directprintbody.h"
#include "directprintserverjobguard.h"
#include "directprintnumofcopies.h"
 
namespace
	{
	// panic
	_LIT( KDirectPrintSessionPanic, "CIPSes" );
	enum TDirectPrintSessionPanic
		{
		ENotSupported
		};
	
	void PanicClient( const TDpMessage& aMessage, TDirectPrintSessionPanic aCategory )
		{
		aMessage.Panic( KDirectPrintSessionPanic, aCategory );
		}
	}

CDirectPrintSession::~CDirectPrintSession()
	{ 
	LOG("CDirectPrintSession::~CDirectPrintSession begin");
	delete iNumOfCopies;
	delete iJobGuard;
	iCapabilityIDs.Close();
	iMsgWrappers.ResetAndDestroy();
	iMsgWrappers.Close();
	Server().ReleaseEngine( this );
	Server().RemoveSession();
	LOG("CDirectPrintSession::~CDirectPrintSession end");
	}

CDirectPrintServer& CDirectPrintSession::Server()
    {
	return *static_cast<CDirectPrintServer*>( const_cast<CServer2*>( CSession2::Server() ) );
    }

void CDirectPrintSession::CreateL()
    {
    LOG("CDirectPrintSession::CreateL begin");
	Server().AddSession();
	iJobGuard = CDirectPrintServerJobGuard::NewL( Server().Engine() );
	iNumOfCopies = CDirectPrintNumOfCopies::NewL();
	iConnected = ETrue;
	LOG("CDirectPrintSession::CreateL end");
    }

void CDirectPrintSession::Disconnect(const RMessage2 &aMessage)
	{
	LOG("CDirectPrintSession::Disconnect begin");
//	Server().Engine().HandleSessionDisconnect( iIdleGuard, iDiscoveryGuard, iJobGuard );
	Server().Engine().HandleSessionDisconnect( NULL, NULL, iJobGuard );
	iConnected = EFalse;	
	CSession2::Disconnect( aMessage );
	LOG("CDirectPrintSession::Disconnect end");
	}

void CDirectPrintSession::ServiceL( const RMessage2& aMessage )
    {
    LOG1("CDirectPrintSession::ServiceL aMessage.Function(): %d", aMessage.Function());
    
    // wrapper array clean begin
	TInt index = iMsgWrappers.Count();
	LOG1("CDirectPrintSession::ServiceL index: %d", index);
	TBool result( EFalse );
	while( --index >= 0 && !result )
		{
		if( ( iMsgWrappers[index] )->Disposable() )
			{
			TDpMessage* toDispose = iMsgWrappers[index];
			iMsgWrappers.Remove( index );
			delete toDispose;
			toDispose = NULL;
			result = ETrue;
			}
		}	
	// wrapper array clean end
    
    TDpMessage* msgWrapper = new (ELeave) TDpMessage( aMessage, iConnected );    
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
    else if ( messageIndex >= EGetPrinterCapEnumCount && messageIndex <= EGetProtocolName )
    	{
		ServiceThirdPartL ( messageIndex );
    	}
    else 
    	{		
    	PanicClient( *iMsgWrappers[0], ENotSupported );    
    	}	

	LOG("CDirectPrintSession::ServiceL end");
    }
void CDirectPrintSession::ServiceFirstPartL( TInt aIndex )
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
		LOG1("[CDirectPrintSession::ServiceFirstPartL]\t Method reached to an end with index: %d", aIndex);
			break;
		}
	}
            
void CDirectPrintSession::ServiceSecondPartL( TInt aIndex )
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
			LOG1("[CDirectPrintSession::ServiceSecondPartL]\t Method reached to an end with index: %d", aIndex);
			break;
		}
	}

void CDirectPrintSession::ServiceThirdPartL( TInt aIndex )
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
			
		case EGetProtocolNamesCount:
			GetProtocolNamesCountL( *iMsgWrappers[0] );
			break;
			
		case EGetProtocolName:
			GetProtocolNameL( *iMsgWrappers[0] );
			break;
			
		default:
			LOG1("[CDirectPrintSession::ServiceThirdPartL]\t Method reached to an end with index: %d", aIndex);
			break;
		}
	}


void CDirectPrintSession::CountConnectionsL( TDpMessage& aMessage )
	{	
	TInt count = Server().SessionCount();
	TPckgC<TInt> countDes( count );
	aMessage.WriteL( 0, countDes );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNone );
	}
		
void CDirectPrintSession::SetForegroundL( TDpMessage& aMessage )
	{
	TInt fg = aMessage.Int0();
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNone );
	}
	
void CDirectPrintSession::ReserveEngine( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}

void CDirectPrintSession::ReleaseEngine( TDpMessage& aMessage )
	{
	TInt err = Server().ReleaseEngine( this );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}

void CDirectPrintSession::SupportedProtocols( TDpMessage& aMessage )
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
	
void CDirectPrintSession::GetNumPrintPages( TDpMessage& aMessage )
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

void CDirectPrintSession::GetJobStatus( TDpMessage& aMessage )
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
	
void CDirectPrintSession::GetPrinterStatus( TDpMessage& aMessage )
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
	
void CDirectPrintSession::CancelDiscovery( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		Server().Engine().CancelDiscovery();
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CDirectPrintSession::SubmitPrintJobL( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		Server().Engine().SubmitPrintJobL();			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CDirectPrintSession::CancelPrintJob( TDpMessage& aMessage )
	{	
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		err = Server().Engine().CancelPrintJob();
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CDirectPrintSession::ContinuePrintJobL( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		err = Server().Engine().ContinuePrintJobL();			
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}			

void CDirectPrintSession::RemoveCachedPrinterL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetJobTemplateIconL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetNumPreviewPages( TDpMessage& aMessage )
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
	
void CDirectPrintSession::SetJobSettingL( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )
		{
		TInt capabilityID = aMessage.Int0();
		TInt value = aMessage.Int1();
		TInt affectedCapability;
		
		LOG2("CDirectPrintSession::SetJobSettingL || capabilityID = %d, value = %d", capabilityID, value );
		
		err = Server().Engine().SetJobSettingL( capabilityID, value, affectedCapability );
		TPckgC<TInt> affectedCapabilityDes( affectedCapability );
		aMessage.WriteL( 2, affectedCapabilityDes );
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CDirectPrintSession::GetJobSettingL( TDpMessage& aMessage )
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
	
void CDirectPrintSession::GetPrinterCapabilityL( TDpMessage& aMessage )
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
	
void CDirectPrintSession::GetPrinterCapIdL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapTypeL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapDefL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapLowL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapHighL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapEnumCountL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapEnumL( TDpMessage& aMessage )	
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
	

void CDirectPrintSession::GetPrinterCapabilityIDsCountL( TDpMessage& aMessage )
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

void CDirectPrintSession::GetPrinterCapabilityIDL( TDpMessage& aMessage )
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
	
void CDirectPrintSession::RegisterIdleObserverL( TDpMessage& aMessage )
	{
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNotSupported );
	}
	
void CDirectPrintSession::CancelRegisterIdleObserverL( TDpMessage& aMessage )
	{
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNotSupported );
	}
	
void CDirectPrintSession::StartDiscoveryL( TDpMessage& aMessage )
	{
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNotSupported );
	}

void CDirectPrintSession::ContinueDiscoveryL( TDpMessage& aMessage )
	{
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNotSupported );
	}
	
void CDirectPrintSession::PrepareJobL( TDpMessage& aMessage )
	{
	iJobGuard->PrepareL( aMessage );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNone );
	}

void CDirectPrintSession::CreateJobL( TDpMessage& aMessage )
	{
	TInt err = iJobGuard->CreateL( aMessage );
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}
	
void CDirectPrintSession::ContinueCreateJobL( TDpMessage& aMessage )
	{
	iJobGuard->ContinueCreateL( aMessage );
	}
	
void CDirectPrintSession::CancelStartDiscoveryL( TDpMessage& aMessage )
	{
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( KErrNotSupported );
	}

void CDirectPrintSession::CancelCreateJobL( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )	
		{
		iJobGuard->Stop();					
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );	
	}
	
void CDirectPrintSession::IsPictBridgeModeL( TDpMessage& aMessage )
	{
#ifdef _DIRECTPRINT_USE_USB
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
		if( dState == EUsbDeviceStateConfigured && personality == KUsbPersonalityIdMTP )
			err = ETrue;		

		aMessage.SetDisposable( ETrue );
		aMessage.Complete( err );
#else
		aMessage.SetDisposable( EFalse );
		aMessage.Complete( EFalse );
#endif // _DIRECTPRINT_USE_USB
	}	

void CDirectPrintSession::SetNumberOfCopiesCountL( TDpMessage& aMessage )
	{
	LOG("CDirectPrintSession::CheckNumberOfCopiesL begin");
	TInt count = aMessage.Int0();
	LOG1("CDirectPrintSession::CheckNumberOfCopiesL count: %d", count);
	TInt err = ( count > 0 ) ? KErrNone : KErrInvalidData;
	if( !err )
		{
		iNumOfCopies->Reset( count );
		}					
	LOG1("CDirectPrintSession::CheckNumberOfCopiesL return: %d", err);
	aMessage.SetDisposable( ETrue );	
	aMessage.Complete( err );
	}

void CDirectPrintSession::SetNumberOfCopiesL( TDpMessage& aMessage )
	{
	LOG("CDirectPrintSession::SetNumberOfCopiesL begin");
	TInt copies = aMessage.Int0();
	LOG1("CDirectPrintSession::SetNumberOfCopiesL copies: %d", copies);	
	TInt err = ( copies < 0 ) ? KErrInvalidData : KErrNone;
	if( !err )
		{
		TBool full;
		iNumOfCopies->AddNumOfCopyL( copies, full );
		LOG1("CDirectPrintSession::SetNumberOfCopiesL full: %d", full);
		if( full )
			{
			Server().Engine().SetNumsOfCopiesL( iNumOfCopies->NumsOfCopies(), err );	
			}						
		}	
	LOG1("CDirectPrintSession::SetNumberOfCopiesL return: %d", err);
	aMessage.SetDisposable( ETrue );	
	aMessage.Complete( err );
	}

void CDirectPrintSession::GetProtocolNamesCountL( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )
		{
		TPckgC<TInt> count( Server().Engine().ProtocolCount() );
		aMessage.WriteL( 0, count );
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}

void CDirectPrintSession::GetProtocolNameL( TDpMessage& aMessage )
	{
	TInt err = Server().ReserveEngine( this );
	if( !err )
		{
		err = Server().Engine().GetProtocolNameL( aMessage );
		}
	aMessage.SetDisposable( ETrue );
	aMessage.Complete( err );
	}

//  End of File
