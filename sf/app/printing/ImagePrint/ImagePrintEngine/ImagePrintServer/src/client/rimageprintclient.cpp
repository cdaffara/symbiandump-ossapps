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


#include <e32svr.h>
#include <e32math.h>

#include "rimageprintclient.h"
#include "imageprintclientserver.h"
#include "tprintcapability.h"
#include "clog.h"
#include "tidleguarddata.h"
#include "tdiscoveryguarddata.h"
#include "tjobguarddata.h"
 
namespace
	{	
	// Server startup code
	TInt StartServer()
		{
		// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
		// launching of two such processes should be detected when the second one
		// attempts to create the server object, failing with KErrAlreadyExists.
		RProcess server;
		TInt r = server.Create( KImagePrintServerImg, KNullDesC );

		if( r != KErrNone )
			return r;
		TRequestStatus stat;
		server.Rendezvous(stat);
		if (stat!=KRequestPending)
			server.Kill(0);		// abort startup
		else
			server.Resume();	// logon OK - start the server
		User::WaitForRequest(stat);		// wait for start or death
		// we can't use the 'exit reason' if the server panicked as this
		// is the panic 'reason' and may be '0' which cannot be distinguished
		// from KErrNone
		r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();

		server.Close();
		return r;
		}	
	}

TVersion RImagePrintClient::Version() const
    {
	return TVersion( KImagePrintServerMajor, KImagePrintServerMinor, KImagePrintServerBuild );
    }

EXPORT_C RImagePrintClient::RImagePrintClient() :  RSessionBase(),
													iDicsoveryDataPtr(NULL, 0, 0),
													iIdleDataPtr(NULL, 0, 0),
													iJobDataPtr(NULL, 0, 0)
    {
    iCapability = NULL;
    }

EXPORT_C TInt RImagePrintClient::ConnectL()
	{
	LOG1("RImagePrintClient::Connect Handle(): %d", Handle());
	// check against double-connect
	if( Handle() != KNullHandle ) return KErrAlreadyExists;
	
	iCapability = new (ELeave) TPrintCapability();
	if( ! iCapability ) return KErrNoMemory;		
	
	TInt retry = 2;
	for (;;)
		{
		LOG1("RImagePrintClient::Connect retry: %d", retry);
		TInt r = CreateSession( KImagePrintServerName, Version() );
		LOG1("RImagePrintClient::Connect r after CreateSession: %d", r);

		if( r != KErrNotFound && r!= KErrServerTerminated )
			return r;
		if( --retry == 0 )
			return r;
		r = StartServer();
		LOG1("RImagePrintClient::Connect r after StartServer: %d", r);
		if( r != KErrNone && r != KErrAlreadyExists )
			return r;
		}
	}
	
EXPORT_C void RImagePrintClient::Close()
	{
	LOG("RImagePrintClient::Close begin");
	if( iCapability )
		{		
		delete iCapability;
		iCapability = NULL;	
		}	
	RSessionBase::Close();
	LOG("RImagePrintClient::Close end");
	}

EXPORT_C TInt RImagePrintClient::CountConnections( TInt& aConnections ) const
	{
	TPckg<TInt> connsBuf( aConnections );	
	return SendReceive( ECountConnections, TIpcArgs( &connsBuf ) );
	}

EXPORT_C TInt RImagePrintClient::SetForeground( TInt aFg ) const
	{
	return SendReceive( ESetForeground, TIpcArgs( aFg ) );
	}	

EXPORT_C TInt RImagePrintClient::ReserveEngine() const
	{
	return SendReceive( EReserveEngine );
	}
	
EXPORT_C TInt RImagePrintClient::ReleaseEngine() const
	{
	return SendReceive( EReleaseEngine );
	}		
		
EXPORT_C TInt RImagePrintClient::SupportedProtocols() const
	{
	LOG("RImagePrintClient::SupportedProtocols ESupportedProtocols");
	TInt prots = SendReceive( ESupportedProtocols );
	LOG1("RImagePrintClient::SupportedProtocols end with: %d", prots);
	return prots;
	}
	
EXPORT_C TInt RImagePrintClient::GetNumPrintPages() const
	{
	LOG("RImagePrintClient::GetNumPrintPages EGetNumPrintPages");
	TInt pages = SendReceive( EGetNumPrintPages );
	LOG1("RImagePrintClient::GetNumPrintPages end with: %d", pages);	
	return pages;	
	}	
	
EXPORT_C TInt RImagePrintClient::GetJobStatus() const
	{
	LOG("RImagePrintClient::GetJobStatus EGetJobStatus");
	TInt status = SendReceive( EGetJobStatus );
	LOG1("RImagePrintClient::GetJobStatus end with: %d", status);
	return status;	
	}
	
EXPORT_C TInt RImagePrintClient::GetPrinterStatus( TInt aPrinterID ) const
	{
	LOG1("RImagePrintClient::GetPrinterStatus EGetPrinterStatus aPrinterID: %d", aPrinterID);
	TInt status = SendReceive( EGetPrinterStatus, TIpcArgs( aPrinterID ) );
	LOG1("RImagePrintClient::GetPrinterStatus end with: %d", status);	
	return status;
	}
	
EXPORT_C TInt RImagePrintClient::CancelDiscovery() const
	{
	LOG("RImagePrintClient::CancelDiscovery ECancelDiscovery");
	TInt err = SendReceive( ECancelDiscovery );
	LOG1("RImagePrintClient::CancelDiscovery end with: %d", err);
	return err;
	}
	
EXPORT_C TInt RImagePrintClient::SubmitPrintJob() const
	{
	LOG("RImagePrintClient::SubmitPrintJob ESubmitPrintJob");
	TInt err = SendReceive( ESubmitPrintJob );
	LOG1("RImagePrintClient::SubmitPrintJob end with: %d", err);
	return err;
	}
	
EXPORT_C TInt RImagePrintClient::CancelPrintJob() const
	{
	LOG("RImagePrintClient::CancelPrintJob ECancelPrintJob");
	TInt err = SendReceive( ECancelPrintJob );
	LOG1("RImagePrintClient::CancelPrintJob end with: %d", err);
	return err;
	}
	
EXPORT_C TInt RImagePrintClient::ContinuePrintJob() const
	{
	LOG("RImagePrintClient::ContinuePrintJob EContinuePrintJob");
	TInt err = SendReceive( EContinuePrintJob );
	LOG1("RImagePrintClient::ContinuePrintJob end with: %d", err);
	return err;
	}			
			
EXPORT_C TInt RImagePrintClient::RemoveCachedPrinter( TInt aPrinterID ) const
	{
	LOG1("RImagePrintClient::RemoveCachedPrinter ERemoveCachedPrinter aPrinterID: %d", aPrinterID);
	TInt err = SendReceive( ERemoveCachedPrinter, TIpcArgs( aPrinterID ) );	
	LOG1("RImagePrintClient::RemoveCachedPrinter end with: %d", err);
	return err;
	}
	
EXPORT_C TInt RImagePrintClient::GetJobTemplateIcon( TInt aTemplateID, TInt& aFbsBitmapHandle ) const
	{
	TPckg<TInt> handleBuf( aFbsBitmapHandle );
	LOG1("RImagePrintClient::GetJobTemplateIcon EGetJobTemplateIcon aTemplateID: %d", aTemplateID);	
	TInt err = SendReceive( EGetJobTemplateIcon, TIpcArgs( aTemplateID, &handleBuf ) );
	LOG1("RImagePrintClient::GetJobTemplateIcon aFbsBitmapHandle: %d", aFbsBitmapHandle);
	LOG1("RImagePrintClient::GetJobTemplateIcon end with: %d", err);
	return err;
	}
	
EXPORT_C TInt RImagePrintClient::GetNumPreviewPages() const
	{
	LOG("RImagePrintClient::GetNumPreviewPages EGetNumPreviewPages");
	TInt pages = SendReceive( EGetNumPreviewPages );
	LOG1("RImagePrintClient::GetNumPreviewPages end with: %d", pages);	
	return pages;	
	}		

EXPORT_C TInt RImagePrintClient::SetJobSetting( TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability ) const
	{
	TPckg<TInt> capBuf( aAffectedCapability );
	LOG1("RImagePrintClient::SetJobSetting ESetJobSetting aCapabilityID: %d", aCapabilityID);
	LOG1("RImagePrintClient::SetJobSetting ESetJobSetting aValue: %d", aValue);
	TInt err = SendReceive( ESetJobSetting, TIpcArgs( aCapabilityID, aValue, &capBuf ) );
	LOG1("RImagePrintClient::SetJobSetting aAffectedCapability: %d", aAffectedCapability);
	LOG1("RImagePrintClient::SetJobSetting end with: %d", err);
	return err;
	}		
		
EXPORT_C TInt RImagePrintClient::GetJobSetting( TInt aCapabilityID, TInt& aValue ) const
	{
	TPckg<TInt> valueBuf( aValue );
	LOG1("RImagePrintClient::GetJobSetting EGetJobSetting aCapabilityID: %d", aCapabilityID);
	TInt err = SendReceive( EGetJobSetting, TIpcArgs( aCapabilityID, &valueBuf ) );
	LOG1("RImagePrintClient::GetJobSetting aValue: %d", aValue);
	LOG1("RImagePrintClient::GetJobSetting end with: %d", err);
	return err;
	}

EXPORT_C TInt RImagePrintClient::GetPrinterCapability(TInt aPrinterID, TInt aCapabilityID, TPrintCapability& aCapability) const
	{	
	LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapability aPrinterID: %d", aPrinterID);
	LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapability aCapabilityID: %d", aCapabilityID);
	TInt err = SendReceive( EGetPrinterCapability, TIpcArgs( aPrinterID, aCapabilityID ) );
	LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapability err: %d", err);
		
	if( !err )
		{
		TInt capId;
		TPckg<TInt> capIdBuf( capId );
		LOG("RImagePrintClient::GetPrinterCapability EGetPrinterCapId");
		err = SendReceive( EGetPrinterCapId, TIpcArgs( &capIdBuf ) );
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapId err: %d", err);
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapId capId: %d", capId);
		if( !err )
			{
			iCapability->iCapabilityID = capId;	
			}				
		}
		
	if( !err )
		{
		TInt type;
		TPckg<TInt> typeBuf( type );
		LOG("RImagePrintClient::GetPrinterCapability EGetPrinterCapType");
		err = SendReceive( EGetPrinterCapType, TIpcArgs( &typeBuf ) );
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapType err: %d", err);
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapType type: %d", type);
		if( !err )
			{
			iCapability->iType = static_cast<TPrintCapability::ECapType>(type);	
			}
		}							
		
	if( !err )
		{
		TInt def;
		TPckg<TInt> defBuf( def );
		LOG("RImagePrintClient::GetPrinterCapability EGetPrinterCapDef");
		err = SendReceive( EGetPrinterCapDef, TIpcArgs( &defBuf ) );
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapDef err: %d", err);
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapDef def: %d", def);
		if( !err )
			{
			iCapability->iDefaultValue = def;	
			}
		}
	
	if( !err )
		{
		TInt low;
		TPckg<TInt> lowBuf( low );
		LOG("RImagePrintClient::GetPrinterCapability EGetPrinterCapLow");
		err = SendReceive( EGetPrinterCapLow, TIpcArgs( &lowBuf ) );
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapLow err: %d", err);
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapLow low: %d", low);
		if( !err )
			{
			iCapability->iLow = low;	
			}
		}
	
	if( !err )
		{
		TInt high;
		TPckg<TInt> highBuf( high );
		LOG("RImagePrintClient::GetPrinterCapability EGetPrinterCapHigh");
		err = SendReceive( EGetPrinterCapHigh, TIpcArgs( &highBuf ) );
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapHigh err: %d", err);
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapHigh high: %d", high);
		if( !err )
			{
			iCapability->iHigh = high;	
			}
		}
		
	if( !err )
		{
		TInt count;
		TPckg<TInt> countBuf( count );
		LOG("RImagePrintClient::GetPrinterCapability EGetPrinterCapEnumCount");
		err = SendReceive( EGetPrinterCapEnumCount, TIpcArgs( &countBuf ) );
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapEnumCount err: %d", err);
		LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapEnumCount count: %d", count);
		
		if( !err )
			{
			iCapability->iEnumCount = count;		
			for( TInt i = 0; i < count && ! err; i++ )	
				{
				TInt value;
				TPckg<TInt> valueBuf( value );
				LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapEnum i: %d",i);
				TInt err = SendReceive( EGetPrinterCapEnum, TIpcArgs( i, &valueBuf ) );
				LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapEnum err: %d", err);
				LOG1("RImagePrintClient::GetPrinterCapability EGetPrinterCapEnum value: %d", value);
				
				if( !err )
					{
					iCapability->iEnumCodes[i] = value;
					}
				}
			}
		}
		
	if( !err )		
		{
		aCapability	= *iCapability;
		}
	
	LOG1("RImagePrintClient::GetPrinterCapability end with: %d", err);					
	return err;
	}
			
	
EXPORT_C TInt RImagePrintClient::GetPrinterCapabilityIDs( TInt aPrinterID, RArray<TInt>& aCapabilityIDs ) const
	{
	TInt count;
	TPckg<TInt> countBuf( count );
	
	LOG1("RImagePrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityIDsCount aPrinterID: %d", aPrinterID);
	TInt err = SendReceive( EGetPrinterCapabilityIDsCount, TIpcArgs( aPrinterID, &countBuf ) );
	LOG1("RImagePrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityIDsCount err: %d", err);
	if( !err )
		{
		LOG1("RImagePrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityIDsCount count: %d", count);
		for( TInt i = 0; i < count && !err; i++ )
			{
			TInt capability;
			TPckg<TInt> capBuf( capability );
			LOG1("RImagePrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityID i: %d", i);
			err = SendReceive( EGetPrinterCapabilityID, TIpcArgs( i, &capBuf ) );
			LOG1("RImagePrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityID err: %d", err);
			if( !err )
				{
				LOG1("RImagePrintClient::GetPrinterCapabilityIDs capability id: %d", capability);
				err = aCapabilityIDs.Append( capability );
				LOG1("RImagePrintClient::GetPrinterCapabilityIDs append err: %d", err);
				}
			}
		}
	
	LOG1("RImagePrintClient::GetPrinterCapabilityIDs end with: %d", err);
	return err;		
	}
		
EXPORT_C TInt RImagePrintClient::RegisterIdleObserver( TIdleGuardData& aData, TRequestStatus& aStatus )
    { 
    LOG("RImagePrintClient::RegisterIdleObserver EReserveEngine");
    TInt err = SendReceive( EReserveEngine );
    LOG1("RImagePrintClient::RegisterIdleObserver EReserveEngine err: %d", err);
    if( !err )
    	{ 
    	LOG("RImagePrintClient::RegisterIdleObserver ERegisterIdleObserver");   	    	
    	iIdleDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( ERegisterIdleObserver, TIpcArgs( &iIdleDataPtr ), aStatus );
    	}
    LOG1("RImagePrintClient::RegisterIdleObserver end with: %d", err);
    return err;
    }
    
EXPORT_C TInt RImagePrintClient::CancelRegisterIdleObserver() const 
	{
	LOG("RImagePrintClient::CancelRegisterIdleObserver ECancelRegisterIdleObserver");
	TInt err = SendReceive( EReleaseEngine );	
	LOG1("RImagePrintClient::RegisterIdleObserver EReleaseEngine err: %d", err);
	
	err = SendReceive( ECancelRegisterIdleObserver );	
	LOG1("RImagePrintClient::RegisterIdleObserver ECancelRegisterIdleObserver err: %d", err);
	
	LOG1("RImagePrintClient::CancelRegisterIdleObserver end with: %d", err);
    return err;	
	}
    
EXPORT_C TInt RImagePrintClient::StartDiscovery( TDiscoveryGuardData& aData, TUint aProtocols, TRequestStatus& aStatus )
    { 
    LOG("RImagePrintClient::StartDiscovery EReserveEngine");
    TInt err = SendReceive( EReserveEngine );
    LOG1("RImagePrintClient::StartDiscovery EReserveEngine err: %d", err);
    if( !err )
    	{ 
    	LOG1("RImagePrintClient::StartDiscovery EStartDiscovery aProtocols: %d", aProtocols);   	
    	err = SendReceive( EStartDiscovery, TIpcArgs( aProtocols ) );
    	LOG1("RImagePrintClient::StartDiscovery EStartDiscovery err: %d", err);
    	}
    if( !err )
	    {
	    LOG("RImagePrintClient::StartDiscovery EContinueDiscovery");	    	    
	    iDicsoveryDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueDiscovery, TIpcArgs( &iDicsoveryDataPtr ), aStatus );
	    }
	LOG1("RImagePrintClient::StartDiscovery end with: %d", err);
    return err;
    }
        
EXPORT_C TInt RImagePrintClient::ContinueDiscovery( TDiscoveryGuardData& aData, TRequestStatus& aStatus )
	{
	LOG("RImagePrintClient::ContinueDiscovery EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RImagePrintClient::ContinueDiscovery EReserveEngine err: %d", err);
    if( !err )
    	{
    	LOG("RImagePrintClient::ContinueDiscovery EContinueDiscovery");     	    
	    iDicsoveryDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueDiscovery, TIpcArgs( &iDicsoveryDataPtr ), aStatus );
    	}
    LOG1("RImagePrintClient::ContinueDiscovery end with: %d", err);
    return err;
	}
	
EXPORT_C TInt RImagePrintClient::CreateJob( TInt aPrinterID, TJobGuardData& aData, RPointerArray<TDesC>& aImages, TRequestStatus& aStatus )
	{
	LOG("RImagePrintClient::CreateJob EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RImagePrintClient::CreateJob EReserveEngine err: %d", err);
	if( !err )
		{
		TInt count = aImages.Count();
		LOG1("RImagePrintClient::CreateJob aImages.Count(): %d", aImages.Count());
		for( TInt i = 0; i < count && !err; i++ )
			{
			LOG("RImagePrintClient::CreateJob EPrepareJob");
			err = SendReceive( EPrepareJob, TIpcArgs( aImages[i] ) );
			LOG1("RImagePrintClient::CreateJob EPrepareJob err: %d", err);	
			}
		}		
    if( !err )
    	{
    	LOG("RImagePrintClient::CreateJob ECreateJob");
    	err = SendReceive( ECreateJob, TIpcArgs( aPrinterID ) );
    	LOG1("RImagePrintClient::CreateJob ECreateJob err: %d", err);
    	}
    if( !err )
    	{ 
    	LOG("RImagePrintClient::CreateJob EContinueCreateJob");   	    	
    	iJobDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueCreateJob, TIpcArgs( &iJobDataPtr ), aStatus );
    	}
    LOG1("RImagePrintClient::CreateJob end with: %d", err);
    return err;
	}		
        
EXPORT_C TInt RImagePrintClient::ContinueCreateJob( TJobGuardData& aData, TRequestStatus& aStatus )
	{
	LOG("RImagePrintClient::ContinueCreateJob EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RImagePrintClient::ContinueCreateJob EReserveEngine err: %d", err);
    if( !err )
    	{  
    	LOG("RImagePrintClient::ContinueCreateJob EContinueCreateJob");  	    	
    	iJobDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueCreateJob, TIpcArgs( &iJobDataPtr ), aStatus );
    	}
    LOG1("RImagePrintClient::ContinueCreateJob end with: %d", err);
    return err;
	}
	
EXPORT_C TInt RImagePrintClient::CancelStartDiscovery() const
	{
	LOG("RImagePrintClient::CancelStartDiscovery ECancelStartDiscovery");
	TInt err = SendReceive( ECancelStartDiscovery );
	LOG1("RImagePrintClient::CancelStartDiscovery end with: %d", err);
	return err;	
	}	
		
EXPORT_C TInt RImagePrintClient::CancelCreateJob() const
	{
	LOG("RImagePrintClient::CancelCreateJob ECancelCreateJob");
	TInt err = SendReceive( ECancelCreateJob );
	LOG1("RImagePrintClient::CancelCreateJob end with: %d", err);
	return err;	
	}
	
EXPORT_C TInt RImagePrintClient::IsPictBridgeMode() const
	{
	LOG("RImagePrintClient::IsPictBridgeMode EIsPictBridgeMode");
#ifdef __WINS__
	TInt err( KErrNotFound );
#else
	TInt err = SendReceive( EIsPictBridgeMode );
#endif	
	LOG1("RImagePrintClient::IsPictBridgeMode end with: %d", err);
	return err;	
	}
	
EXPORT_C TInt RImagePrintClient::SetNumberOfCopies( const RArray<TInt>& aArray ) const
	{
	LOG("RImagePrintClient::SetNumberOfCopies EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RImagePrintClient::SetNumberOfCopies EReserveEngine err: %d", err);
    if( !err )
    	{
    	TInt count = aArray.Count();
    	LOG1("RImagePrintClient::SetNumberOfCopies count: %d", count);      	
    	LOG("RImagePrintClient::SetNumberOfCopies ESetNumberOfCopiesCount");
		err = SendReceive( ESetNumberOfCopiesCount, TIpcArgs( count ) );
		LOG1("RImagePrintClient::SetNumberOfCopies ESetNumberOfCopiesCount err: %d", err);    	  	   	
    	for( TInt i = 0; i < count && !err; i++ )
			{
			LOG1("RImagePrintClient::SetNumberOfCopies i: %d", i);
			LOG("RImagePrintClient::SetNumberOfCopies ESetNumberOfCopies");
			err = SendReceive( ESetNumberOfCopies, TIpcArgs( aArray[i] ) );
			LOG1("RImagePrintClient::SetNumberOfCopies ESetNumberOfCopies err: %d", err);	
			}								
    	}
	LOG1("RImagePrintClient::SetNumberOfCopies end with: %d", err);
	return err;
	}

//  End of File
