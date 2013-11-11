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
* Description:         Test case implementations for MMSClientMTMBCTester
*
*/










// INCLUDE FILES
#include <e32math.h>
#include <msvids.h>
#include <mtmuids.h>
#include <mtmdef.h>
#include <txtrich.h>
#include <mmserrors.h>

/***
* CMmsSettings class can only be included if the internal MMS Server Settings
* API is available. See MMSClientMTMBCTester.mmp for instructions on taking
* the class into use.
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING
#include "mmssettings.h"
#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

#include "MMSClientMTMBCTester.h"
#include "MMSClientMTMBCTesterSettings.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CMMSClientMTMBCTester::Case ( const TInt aCaseNumber ) const 
	{
	// Array of test cases - to add new test cases, add new items to this array
    static TCaseInfoInternal const KCases[] =
        {
        ENTRY( "Creating a new message entry", CreateMessageEntry ),
        
        ENTRY( "Creating a new service entry", CreateServiceEntry ),
                
/***
* Test case that deals with CMmsSettings class can only be included and executed
* if the internal MMS Server Settings API is available.
* See MMSClientMTMBCTester.mmp for instructions on taking the case into use.
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING
        
        ENTRY( "Handling the MMS settings", HandleMMSSettings ),

#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

        ENTRY( "Storing MMS settings to disk", StoreMMSSettings ),
        ENTRY( "Restoring MMS settings from disk", RestoreMMSSettings ),
        ENTRY( "Restoring factory MMS settings", RestoreFactoryMMSSettings ),
        ENTRY( "Validating default service", ValidateDefaultService ),
        ENTRY( "Handling the sender", Sender),
        ENTRY( "Handling the message class", MessageClass ),
        ENTRY( "Handling the expiration interval", ExpirationInterval ),
        ENTRY( "Handling the expiration date", ExpirationDate ),
        ENTRY( "Handling the delivery time interval", DeliveryTimeInterval ),
        ENTRY( "Handling the delivery date", DeliveryDate ),
        ENTRY( "Handling the message priority", MessagePriority ),
        ENTRY( "Handling the sender visibility", SenderVisibility ),
        ENTRY( "Handling the delivery report request", DeliveryReportRequest ),
        ENTRY( "Handling the read reply request", ReadReplyRequest ),
        ENTRY( "Getting the sending date and time", SendingDate ),
        ENTRY( "Getting the response text", ResponseText ),
        ENTRY( "Getting the response status", ResponseStatus ),
        ENTRY( "Getting the number of previous senders",
               NumberOfPreviousSenders ),
        ENTRY( "Getting the address of a previous sender", PreviousSender ),
        ENTRY( "Getting a previous sending date", PreviousSendingDate ),
        ENTRY( "Getting the receive time", ReceiveTime ),
        ENTRY( "Getting the actual message size", TransferSize ),
        ENTRY( "Getting the location of the message", ContentLocation ),
        ENTRY( "Handling the message root part MMS header", MessageRootPart ),
        ENTRY( "Handling the maximum image size MMS header", MaximumImageSize ),
        
        ENTRY( "Getting the message size", GetMessageSize ),
        ENTRY( "Setting the message description", SetMessageDescription ),
        
        ENTRY( "Adding a new attribute", AddNewAttribute),
        ENTRY( "Getting an attribute", GetAttribute),
        ENTRY( "Checking if an attribute exists", FindAttribute ),
        ENTRY( "Deleting an attribute", DeleteAttribute ),
        ENTRY( "Resetting all attributes", ResetAttributes ),
        
        ENTRY( "Creating an attachment synchronously", CreateAttachmentSync ),
        ENTRY( "Creating a text attachment", CreateTextAttachment ),
        
        ENTRY( "Sending a message", SendMessage ),
        ENTRY( "Sending a selection of messages", SendMessageSelection ),
        ENTRY( "Fetching all available messages", FetchAllMessages ),
        
        ENTRY( "Getting the type of the MTM", GetMTMType ),
        
        ENTRY( "Setting the context", SetContext ),
        ENTRY( "Switching the context", SwitchContext ),
        ENTRY( "Checking if the context has been set", CheckContext ),
        
        ENTRY( "Restoring and storing current entry data", RestoreAndStore ),
        ENTRY( "Validating the current message", ValidateMessage ),
        ENTRY( "Searching for a specified text in the current message",
               SearchForText ),
        ENTRY( "Creating a reply message", CreateReply ),
        ENTRY( "Creating a forward message", CreateForward ),
        ENTRY( "Adding an addressee", AddAddressee ),
        ENTRY( "Adding an addressee with an alias", AddAddresseeAlias ),
        ENTRY( "Adding a typed addressee", AddAddresseeTyped ),
        ENTRY( "Adding a typed addressee with an alias",
               AddAddresseeAliasTyped ),
        ENTRY( "Removing an addressee", RemoveAddressee ),
        ENTRY( "Accessing the message body", AccessMessageBody ),
        ENTRY( "Accessing the message body, const version",
               AccessMessageBodyConst ),
        ENTRY( "Handling the message subject", HandleMessageSubject ),
        
        ENTRY( "Querying for a capability", QueryCapability ),
        ENTRY( "Invoking a synchronous function", InvokeSynchronousFunction ),
        ENTRY( "Invoking an asynchronous function",
               InvokeAsynchronousFunction ),
        ENTRY( "Getting access to the session", GetSession ),
        
        ENTRY( "Adding a file attachment by file path", AddAttachmentFilePath ),
        ENTRY( "Adding a file attachment by an open file handle",
               AddAttachmentOpenHandle ),
        ENTRY( "Adding a file attachment as a linked file",
               AddAttachmentLinkedFile ),
        ENTRY( "Adding a message entry as an attachment",
               AddAttachmentMessageEntry ),
        ENTRY( "Creating an attachment asynchronously", CreateAttachmentAsync ),
        ENTRY( "Canceling an attachment operation", CancelAttachmentOperation ),
        
        ENTRY( "Creating a new message", CreateMessage ),
        ENTRY( "Notifying of a BIO type change", NotifyBIOTypeChange ),
        ENTRY( "Removing the default service", RemoveDefaultService ),
        ENTRY( "Changing the default service", ChangeDefaultService ) 
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( ( const TText* ) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;
    }


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::HandleResult
// Reporting of a test case result
// -----------------------------------------------------------------------------
//
TBool CMMSClientMTMBCTester::HandleResult( const TInt aCaseNumber,
                                           const TInt aValue,
                                           const TDesC& aErrorMessage,
                                           TTestResult& aResult,
                                           TBool aFailOnly )
	{
	// Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );
    
    // Check the result of the test case execution and print information to the
    // log file and result record accordingly
    if ( aValue != KErrNone ) // The test case has failed
    	{    	
    	iLog->Log( KCaseFailed, &tmp.iCaseName );
    	aResult.SetResult( aValue, aErrorMessage );
    	return EFalse;
    	}
    else if ( !aFailOnly ) // The test case has passed
    	{
    	iLog->Log( KCasePassed, &tmp.iCaseName );
    	aResult.SetResult( KErrNone, KSuccess );    	
    	}
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::InitializeMessageEntry
// Creating a new message entry and initializing it - a common series of
// procedures used by many of the test cases
// -----------------------------------------------------------------------------
//
TBool CMMSClientMTMBCTester::InitializeMessageEntry( const TInt aCaseNumber,
                                                     TTestResult& aResult )
	{
	// Create a new message entry
	TMsvId messageId = 0;
	TRAPD ( result,
		messageId = iController->CreateNewMessageEntryL( KMsvDraftEntryId ) );
	TBool ok = HandleResult( aCaseNumber, result, KErrorCreateMessageEntry,
	                         aResult, ETrue );
	if ( !ok ) return EFalse;	
    
    // Switch the context to the new message entry
    result = iController->SwitchContext( messageId );
    ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
                       ETrue );
	if ( !ok ) return EFalse;
	
	return ETrue;
	}


// ======================= TEST CASE FUNCTIONS =================================


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateMessageEntry
// Creating a new message entry
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateMessageEntry( const TInt aCaseNumber,
                                                TTestResult& aResult )
	{
	// Attempt to create a new message entry
	TRAPD ( result, iController->CreateNewMessageEntryL( KMsvDraftEntryId ) );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorCreateMessageEntry, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateServiceEntry
// Creating a new service entry
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateServiceEntry( const TInt aCaseNumber,
                                                TTestResult& aResult )
	{
	// Attempt to create a new service entry
	TInt result = iController->CreateNewServiceEntry();
	
	// Accept also KErrAccessDenied as a valid return value
	if ( result == KErrAccessDenied )
		{
		result = KErrNone;
		}

	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorCreateServiceEntry, aResult );
	return KErrNone;
	}


/***
* Test case that deals with CMmsSettings class can only be included and executed
* if the internal MMS Server Settings API is available.
* See MMSClientMTMBCTester.mmp for instructions on taking the case into use.
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::HandleMMSSettings
// Handling the MMS settings
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::HandleMMSSettings( const TInt aCaseNumber,
                                               TTestResult& aResult )
	{
	// Get the MMS settings
	CMmsSettings* settings = NULL;
	TRAPD ( error, settings = iController->GetMMSSettingsL() );
	TBool ok = HandleResult( aCaseNumber, error, KErrorGetMMSSettings, aResult,
	                         ETrue );
	if ( !ok ) return KErrNone;
		
	// Now set the MMS settings
	TInt result = iController->SetMMSSettings( *settings );

	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorSetMMSSettings, aResult );
	delete settings;
	return KErrNone;
	}

#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::StoreMMSSettings
// Storing MMS settings to disk
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::StoreMMSSettings( const TInt aCaseNumber,
                                              TTestResult& aResult )
	{
	// Attempt to store the MMS settings to disk
	TInt result = iController->StoreMMSSettings();
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorStoreMMSSettings, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::RestoreMMSSettings
// Restoring MMS settings from disk
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::RestoreMMSSettings( const TInt aCaseNumber,
                                                TTestResult& aResult )
	{
	// Attempt to restore the MMS settings from disk
	TInt result = iController->RestoreMMSSettings();
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorRestoreMMSSettings, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::RestoreFactoryMMSSettings
// Restoring factory MMS settings
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::RestoreFactoryMMSSettings( const TInt aCaseNumber,
                                                       TTestResult& aResult )
	{
	// Attempt to restore the factory MMS settings
	TInt result = iController->RestoreFactoryMMSSettings();

	// Accept also KErrAccessDenied as a valid return value
	if ( result == KErrAccessDenied )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorRestoreFactoryMMSSettings,
	              aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ValidateDefaultService
// Validating default service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ValidateDefaultService( const TInt aCaseNumber,
                                                    TTestResult& aResult )
	{
	// Attempt to get the service id of the default service
	TMsvId defaultService;
	TInt result = iController->GetDefaultService( defaultService );
	TBool ok = HandleResult( aCaseNumber, result, KErrorGetDefaultService,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Validate the default service
	result = iController->ValidateService( defaultService );

	// Test case was executed
	if ( result <= MMS_ERROR_BASE && result >= MMS_ERROR_BASE - 6 )
		{
		result = KErrNone;
		}
	HandleResult( aCaseNumber, result, KErrorValidateService, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::Sender
// Handling the sender
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::Sender( const TInt aCaseNumber,
                                    TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the sender
	HBufC* sender = NULL;
	sender = iController->GetItemFromFile( KEmailAddress, sender );
	TInt result = iController->SetSender( *sender );
	ok = HandleResult( aCaseNumber, result, KErrorSetSender, aResult, ETrue );
	if ( !ok )
		{
		delete sender;
		return KErrNone;
		}
	
	// Get the sender
	TPtrC returnedSender = iController->GetSender();
	
	// Check that the correct sender was returned
	result = returnedSender.Compare( *sender );

	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorGetSender, aResult );
    delete sender;	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::MessageClass
// Handling the message class
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::MessageClass( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the message class
	iController->SetMessageClass( KMessageClass );
	
	// Get the message class
	TInt messageClass = iController->GetMessageClass();
	
	// Check that the correct message class was returned
	TInt result = KErrGeneral;
	if ( messageClass == KMessageClass )
		{
		result = KErrNone;
		}
		
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorMessageClass, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ExpirationInterval
// Handling the expiration interval
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ExpirationInterval( const TInt aCaseNumber,
                                                TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the expiration interval
	TTimeIntervalSeconds input( KTimeInterval );
	iController->SetExpirationInterval( input );
	
	// Get the expiration interval
	TTimeIntervalSeconds output = iController->GetExpirationInterval();
	
	// Check that the correct interval was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorExpirationInterval, aResult );	
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ExpirationDate
// Handling the expiration date
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ExpirationDate( const TInt aCaseNumber,
                                            TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the expiration date
	TTime input( KDate );
	iController->SetExpirationDate( input );
	
	// Get the expiration date
	TTime output = iController->GetExpirationDate();
	
	// Check that the correct time was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorExpirationDate, aResult );	
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::DeliveryTimeInterval
// Handling the delivery time interval
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::DeliveryTimeInterval( const TInt aCaseNumber,
                                                  TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the delivery time interval
	TTimeIntervalSeconds input( KTimeInterval );
	iController->SetDeliveryTimeInterval( input );
	
	// Get the delivery time interval
	TTimeIntervalSeconds output = iController->GetDeliveryTimeInterval();
	
	// Check that the correct interval was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorDeliveryTimeInterval, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::DeliveryDate
// Handling the delivery date
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::DeliveryDate( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the delivery date
	TTime input( KDate );
	iController->SetDeliveryDate( input );
	
	// Get the delivery date
	TTime output = iController->GetDeliveryDate();
	
	// Check that the correct date was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorDeliveryDate, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::MessagePriority
// Handling the message priority
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::MessagePriority( const TInt aCaseNumber,
                                      TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the message priority
	TMmsMessagePriority input = KMessagePriority;
	iController->SetMessagePriority( input );
	
	// Get the message priority
	TInt output = iController->GetMessagePriority();
	
	// Check that the correct priority was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorMessagePriority, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SenderVisibility
// Handling the sender visibility
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SenderVisibility( const TInt aCaseNumber,
                                              TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the sender visibility
	TMmsMessageSenderVisibility input = KSenderVisibility;
	iController->SetSenderVisibility( input );
	
	// Get the sender visibility
	TInt output = iController->GetSenderVisibility();
	
	// Check that the correct visibility was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorSenderVisibility, aResult );	
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::DeliveryReportRequest
// Handling the delivery report request
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::DeliveryReportRequest( const TInt aCaseNumber,
                                                   TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the delivery report request
	TMmsYesNo input = KRequest;
	iController->SetDeliveryReportRequest( input );
	
	// Get the delivery report request
	TInt output = iController->GetDeliveryReportRequest();
	
	// Check that the correct request value was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorDeliveryReportRequest, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ReadReplyRequest
// Handling the read reply request
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ReadReplyRequest( const TInt aCaseNumber,
                                              TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the delivery report request
	TMmsYesNo input = KRequest;
	iController->SetReadReplyRequest( input );
	
	// Get the delivery report request
	TInt output = iController->GetReadReplyRequest();
	
	// Check that the correct request value was returned
	TInt result = KErrGeneral;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorReadReplyRequest, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::GetSendingDate
// Getting the sending date and time
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SendingDate( const TInt aCaseNumber,
                                         TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the sending date and time
	TTime sent = iController->GetSendingDate();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetSendingDate, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ResponseText
// Getting the response text
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ResponseText( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the response text
	TPtrC responseText = iController->GetResponseText();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetResponseText, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ResponseStatus
// Getting the response status
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ResponseStatus( const TInt aCaseNumber,
                                            TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the response status
	TInt responseStatus = iController->GetResponseStatus();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetResponseStatus, aResult );
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::NumberOfPreviousSenders
// Getting the number of previous senders
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::NumberOfPreviousSenders( const TInt aCaseNumber,
                                                     TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the number of previous senders
	TInt senders = iController->GetNumberOfPreviousSenders();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetNumberOfPreviousSenders,
	              aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::PreviousSender    
// Getting the address of a previous sender
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::PreviousSender( const TInt aCaseNumber,
                                            TTestResult& aResult)
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the address of a previous sender
	TPtrC sender = iController->GetPreviousSender( 1 );
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetPreviousSender, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::PreviousSendingDate    
// Getting a previous sending date
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::PreviousSendingDate( const TInt aCaseNumber,
                                                 TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get a previous sending date
	TTime sendingDate = iController->GetPreviousSendingDate( 1 );
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetPreviousSendingDate,
	              aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ReceiveTime
// Getting the receive time
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ReceiveTime( const TInt aCaseNumber,
                                         TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the message receive time
	TTime receiveTime = iController->GetReceiveTime();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetReceiveTime, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::TransferSize
// Getting the actual message size
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::TransferSize( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the actual message size
	TInt size = iController->GetTransferSize();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetTransferSize, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ContentLocation
// Getting the location of the message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ContentLocation( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the location of the message
	TPtrC8 location = iController->GetContentLocation();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetContentLocation, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::MessageRootPart
// Handling the message root part
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::MessageRootPart( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry
	TMsvId messageId = 0;
	TRAPD ( result,
		messageId = iController->CreateNewMessageEntryL( KMsvDraftEntryId ) );
	TBool ok = HandleResult( aCaseNumber, result, KErrorCreateMessageEntry,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;	
    
    // Switch the context to the new message entry
    result = iController->SwitchContext( messageId );
    ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
                       ETrue );
	if ( !ok ) return KErrNone;
	
	// Set the message root part
	result = iController->SetMessageRootPart( messageId );
	ok = HandleResult( aCaseNumber, result, KErrorSetMessageRootPart, aResult,
	                   ETrue );
	
	// Get the message root part
	TMsvAttachmentId root = iController->GetMessageRootPart();
	
	// Check that the correct root part was returned
	result = KErrGeneral;
	if ( root == messageId )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorGetMessageRootPart, aResult );	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::MaximumImageSize
// Handling the maximum image size
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::MaximumImageSize( const TInt aCaseNumber,
                                              TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Set the maximum image size
	iController->SetMaximumImageSize( KImageWidth, KImageHeight );
	
	// Get the maximum image size
	TInt outputWidth = 0;
	TInt outputHeight = 0;
	iController->GetMaximumImageSize( outputWidth, outputHeight );
	
	// Check that the correct width and height values were returned
	TInt result = KErrGeneral;
	if ( outputWidth == KImageWidth && outputHeight == KImageHeight )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorMaximumImageSize, aResult );	
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::GetMessageSize
// Getting the message size
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::GetMessageSize( const TInt aCaseNumber,
                                            TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Get the message size
	TInt32 size = iController->GetMessageSize();

	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetMessageSize, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SetMessageDescription
// Setting the message description
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SetMessageDescription( const TInt aCaseNumber,
                                                   TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Attempt to set the message description
	HBufC* description = NULL;
	description = iController->GetItemFromFile( KMsgDescription, description );
	TInt result = iController->SetMessageDescription( *description );
	HandleResult( aCaseNumber, result, KErrorSetMessageDescription, aResult );
	delete description;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddNewAttribute
// Adding a new attribute
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddNewAttribute( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Attempt to add a new attribute
    HBufC* attrName = NULL;
    HBufC* attrValue = NULL;
    attrName = iController->GetItemFromFile( KAttributeName, attrName );
    attrValue = iController->GetItemFromFile( KAttributeValue, attrValue);
    TInt result = iController->AddNewAttribute( *attrName, *attrValue );
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorAddNewAttribute, aResult );
	delete attrValue;
	delete attrName;    
    return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::GetAttribute
// Getting an attribute
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::GetAttribute( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Add a new attribute
    HBufC* attrName = NULL;
    HBufC* attrValue = NULL;
    attrName = iController->GetItemFromFile( KAttributeName, attrName );
    attrValue = iController->GetItemFromFile( KAttributeValue, attrValue);
    TInt result = iController->AddNewAttribute( *attrName, *attrValue );   
    ok = HandleResult( aCaseNumber, result, KErrorAddNewAttribute, aResult,
                       ETrue );
	if ( !ok )
		{
		delete attrValue;
		delete attrName;
		return KErrNone;
		}
    
    // Attempt to get the added attribute's value
    TPtrC receivedValue;
    result = iController->GetAttribute( *attrName, receivedValue );
    
    // Verify the results
    if ( result != KErrNone || receivedValue.Compare( *attrValue ) != 0 )
    	{
    	result = KErrGeneral;
    	}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorGetAttribute, aResult );
    delete attrValue;
    delete attrName;
    return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::FindAttribute
// Checking if an attribute exists
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::FindAttribute( const TInt aCaseNumber,
                                           TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Add a new attribute
    HBufC* attrName = NULL;
    HBufC* attrValue = NULL;
    attrName = iController->GetItemFromFile( KAttributeName, attrName );
    attrValue = iController->GetItemFromFile( KAttributeValue, attrValue);
    TInt result = iController->AddNewAttribute( *attrName, *attrValue );   
    ok = HandleResult( aCaseNumber, result, KErrorAddNewAttribute, aResult,
                       ETrue );
	if ( !ok )
		{
		delete attrValue;
		delete attrName;
		return KErrNone;
		}
    
    // Check if the added attribute exists
    TBool exists = iController->FindAttribute( *attrName );
    
    // Verify the result: the attribute should have been found
    result = KErrNotFound;
    if ( exists )
    	{
    	result = KErrNone;
    	}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorFindAttribute, aResult );
    delete attrValue;
    delete attrName;
    return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::DeleteAttribute
// Deleting an attribute
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::DeleteAttribute( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Add a new attribute
    HBufC* attrName = NULL;
    HBufC* attrValue = NULL;
    attrName = iController->GetItemFromFile( KAttributeName, attrName );
    attrValue = iController->GetItemFromFile( KAttributeValue, attrValue);
    TInt result = iController->AddNewAttribute( *attrName, *attrValue );   
    ok = HandleResult( aCaseNumber, result, KErrorAddNewAttribute, aResult,
                       ETrue );
	if ( !ok )
		{
		delete attrValue;
		delete attrName;
		return KErrNone;
		}

	// Delete the added attribute
	iController->DeleteAttribute( *attrName );
	
	// Make sure that the attribute can no longer be found
	result = KErrGeneral;
	if ( !iController->FindAttribute( *attrName ) )
		{
		result = KErrNone;
		}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorDeleteAttribute, aResult );
    delete attrValue;
    delete attrName;
    return KErrNone;		
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ResetAttributes
// Resetting all attributes
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ResetAttributes( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Add a new attribute
    HBufC* attrName = NULL;
    HBufC* attrValue = NULL;
    attrName = iController->GetItemFromFile( KAttributeName, attrName );
    attrValue = iController->GetItemFromFile( KAttributeValue, attrValue);
    TInt result = iController->AddNewAttribute( *attrName, *attrValue );   
    ok = HandleResult( aCaseNumber, result, KErrorAddNewAttribute, aResult,
                       ETrue );
	if ( !ok )
		{
		delete attrValue;
		delete attrName;
		return KErrNone;
		}

	// Reset all attributes
	iController->ResetAttributes();
	
	// The previously added attribute should not be found now
	result = KErrGeneral;
	if ( !iController->FindAttribute( *attrName ) )
		{
		result = KErrNone;
		}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorResetAttributes, aResult );
    delete attrValue;
    delete attrName;
    return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateAttachmentSync
// Creating an attachment synchronously
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateAttachmentSync( const TInt aCaseNumber,
                                                  TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Attempt to create a new attachment
    HBufC* fileType = NULL;
    HBufC* filePath = NULL;
    fileType = iController->GetItemFromFile( KAttachmentType, fileType );
    filePath = iController->GetItemFromFile( KAttachmentFile, filePath );
    TRAPD ( error, iController->AddAttachmentSyncL( *fileType, *filePath,
    	EMMSClientMTMBCTesterAttaTypeNormal ) );
    
    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorAddAttachmentSync, aResult );
    delete fileType;
	delete filePath;        
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateTextAttachment
// Creating a text attachment
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateTextAttachment( const TInt aCaseNumber,
                                                  TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
    // Attempt to create a new text attachment
    HBufC* attaText = NULL;
    HBufC* attaFile = NULL;
    attaText = iController->GetItemFromFile( KTextAttachmentText, attaText );
    attaFile = iController->GetItemFromFile( KTextAttachmentName, attaFile );
    TRAPD ( error, iController->AddTextAttachmentL( *attaText, *attaFile,
    	EMMSClientMTMBCTesterAttaTypeNormal ) );	

    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorAddTextAttachment, aResult );
	delete attaFile;
	delete attaText;	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SendMessage
// Sending a message using default service settings
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SendMessage( const TInt aCaseNumber,
                                         TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	result = iController->AddAddressee( *recipient );
	ok = HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult,
	                   ETrue );
	if ( !ok )
		{
		delete recipient;
		return KErrNone;
		}

    // Save the message
    result = iController->SaveMessage();
    ok = HandleResult( aCaseNumber, result, KErrorSaveMessage, aResult, ETrue );
	if ( !ok )
		{
		delete recipient;
		return KErrNone;
		}
    
    // Attempt to send the message
    TRAPD ( error, iController->SendMessageL() );
    
    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorSendMessage, aResult );
    delete recipient;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SendMessageSelection
// Sending a selection of messages using default service settings
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SendMessageSelection( const TInt aCaseNumber,
		                                          TTestResult& aResult )
	{
	// Get the default service id
	TMsvId serviceId = 0;
	TInt result = iController->GetDefaultService( serviceId );
	TBool ok = HandleResult( aCaseNumber, result, KErrorGetDefaultService,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Clear the draft folder in orded to make sure we don't send more messages
	// than necessary
	TRAPD ( error, iController->ClearFolderL( KMsvDraftEntryId ) );
	ok = HandleResult( aCaseNumber, error, KErrorClearFolder, aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Create some messages to be sent
	HBufC* recipient = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	for ( TInt i = 0; i < KMessageSelectionSize; ++i )
		{
		// Switch the context to the draft folder
		result = iController->SwitchContext( KMsvDraftEntryId );
		ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
		                   ETrue );
		if ( !ok ) return KErrNone;
		
		// Create the message
		result = iController->CreateMessage( serviceId );
		ok = HandleResult( aCaseNumber, result, KErrorCreateMessage, aResult,
		                   ETrue );
		if ( !ok )
			{
			delete recipient;
			return KErrNone;
			}
		
		// Add an addressee
		result = iController->AddAddressee( *recipient );
		ok = HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult,
		                   ETrue );
		if ( !ok )
			{
			delete recipient;
			return KErrNone;
			}
		
		// Save the message
    	result = iController->SaveMessage();
    	ok = HandleResult( aCaseNumber, result, KErrorSaveMessage, aResult,
    	                   ETrue );
		if ( !ok )
			{
			delete recipient;
			return KErrNone;
			}
		}		
	
	// Attempt to send the messages
	TRAPD ( error2, iController->SendMessageSelectionL( KMsvDraftEntryId ) );
	
	// Test case was executed
    HandleResult( aCaseNumber, error2, KErrorSendMessageSelection, aResult );
	delete recipient;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::FetchAllMessages
// Fetching all available messages
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::FetchAllMessages( const TInt aCaseNumber,
                                              TTestResult& aResult )
	{
	TRAPD( result, iController->FetchAllMessagesL() );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorFetchAllMessages, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::GetMTMType
// Getting the type of the MTM
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::GetMTMType( const TInt aCaseNumber,
                                        TTestResult& aResult )
	{
	// Get the type of the MTM
	TUid type = iController->GetType();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetType, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SetContext
// Setting the context
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SetContext( const TInt aCaseNumber,
                                        TTestResult& aResult )
	{
	// Create a new message entry
	TMsvId messageId = 0;
	TRAPD( result,
		messageId = iController->CreateNewMessageEntryL( KMsvDraftEntryId ) );
	TBool ok = HandleResult( aCaseNumber, result, KErrorCreateMessageEntry,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Get the entry of the new message
	CMsvEntry* entry = NULL;
	result = iController->GetEntry( messageId, entry );
	ok = HandleResult( aCaseNumber, result, KErrorGetEntry, aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Set the context to the new message
	result = iController->SetContext( entry );
	if ( result == KErrNone )
		{
		// Check that the context is now set to the correct entry
    	TMsvId entryId = iController->GetCurrentEntry().EntryId();
    	result = KErrGeneral;
    	if ( entryId == messageId )
    		{
    		result = KErrNone;
    		}
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorSetContext, aResult );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SwitchContext
// Switching the context
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SwitchContext( const TInt aCaseNumber,
                                           TTestResult& aResult )
	{
	// Create a new message entry
	TMsvId messageId = 0;
	TRAPD( result,
		messageId = iController->CreateNewMessageEntryL( KMsvDraftEntryId ) );
	TBool ok = HandleResult( aCaseNumber, result, KErrorCreateMessageEntry,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;
    
    // Switch the context to the new message entry
    result = iController->SwitchContext( messageId );
    if ( result == KErrNone )
    	{
    	// Check that the context is now set to the correct entry
    	TMsvId entryId = iController->GetCurrentEntry().EntryId();
    	result = KErrGeneral;
    	if ( entryId == messageId )
    		{
    		result = KErrNone;
    		}
    	}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CheckContext
// Checking if the context has been set
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CheckContext( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry
	TMsvId messageId = 0;
	TRAPD( result,
		messageId = iController->CreateNewMessageEntryL( KMsvDraftEntryId ) );
	TBool ok = HandleResult( aCaseNumber, result, KErrorCreateMessageEntry,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;
    
    // Switch the context to the new message entry
    result = iController->SwitchContext( messageId );
    ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
                       ETrue );
	if ( !ok ) return KErrNone;
	
	// Check if the context has been set
	result = KErrNotFound;
	if ( iController->HasContext() )
		{
		result = KErrNone;
		}
	
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorHasContext, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::RestoreAndStore
// Restoring and storing current entry data
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::RestoreAndStore( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

    // Save the message
    result = iController->SaveMessage();
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorSaveMessage, aResult );
    return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ValidateMessage
// Validating the current message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ValidateMessage( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Try to validate the message
	TMsvPartList partList = KMsvMessagePartOriginator |
	                        KMsvMessagePartDescription;
	TInt result = iController->ValidateMessage( partList );
	
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorValidateMessage, aResult );
    return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::SearchForText
// Searching for a specified text in the current message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::SearchForText( const TInt aCaseNumber,
                                           TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
	
	// Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	recipient = iController->GetItemFromFile( KEmailAddress, recipient );
	result = iController->AddAddressee( *recipient );
	ok = HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult,
	                   ETrue );
	if ( !ok )
		{
		delete recipient;
		return KErrNone;
		}

    // Save the message
    result = iController->SaveMessage();
    ok = HandleResult( aCaseNumber, result, KErrorSaveMessage, aResult, ETrue );
	if ( !ok )
		{
		delete recipient;
		return KErrNone;
		}
	
	// Search for text
	TMsvPartList input = KMsvMessagePartRecipient;
	TMsvPartList output = iController->SearchForText( *recipient, input );
	
	// Verify the results
	result = KErrNotFound;
	if ( input == output )
		{
		result = KErrNone;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorSearchForText, aResult );
    delete recipient;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateReply
// Creating a reply message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateReply( const TInt aCaseNumber,
                                         TTestResult& aResult )
	{
	// Switch the context to the draft folder
	TInt result = iController->SwitchContext( KMsvDraftEntryId );
	TBool ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
	                         ETrue );
	if ( !ok ) return KErrNone;
	
	// Get the default service id
	TMsvId serviceId = 0;
	result = iController->GetDefaultService( serviceId );
	ok = HandleResult( aCaseNumber, result, KErrorGetDefaultService, aResult,
	                   ETrue );
	if ( !ok ) return KErrNone;
	
	// Create a new message
	result = iController->CreateMessage( serviceId );
	ok = HandleResult( aCaseNumber, result, KErrorCreateMessage, aResult,
	                   ETrue );
	if ( !ok ) return KErrNone;
    
    // Set the sender to the initial message
	HBufC* sender = NULL;
	sender = iController->GetItemFromFile( KPhoneNumber, sender );
	result = iController->SetSender( *sender );
	ok = HandleResult( aCaseNumber, result, KErrorSetSender, aResult, ETrue );
	if ( !ok )
		{
		delete sender;
		return KErrNone;
		}
    
    // Save the message
    result = iController->SaveMessage();
    ok = HandleResult( aCaseNumber, result, KErrorSaveMessage, aResult, ETrue );
	if ( !ok )
		{
		delete sender;
		return KErrNone;
		}
    
    // Attempt to create a reply message
    TMsvId replyId = 0;
    TRAPD ( error, replyId = iController->CreateReplyL( KMsvDraftEntryId ) );
    if ( error == KErrNone )
    	{
        // Switch the context to the reply message
        result = iController->SwitchContext( replyId );
        ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
                           ETrue );
	    if ( !ok )
	        {
	    	delete sender;
	    	return KErrNone;
	    	}
	    
	    // Load the new message
    	result = iController->LoadMessage();
    	ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
		if ( !ok )
			{
			delete sender;
			return KErrNone;
			}
    	
    	// Check that the reply message's recipient is the initial message's sender
		error = iController->AddresseeExists( *sender );
    	}
	
	// Test case was executed
	HandleResult( aCaseNumber, error, KErrorCreateReply, aResult );
    delete sender;
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateForward
// Creating a forward message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateForward( const TInt aCaseNumber,
                                           TTestResult& aResult )
	{
	// Switch the context to the draft folder
	TInt result = iController->SwitchContext( KMsvDraftEntryId );
	TBool ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
	                         ETrue );
	if ( !ok ) return KErrNone;
	
	// Get the default service id
	TMsvId serviceId = 0;
	result = iController->GetDefaultService( serviceId );
	ok = HandleResult( aCaseNumber, result, KErrorGetDefaultService, aResult,
	                   ETrue );
	if ( !ok ) return KErrNone;
	
	// Create a new message
	result = iController->CreateMessage( serviceId );
	ok = HandleResult( aCaseNumber, result, KErrorCreateMessage, aResult,
	                   ETrue );
	if ( !ok ) return KErrNone;
    
    // Save the message
    result = iController->SaveMessage();
    ok = HandleResult( aCaseNumber, result, KErrorSaveMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;
	
    // Attempt to create a forward message
    TRAPD ( error, iController->CreateForwardL( KMsvDraftEntryId ) );
    
    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorCreateForward, aResult );
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAddressee
// Adding an addressee
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAddressee( const TInt aCaseNumber,
                                          TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	result = iController->AddAddressee( *recipient );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult );
	delete recipient;
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAddresseeAlias
// Adding an addressee with an alias
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAddresseeAlias( const TInt aCaseNumber,
                                               TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	HBufC* alias = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	alias = iController->GetItemFromFile( KAlias, alias );
	result = iController->AddAddressee( *recipient, *alias );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult );
	delete alias;
	delete recipient;
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAddresseeTyped
// Adding a typed addressee
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAddresseeTyped( const TInt aCaseNumber,
                                               TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	TMsvRecipientType recipientType = EMsvRecipientCc;
	result = iController->AddAddressee( recipientType, *recipient );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult );
	delete recipient;
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAddresseeAliasTyped
// Adding a typed addressee with an alias
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAddresseeAliasTyped( const TInt aCaseNumber,
                                                    TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	HBufC* alias = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	alias = iController->GetItemFromFile( KAlias, alias );
	TMsvRecipientType recipientType = EMsvRecipientCc;
	result = iController->AddAddressee( recipientType, *recipient, *alias );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult );
	delete alias;
	delete recipient;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::RemoveAddressee
// Removing an addressee
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::RemoveAddressee( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;

	// Add a recipient for the message
	HBufC* recipient = NULL;
	recipient = iController->GetItemFromFile( KPhoneNumber, recipient );
	result = iController->AddAddressee( *recipient );
	ok = HandleResult( aCaseNumber, result, KErrorAddAddressee, aResult,
	                   ETrue );
	if ( !ok )
		{
		delete recipient;
		return KErrNone;
		}
	
	// Remove the previously added recipient
	iController->RemoveAddressee( 0 );
	
	// Let's check that the recipient was removed
	result = iController->AddresseeExists( *recipient );
	if ( result == KErrNotFound )
		{
		result = KErrNone;
		}
	else
		{
		result = KErrGeneral;
		}
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorRemoveAddressee, aResult );
	delete recipient;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AccessMessageBody
// Accessing the message body
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AccessMessageBody( const TInt aCaseNumber,
                                               TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Get access to the message body
	CRichText& body = iController->GetMessageBody();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetMessageBody, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AccessMessageBodyConst
// Accessing the message body, const version
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AccessMessageBodyConst( const TInt aCaseNumber,
                                                    TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Get access to the message body
	const CRichText& body = iController->GetMessageBodyConst();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetMessageBody, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::HandleMessageSubject
// Handling the message subject
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::HandleMessageSubject( const TInt aCaseNumber,
                                                  TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Load the new message
    TInt result = iController->LoadMessage();
    ok = HandleResult( aCaseNumber, result, KErrorLoadMessage, aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Set the message subject
	HBufC* input = NULL;
	input = iController->GetItemFromFile( KMsgSubject, input );
	result = iController->SetMessageSubject( *input );
	ok = HandleResult( aCaseNumber, result, KErrorSetMessageSubject, aResult,
	                   ETrue );
	if ( !ok )
		{
		delete input;
		return KErrNone;
		}
	
	// Get the message subject
	TPtrC output;
	result = iController->GetMessageSubject( output );
	
	//  Check that the returned subject is correct
    if ( result != KErrNone || output.Compare( *input ) != 0 )
    	{
    	result = KErrGeneral;
    	}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorGetMessageSubject, aResult );
    delete input;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::QueryCapability
// Querying for a capability
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::QueryCapability( const TInt aCaseNumber,
                                             TTestResult& aResult )
	{
	// Query for a capability
	TInt response = 0;
	TInt result = iController->QueryCapability( KUidMtmQuerySupportSubject,
	                                            response );

	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorQueryCapability, aResult );
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::InvokeSynchronousFunction
// Invoking a synchronous function
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::InvokeSynchronousFunction( const TInt aCaseNumber,
                                                       TTestResult& aResult )
	{
	// Allocate memory for the entry record
	CMsvEntrySelection* entries = new CMsvEntrySelection;
    
	// Invoke a synchronous function
	// The method (CMmsClientMtm::InvokeSyncFunctionL()) is not supported so
	// nothing should happen
	TRAPD ( result, iController->InvokeSyncFunctionL( /*EMmsReceive*/KMtmFirstFreeMtmFunctionId+1, *entries ) );

	// Test case was executed, the expected return value is KErrNotSupported
	if ( result == KErrNotSupported )
		{
		result = KErrNone;
		}
	else
		{
		result = KErrGeneral;
		}
	HandleResult( aCaseNumber, result, KErrorReturnValue, aResult );
	delete entries;	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::InvokeAsynchronousFunction
// Invoking an asynchronous function
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::InvokeAsynchronousFunction( const TInt aCaseNumber,
                                                        TTestResult& aResult )
	{
	// Get the service id of the default service
	TMsvId defaultService;
	TInt result = iController->GetDefaultService( defaultService );
	TBool ok = HandleResult( aCaseNumber, result, KErrorGetDefaultService,
	                         aResult, ETrue );
	if ( !ok ) return KErrNone;
	
	// Form the entry record
	_LIT( KEntryError, "Entry insertion failed" );
	CMsvEntrySelection* entries = new CMsvEntrySelection;
	TRAPD ( error, entries->InsertL( 0, defaultService ) );
	ok = HandleResult( aCaseNumber, error, KEntryError, aResult, ETrue );
    if ( !ok )
    	{
    	delete entries;
    	return KErrNone;
    	}
	
	// Invoke an asynchronous function
	TRAPD ( result2,
	        iController->InvokeAsyncFunctionL( /*EMmsReceive*/KMtmFirstFreeMtmFunctionId+1, *entries ) );

	// Test case was executed
	HandleResult( aCaseNumber, result2, KErrorInvokeAsyncFunction, aResult );
	delete entries;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::GetSession
// Getting access to the session
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::GetSession( const TInt aCaseNumber,
                                        TTestResult& aResult )
	{
	// Get a reference to the session
	CMsvSession& session = iController->GetSession();
	
	// Test case was executed (this will always pass)
	HandleResult( aCaseNumber, KErrNone, KErrorGetSession, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAttachmentFilePath
// Adding a file attachment by file path
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAttachmentFilePath( const TInt aCaseNumber,
                                                   TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Attempt to create a new attachment
    HBufC* fileType = NULL;
    HBufC* filePath = NULL;
    fileType = iController->GetItemFromFile( KAttachmentType, fileType );
    filePath = iController->GetItemFromFile( KAttachmentFile, filePath );
    TRAPD( error, iController->AddAttachmentL( *fileType, *filePath,
    	EMMSClientMTMBCTesterAttaTypeNormal ) );
    
    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorAddAttachment, aResult );
	delete filePath;
	delete fileType;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAttachmentOpenHandle
// Adding a file attachment by an open file handle
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAttachmentOpenHandle( const TInt aCaseNumber,
                                                     TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Attempt to create a new attachment
    HBufC* fileType = NULL;
    HBufC* filePath = NULL;
    fileType = iController->GetItemFromFile( KAttachmentType, fileType );
    filePath = iController->GetItemFromFile( KAttachmentFile, filePath );
    TRAPD ( error, iController->AddAttachmentByHandleL( *fileType, *filePath,
    	EMMSClientMTMBCTesterAttaTypeNormal ) );
    
    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorAddAttachment, aResult );
	delete filePath;
	delete fileType;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAttachmentLinkedFile
// Adding a file attachment as a linked file
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAttachmentLinkedFile( const TInt aCaseNumber,
                                                     TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Attempt to create a new attachment
    HBufC* fileType = NULL;
    HBufC* filePath = NULL;
    fileType = iController->GetItemFromFile( KAttachmentType, fileType );
    filePath = iController->GetItemFromFile( KAttachmentFile, filePath );
    TRAPD (error, iController->AddAttachmentAsLinkedL( *fileType, *filePath,
    	EMMSClientMTMBCTesterAttaTypeNormal ) );
    
    // Test case was executed
    HandleResult( aCaseNumber, error, KErrorAddAttachment, aResult );
	delete filePath;
	delete fileType;
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::AddAttachmentMessageEntry
// Adding a message entry as an attachment
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::AddAttachmentMessageEntry( const TInt aCaseNumber,
                                                       TTestResult& aResult )
	{
	// Make a call for the adding function
	// The return value should be KErrNotSupported since no message attachments
	// are allowed in MMS
	TMsvId id = 0;
	TInt result = KErrGeneral;
	TRAPD ( error, iController->AddAttachmentMsgEntryL( id ) );
	if ( error == KErrNotSupported )
		{
		result = KErrNone;
		}
	
	// Test case was executed
    HandleResult( aCaseNumber, result, KErrorReturnValue, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateAttachmentAsync
// Creating an attachment asynchronously
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateAttachmentAsync( const TInt aCaseNumber,
                                                   TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Attempt to create a new attachment
    HBufC* fileType = NULL;
    HBufC* fileName = NULL;
    fileType = iController->GetItemFromFile( KAttachmentType, fileType );
    fileName = iController->GetItemFromFile( KAttachmentName, fileName );
    TRAPD ( result, iController->AddAttachmentAsyncL( *fileType, *fileName,
                                 EMMSClientMTMBCTesterAttaTypeNormal ) );
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorAddAttachment, aResult );
	delete fileName;
	delete fileType;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CancelAttachmentOperation
// Canceling an attachment operation
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CancelAttachmentOperation( const TInt aCaseNumber,
                                                       TTestResult& aResult )
	{
	// Create a new message entry and set the context to it
	TBool ok = InitializeMessageEntry( aCaseNumber, aResult );
	if ( !ok ) return KErrNone;
    
    // Start to create a new attachment and then cancel the operation
    HBufC* fileType = NULL;
    HBufC* filePath = NULL;
    fileType = iController->GetItemFromFile( KAttachmentType, fileType );
    filePath = iController->GetItemFromFile( KAttachmentFile, filePath );
    TRAPD( error, iController->AddAttachmentL( *fileType, *filePath,
    	EMMSClientMTMBCTesterAttaTypeCancel ) );
    
    // The wanted result is now KErrCancel
    TInt result = KErrGeneral;
    if ( error == KErrCancel )
    	{
    	result = KErrNone;
    	}
    
    // Test case was executed
    HandleResult( aCaseNumber, result, KErrorCancelAttachmentOp, aResult );
	delete filePath;
	delete fileType;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::CreateMessage
// Creating a new message
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::CreateMessage( const TInt aCaseNumber,
                                           TTestResult& aResult )
	{
	// Switch the context to the draft folder
	TInt result = iController->SwitchContext( KMsvDraftEntryId );
	TBool ok = HandleResult( aCaseNumber, result, KErrorSwitchContext, aResult,
	                         ETrue );
	if ( !ok ) return KErrNone;
	
	// Get the default service id
	TMsvId serviceId = 0;
	result = iController->GetDefaultService( serviceId );
	ok = HandleResult( aCaseNumber, result, KErrorGetDefaultService, aResult,
	                   ETrue );
	if ( !ok ) return KErrNone;
	
	// Create a new message
	result = iController->CreateMessage( serviceId );
	HandleResult( aCaseNumber, result, KErrorCreateMessage, aResult );
	return KErrNone;	
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::NotifyBIOTypeChange
// Notifying of a BIO type change
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::NotifyBIOTypeChange( const TInt aCaseNumber,
                                                 TTestResult& aResult )
	{
	// Call for the notification method
	// Nothing should happen since MMS does not support BIO
	TUid type = TUid::Uid( 0 );
	TInt result = iController->BIOTypeChanged( type );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorReturnValue, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::RemoveDefaultService
// Removing the default service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::RemoveDefaultService( const TInt aCaseNumber,
                                                  TTestResult& aResult )
	{
	// Call for the remove method
	// Nothing should happen since the method is not supported
	TInt result = iController->RemoveDefaultService();
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorReturnValue, aResult );
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CMMSClientMTMBCTester::ChangeDefaultService
// Changing the default service
// -----------------------------------------------------------------------------
//
TInt CMMSClientMTMBCTester::ChangeDefaultService( const TInt aCaseNumber,
                                                  TTestResult& aResult )
	{
	// Call for the change method
	// Nothing should happen since the method is not supported
	TMsvId service = 0;
	TInt result = iController->ChangeDefaultService( service );
	
	// Test case was executed
	HandleResult( aCaseNumber, result, KErrorReturnValue, aResult );
	return KErrNone;
	}


//  End of file: MMSClientMTMBCTesterCases.cpp
