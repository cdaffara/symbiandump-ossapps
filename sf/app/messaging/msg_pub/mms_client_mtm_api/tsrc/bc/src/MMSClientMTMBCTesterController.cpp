/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         Test controller implementation
*
*/










// INCLUDE FILES
#include "MMSClientMTMBCTesterController.h"
#include "MMSClientMTMBCTester.h"
#include "CMsvMimeHeaders.h"

/***
* CMmsSettings class can only be included if the internal MMS Server Settings
* API is available. See MMSClientMTMBCTester.mmp for instructions on taking
* the class into use.
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING
#include "mmssettings.h"
#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

#include <mtmdef.h>
#include <msvids.h>


// CONSTANTS

// Character strings with logging
_LIT( KResult,			"    result == %d" );
_LIT( KInitialResult,	"    initial result == %d" );
_LIT( KFinalResult,		"    final result == %d" );


// PUBLIC MEMBER FUNCTIONS


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CMMSClientMTMBCTesterController* CMMSClientMTMBCTesterController::NewL()
	{
	CMMSClientMTMBCTesterController* self =
		new(ELeave) CMMSClientMTMBCTesterController();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::~CMMSClientMTMBCTesterController
// Destructor
// -----------------------------------------------------------------------------
//
CMMSClientMTMBCTesterController::~CMMSClientMTMBCTesterController()
	{
	// Clear all messages
	Cleanup();
	
	// Clean up used memory
	delete iFileBuf;
	delete iClient;
	delete iRegistry;
	delete iSession;
	delete iLogger;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::HandleSessionEventL
// From MMsvSessionObserver interface
// Handles the multimedia server session events
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::HandleSessionEventL(
	TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
	{
	_LIT( KHandleSessionEvent, "    HandleSessionEventL: aEvent == %d" );
	iLogger->Log( KHandleSessionEvent, aEvent );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::CreateNewMessageEntryL
// Creating a new message entry
// -----------------------------------------------------------------------------
//
TMsvId CMMSClientMTMBCTesterController::CreateNewMessageEntryL(
	TMsvId aDestination )
	{
	_LIT( KFunc, "CreateNewMessageEntryL:" );
	_LIT( KParam, "    aDestination == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aDestination );
	
	// Begin to create a new message entry
	CMsvOperation* operation = NULL;
	CMsvOperationActiveSchedulerWait* wait =
		CMsvOperationActiveSchedulerWait::NewLC();
	operation = iClient->CreateNewEntryL( aDestination, wait->iStatus );
	CleanupStack::PushL( operation );

	// Start the operation
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KResult, result );
    User::LeaveIfError( result );
	
	// Get the message id of the newly created message
	TPckgBuf< TMsvId > pkg;
	pkg.Copy( operation->ProgressL() );
	TMsvId id = 0;
	id = pkg();
	_LIT( KMsgId, "    message id == %d" );
	iLogger->Log( KMsgId, id );

	// Clean up and return the determined id	
	CleanupStack::PopAndDestroy( 2 ); // operation, wait
	return id;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::CreateNewServiceEntry
// Creating a new service entry
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::CreateNewServiceEntry()
	{
	_LIT( KFunc, "CreateNewServiceEntry:" );
	iLogger->Log( KFunc );

	// Attempt to create a new service entry
	TRAPD ( result, iClient->CreateServiceL() );
	iLogger->Log( KResult, result );

	return result;
	}


/***
* Functions that deal with CMmsSettings class can only be included if the
* internal MMS Server Settings API is available.
* See MMSClientMTMBCTester.mmp for instructions on taking the functions to use.
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMMSSettings
// Getting the MMS settings
// -----------------------------------------------------------------------------
//
CMmsSettings* CMMSClientMTMBCTesterController::GetMMSSettingsL()
	{
	_LIT( KFunc, "GetMMSSettingsL" );
	iLogger->Log( KFunc );

	// Fetch the MMS settings and return a pointer to them	
	CMmsSettings* settings = CMmsSettings::NewL();
	CleanupStack::PushL( settings );
	settings->CopyL( iClient->MmsSettings() );
	CleanupStack::Pop(); // settings
	return settings;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMMSSettings
// Setting the MMS settings
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SetMMSSettings(
	const CMmsSettings& aSettings )
	{
	_LIT( KFunc, "SetMMSSettings:" );
	iLogger->Log( KFunc );
	
	// Attempt to set the MMS settings
	TRAPD ( result, iClient->SetSettingsL( aSettings ) );
	iLogger->Log( KResult, result );
	
	return result;
	}

#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::StoreMMSSettings
// Storing MMS settings to disk
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::StoreMMSSettings()
	{
	_LIT( KFunc, "StoreMMSSettings:" );
	iLogger->Log( KFunc );
	
	// Attempt to store the MMS settings to disk
	TRAPD ( result, iClient->StoreSettingsL() );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::RestoreMMSSettings
// Restoring MMS settings from disk
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::RestoreMMSSettings()
	{
	_LIT( KFunc, "RestoreMMSSettings:" );
	iLogger->Log( KFunc );
	
	// Attempt to restore the MMS settings from disk
	TRAPD ( result, iClient->RestoreSettingsL() );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::RestoreFactoryMMSSettings
// Restoring factory MMS settings
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::RestoreFactoryMMSSettings()
	{
	_LIT( KFunc, "RestoreFactoryMMSSettings:" );
	iLogger->Log( KFunc );
	
	// Attempt to restore the factory MMS settings
	TRAPD ( result, iClient->RestoreFactorySettingsL() );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ValidateService
// Validating service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::ValidateService( TMsvId aServiceId )
	{
	_LIT( KFunc, "ValidateService:" );
	_LIT( KParam, "    aServiceId == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aServiceId );
	
	// Validate the service with the service id given as a parameter
	TInt result = iClient->ValidateService( aServiceId );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetDefaultService
// Getting the id of the default service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetDefaultService( TMsvId& aServiceId )
	{
	_LIT( KFunc, "GetDefaultService:" );
	iLogger->Log( KFunc );
	
	// Attempt to get the default service id
	TRAPD ( result, aServiceId = iClient->DefaultServiceL() );
	iLogger->Log( KResult, result );
	if ( result == KErrNone )
		{
		_LIT( KServiceId, "    aServiceId == %d" );
		iLogger->Log( KServiceId, aServiceId );
		}
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetSender
// Setting the sender MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SetSender( const TDesC& aSender )
	{
	_LIT( KFunc, "SetSender:" );
	_LIT( KParam, "    aSender == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aSender );	
	
	// Attempt to set the sender MMS header
	TRAPD ( result, iClient->SetSenderL( aSender ) );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetSender
// Getting the sender MMS header
// -----------------------------------------------------------------------------
//
const TPtrC CMMSClientMTMBCTesterController::GetSender()
	{
	_LIT( KFunc, "GetSender:" );
	iLogger->Log( KFunc );
	
	// Get the sender
	TPtrC sender = iClient->Sender();
	_LIT( KSender, "    sender == %S" );
	iLogger->Log( KSender, &sender );
	
	return sender;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMessageClass
// Setting the message class MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetMessageClass(
	TMmsMessageClass aMessageClass )
	{
	_LIT( KFunc, "SetMessageClass:" );
	_LIT( KParam, "    aMessageClass == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aMessageClass );
	
	// Set the message class
	iClient->SetMessageClass( aMessageClass );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessageClass    
// Getting the message class MMS header
// -----------------------------------------------------------------------------
//    
TInt CMMSClientMTMBCTesterController::GetMessageClass()
	{
	_LIT( KFunc, "GetMessageClass:" );
	iLogger->Log( KFunc );
	
	// Get the message class
	TInt messageClass = iClient->MessageClass();
	_LIT( KMsgClass, "    messageClass == %d" );
	iLogger->Log( KMsgClass, messageClass );
	
	return messageClass;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetExpirationInterval
// Setting the expiration interval MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetExpirationInterval(
	TTimeIntervalSeconds aInterval )
	{
	_LIT( KFunc, "SetExpirationInterval:" );
	_LIT( KParam, "    aInterval == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aInterval.Int() );
	
	// Set the expiration interval
	iClient->SetExpiryInterval( aInterval );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetExpirationInterval
// Getting the expiration interval MMS header
// -----------------------------------------------------------------------------
//
TTimeIntervalSeconds CMMSClientMTMBCTesterController::GetExpirationInterval()
	{
	_LIT( KFunc, "GetExpirationInterval:" );
	iLogger->Log( KFunc );
	
	// Get the expiration interval
	TTimeIntervalSeconds expiryInterval = iClient->ExpiryInterval();
	_LIT( KInterval, "    expiryInterval == %d" );
	iLogger->Log( KInterval, expiryInterval.Int() );
	
	return expiryInterval;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetExpirationDate
// Setting the expiration date MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetExpirationDate( TTime aDate )
	{
	_LIT( KFunc, "SetExpirationDate:" );
	_LIT( KParam, "    aDate == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aDate.Int64() );
	
	// Set the expiration date
	iClient->SetExpiryDate( aDate );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetExpirationDate
// Getting the expiration date MMS header
// -----------------------------------------------------------------------------
//
TTime CMMSClientMTMBCTesterController::GetExpirationDate()
	{
	_LIT( KFunc, "GetExpirationDate" );
	iLogger->Log( KFunc );
	
	// Get the expiration date
	TTime expiryDate = iClient->ExpiryDate();
	_LIT( KExpiryDate, "    expiryDate == %d" );
	iLogger->Log( KExpiryDate, expiryDate.Int64() );
	
	return expiryDate;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetDeliveryTimeInterval
// Setting the delivery time interval MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetDeliveryTimeInterval(
	TTimeIntervalSeconds aInterval )
	{
	_LIT( KFunc, "SetDeliveryTimeInterval:" );
	_LIT( KParam, "    aInterval == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aInterval.Int() );
	
	// Set the delivery time interval
	iClient->SetDeliveryTimeInterval( aInterval );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetDeliveryTimeInterval
// Getting the delivery time interval MMS header
// -----------------------------------------------------------------------------
//
TTimeIntervalSeconds CMMSClientMTMBCTesterController::GetDeliveryTimeInterval()
	{
	_LIT( KFunc, "GetDeliveryTimeInterval:" );
	iLogger->Log( KFunc );
	
	// Get the delivery time interval
	TTimeIntervalSeconds deliveryTimeInterval = iClient->DeliveryTimeInterval();
	_LIT( KInterval, "    deliveryTimeInterval == %d" );
	iLogger->Log( KInterval, deliveryTimeInterval.Int() );
	
	return deliveryTimeInterval;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetDeliveryDate
// Setting the delivery date MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetDeliveryDate( TTime aDate )
	{
	_LIT( KFunc, "SetDeliveryDate:" );
	_LIT( KParam, "    aDate == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aDate.Int64() );
	
	// Set the delivery date
	iClient->SetDeliveryDate( aDate );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetDeliveryDate
// Getting the delivery date MMS header
// -----------------------------------------------------------------------------
//
TTime CMMSClientMTMBCTesterController::GetDeliveryDate()
	{
	_LIT( KFunc, "GetDeliveryDate:" );
	iLogger->Log( KFunc );
	
	// Get the delivery date
	TTime deliveryDate = iClient->DeliveryDate();
	_LIT( KDelivery, "    deliveryDate == %d" );
	iLogger->Log( KDelivery, deliveryDate.Int64() );
	
	return deliveryDate;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMessagePriority
// Setting the message priority MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetMessagePriority(
	TMmsMessagePriority aPriority )
	{
	_LIT( KFunc, "SetMessagePriority:" );
	_LIT( KParam, "    aPriority == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aPriority );
	
	// Set the message priority
	iClient->SetMessagePriority( aPriority );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessagePriority
// Getting the message priority MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetMessagePriority()
	{
	_LIT( KFunc, "GetMessagePriority:" );
	iLogger->Log( KFunc );
	
	// Get the message priority
	TInt messagePriority = iClient->MessagePriority();
	_LIT( KPriority, "    messagePriority == %d" );
	iLogger->Log( KPriority, messagePriority );
	
	return messagePriority;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetSenderVisibility
// Setting the sender visibility MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetSenderVisibility(
	TMmsMessageSenderVisibility aVisibility )
	{
	_LIT( KFunc, "SetSenderVisibility:" );
	_LIT( KParam, "    aVisibility == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aVisibility );
	
	// Set the sender visibility
	iClient->SetSenderVisibility( aVisibility );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetSenderVisibility
// Getting the sender visibility MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetSenderVisibility()
	{
	_LIT( KFunc, "GetSenderVisibility:" );
	iLogger->Log( KFunc );
	
	// Get the sender visibility
	TInt senderVisibility = iClient->SenderVisibility();
	_LIT( KVisibility, "    senderVisibility == %d" );
	iLogger->Log( KVisibility, senderVisibility );
	
	return senderVisibility;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetDeliveryReportRequest
// Setting the delivery report request MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetDeliveryReportRequest(
	TMmsYesNo aRequest )
	{
	_LIT( KFunc, "SetDeliveryReportRequest:" );
	_LIT( KParam, "    aRequest == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aRequest );
	
	// Set the delivery report request
	iClient->SetDeliveryReport( aRequest );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetDeliveryReportRequest
// Getting the delivery report request MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetDeliveryReportRequest()
	{
	_LIT( KFunc, "GetDeliveryReportRequest:" );
	iLogger->Log( KFunc );
	
	// Get the delivery report request
	TInt request = iClient->DeliveryReport();
	_LIT( KReport, "    request == %d" );
	iLogger->Log( KReport, request );
	
	return request;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetReadReplyRequest
// Setting the read reply request MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetReadReplyRequest(
	TMmsYesNo aRequest )
	{
	_LIT( KFunc, "SetReadReplyRequest:" );
	_LIT( KParam, "    aRequest == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aRequest );
	
	// Set the read reply request
	iClient->SetReadReply( aRequest );	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetReadReplyRequest
// Getting the read reply request MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetReadReplyRequest()
	{
	_LIT( KFunc, "GetReadReplyRequest:" );
	iLogger->Log( KFunc );
	
	// Get the read reply request
	TInt request = iClient->ReadReply();
	_LIT( KReadReply, "    request == %d" );
	iLogger->Log( KReadReply, request );
	
	return request;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetSendingDate
// Getting the sending date MMS header
// -----------------------------------------------------------------------------
//
TTime CMMSClientMTMBCTesterController::GetSendingDate()
	{
	_LIT( KFunc, "GetSendingDate:" );
	iLogger->Log( KFunc );
	
	// Get the sending date
	TTime time = iClient->SendingDate();
	_LIT( KSending, "    time == %d" );
	iLogger->Log( KSending, time.Int64() );
	
	return time;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetResponseText
// Getting the response text MMS header
// -----------------------------------------------------------------------------
//
TPtrC CMMSClientMTMBCTesterController::GetResponseText()
	{
	_LIT( KFunc, "GetResponseText:" );
	iLogger->Log( KFunc );
	
	// Get the response text
	TPtrC responseText = iClient->ResponseText();
	_LIT( KText, "    responseText == %S" );
	iLogger->Log( KText, &responseText );
	
	return responseText;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetResponseStatus
// Getting the response status MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetResponseStatus()
	{
	_LIT( KFunc, "GetResponseStatus:" );
	iLogger->Log( KFunc );
	
	// Get the response status
	TInt responseStatus = iClient->ResponseStatus();
	_LIT( KStatus, "    responseStatus == %d" );
	iLogger->Log( KStatus, responseStatus );
	
	return responseStatus;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetNumberOfPreviousSenders
// Getting the number of previous senders
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetNumberOfPreviousSenders()
	{
	_LIT( KFunc, "GetNumberOfPreviousSenders:" );
	iLogger->Log( KFunc );
	
	// Get the number of previous senders
	TInt senders = iClient->NumberOfPreviousSenders();
	_LIT( KSenders, "    senders == %d" );
	iLogger->Log( KSenders, senders );
	
	return senders;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetPreviousSender
// Getting the address of a previous sender
// -----------------------------------------------------------------------------
//
TPtrC CMMSClientMTMBCTesterController::GetPreviousSender( TInt aIndex )
	{
	_LIT( KFunc, "GetPreviousSender:" );
	_LIT( KParam, "    aIndex == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aIndex );
	
	// Get the address of the previous sender at the given index
	TPtrC sender = iClient->PreviousSender( aIndex );
	_LIT( KSender, "    sender == %S" );
	iLogger->Log( KSender, &sender );
	
	return sender;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetPreviousSendingDate
// Getting the date and time of a previous sending
// -----------------------------------------------------------------------------
//
TTime CMMSClientMTMBCTesterController::GetPreviousSendingDate( TInt aIndex )
	{
	_LIT( KFunc, "GetPreviousSendingDate:" );
	_LIT( KParam, "    aIndex == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aIndex );
	
	// Get the sending date of the previous sending at the given index
	TTime sendingDate = iClient->PreviousSendingDate( aIndex );
	_LIT( KSendingDate, "    sendingDate == %d" );
	iLogger->Log( KSendingDate, sendingDate.Int64() );
	
	return sendingDate;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetReceiveTime
// Getting the receive time MMS header
// -----------------------------------------------------------------------------
//
TTime CMMSClientMTMBCTesterController::GetReceiveTime()
	{
	_LIT( KFunc, "GetReceiveTime:" );
	iLogger->Log( KFunc );
	
	// Get the receive time
	TTime receiveTime = iClient->MessageReceiveTime();
	_LIT( KTime, "    receiveTime == %d" );
	iLogger->Log( KTime, receiveTime.Int64() );
	
	return receiveTime;
	}

// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetTransferSize
// Getting the message transfer size MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetTransferSize()
	{
	_LIT( KFunc, "GetTransferSize:" );
	iLogger->Log( KFunc );
	
	// Get the message transfer size
	TInt size = iClient->MessageTransferSize();
	_LIT( KSize, "    size == %d" );
	iLogger->Log( KSize, size );
	
	return size;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetContentLocation
// Getting the content location MMS header
// -----------------------------------------------------------------------------
//
TPtrC8 CMMSClientMTMBCTesterController::GetContentLocation()
	{
	_LIT( KFunc, "GetContentLocation:" );
	iLogger->Log( KFunc );
	
	// Get the content location
	TPtrC8 location = iClient->MessageContentLocation();
	_LIT( KLocation, "    location == %S" );
	iLogger->Log( KLocation, &location );
	
	return location;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMessageRootPart
// Setting the message root part MMS header
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SetMessageRootPart(
	TMsvAttachmentId aRoot )
	{
	_LIT( KFunc, "SetMessageRootPart:" );
	_LIT( KParam, "    aRoot == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aRoot );	
	
	// Attempt to set the message root MMS header
	TRAPD ( result, iClient->SetMessageRootL( aRoot ) );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessageRootPart
// Getting the message root part MMS header
// -----------------------------------------------------------------------------
//
TMsvAttachmentId CMMSClientMTMBCTesterController::GetMessageRootPart()
	{
	_LIT( KFunc, "GetMessageRootPart:" );
	iLogger->Log( KFunc );
	
	// Get the message root
	TMsvAttachmentId root = iClient->MessageRootAttachment();
	_LIT( KRoot, "    root == %d" );
	iLogger->Log( KRoot, root );
	
	return root;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMaximumImageSize
// Setting the maximum image size MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SetMaximumImageSize( TInt aWidth,
                                                           TInt aHeight )
	{
	_LIT( KFunc, "SetMaximumImageSize:" );
	_LIT( KParam1, "    aWidth == %d" );
	_LIT( KParam2, "    aHeight == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, aWidth );
	iLogger->Log( KParam2, aHeight );
	
	// Set the maximum image size according to the received values
	iClient->SetMaximumImage( aWidth, aHeight );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMaximumImageSize
// Getting the maximum image size MMS header
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::GetMaximumImageSize( TInt& aWidth,
                                                           TInt& aHeight )
	{
	_LIT( KFunc, "GetMaximumImageSize:" );
	iLogger->Log( KFunc );
	
	// Get the maximum image size and set the out-parameters
	iClient->GetMaximumImage( aWidth, aHeight );
	_LIT( KWidth, "    aWidth == %d" );
	_LIT( KHeight, "    aHeight == %d" );
	iLogger->Log( KWidth, aWidth );
	iLogger->Log( KHeight, aHeight );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessageSize
// Getting the message size
// -----------------------------------------------------------------------------
//
TInt32 CMMSClientMTMBCTesterController::GetMessageSize()
	{
	_LIT( KFunc, "GetMessageSize:" );
	iLogger->Log( KFunc );
	
	// Get the message size
	TInt32 messageSize = iClient->MessageSize();
	_LIT( KSize, "    messageSize == %d" );
	iLogger->Log( KSize, messageSize );
	
	return messageSize;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMessageDescription
// Setting the message description
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SetMessageDescription(
	const TDesC& aDescription )
	{
	_LIT( KFunc, "SetMessageDescription:" );
	_LIT( KParam, "    aDescription == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aDescription );
	
	// Attempt to set the message description
	TRAPD ( result, iClient->SetMessageDescriptionL( aDescription ) );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddNewAttribute
// Adding a new attribute
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddNewAttribute( const TDesC& aName,
                                                       const TDesC& aValue )
	{
	_LIT( KFunc, "AddNewAttribute:" );
	_LIT( KParam1, "    aName == %S" );
	_LIT( KParam2, "    aValue == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aName );
	iLogger->Log( KParam2, &aValue );
	
	// Attempt to add a new attribute according to the given parameters
	TRAPD ( result, iClient->AddAttributeL( aName, aValue ) );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetAttribute
// Getting an attribute
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetAttribute( const TDesC& aName,
                                                    TPtrC& aValue )
	{
	_LIT( KFunc, "GetAttribute:" );
	_LIT( KParam, "    aName == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aName );
	
	// Attempt to get the given attribute's value
	TRAPD ( result, aValue.Set( iClient->GetAttributeL( aName ) ) );
	iLogger->Log( KResult, result );
	if ( result == KErrNone )
		{
		_LIT( KValue, "    value == %S" );
		iLogger->Log( KValue, &aValue );		
		}

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::FindAttribute
// Checking if an attribute exists
// -----------------------------------------------------------------------------
//
TBool CMMSClientMTMBCTesterController::FindAttribute( const TDesC& aName )
	{
	_LIT( KFunc, "FindAttribute:" );
	_LIT( KParam, "    aName == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aName );
	
	// Check if the attribute exists
	TBool result = iClient->FindAttribute( aName );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::DeleteAttribute
// Deleting an attribute
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::DeleteAttribute( const TDesC& aName )
	{
	_LIT( KFunc, "DeleteAttribute:" );
	_LIT( KParam, "    aName == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aName );
	
	// Delete the attribute
	iClient->DeleteAttribute( aName );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ResetAttributes
// Resetting all attributes
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::ResetAttributes()
	{
	_LIT( KFunc, "ResetAttributes" );
	iLogger->Log( KFunc );

	// Reset the attributes
	iClient->ResetAttributes();
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAttachmentSyncL
// Adding an attachment synchronously
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddAttachmentSyncL(
	const TDesC& aFileType, const TDesC& aFilePath,
	TMMSClientMTMBCTesterAttaType aType )
	{
	_LIT( KFunc, "AddAttachmentSyncL:" );
	_LIT( KParam1, "    aFileType == %S" );
	_LIT( KParam2, "    aFilePath == %S" );
	_LIT( KParam3, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aFileType );
	iLogger->Log( KParam2, &aFilePath );
	iLogger->Log( KParam3, aType );
	
	// Get access to the message store
	CMsvStore* store = iClient->Entry().EditStoreL();
	CleanupStack::PushL( store );
	
	// Variables for locating the file which is to be attached
	RFile attaFile;
	TFileName filePath( aFilePath );
	
	// Initialize needed MIME headers
	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
	CleanupStack::PushL( mimeHeaders );
	TPtrC8 fileType( reinterpret_cast< const TUint8* >( aFileType.Ptr() ),
		( aFileType.Size() * 2 ) );
	TInt pos = fileType.Locate( '/' );
	mimeHeaders->SetContentTypeL( fileType.Left( pos ) );
	mimeHeaders->SetContentSubTypeL( fileType.Mid( pos + 1 ) );
	
	// Connect to the file which is to be attached
	RFs fs;	
	User::LeaveIfError( fs.Connect() );
	User::LeaveIfError( attaFile.Open( fs, filePath,
	                                   EFileShareReadersOnly | EFileRead ) );
	CleanupClosePushL( attaFile );

	// Variables for attachment info and id
	CMsvAttachment* attaInfo = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
	TMsvAttachmentId attaId = 0;

	// Attempt to create the attachment
	iClient->CreateAttachment2L( *store, attaFile, fileType, *mimeHeaders,
	                             attaInfo, attaId );

	// Cleaning up
	attaInfo = NULL; // Attachment manager owns attaInfo now
	CleanupStack::PopAndDestroy(); // attaFile.Close()
	CleanupStack::PopAndDestroy(); // mimeHeaders
	store->CommitL(); // Store must be committed before it is destroyed
	CleanupStack::PopAndDestroy(); // store

	// Return the attachment id of the new attachment
	_LIT( KAttaId, "    attachment id == %d" );	
	iLogger->Log( KAttaId, attaId );
	return attaId;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddTextAttachment
// Adding a text attachment synchronously
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddTextAttachmentL( const TDesC& aText,
	const TDesC& aFileName, TMMSClientMTMBCTesterAttaType aType )
	{
	_LIT( KFunc, "AddTextAttachmentL:" );
	_LIT( KParam1, "    aText == %S" );
	_LIT( KParam2, "    aFileName == %S" );
	_LIT( KParam3, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aText );
	iLogger->Log( KParam2, &aFileName );
	iLogger->Log( KParam3, aType );
	
	// Name for the attachment file which is to be created
	TFileName fileName( aFileName );
	
	// Get access to the message store
	CMsvStore* store = iClient->Entry().EditStoreL();
	CleanupStack::PushL( store );
	
	// Variable for the attachment id
	TMsvAttachmentId attaId = 0;
	
	// Attempt to create the text attachment
	iClient->CreateTextAttachmentL( *store, attaId, aText, fileName );

	// Cleanup
	store->CommitL(); // Store must be committed before it is destroyed
	CleanupStack::PopAndDestroy(); // store

	_LIT( KAttaId, "    attachment id == %d" );	
	iLogger->Log( KAttaId, attaId );
	return attaId;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SendMessage
// Sending a message
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SendMessageL()
	{
	_LIT( KFunc, "SendMessageL:" );
	iLogger->Log( KFunc );
	
	// Initiate the sending operation
	CMsvOperationActiveSchedulerWait* wait =
		CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD ( error, iClient->SendL( wait->iStatus ) );
	iLogger->Log( KInitialResult, error );

    // Check if the message sending has successfully begun
	User::LeaveIfError( error );

	// Start the sending operation
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );

	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy(); // wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SendMessageSelection
// Sending a selection of messages using default service settings
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::SendMessageSelectionL( TMsvId aFolder )
	{
	_LIT( KFunc, "SendMessageSelectionL:" );
	_LIT( KParam, "    aFolder == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aFolder );
	
	// Form the message selection
	CMsvEntry* cEntry = iSession->GetEntryL( aFolder );
    CleanupStack::PushL( cEntry );
    CMsvEntrySelection* selection = NULL;
    selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PopAndDestroy(); // cEntry
    _LIT( KSelectionCount, "    selection.Count == %d" );
    iLogger->Log( KSelectionCount, selection->Count() );
    CleanupStack::PushL( selection );
	
	// Initiate the sending operation
	CMsvOperationActiveSchedulerWait* wait =
		CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD ( error, iClient->SendL( *selection, wait->iStatus ) );
	iLogger->Log( KInitialResult, error );

    // Check if the message sending has successfully begun
	User::LeaveIfError( error );

	// Start the sending operation
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );

	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy( 2 ); // wait, selection
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::FetchAllMessagesL
// Fetching all available messages to inbox
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::FetchAllMessagesL()
	{
	_LIT( KFunc, "FetchAllMessagesL:" );
	iLogger->Log( KFunc );
	
	// Initiate the fetching operation
	CMsvOperationActiveSchedulerWait* wait =
		CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD ( error, iClient->FetchAllL( wait->iStatus, ETrue ) );
	iLogger->Log( KInitialResult, error );

    // Check if the fetching has successfully begun
	User::LeaveIfError( error );

	// Start the sending operation
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );
	
	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy(); // wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetType
// Getting the type of the MTM
// -----------------------------------------------------------------------------
//
TUid CMMSClientMTMBCTesterController::GetType()
	{
	_LIT( KFunc, "GetType:" );
	iLogger->Log( KFunc );
	
	// Get the MTM type
	TUid type = iClient->Type();
	_LIT( KType, "    type == %d" );
	iLogger->Log( KType, type );
	
	return type;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetContext
// Setting the context to the given entry
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SetContext( CMsvEntry* aEntry )
	{
	_LIT( KFunc, "SetContext:" );
	iLogger->Log( KFunc );
	TMsvId entryId = aEntry->EntryId();
	_LIT( KId, "    aEntry.id == %d" );
	iLogger->Log( KId, entryId );
	
	// Attempt to set the context
	TRAPD ( result, iClient->SetCurrentEntryL( aEntry ) );
	iLogger->Log( KResult, result );

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SwitchContext
// Switching the context to the given entry
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SwitchContext( TMsvId aEntry )
	{
	_LIT( KFunc, "SwitchContext:" );
	_LIT( KParam, "    aEntry == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aEntry );
	
	// Attempt to switch the context
	TRAPD ( result, iClient->SwitchCurrentEntryL( aEntry ) );
	iLogger->Log( KResult, result );

	return result;		
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::HasContext
// Check if the context has been set
// -----------------------------------------------------------------------------
//
TBool CMMSClientMTMBCTesterController::HasContext()
	{
	_LIT( KFunc, "HasContext:" );
	iLogger->Log( KFunc );
	
	// Check for the context
	TBool result = iClient->HasContext();
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetCurrentEntry
// Getting the current entry
// -----------------------------------------------------------------------------
//
CMsvEntry& CMMSClientMTMBCTesterController::GetCurrentEntry()
	{
	_LIT( KFunc, "GetCurrentEntry" );
	iLogger->Log( KFunc );
	
	// Get and return a reference to the current entry
	return iClient->Entry();
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetEntry
// Getting an entry with the specified id
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetEntry( TMsvId aEntryId,
                                                CMsvEntry*& aEntry )
	{
	_LIT( KFunc, "GetEntry:" );
	_LIT( KParam, "    aEntryId == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aEntryId );
	
	// Attempt to get the entry with the wanted id
	TRAPD ( result, aEntry = iSession->GetEntryL( aEntryId ) );
	iLogger->Log( KResult, result );
	
	// If something went wrong, set the entry value to NULL just in case
	if ( result != KErrNone )
		{
		aEntry = NULL;
		}
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SaveMessage
// Saving a message (storing current entry data)
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SaveMessage()
	{
	_LIT( KFunc, "SaveMessage:" );
	iLogger->Log( KFunc );

	// Attempt to save the message
	TRAPD ( result, iClient->SaveMessageL() );
	iLogger->Log( KResult, result );
	
	// If the saving was successful, make the message visible
	if ( result == KErrNone )
		{
		TMsvEntry indexEntry = iClient->Entry().Entry();
		indexEntry.SetInPreparation( EFalse );
		indexEntry.SetVisible( ETrue );
		indexEntry.iDate.HomeTime();
		TRAPD( error, iClient->Entry().ChangeL( indexEntry ) );
		if ( error != KErrNone )
			{
			return error;
			}
		}

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::LoadMessage
// Loading a message (restoring current entry data)
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::LoadMessage()
	{
	_LIT( KFunc, "LoadMessage:" );
	iLogger->Log( KFunc );

	// Attempt to load the message
	TRAPD ( result, iClient->LoadMessageL() );
	iLogger->Log( KResult, result );

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ValidateMessage
// Validating selected parts of the current message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::ValidateMessage( TMsvPartList aPartList )
	{
	_LIT( KFunc, "ValidateMessage:" );
	iLogger->Log( KFunc );
	
	// Attempt to validate the message
	TInt result = iClient->ValidateMessage( aPartList );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SearchForText
// Searching for the given text in specified parts of the current message
// -----------------------------------------------------------------------------
//
TMsvPartList CMMSClientMTMBCTesterController::SearchForText( const TDesC& aText,
	TMsvPartList aPartList )
	{
	_LIT( KFunc, "SearchForText:" );
	_LIT( KParam, "    aText == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aText );
	
	// Call for the search function and return the results
	return iClient->Find( aText, aPartList );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::CreateReplyL
// Creating a reply message to the current message
// -----------------------------------------------------------------------------
//
TMsvId CMMSClientMTMBCTesterController::CreateReplyL( TMsvId aDestination )
	{
	_LIT( KFunc, "CreateReplyL:" );
	_LIT( KParam, "    aDestination == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aDestination );
	
	// Create the part list
	TMsvPartList partList = KMsvMessagePartOriginator |
	                        KMsvMessagePartDescription;
	
	// Begin to create a new message entry
	CMsvOperation* operation = NULL;
	CMsvOperationActiveSchedulerWait* wait =
		CMsvOperationActiveSchedulerWait::NewLC();
	operation = iClient->ReplyL( aDestination, partList, wait->iStatus );
	CleanupStack::PushL( operation );

	// Start the operation
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KResult, result );
    User::LeaveIfError( result );
	
	// Get the message id of the newly created message
	TPckgBuf< TMsvId > pkg;
	pkg.Copy( operation->ProgressL() );
	TMsvId id = 0;
	id = pkg();
	_LIT( KMsgId, "    message id == %d" );
	iLogger->Log( KMsgId, id );

	// Clean up and return the determined id	
	CleanupStack::PopAndDestroy( 2 ); // operation, wait
	return id;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::CreateForwardL
// Creating a message to be forwarded
// -----------------------------------------------------------------------------
//
TMsvId CMMSClientMTMBCTesterController::CreateForwardL( TMsvId aDestination )
	{
	_LIT( KFunc, "CreateForwardL:" );
	_LIT( KParam, "    aDestination == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aDestination );
	
	// Create the part list
	TMsvPartList partList = KMsvMessagePartNone;
	
	// Begin to create a new message entry
	CMsvOperation* operation = NULL;
	CMsvOperationActiveSchedulerWait* wait =
		CMsvOperationActiveSchedulerWait::NewLC();
	operation = iClient->ForwardL( aDestination, partList, wait->iStatus );
	CleanupStack::PushL( operation );

	// Start the operation
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KResult, result );
    User::LeaveIfError( result );
	
	// Get the message id of the newly created message
	TPckgBuf< TMsvId > pkg;
	pkg.Copy( operation->ProgressL() );
	TMsvId id = 0;
	id = pkg(); 
	_LIT( KMsgId, "    message id == %d" );
	iLogger->Log( KMsgId, id );

	// Clean up and return the determined id	
	CleanupStack::PopAndDestroy( 2 ); // operation, wait
	return id;		
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAddressee
// Adding an addressee for the message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddAddressee( const TDesC& aAddressee )
	{
	_LIT( KFunc, "AddAddressee:" );
	_LIT( KParam, "    aAddressee == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aAddressee );
	
	// Attempt to add the given addressee for the current message
	TRAPD ( result, iClient->AddAddresseeL( aAddressee ) );
	iLogger->Log( KResult, result );

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAddressee [2]
// Adding an addressee with an alias for the message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddAddressee( const TDesC& aAddressee,
                                                    const TDesC& aAlias )
	{
	_LIT( KFunc, "AddAddressee:" );
	_LIT( KParam1, "    aAddressee == %S" );
	_LIT( KParam2, "    aAlias == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aAddressee );
	iLogger->Log( KParam2, &aAlias );
	
	// Attempt to add the given addressee to the current message
	TRAPD ( result, iClient->AddAddresseeL( aAddressee, aAlias ) );
	iLogger->Log( KResult, result );

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAddressee [3]
// Adding a typed addressee for the message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddAddressee(
	TMsvRecipientType aRecipientType, const TDesC& aAddressee )
	{
	_LIT( KFunc, "AddAddressee:" );
	_LIT( KParam1, "    aRecipientType == %d" );
	_LIT( KParam2, "    aAddressee == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, aRecipientType );
	iLogger->Log( KParam2, &aAddressee );
	
	// Attempt to add the given addressee to the current message
	TRAPD ( result, iClient->AddAddresseeL( aRecipientType, aAddressee ) );
	iLogger->Log( KResult, result );

	return result;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAddressee [4]
// Adding a typed addressee with an alias for the message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddAddressee(
	TMsvRecipientType aRecipientType, const TDesC& aAddressee,
	const TDesC& aAlias )
	{
	_LIT( KFunc, "AddAddressee:" );
	_LIT( KParam1, "    aRecipientType == %d" );
	_LIT( KParam2, "    aAddressee == %S" );
	_LIT( KParam3, "    aAlias == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, aRecipientType );
	iLogger->Log( KParam2, &aAddressee );
	iLogger->Log( KParam3, &aAlias );
	
	// Attempt to add the given addressee to the current message
	TRAPD ( result, iClient->AddAddresseeL( aRecipientType, aAddressee,
	                                        aAlias ) );
	iLogger->Log( KResult, result );

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::RemoveAddressee
// Removing an addressee from the message
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::RemoveAddressee( TInt aIndex )
	{
	_LIT( KFunc, "RemoveAddressee:" );
	_LIT( KParam, "    aIndex == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aIndex );
	
	// Attempt to remove the addressee from the given position
	iClient->RemoveAddressee( aIndex );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessageBody
// Getting access to the message body
// -----------------------------------------------------------------------------
//
CRichText& CMMSClientMTMBCTesterController::GetMessageBody()
	{
	_LIT( KFunc, "GetMessageBody" );
	iLogger->Log( KFunc );
	
	return iClient->Body();
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessageBody [2]
// Getting access to the message body, const version
// -----------------------------------------------------------------------------
//
const CRichText& CMMSClientMTMBCTesterController::GetMessageBodyConst()
	{
	_LIT( KFunc, "GetMessageBody" );
	iLogger->Log( KFunc );
	
	return iClient->Body();
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::SetMessageSubject
// Setting the message subject
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::SetMessageSubject( const TDesC& aSubject )
	{
	_LIT( KFunc, "SetMessageSubject:" );
	_LIT( KParam, "    aSubject == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aSubject );
	
	// Try to set the message subject according to the received parameter
	TRAPD ( result, iClient->SetSubjectL( aSubject ) );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetMessageSubject
// Getting the message subject
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::GetMessageSubject( TPtrC& aSubject )
	{
	_LIT( KFunc, "GetMessageSubject:" );
	iLogger->Log( KFunc );

	// Attempt to get the message subject
	TRAPD ( result, aSubject.Set( iClient->SubjectL() ) );
	iLogger->Log( KResult, result );
	if ( result == KErrNone )
		{
		_LIT( KSubject, "    subject == %S" );
		iLogger->Log( KSubject, &aSubject );		
		}
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::QueryCapability
// Asking for the existence and the value of a capability
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::QueryCapability( TUid aCapability,
                                                       TInt& aResponse )
	{
	_LIT( KFunc, "QueryCapability:" );
	_LIT( KParam, "    aCapability == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aCapability );
	
	// Query regarding the capability in question
	TInt result = iClient->QueryCapability( aCapability, aResponse );
	iLogger->Log( KResult, result );
	if ( result == KErrNone )
		{
		_LIT( KResponse, "    response == %d" );
		iLogger->Log( KResponse, aResponse );
		}

	return result;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::InvokeSyncFunctionL
// Invoking a synchronous function
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::InvokeSyncFunctionL( TInt aFunction,
	const CMsvEntrySelection& aEntries )
	{
	_LIT( KFunc, "InvokeSyncFunctionL:" );
	_LIT( KParam1, "    aFunction == %d" );
	_LIT( KParam2, "    aEntries.Count == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, aFunction );
	iLogger->Log( KParam2, aEntries.Count() );
	
	// Attempt to invoke the requested function
	TPtr8 descriptor( NULL, 0 );
	TRAPD ( result,
	        iClient->InvokeSyncFunctionL( aFunction, aEntries, descriptor ) );
	iLogger->Log( KResult, result );
	
	// Leave if something went wrong
	User::LeaveIfError( result );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::InvokeAsyncFunctionL
// Invoking an asynchronous function
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::InvokeAsyncFunctionL( TInt aFunction,
	const CMsvEntrySelection& aEntries )
	{
	_LIT( KFunc, "InvokeAsyncFunctionL:" );
	_LIT( KParam1, "    aFunction == %d" );
	_LIT( KParam2, "    aEntries.Count == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, aFunction );
	iLogger->Log( KParam2, aEntries.Count() );
	
	// Attempt to invoke the requested function
	TPtr8 descriptor( NULL, 0 );
	CMsvOperation* operation = NULL;
	CMsvOperationActiveSchedulerWait* wait = 
    	CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD ( error, operation = iClient->InvokeAsyncFunctionL( aFunction,
		aEntries, descriptor, wait->iStatus ) );
	iLogger->Log( KInitialResult, error );
	CleanupStack::PushL( operation );
	
	// Check if the operation has successfully begun
	User::LeaveIfError( error );

	// Start for real
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );
	
	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy( 2 ); // operation, wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::GetSession
// Getting access to the session
// -----------------------------------------------------------------------------
//
CMsvSession& CMMSClientMTMBCTesterController::GetSession()
	{
	_LIT( KFunc, "GetSession" );
	iLogger->Log( KFunc );
	
	// Get and return a reference to the session object
	return iClient->Session();
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAttachmentL
// Adding a file attachment by its file path to the current message
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::AddAttachmentL( const TDesC& aFileType,
	const TDesC& aFilePath, TMMSClientMTMBCTesterAttaType aType )
	{
	_LIT( KFunc, "AddAttachmentL:" );
	_LIT( KParam1, "    aFileType == %S" );
	_LIT( KParam2, "    aFilePath == %S" );
	_LIT( KParam3, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aFileType );
	iLogger->Log( KParam2, &aFilePath );
	iLogger->Log( KParam3, aType );
	
	TFileName filepath( aFilePath );	
	TPtrC8 fileType( reinterpret_cast< const TUint8* >( aFileType.Ptr() ),
		( aFileType.Size() * 2 ) );
	TUint charset = 0;

	// Initiate the adding operation
	CMsvOperationActiveSchedulerWait* wait =
    	CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD( error, iClient->AddAttachmentL( filepath, fileType, charset,
	                                       wait->iStatus) );
	iLogger->Log( KInitialResult, error );
	
	// Check if the adding has successfully begun
	User::LeaveIfError( error );
	
	// Cancel the operation if requested
	if ( aType == EMMSClientMTMBCTesterAttaTypeCancel )
		{
		iClient->CancelAttachmentOperation();
		}

    // Start the operation for real
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );

	// Leave if something went wrong
	User::Leave( result );
	CleanupStack::PopAndDestroy(); // wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAttachmentByHandleL
// Adding a file attachment by an open file handle
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::AddAttachmentByHandleL(
	const TDesC& aFileType, const TDesC& aFilePath,
	TMMSClientMTMBCTesterAttaType aType )
	{
	_LIT( KFunc, "AddAttachmentByHandleL:" );
	_LIT( KParam1, "    aFileType == %S" );
	_LIT( KParam2, "    aFilePath == %S" );
	_LIT( KParam3, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aFileType );
	iLogger->Log( KParam2, &aFilePath );
	iLogger->Log( KParam3, aType );

	// Initializing needed variables
	TFileName attaFile( aFilePath );
	RFile fileHandle;
	TPtrC8 fileType( reinterpret_cast< const TUint8* >( aFileType.Ptr() ),
		( aFileType.Size() * 2 ) );
	TUint charset = 0;

	// Connect to the file which is to be attached
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	User::LeaveIfError( fileHandle.Open( fs, attaFile,
	                                     EFileShareReadersOnly | EFileRead ) );
	CleanupClosePushL( fileHandle );

	// Initiate the adding operation
	CMsvOperationActiveSchedulerWait* wait = 
    	CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD ( error, iClient->AddAttachmentL( fileHandle, fileType, charset,
	                                        wait->iStatus ) );
	iLogger->Log( KInitialResult, error );
	
	// Check if the adding has successfully begun
	User::LeaveIfError( error );

	// Start for real
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );
	
	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy(); // wait
	CleanupStack::Pop(); // The file handle was closed if all went well
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAttachmentAsLinkedL
// Adding a file attachment as a linked file
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::AddAttachmentAsLinkedL(
	const TDesC& aFileType, const TDesC& aFilePath,
	TMMSClientMTMBCTesterAttaType aType )
	{
	_LIT( KFunc, "AddAttachmentAsLinkedL:" );
	_LIT( KParam1, "    aFileType == %S" );
	_LIT( KParam2, "    aFilePath == %S" );
	_LIT( KParam3, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aFileType );
	iLogger->Log( KParam2, &aFilePath );
	iLogger->Log( KParam3, aType );
	
	TFileName filepath( aFilePath );
	TPtrC8 fileType( reinterpret_cast< const TUint8* >( aFileType.Ptr() ),
		( aFileType.Size() * 2 ) );
	TUint charset = 0;

	// Initiate the adding operation
	CMsvOperationActiveSchedulerWait* wait = 
    	CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD( error, iClient->AddLinkedAttachmentL( filepath, fileType, charset,
	                                             wait->iStatus) );
	iLogger->Log( KInitialResult, error );
	
	// Check if the adding has successfully begun
	User::LeaveIfError( error );

	// Start for real
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );

	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy(); // wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAttachmentMsgEntryL
// Adding a message entry as an attachment
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::AddAttachmentMsgEntryL(
	TMsvId aAttachment )
	{
	_LIT( KFunc, "AddAttachmentMsgEntryL:" );
	_LIT( KParam, "    aAttachment == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aAttachment );
	
	// Initiate the adding operation
	CMsvOperationActiveSchedulerWait* wait = 
    	CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD( error, iClient->AddEntryAsAttachmentL( aAttachment,
	                                              wait->iStatus ) );
	iLogger->Log( KInitialResult, error );
	
	// Check if the adding has successfully begun
	User::LeaveIfError( error );

	// Start for real
	wait->Start();

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );

	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy(); // wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddAttachmentAsyncL
// Adding an attachment asynchronously
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::AddAttachmentAsyncL(
	const TDesC& aFileType, const TDesC& aFileName,
	TMMSClientMTMBCTesterAttaType aType )
	{
	_LIT( KFunc, "AddAttachmentAsyncL:" );
	_LIT( KParam1, "    aFileType == %S" );
	_LIT( KParam2, "    aFileName == %S" );
	_LIT( KParam3, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam1, &aFileType );
	iLogger->Log( KParam2, &aFileName );
	iLogger->Log( KParam3, aType );
	
	// Initializing needed variables
	TFileName attaFile( aFileName );
	RFile fileHandle;
	TPtrC8 fileType( reinterpret_cast< const TUint8* >( aFileType.Ptr() ),
		( aFileType.Size() * 2 ) );
	TUint charset = 0;

	// Initiate the adding operation
	CMsvOperationActiveSchedulerWait* wait = 
    	CMsvOperationActiveSchedulerWait::NewLC();
	TRAPD ( error, iClient->CreateAttachmentL( attaFile, fileHandle, fileType,
	                                           charset, wait->iStatus ) );
	iLogger->Log( KInitialResult, error );
	
	// Check if the adding has successfully begun
	User::LeaveIfError( error );

	// Start for real
	wait->Start();

	CleanupClosePushL(fileHandle);
	// This is where we could write data through the file handle...

	// Check the result
	TInt result = wait->iStatus.Int();
	iLogger->Log( KFinalResult, result );

	// Leave if something went wrong
	User::LeaveIfError( result );
	CleanupStack::PopAndDestroy(); // close the file handle
	CleanupStack::PopAndDestroy(); // wait
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::CreateMessage
// Creating a new empty message entry
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::CreateMessage( TMsvId aServiceId )
	{
	_LIT( KFunc, "CreateMessage:" );
	_LIT( KParam, "    aServiceId == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aServiceId );
	
	// Attempt to create the new message entry
	TRAPD ( result, iClient->CreateMessageL( aServiceId ) );
	iLogger->Log( KResult, result );

	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::BIOTypeChanged
// Notifying of a changed BIO type
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::BIOTypeChanged( TUid aType )
	{
	_LIT( KFunc, "BIOTypeChanged:" );
	_LIT( KParam, "    aType == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aType );
	
	// Notify the MMS client of the changed BIO type
	TRAPD ( result, iClient->BioTypeChangedL( aType ) );
	iLogger->Log( KResult, result );
	
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::RemoveDefaultService
// Removing the default service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::RemoveDefaultService()
	{
	_LIT( KFunc, "RemoveDefaultService:" );
	iLogger->Log( KFunc );
	
	// Attempt to remove the default service
	TRAPD ( result, iClient->RemoveDefaultServiceL() );
	iLogger->Log( KResult, result );
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ChangeDefaultService
// Changing the default service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::ChangeDefaultService( TMsvId& aService )
	{
	_LIT( KFunc, "ChangeDefaultService:" );
	_LIT( KParam, "    aService == %d" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, aService );
	
	// Attempt to change the default service
	TRAPD ( result, iClient->ChangeDefaultServiceL( aService ) );
	iLogger->Log( KResult, result );
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::AddresseeExists
// Determining if the given item is found in the addressee list
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTesterController::AddresseeExists( const TDesC& aItem )
	{
	_LIT( KFunc, "AddresseeExists:" );
	_LIT( KParam, "    aItem == %S" );
	iLogger->Log( KFunc );
	iLogger->Log( KParam, &aItem );
	
	// Get the addressee list
	const CMsvRecipientList& list = iClient->AddresseeList();
	
	// Check if the given item is found in the list
	TInt result = KErrNotFound;
	for ( TUint i = 0; i < list.Count(); ++i )
		{
		if ( list[ i ].Compare( aItem ) == 0 )
			{
			result = KErrNone;
			break;
			}
		}
	
	iLogger->Log( KResult, result );
	return result;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::Cleanup
// Cleaning up: removing all messages from all the folders in use
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::Cleanup()
	{
	_LIT( KFunc, "Cleanup:" );
	iLogger->Log( KFunc );
	
	// Clear the draft folder
	TRAPD ( result, ClearFolderL( KMsvDraftEntryId ) );
	_LIT( KFolder1, "    drafts: %d" );
	iLogger->Log( KFolder1, result );
	
	// Clear the outbox
	TRAPD ( result2, ClearFolderL( KMsvGlobalOutBoxIndexEntryId ) );
	_LIT( KFolder2, "    outbox: %d" );
	iLogger->Log( KFolder2, result2 );
	
	// Clear the inbox
	TRAPD ( result3, ClearFolderL( KMsvGlobalInBoxIndexEntryId ) );
	_LIT( KFolder3, "    inbox: %d" );
	iLogger->Log( KFolder3, result3 );
	
	// Clear the sent messages folder
	TRAPD ( result4, ClearFolderL( KMsvSentEntryId ) );
	_LIT( KFolder4, "    sent: %d" );
	iLogger->Log( KFolder4, result4 );
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ClearFolderL
// Removing all messages from the given folder
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::ClearFolderL( TMsvId aFolder )
	{
	// Get access to the wanted folder
    CMsvEntry* cEntry = NULL;
    cEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    cEntry->SetEntryL( aFolder );
    
    // Select the items that will be deleted
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone,
                                                 ETrue ) );
    CMsvEntrySelection* msvEntrySelection =
    	cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( msvEntrySelection );
    CMsvEntrySelection* selection = NULL;
    
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone,
                                                 ETrue ) );
    selection = cEntry->ChildrenWithMtmL( KUidMsgMMSNotification );
    CleanupStack::PushL( selection );
    if ( selection->Count() > 0 )
        {
        msvEntrySelection->AppendL( selection->Back( 0 ), selection->Count() );
        }
    CleanupStack::PopAndDestroy(); // selection

	// Delete the wanted item...
    TInt i;
    for ( i = 0; i < msvEntrySelection->Count(); ++i )
        {
        CMsvEntry* entry = iSession->GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        entry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping,
                                                    EMsvSortByNone, EFalse ) );

		// ...and its children
        CMsvEntrySelection* childSelection = entry->ChildrenL();
        CleanupStack::PushL( childSelection );
        for ( TInt j = 0; j < childSelection->Count(); j++)
            {
            entry->DeleteL( childSelection->At( j ) );
            }
        CleanupStack::PopAndDestroy(); // childSelection
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly( EFalse );
        entry->ChangeL( tEntry );
        
        cEntry->DeleteL( msvEntrySelection->At( i ) );
        CleanupStack::PopAndDestroy(); // entry
        }

    CleanupStack::PopAndDestroy( 2 ); // cEntry, msvEntrySelection
	}


// PRIVATE MEMBER FUNCTIONS


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::CMMSClientMTMBCTesterController
// First phase constructor
// -----------------------------------------------------------------------------
//
CMMSClientMTMBCTesterController::CMMSClientMTMBCTesterController()
	: iLogger( NULL ), iSession( NULL ), iRegistry( NULL ), iClient( NULL ),
	  iFileBuf( NULL )
	{
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTesterController::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CMMSClientMTMBCTesterController::ConstructL()
	{
	// Create the STIF Test Framework logger
    iLogger = CStifLogger::NewL( KMMSClientMTMBCTesterLogPath,
                                 KMMSClientMTMBCTesterLogFile );

	// Some settings needed in reading the configuration file
	iExtraSpace = KExtraSpace;
    iBufSize = KBasicBufferSize + iExtraSpace;

	// Initialize the MMS Client MTM
	iSession = CMsvSession::OpenSyncL( *this );
	iRegistry = CClientMtmRegistry::NewL( *iSession );
	iClient = (CMmsClientMtm *) iRegistry->NewMtmL( KUidMsgTypeMultimedia );
	_LIT( KInitialized, "MMS Client MTM has been initialized" );
	iLogger->Log( KInitialized );
	}


// End of file: MMSClientMTMBCTesterController.cpp
