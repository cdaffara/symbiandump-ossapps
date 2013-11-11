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


#include <e32svr.h>
#include <e32math.h>

#include "directprintclient.h"
#include "directprintclientserver.h"
#include "directprintcapability.h"
#include "clog.h"
#include "directprintjobguarddata.h"

namespace
	{	
	// Server startup code
	TInt StartServer()
		{
		// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
		// launching of two such processes should be detected when the second one
		// attempts to create the server object, failing with KErrAlreadyExists.
		RProcess server;
		TInt r = server.Create( KDirectPrintServerImg, KNullDesC );

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

TVersion RDirectPrintClient::Version() const
	{
	return TVersion( KDirectPrintServerMajor, KDirectPrintServerMinor, KDirectPrintServerBuild );
	}

EXPORT_C RDirectPrintClient::RDirectPrintClient()
	: RSessionBase()
	, iDicsoveryDataPtr(NULL, 0, 0)
	, iIdleDataPtr(NULL, 0, 0)
	, iJobDataPtr(NULL, 0, 0)
	, iNameDataPtr(NULL, 0, 0)
    {
    iCapability = NULL;
    }

EXPORT_C void RDirectPrintClient::ConnectL()
	{
	LOG1("RDirectPrintClient::Connect Handle(): %d", Handle());
	// check against double-connect
	if( Handle() != KNullHandle )
		{
		User::Leave(KErrAlreadyExists);
		}

	iCapability = new (ELeave) TDirectPrintCapability();

	// create process
	TInt err = StartServer();
	if (err == KErrNone)
		{
		// create session
		err = CreateSession( KDirectPrintServerName, Version() );
		}
	User::LeaveIfError(err);
	}

EXPORT_C void RDirectPrintClient::Close()
	{
	LOG("RDirectPrintClient::Close begin");
	if( iCapability )
		{		
		delete iCapability;
		iCapability = NULL;	
		}	
	RSessionBase::Close();
	LOG("RDirectPrintClient::Close end");
	}

EXPORT_C TInt RDirectPrintClient::CountConnections( TInt& aConnections ) const
	{
	TPckg<TInt> connsBuf( aConnections );	
	return SendReceive( ECountConnections, TIpcArgs( &connsBuf ) );
	}

EXPORT_C TInt RDirectPrintClient::SetForeground( TInt aFg ) const
	{
	return SendReceive( ESetForeground, TIpcArgs( aFg ) );
	}

EXPORT_C TInt RDirectPrintClient::ReserveEngine() const
	{
	return SendReceive( EReserveEngine );
	}

EXPORT_C TInt RDirectPrintClient::ReleaseEngine() const
	{
	return SendReceive( EReleaseEngine );
	}

EXPORT_C TInt RDirectPrintClient::SupportedProtocols() const
	{
	LOG("RDirectPrintClient::SupportedProtocols ESupportedProtocols");
	TInt prots = SendReceive( ESupportedProtocols );
	LOG1("RDirectPrintClient::SupportedProtocols end with: %d", prots);
	return prots;
	}

EXPORT_C TInt RDirectPrintClient::GetNumPrintPages() const
	{
	LOG("RDirectPrintClient::GetNumPrintPages EGetNumPrintPages");
	TInt pages = SendReceive( EGetNumPrintPages );
	LOG1("RDirectPrintClient::GetNumPrintPages end with: %d", pages);
	return pages;	
	}

EXPORT_C TInt RDirectPrintClient::GetJobStatus() const
	{
	LOG("RDirectPrintClient::GetJobStatus EGetJobStatus");
	TInt status = SendReceive( EGetJobStatus );
	LOG1("RDirectPrintClient::GetJobStatus end with: %d", status);
	return status;
	}

EXPORT_C TInt RDirectPrintClient::GetPrinterStatus( TInt aPrinterID ) const
	{
	LOG1("RDirectPrintClient::GetPrinterStatus EGetPrinterStatus aPrinterID: %d", aPrinterID);
	TInt status = SendReceive( EGetPrinterStatus, TIpcArgs( aPrinterID ) );
	LOG1("RDirectPrintClient::GetPrinterStatus end with: %d", status);	
	return status;
	}

EXPORT_C TInt RDirectPrintClient::CancelDiscovery() const
	{
	LOG("RDirectPrintClient::CancelDiscovery ECancelDiscovery");
	TInt err = SendReceive( ECancelDiscovery );
	LOG1("RDirectPrintClient::CancelDiscovery end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::SubmitPrintJob() const
	{
	LOG("RDirectPrintClient::SubmitPrintJob ESubmitPrintJob");
	TInt err = SendReceive( ESubmitPrintJob );
	LOG1("RDirectPrintClient::SubmitPrintJob end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::CancelPrintJob() const
	{
	LOG("RDirectPrintClient::CancelPrintJob ECancelPrintJob");
	TInt err = SendReceive( ECancelPrintJob );
	LOG1("RDirectPrintClient::CancelPrintJob end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::ContinuePrintJob() const
	{
	LOG("RDirectPrintClient::ContinuePrintJob EContinuePrintJob");
	TInt err = SendReceive( EContinuePrintJob );
	LOG1("RDirectPrintClient::ContinuePrintJob end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::RemoveCachedPrinter( TInt aPrinterID ) const
	{
	LOG1("RDirectPrintClient::RemoveCachedPrinter ERemoveCachedPrinter aPrinterID: %d", aPrinterID);
	TInt err = SendReceive( ERemoveCachedPrinter, TIpcArgs( aPrinterID ) );	
	LOG1("RDirectPrintClient::RemoveCachedPrinter end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::GetJobTemplateIcon( TInt aTemplateID, TInt& aFbsBitmapHandle ) const
	{
	TPckg<TInt> handleBuf( aFbsBitmapHandle );
	LOG1("RDirectPrintClient::GetJobTemplateIcon EGetJobTemplateIcon aTemplateID: %d", aTemplateID);
	TInt err = SendReceive( EGetJobTemplateIcon, TIpcArgs( aTemplateID, &handleBuf ) );
	LOG1("RDirectPrintClient::GetJobTemplateIcon aFbsBitmapHandle: %d", aFbsBitmapHandle);
	LOG1("RDirectPrintClient::GetJobTemplateIcon end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::GetNumPreviewPages() const
	{
	LOG("RDirectPrintClient::GetNumPreviewPages EGetNumPreviewPages");
	TInt pages = SendReceive( EGetNumPreviewPages );
	LOG1("RDirectPrintClient::GetNumPreviewPages end with: %d", pages);	
	return pages;
	}

EXPORT_C TInt RDirectPrintClient::SetJobSetting( TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability ) const
	{
	TPckg<TInt> capBuf( aAffectedCapability );
	LOG1("RDirectPrintClient::SetJobSetting ESetJobSetting aCapabilityID: %d", aCapabilityID);
	LOG1("RDirectPrintClient::SetJobSetting ESetJobSetting aValue: %d", aValue);
	TInt err = SendReceive( ESetJobSetting, TIpcArgs( aCapabilityID, aValue, &capBuf ) );
	LOG1("RDirectPrintClient::SetJobSetting aAffectedCapability: %d", aAffectedCapability);
	LOG1("RDirectPrintClient::SetJobSetting end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::GetJobSetting( TInt aCapabilityID, TInt& aValue ) const
	{
	TPckg<TInt> valueBuf( aValue );
	LOG1("RDirectPrintClient::GetJobSetting EGetJobSetting aCapabilityID: %d", aCapabilityID);
	TInt err = SendReceive( EGetJobSetting, TIpcArgs( aCapabilityID, &valueBuf ) );
	LOG1("RDirectPrintClient::GetJobSetting aValue: %d", aValue);
	LOG1("RDirectPrintClient::GetJobSetting end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::GetPrinterCapability(TInt aPrinterID, TInt aCapabilityID, TDirectPrintCapability& aCapability) const
	{
	LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapability aPrinterID: %d", aPrinterID);
	LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapability aCapabilityID: %d", aCapabilityID);
	TInt err = SendReceive( EGetPrinterCapability, TIpcArgs( aPrinterID, aCapabilityID ) );
	LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapability err: %d", err);

	if( !err )
		{
		TInt capId;
		TPckg<TInt> capIdBuf( capId );
		LOG("RDirectPrintClient::GetPrinterCapability EGetPrinterCapId");
		err = SendReceive( EGetPrinterCapId, TIpcArgs( &capIdBuf ) );
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapId err: %d", err);
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapId capId: %d", capId);
		if( !err )
			{
			iCapability->iCapabilityID = capId;
			}
		}

	if( !err )
		{
		TInt type;
		TPckg<TInt> typeBuf( type );
		LOG("RDirectPrintClient::GetPrinterCapability EGetPrinterCapType");
		err = SendReceive( EGetPrinterCapType, TIpcArgs( &typeBuf ) );
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapType err: %d", err);
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapType type: %d", type);
		if( !err )
			{
			iCapability->iType = static_cast<TDirectPrintCapability::ECapType>(type);
			}
		}

	if( !err )
		{
		TInt def;
		TPckg<TInt> defBuf( def );
		LOG("RDirectPrintClient::GetPrinterCapability EGetPrinterCapDef");
		err = SendReceive( EGetPrinterCapDef, TIpcArgs( &defBuf ) );
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapDef err: %d", err);
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapDef def: %d", def);
		if( !err )
			{
			iCapability->iDefaultValue = def;
			}
		}

	if( !err )
		{
		TInt low;
		TPckg<TInt> lowBuf( low );
		LOG("RDirectPrintClient::GetPrinterCapability EGetPrinterCapLow");
		err = SendReceive( EGetPrinterCapLow, TIpcArgs( &lowBuf ) );
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapLow err: %d", err);
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapLow low: %d", low);
		if( !err )
			{
			iCapability->iLow = low;
			}
		}

	if( !err )
		{
		TInt high;
		TPckg<TInt> highBuf( high );
		LOG("RDirectPrintClient::GetPrinterCapability EGetPrinterCapHigh");
		err = SendReceive( EGetPrinterCapHigh, TIpcArgs( &highBuf ) );
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapHigh err: %d", err);
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapHigh high: %d", high);
		if( !err )
			{
			iCapability->iHigh = high;
			}
		}

	if( !err )
		{
		TInt count;
		TPckg<TInt> countBuf( count );
		LOG("RDirectPrintClient::GetPrinterCapability EGetPrinterCapEnumCount");
		err = SendReceive( EGetPrinterCapEnumCount, TIpcArgs( &countBuf ) );
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapEnumCount err: %d", err);
		LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapEnumCount count: %d", count);
		
		if( !err )
			{
			iCapability->iEnumCount = count;
			for( TInt i = 0; i < count && ! err; i++ )
				{
				TInt value;
				TPckg<TInt> valueBuf( value );
				LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapEnum i: %d",i);
				TInt err = SendReceive( EGetPrinterCapEnum, TIpcArgs( i, &valueBuf ) );
				LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapEnum err: %d", err);
				LOG1("RDirectPrintClient::GetPrinterCapability EGetPrinterCapEnum value: %d", value);
				
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

	LOG1("RDirectPrintClient::GetPrinterCapability end with: %d", err);
	return err;
	}


EXPORT_C TInt RDirectPrintClient::GetPrinterCapabilityIDs( TInt aPrinterID, RArray<TInt>& aCapabilityIDs ) const
	{
	TInt count;
	TPckg<TInt> countBuf( count );
	
	LOG1("RDirectPrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityIDsCount aPrinterID: %d", aPrinterID);
	TInt err = SendReceive( EGetPrinterCapabilityIDsCount, TIpcArgs( aPrinterID, &countBuf ) );
	LOG1("RDirectPrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityIDsCount err: %d", err);
	if( !err )
		{
		LOG1("RDirectPrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityIDsCount count: %d", count);
		for( TInt i = 0; i < count && !err; i++ )
			{
			TInt capability;
			TPckg<TInt> capBuf( capability );
			LOG1("RDirectPrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityID i: %d", i);
			err = SendReceive( EGetPrinterCapabilityID, TIpcArgs( i, &capBuf ) );
			LOG1("RDirectPrintClient::GetPrinterCapabilityIDs EGetPrinterCapabilityID err: %d", err);
			if( !err )
				{
				LOG1("RDirectPrintClient::GetPrinterCapabilityIDs capability id: %d", capability);
				err = aCapabilityIDs.Append( capability );
				LOG1("RDirectPrintClient::GetPrinterCapabilityIDs append err: %d", err);
				}
			}
		}
	
	LOG1("RDirectPrintClient::GetPrinterCapabilityIDs end with: %d", err);
	return err;
	}
/*
EXPORT_C TInt RDirectPrintClient::RegisterIdleObserver( TIdleGuardData& aData, TRequestStatus& aStatus )
	{ 
	LOG("RDirectPrintClient::RegisterIdleObserver EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RDirectPrintClient::RegisterIdleObserver EReserveEngine err: %d", err);
	if( !err )
		{
		LOG("RDirectPrintClient::RegisterIdleObserver ERegisterIdleObserver");
		iIdleDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
		SendReceive( ERegisterIdleObserver, TIpcArgs( &iIdleDataPtr ), aStatus );
		}
	LOG1("RDirectPrintClient::RegisterIdleObserver end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::CancelRegisterIdleObserver() const 
	{
	LOG("RDirectPrintClient::CancelRegisterIdleObserver ECancelRegisterIdleObserver");
	TInt err = SendReceive( EReleaseEngine );
	LOG1("RDirectPrintClient::RegisterIdleObserver EReleaseEngine err: %d", err);

	err = SendReceive( ECancelRegisterIdleObserver );
	LOG1("RDirectPrintClient::RegisterIdleObserver ECancelRegisterIdleObserver err: %d", err);
	
	LOG1("RDirectPrintClient::CancelRegisterIdleObserver end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::StartDiscovery( TDiscoveryGuardData& aData, TUint aProtocols, TRequestStatus& aStatus )
	{ 
	LOG("RDirectPrintClient::StartDiscovery EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RDirectPrintClient::StartDiscovery EReserveEngine err: %d", err);
	if( !err )
		{ 
		LOG1("RDirectPrintClient::StartDiscovery EStartDiscovery aProtocols: %d", aProtocols);
		err = SendReceive( EStartDiscovery, TIpcArgs( aProtocols ) );
		LOG1("RDirectPrintClient::StartDiscovery EStartDiscovery err: %d", err);
		}
	if( !err )
		{
		LOG("RDirectPrintClient::StartDiscovery EContinueDiscovery");
		iDicsoveryDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
		SendReceive( EContinueDiscovery, TIpcArgs( &iDicsoveryDataPtr ), aStatus );
		}
	LOG1("RDirectPrintClient::StartDiscovery end with: %d", err);
	return err;
	}

EXPORT_C TInt RDirectPrintClient::ContinueDiscovery( TDiscoveryGuardData& aData, TRequestStatus& aStatus )
	{
	LOG("RDirectPrintClient::ContinueDiscovery EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RDirectPrintClient::ContinueDiscovery EReserveEngine err: %d", err);
    if( !err )
    	{
    	LOG("RDirectPrintClient::ContinueDiscovery EContinueDiscovery");
	    iDicsoveryDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueDiscovery, TIpcArgs( &iDicsoveryDataPtr ), aStatus );
    	}
    LOG1("RDirectPrintClient::ContinueDiscovery end with: %d", err);
    return err;
	}
*/
EXPORT_C TInt RDirectPrintClient::CreateJob( TInt aPrinterID, TDirectPrintJobGuardData& aData, RPointerArray<TDesC>& aImages, TRequestStatus& aStatus )
	{
	LOG("RDirectPrintClient::CreateJob EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RDirectPrintClient::CreateJob EReserveEngine err: %d", err);
	if( !err )
		{
		TInt count = aImages.Count();
		LOG1("RDirectPrintClient::CreateJob aImages.Count(): %d", aImages.Count());
		for( TInt i = 0; i < count && !err; i++ )
			{
			LOG("RDirectPrintClient::CreateJob EPrepareJob");
			err = SendReceive( EPrepareJob, TIpcArgs( aImages[i] ) );
			LOG1("RDirectPrintClient::CreateJob EPrepareJob err: %d", err);	
			}
		}
    if( !err )
    	{
    	LOG("RDirectPrintClient::CreateJob ECreateJob");
    	err = SendReceive( ECreateJob, TIpcArgs( aPrinterID ) );
    	LOG1("RDirectPrintClient::CreateJob ECreateJob err: %d", err);
    	}
    if( !err )
    	{ 
    	LOG("RDirectPrintClient::CreateJob EContinueCreateJob");
    	iJobDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueCreateJob, TIpcArgs( &iJobDataPtr ), aStatus );
    	}
    LOG1("RDirectPrintClient::CreateJob end with: %d", err);
    return err;
	}		
        
EXPORT_C TInt RDirectPrintClient::ContinueCreateJob( TDirectPrintJobGuardData& aData, TRequestStatus& aStatus )
	{
	LOG("RDirectPrintClient::ContinueCreateJob EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RDirectPrintClient::ContinueCreateJob EReserveEngine err: %d", err);
    if( !err )
    	{  
    	LOG("RDirectPrintClient::ContinueCreateJob EContinueCreateJob");  	    	
    	iJobDataPtr.Set(reinterpret_cast<TUint8*>(&aData), sizeof(aData), sizeof(aData));
	    SendReceive( EContinueCreateJob, TIpcArgs( &iJobDataPtr ), aStatus );
    	}
    LOG1("RDirectPrintClient::ContinueCreateJob end with: %d", err);
    return err;
	}
	
EXPORT_C TInt RDirectPrintClient::CancelStartDiscovery() const
	{
	LOG("RDirectPrintClient::CancelStartDiscovery ECancelStartDiscovery");
	TInt err = SendReceive( ECancelStartDiscovery );
	LOG1("RDirectPrintClient::CancelStartDiscovery end with: %d", err);
	return err;	
	}	
		
EXPORT_C TInt RDirectPrintClient::CancelCreateJob() const
	{
	LOG("RDirectPrintClient::CancelCreateJob ECancelCreateJob");
	TInt err = SendReceive( ECancelCreateJob );
	LOG1("RDirectPrintClient::CancelCreateJob end with: %d", err);
	return err;	
	}
	
EXPORT_C TInt RDirectPrintClient::IsPictBridgeMode() const
	{
	LOG("RDirectPrintClient::IsPictBridgeMode EIsPictBridgeMode");
#ifdef __WINS__
	TInt err( KErrNotFound );
#else
	TInt err = SendReceive( EIsPictBridgeMode );
#endif	
	LOG1("RDirectPrintClient::IsPictBridgeMode end with: %d", err);
	return err;	
	}
	
EXPORT_C TInt RDirectPrintClient::SetNumberOfCopies( const RArray<TInt>& aArray ) const
	{
	LOG("RDirectPrintClient::SetNumberOfCopies EReserveEngine");
	TInt err = SendReceive( EReserveEngine );
	LOG1("RDirectPrintClient::SetNumberOfCopies EReserveEngine err: %d", err);
    if( !err )
    	{
    	TInt count = aArray.Count();
    	LOG1("RDirectPrintClient::SetNumberOfCopies count: %d", count);
    	LOG("RDirectPrintClient::SetNumberOfCopies ESetNumberOfCopiesCount");
		err = SendReceive( ESetNumberOfCopiesCount, TIpcArgs( count ) );
		LOG1("RDirectPrintClient::SetNumberOfCopies ESetNumberOfCopiesCount err: %d", err);
    	for( TInt i = 0; i < count && !err; i++ )
			{
			LOG1("RDirectPrintClient::SetNumberOfCopies i: %d", i);
			LOG("RDirectPrintClient::SetNumberOfCopies ESetNumberOfCopies");
			err = SendReceive( ESetNumberOfCopies, TIpcArgs( aArray[i] ) );
			LOG1("RDirectPrintClient::SetNumberOfCopies ESetNumberOfCopies err: %d", err);
			}
    	}
	LOG1("RDirectPrintClient::SetNumberOfCopies end with: %d", err);
	return err;
	}
	
EXPORT_C TInt RDirectPrintClient::GetProtocolNames(RSelectItemArray& aNames) const
	{
	//TInt err = SendReceive( EReserveEngine );
	TInt count;
	TPckg<TInt> countBuf( count );

	TInt err = SendReceive( EGetProtocolNamesCount, TIpcArgs( &countBuf ) );

	if( !err )
		{
		aNames.Reset();

		for( TInt i = 0; i < count && !err; i++ )
			{
			TDirectPrintSelectItem item;
			//iNameDataPtr.Set(reinterpret_cast<TUint8*>(&item), sizeof(item), sizeof(item));
			TPtr8 ptr(reinterpret_cast<TUint8*>(&item), sizeof(item), sizeof(item));
			//err = SendReceive( EGetProtocolName, TIpcArgs( i, &iNameDataPtr ) );
			err = SendReceive( EGetProtocolName, TIpcArgs( i, &ptr ) );
			if( !err )
				{
				aNames.Append(item);
				}
			}
		}

	return err;
	}

//  End of File
