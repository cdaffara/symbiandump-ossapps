/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CPrinterControlPoint class
*
*/


#include <f32file.h>
#include <in_sock.h>
#include <upnpservice.h>
#include <upnpdevice.h>
#include <upnpaction.h>
#include <upnphttpmessagefactory.h>

#include "cuplogger.h"
#include "cprintercontrolpoint.h"
#include "mpcpobserver.h"

// CONSTANTS
#define KMaxCancelReqs 100

// -----------------------------------------------------------------------------
// CPrinterControlPoint::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CPrinterControlPoint* CPrinterControlPoint::NewL(MPCPObserver& aPCPObserver)
{
	LOG("[CPrinterControlPoint] \tCPrinterControlPoint::NewL");
	CPrinterControlPoint* self = new (ELeave) CPrinterControlPoint( aPCPObserver);
	CleanupStack::PushL( self );
	self -> ConstructL( );
	CleanupStack::Pop( self );
	return self;
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::CPrinterControlPoint
// C++ default constructor
// -----------------------------------------------------------------------------
//
CPrinterControlPoint::CPrinterControlPoint( MPCPObserver& aPCPObserver)
	: CUpnpControlPoint(), iPCPObserver( aPCPObserver )
{
}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CPrinterControlPoint::ConstructL()
{
	LOG("[CPrinterControlPoint] \tCPrinterControlPoint::ConstructL");
	_LIT8( KPrinter, "Printer:1" );
  	
	// Base class ConstructL
	CUpnpControlPoint::ConstructL( KPrinter());

	// Share Xhtml folder (create if necessary)
	RFs fsSession;
	User::LeaveIfError( fsSession.Connect() );
	CleanupClosePushL( fsSession );
	TInt err = fsSession.MkDirAll( KUPnPXhtmlPath() );
	if (err != KErrNone && err != KErrAlreadyExists)
	{
		User::Leave(err);
	}
	CleanupStack::PopAndDestroy(&fsSession);

	iJobStateEvents = NULL;

	// Start printer search
	TPtrC8 device_ptr;
	device_ptr.Set( KPrinterDevice );
	SearchL( device_ptr );

}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::~CPrinterControlPoint
// C++ default destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CPrinterControlPoint::~CPrinterControlPoint()
{
	LOG("[CPrinterControlPoint] \tCPrinterControlPoint::~CPrinterControlPoint()");
    iCancelledJobSIDs.Close();   

    if(iJobStateEvents)
	{
		delete iJobStateEvents;
	}
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::CreateJobL
// Create printer job
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPrinterControlPoint::CreateJobL( CUpnpDevice* aPrinter, const TDesC8& aPrintFile,
                                  const TDesC8& aUserName, const TDesC8& aCopies, const TDesC8& aSides,
                                  const TDesC8& aNumberUp, const TDesC8& aOrientationRequest, const TDesC8& aMediaSize,
                                  const TDesC8& aMediaType, const TDesC8& aPrintQuality, TBool aPrintBasic )
{
	LOG("[CPrinterControlPoint] \tCPrinterControlPoint::CreateJobL");

	if(!aPrinter)
	{
		User::Leave(KErrArgument);
	}

	if(iJobStateEvents)
	{
		iJobStateEvents->InitL();
	}
	else
	{
		iJobStateEvents = CJobStateEvents::NewL();
	}

    // Set printer as authorized device
   	//  - remove the previous printer address from allowed list
  	//  - add the current printer address to allowed list
	
	TBuf8<20> logStr;
	CUpnpHttpMessage::AddrOutput(aPrinter->Address(), logStr);
	LOG81("[CPrinterControlPoint::CreateJobL]/t Printer address: %S", &logStr);

   	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );


	if (serviceType == EServiceNone)
	{
		User::Leave(KErrNotSupported);
	}

   	if (aPrintBasic)
   	{
   		// Force to use PrintBasic:1
   		serviceType = EPrintBasic;
   	}

	// PrintBasic:1 or PrintEnhanced:1 supported
	SubscribeL( service );
	CUpnpAction* action;

   	// Create action according to service type

   	if (serviceType == EPrintEnhanced)
   	{
   		LOG("[CPrinterControlPoint] \t -PrintEnhanced:1 - CreateJobV2");
    	action = service->CreateActionLC( KCreateJobV2 );
   	}

   	else // serviceType == EPrintBasic
   	{ 
   		LOG("[CPrinterControlPoint] \t -PrintBasic:1 - CreateJob");
		  action = service->CreateActionLC( KCreateJob() );
  	}

	User::LeaveIfNull(action);

	// Common arguments for both action types
	User::LeaveIfError(action -> SetArgumentL( KJobName, aPrintFile ));
	User::LeaveIfError(action -> SetArgumentL( KUserName,  aUserName));
	User::LeaveIfError(action -> SetArgumentL( KDocFormat, KUPnPDocFormat() ));
	User::LeaveIfError(action -> SetArgumentL( KCopies, aCopies ));
	User::LeaveIfError(action -> SetArgumentL( KSides, aSides ));
	User::LeaveIfError(action -> SetArgumentL( KNumberUp, aNumberUp ));
	User::LeaveIfError(action -> SetArgumentL( KOrientationRequest, aOrientationRequest ));
	User::LeaveIfError(action -> SetArgumentL( KSize, aMediaSize ));
	User::LeaveIfError(action -> SetArgumentL( KType, aMediaType ));
	User::LeaveIfError(action -> SetArgumentL( KQuality, aPrintQuality ));

	if (serviceType == EPrintEnhanced)
	{
		// Fill in the rest arguments
		User::LeaveIfError(action->SetArgumentL( KCriticalAttributes, KNullDesC8() ));
	}
	
	// Send action
	SendL( action );
	
	// return session id
	TInt sessionId = action->SessionId();
   	CleanupStack::Pop(); // action
    return sessionId;
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::PrintingFinishedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::PrintingFinishedL( CUpnpDevice* aPrinter )
{

	LOG("[CPrinterControlPoint::PrintingFinishedL] \t");

	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );

	if (serviceType == EServiceNone)
	{
		User::Leave(KErrNotSupported);
	}

	// Service is PrintBasic:1 or PrintEnhanced:1 - Unsubscribe
	UnsubscribeL(service);

	LOG("[CPrinterControlPoint::PrintingFinishedL] \t - unsubscribed... ...deleting event");

	// init job state events
	if ( iJobStateEvents )
		{
		iJobStateEvents->InitL();
		}
    // Remove last printer address from allowed list

	LOG("[CPrinterControlPoint::PrintingFinishedL] \t - out");
}



// -----------------------------------------------------------------------------
// CPrinterControlPoint::CancelJobL
// Cancel printer job
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::CancelJobL( CUpnpDevice* aPrinter,
    const TDesC8& aJobId )
{

	LOG81("[CPrinterControlPoint] \tCPrinterControlPoint::CancelJobL %S", &aJobId);

	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );

	if (serviceType == EServiceNone)
	{
		User::Leave(KErrNotSupported);
	}

	// Service is PrintBasic:1 or PrintEnhanced:1
	DoCancelJobL(*service, aJobId);
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::CancelJobL
// Cancel printer job by session id
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::CancelJobL( const TInt aSessionId )
{

	LOG1("[CPrinterControlPoint::CancelJobL] \t by session id %d", aSessionId);

	// Add id to cancelled jobs - nothing else to do at this point
	// Actual cancelling is done when a message with this session id and job id is received from printer
	iCancelledJobSIDs.AppendL(aSessionId);

	// If number of pending cancel request exceeds KMaxCancelReqs delete the oldest request
	if (iCancelledJobSIDs.Count() > KMaxCancelReqs)
	{
		iCancelledJobSIDs.Remove(0);
	}


}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::GetPrinterAttributesL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::GetPrinterAttributesL( CUpnpDevice* aPrinter )
{

	LOG("[CPrinterControlPoint::GetPrinterAttributesL]");

	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );

	if (serviceType == EServiceNone)
	{
		User::Leave(KErrNotSupported);
	}

	if( serviceType == EPrintBasic )
   	{
		CUpnpAction* action = service -> CreateActionLC( KGetPrinterAttr );
		User::LeaveIfNull(action);
		SendL( action );
		CleanupStack::Pop();	// action
   	}

	if( serviceType == EPrintEnhanced )
   	{
		CUpnpAction* action = service -> CreateActionLC( KGetPrinterAttrV2 );
		User::LeaveIfNull(action);
		SendL( action );
		CleanupStack::Pop();	// action
   	}

}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::GetMarginsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::GetMarginsL( CUpnpDevice* aPrinter, const TDesC8& aMediaSize, const TDesC8& aMediaType )
{

	LOG("[CPrinterControlPoint::GetMarginsL]");

	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );

	if( serviceType == EPrintEnhanced )
   	{
		CUpnpAction* action = service -> CreateActionLC( KGetMargins );
		User::LeaveIfNull(action);
		User::LeaveIfError(action -> SetArgumentL( KSize, aMediaSize ));
		User::LeaveIfError(action -> SetArgumentL( KType,  aMediaType));
		SendL( action );
		CleanupStack::Pop();	// action
   	}
   	else
   	{
		User::Leave(KErrNotSupported);
   	}

}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::GetMarginsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::GetMediaListL( CUpnpDevice* aPrinter, const TDesC8& aMediaSize, const TDesC8& aMediaType )
{

	LOG("[CPrinterControlPoint::GetMediaListL]");

	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );

	if( serviceType == EPrintEnhanced )
   	{
		CUpnpAction* action = service -> CreateActionLC( KGetMediaList );
		User::LeaveIfNull(action);
		User::LeaveIfError(action -> SetArgumentL( KSize, aMediaSize ));
		User::LeaveIfError(action -> SetArgumentL( KType,  aMediaType));
		SendL( action );
		CleanupStack::Pop();	// action
   	}
   	else
   	{
		User::Leave(KErrNotSupported);
   	}

}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::GetJobAttributesL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::GetJobAttributesL( CUpnpDevice* aPrinter,
    const TDesC8& aJobId )
{

	LOG("[CPrinterControlPoint::GetJobAttributesL]");

	// Check services
	TPrintService serviceType;
	CUpnpService* service = GetServiceFromDevice( aPrinter, serviceType );

	if (serviceType == EServiceNone)
	{
		User::Leave(KErrNotSupported);
	}

	CUpnpAction* action = service -> CreateActionLC( KGetJobAttr );
	User::LeaveIfNull(action);
	User::LeaveIfError(action -> SetArgumentL( KJobId, aJobId ));
	SendL( action );
	CleanupStack::Pop();	// action

}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::StateUpdatedL
// From ControlPoint
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::StateUpdatedL( CUpnpService* aService )
{
	// Seven evented state variables (see PrintEnhanced:1 and PrintBasic:1 specification for details):
	// PrinterState, PrinterStateReasons, JobIdList, JobEndState,
	// JobMediaSheetsConpleted, JobAbortState and ContentCompleteList

	// JobEndState == JobId,JobName,JobOriginatingUser,
	// JobMediaSheetsConpleted,job-completion-state

	LOG("[CPrinterControlPoint::StateUpdatedL]");

	User::LeaveIfNull(aService);
	
	JobStateLog(aService);

	CUpnpStateVariable* prntState = aService->StateVariable( KPrinterState() );
	CUpnpStateVariable* prntStateReasons = aService->StateVariable( KPrinterStateReasons() );
	CUpnpStateVariable* jobIdList = aService->StateVariable( KJobIdList() );
	CUpnpStateVariable* jobEndState = aService->StateVariable( KJobEndState() );
	CUpnpStateVariable* sheets = aService->StateVariable( KSheetsCompleted() );

	// The next ones are supported only in PrintEnhanced:1
	CUpnpStateVariable* jobAbortState = aService->StateVariable( KJobAbortState() );
	CUpnpStateVariable* contentCompList = aService->StateVariable( KContentCompleteList() );

	if ( iJobStateEvents )
		{	
	    if( prntState )
			{
				TPtrC8 ptr = RemoveTags( prntState->Value());
		        if(iJobStateEvents->ChangedEventL(ptr, CJobStateEvents::EPrinterState))
		        	iPCPObserver.PrinterStateUpdated( ptr );
			}		    
		    if ( prntStateReasons )
			    {
					TPtrC8 ptr = RemoveTags( prntStateReasons->Value());
			        if(iJobStateEvents->ChangedEventL(ptr, CJobStateEvents::EPrinterStateReasons))
			        	iPCPObserver.PrinterStateReasonsUpdated( ptr );
			    }
		    
		    if ( jobIdList )
			    {
					TPtrC8 ptr = RemoveTags( jobIdList->Value());
			        if(iJobStateEvents->ChangedEventL( ptr, CJobStateEvents::EJobIdList))
			        	iPCPObserver.JobIdListUpdated( ptr );
			    }
		    
		    if ( jobEndState )
			    {
					TPtrC8 ptr = RemoveTags( jobEndState->Value());
			        if(iJobStateEvents->ChangedEventL( ptr, CJobStateEvents::EJobEndState))
			        	iPCPObserver.JobEndStateUpdated( ptr );
			    }
		    
		    if ( sheets )
			    {
					TPtrC8 ptr = RemoveTags( sheets->Value());
			        if(iJobStateEvents->ChangedEventL(ptr, CJobStateEvents::EJobMediaSheetsCompleted))
			        	iPCPObserver.JobMediaSheetsCompletedUpdated( ptr );
			    }
		    
		    if ( jobAbortState )
			    {
					TPtrC8 ptr = RemoveTags( jobAbortState->Value());
			        if(iJobStateEvents->ChangedEventL(ptr, CJobStateEvents::EContentCompleteList))
			        	iPCPObserver.JobAbortStateUpdated( ptr );
			    }
		    
		    if ( contentCompList )
			    {
					TPtrC8 ptr = RemoveTags( contentCompList->Value());
			        if(iJobStateEvents->ChangedEventL( ptr, CJobStateEvents::EJobAbortState))
			        	iPCPObserver.ContentCompleteListUpdated( ptr );
			    }
		}
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::DeviceDiscoveredL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::DeviceDiscoveredL( CUpnpDevice* aDevice)
{
	LOG("[CPrinterControlPoint::DeviceDiscoveredL]");
	iPCPObserver.DeviceDiscoveredL( aDevice );
}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::DeviceDisappearedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::DeviceDisappearedL( CUpnpDevice* aDevice )
{
	LOG("[CPrinterControlPoint::DeviceDisappearedL]");
	iPCPObserver.DeviceDisappeared( aDevice );
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::ActionResponseReceivedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::ActionResponseReceivedL( CUpnpAction* aAction )
{
	User::LeaveIfNull(aAction);

	LOG8S("[CPrinterControlPoint::ActionResponseReceivedL]\t - action: ", aAction->Name());

   	// First check out for pending cancel responses (ie. cancel requestes with session id)
	TInt sessionId = aAction->SessionId();
	TInt index = iCancelledJobSIDs.Find(sessionId);

	if ( index >= 0 )
	{
		// Cancel requested for this session's job
		const TDesC8& jobId = aAction->ArgumentValue( KJobId() );

		if (jobId.Length() > 0)
		{
			LOG("[CPrinterControlPoint::ActionResponseReceivedL] \t - Executing session id based canceling");
			DoCancelJobL( aAction->Service(), jobId);

			// Remove "queued" cancel job's session id from array
			iCancelledJobSIDs.Remove(index);

			// No need to handle create job responses for this job id since it was canceled
			if(( aAction->Name().Compare( KCreateJob ) == 0 ) ||
			  ( aAction->Name().Compare( KCreateURIJob ) == 0))
			{
				return;
			}
		}
	}

	TInt err = KErrNone;
	if( EHttp200Ok != aAction->Error() )
	{
		err = aAction->Error();
	}

	// Check CreateJobResponse
	if( aAction->Name().Compare( KCreateJob ) == 0  || aAction->Name().Compare( KCreateJobV2 ) == 0 )
    {
		if( KErrNone == err )
    	{
			const TDesC8& jobName 		= aAction->ArgumentValue( KJobName() );
			const TDesC8& dataSinkUrl 	= aAction->ArgumentValue( KDataSink() );

			// Send XHTML-print data (print data) via HTTP POST
			
			LOG("[CPrinterControlPoint] \t - Sending XHTML-print data via HTTP POST");
		    TInetAddr addr;
		    CUpnpHttpMessage::AddrInput(addr, DataSinkUrlAddress( dataSinkUrl ));
	    	addr.SetPort( DataSinkUrlPort( dataSinkUrl ) );

		    CUpnpHttpMessage* printDataMessage;
				//CUpnpHttpMessage* printPictureMessage;


	    	printDataMessage = RUpnpHttpMessageFactory::HttpPostL( addr, DataSinkUrlPath( dataSinkUrl ) );

				//printPictureMessage = RUpnpHttpMessageFactory::HttpPostL( addr, DataSinkUrlPath( dataSinkUrl ) );
			
			TFileName8 filePath;
			filePath.Copy( KUPnPXhtmlPath() );
			filePath.Append(jobName);
			printDataMessage->SetOutFilenameL( filePath );

		   	TRAP(err, SendL( printDataMessage ));
		   	
    	}

		iPCPObserver.CreateJobResponse( aAction, err );
		
		
		LOG1("[CPrinterControlPoint] \t - HTTP POST returns: %d", err);
	}
	else if( aAction->Name().Compare( KCreateURIJob ) == 0 )
	{
		iPCPObserver.CreateJobResponse( aAction, err );
	}
	else if( aAction->Name().Compare( KCancelJob ) == 0 )
	{
		iPCPObserver.CancelJobResponse( aAction, err );
   	}
	else if( aAction->Name().Compare( KGetPrinterAttr ) == 0 )
    {
		iPCPObserver.GetPrinterAttributesResponse( aAction, err );
   	}
	else if( aAction->Name().Compare( KGetPrinterAttrV2 ) == 0 )
    {
		iPCPObserver.GetPrinterAttributesResponse( aAction, err );
   	}
	else if( aAction->Name().Compare( KGetJobAttr ) == 0 )
    {
		iPCPObserver.GetJobAttributesResponse( aAction, err );
   	}
	else if( aAction->Name().Compare( KGetMargins ) == 0 )
    {
		iPCPObserver.GetMarginsResponse( aAction, err );
   	}
	else if( aAction->Name().Compare( KGetMediaList ) == 0 )
    {
		iPCPObserver.GetMediaListResponse( aAction, err );
   	}
    else
    {
    	//to avoid lint error
    }

}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::HttpResponseReceivedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPrinterControlPoint::HttpResponseReceivedL(
    CUpnpHttpMessage* aMessage )
{
	LOG("[CPrinterControlPoint::HttpResponseReceivedL] \t");
	if(aMessage)
    {
		LOG1( "[CPrinterControlPoint] \t - HTTP Response: %d", aMessage->Error() );
    }
}

// private functions

// -----------------------------------------------------------------------------
// CPrinterControlPoint::GetServiceFromDevice
// -----------------------------------------------------------------------------
//
CUpnpService* CPrinterControlPoint::GetServiceFromDevice( CUpnpDevice* aPrinter, TPrintService& aService  )
{
	LOG("[CPrinterControlPoint::GetServiceFromDevice]");
	CUpnpService* service = NULL;
	aService = EServiceNone;

	if(!aPrinter)
	{
		return service;
	}

	const RPointerArray<CUpnpDevice>& devices = DeviceList();

	//Search of printer device
	for( TInt i = 0; i < devices.Count(); i++ )
    {
		if( devices[i]->DeviceType().Compare( aPrinter->DeviceType() ) == 0 &&
		    devices[i]->Uuid().Compare( aPrinter->Uuid() ) == 0 )
	    {
	    	// Printer found - search of services
	    	RPointerArray<CUpnpService>& services = devices[i]->ServiceList();

			for( TInt j = 0; j < services.Count(); j++ )
	    	{
				//PrintBasic:1
				if( services[j]->ServiceType().Compare( KPrintBasic ) == 0 )
		    	{
					aService = EPrintBasic;
					service = services[j];
					continue; // Continue to search for PrintEnhanced:1
		    	}

				// PrintEnhanced:1
				if( services[j]->ServiceType().Compare( KPrintEnhanced ) == 0 )
		    	{
					aService = EPrintEnhanced;
					service = services[j];
					break;
		    	}

			}
			break;
	    }
    }

	return service;
}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::DataSinkUrlAddress
// -----------------------------------------------------------------------------
//
const TPtrC8 CPrinterControlPoint::DataSinkUrlAddress( const TDesC8& aDataSinkUrl )
{
	LOG("[CPrinterControlPoint::DataSinkUrlAddress] \t");

	if ( aDataSinkUrl.Length() > UpnpHTTP::KHTTPUrl().Length() )
    {
		TPtrC8 addrAndPath = aDataSinkUrl.Right(aDataSinkUrl.Length()-UpnpHTTP::KHTTPUrl().Length());
		TInt index = addrAndPath.Find( KUPnPColon() );
		if ( index == KErrNotFound )
		{
			index = addrAndPath.Find( KUPnPSlash() );
		}

		if ( index == KErrNotFound )
	    {
			return TPtrC8( KNullDesC8().Ptr(), 0);
	    }

		TPtrC8 addr = addrAndPath.Left( index );

		return addr;
   	}

	return TPtrC8( KNullDesC8().Ptr(), 0);
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::DataSinkUrlPath
// -----------------------------------------------------------------------------
//
const TPtrC8 CPrinterControlPoint::DataSinkUrlPath( const TDesC8& aDataSinkUrl )
{
	LOG("[CPrinterControlPoint::DataSinkUrlPath] \t");

	if ( aDataSinkUrl.Length() > KHttpPrefix().Length() )
    {
		TPtrC8 addrAndPath = aDataSinkUrl.Right(aDataSinkUrl.Length()-KHttpPrefix().Length());
        TPtrC8 addr;

        // Find slash from URL and remove it, if it exists.
		TInt index = addrAndPath.Find( KUPnPSlash() );
        // If not found, assume that path is right as is
		if ( index == KErrNotFound )
        {
            addr.Set( addrAndPath.Right( addrAndPath.Length() ) );
        }
        // Slash found, copy data after it
        else
        {
            addr.Set( addrAndPath.Right( addrAndPath.Length() - index ) );
        }
		return addr;
	    }
	else
    {
		return TPtrC8( KNullDesC8().Ptr(), 0);
    }
}

// -----------------------------------------------------------------------------
// CPrinterControlPoint::DataSinkUrlPort
//
// -----------------------------------------------------------------------------
//
TInt CPrinterControlPoint::DataSinkUrlPort( const TDesC8& aDataSinkUrl )
{
	LOG("[CPrinterControlPoint::DataSinkUrlPort] \t");

	if (aDataSinkUrl.Length() > KHttpPrefix().Length())
    {
		TPtrC8 addrAndPath = aDataSinkUrl.Right(aDataSinkUrl.Length()-KHttpPrefix().Length());
		TInt separIndex = addrAndPath.Find( KUPnPColon );
		if ( separIndex == KErrNotFound )
        {
			return KDefaultHttpPortNumber;
        }
		TInt slashIndex = addrAndPath.Find( KUPnPSlash() );
		if ( slashIndex != -1 )
		{
			TPtrC8 port = addrAndPath.Mid( separIndex+1, slashIndex-separIndex );

			TLex8 lex( port );
			TInt prt;
			lex.Val( prt );

			return prt;
		}
		else
		{
			TPtrC8 port = addrAndPath.Right( addrAndPath.Length()-separIndex-1 );

			TLex8 lex( port );
			TInt prt;
			lex.Val( prt );

			return prt;
		}
	}
	return KDefaultHttpPortNumber;
}


// -----------------------------------------------------------------------------
// CPrinterControlPoint::DoCancelJobL
//
// -----------------------------------------------------------------------------
//
void CPrinterControlPoint::DoCancelJobL(CUpnpService& aService, const TDesC8& aJobId)
{
	LOG("[CPrinterControlPoint::DoCancelJobL]");
	CUpnpAction* action = aService.CreateActionLC( KCancelJob );
	User::LeaveIfNull(action);
	User::LeaveIfError(action->SetArgumentL( KJobId, aJobId ));
	SendL( action );
	CleanupStack::Pop(); // action
}

//--------------------------------------------------------------------------------------------
//
// CPrinterControlPoint::RemoveTags
//
//--------------------------------------------------------------------------------------------
TPtrC8 CPrinterControlPoint::RemoveTags( const TDesC8& aString)
{
	LOG("[CPrinterControlPoint::RemoveTags]\t");
	_LIT8(KUPnPStartTag, 	">");
	_LIT8(KUPnPEndTag, 		"</");
	_LIT8(KUPnPEmptyTag, 	"<*/>");

	TPtrC8 midPtr;
	midPtr.Set(aString);

	// Return if no tags
	TInt startPos = aString.Find(KUPnPStartTag());
	if(KErrNotFound == startPos)
	{
		return midPtr;
	}
	
	// Return if empty
	if(KErrNotFound < aString.Match(KUPnPEmptyTag()))
	{
		return TPtrC8( KNullDesC8().Ptr(), 0);
	}

	// Return the mid of tags
	startPos += KUPnPStartTag().Length();

	TInt endPos = aString.Find(KUPnPEndTag());
	if(KErrNotFound == endPos)
	{
		return midPtr;
	}
	
	midPtr.Set(aString.Mid(startPos, endPos - startPos));
	return midPtr;
}

//--------------------------------------------------------------------------------------------
//
// CPrinterControlPoint::RemoveTags
//
//--------------------------------------------------------------------------------------------
void CPrinterControlPoint::JobStateLog(CUpnpService* aService)
{
	if(!aService)
	{
		return;
	}
	LOG81("CPrinterControlPoint::JobStateLog\t prntState\t\t : %S", &(aService->StateVariable( KPrinterState() )->Value()));
	LOG81("CPrinterControlPoint::JobStateLog\t prntStateReasons: %S", &(aService->StateVariable( KPrinterStateReasons() )->Value()));
	LOG81("CPrinterControlPoint::JobStateLog\t jobIdList\t\t : %S", &(aService->StateVariable( KJobIdList() )->Value()));
	LOG81("CPrinterControlPoint::JobStateLog\t jobEndState\t : %S", &(aService->StateVariable( KJobEndState() )->Value()));
	LOG81("CPrinterControlPoint::JobStateLog\t sheets\t\t\t : %S", &(aService->StateVariable( KSheetsCompleted() )->Value()));
	LOG81("CPrinterControlPoint::JobStateLog\t jobAbortState\t : %S", &(aService->StateVariable( KJobAbortState() )->Value()));
	LOG81("CPrinterControlPoint::JobStateLog\t contentCompList : %S", &(aService->StateVariable( KContentCompleteList() )->Value()));
}

// End Of File
