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
* Description:         MMSClientMTMBCTesterController definition.
*
*/










#ifndef MMSCLIENTMTMBCTESTERCONTROLLER_H
#define MMSCLIENTMTMBCTESTERCONTROLLER_H


// INCLUDES
#include "MMSClientMTMBCTesterSettings.h"
#include "StifTestModule.h"
#include "mmsclient.h"
#include <StifLogger.h>
#include <msvapi.h>
#include <mtclreg.h>


// CONSTANTS

// For using the configuration file
const TInt KExtraSpace = 100;
const TInt KBasicBufferSize = 200;
const TInt KLineLength = 80;


// FORWARD DECLARATIONS

class CMmsSettings;


// CLASS DECLARATION

/**
* Controller class for MMS Client MTM BC Tester
* Uses the MMS Client MTM according to requests from the test cases
* Derives from:
*    CBase - the base class for all heap-based classes
*    MMsvSessionObserver - interface for notification of events from a message
*                          server session
*/
class CMMSClientMTMBCTesterController: public CBase, public MMsvSessionObserver
{


public:

	/**
	* Two-phased constructor
	*/
	static CMMSClientMTMBCTesterController* NewL();

	/**
	* Destructor
	*/
	virtual ~CMMSClientMTMBCTesterController();
	
	/**
	* From MMsvSessionObserver interface
	* Handles the messaging server session events
	* @param aEvent The event type
	* @param aArg1 Event type-specific argument value
	* @param aArg2 Event type-specific argument value
	* @param aArg3 Event type-specific argument value
	* @return None
	*/
	void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                              TAny* aArg3 );

	/**
	* Creating a new message entry
	* @param aDestination Destination folder for the new entry
	* @return The message id of the new message
	*/
	TMsvId CreateNewMessageEntryL( TMsvId aDestination );

	/**
	* Creating a new service entry
	* @return Result error code (KErrNone if all went well)
	*/
	TInt CreateNewServiceEntry();
	
	
/***
* Functions that deal with CMmsSettings class can only be included if the
* internal MMS Server Settings API is available.
* See MMSClientMTMBCTester.mmp for instructions on taking the functions to use..
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING
	
	/**
	* Getting the MMS settings
	* @return Pointer to the MMS settings that are used by the MTM 
    */
    CMmsSettings* GetMMSSettingsL();

	/**
	* Setting the MMS settings
	* @param aSettings The MMS settings to be set in use
	* @return Result error code (KErrNone if all went well)
	*/
	TInt SetMMSSettings( const CMmsSettings& aSettings );

#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

	
	/**
	* Storing MMS settings to disk
	* @return Result error code (KErrNone if all went well)
	*/
	TInt StoreMMSSettings();
	
	/**
	* Restoring MMS settings from disk
	* @return Result error code (KErrNone if all went well)
	*/
	TInt RestoreMMSSettings();
	
	/**
	* Restoring factory MMS settings
	* @return Result error code (KErrNone if all went well)
	*/
    TInt RestoreFactoryMMSSettings();
    
    
    /**
    * Validating service
    * @param aServiceId The service id of the service to be validated
    * @return Result error code (KErrNone if all went well)
    */
    TInt ValidateService( TMsvId aServiceId );
    
    /**
    * Getting the id of the default service
    * @param aServiceId On return, the default service's service id
    *                   (if the operation is successful)
    * @return Result error code (KErrNone if all went well)
    */
    TInt GetDefaultService( TMsvId& aServiceId );
    
    /**
    * Setting the message sender
    * @param aSender The new sender
    * @return Result error code (KErrNone if all went well)
    */
    TInt SetSender( const TDesC& aSender );
    
    /**
    * Getting the message sender
    * @return The sender
    */
    const TPtrC GetSender();

	/**
    * Setting the message class
    * @param aMessageClass The new message class
    * @return None
    */
    void SetMessageClass( TMmsMessageClass aMessageClass );
    
    /**
    * Getting the message class
    * @return The message class
    */
    TInt GetMessageClass();

	/**
    * Setting the message expiration interval
    * @param aInterval The new expiration interval
    * @return None
    */
    void SetExpirationInterval( TTimeIntervalSeconds aInterval );
    
    /**
    * Getting the message expiration interval
    * @return The expiration interval
    */
    TTimeIntervalSeconds GetExpirationInterval();
    
    /**
    * Setting the message expiration date MMS
    * @param aDate The new expiration date
    * @return None
    */
    void SetExpirationDate( TTime aDate );
    
    /**
    * Getting the message expiration date
    * @return The expiration date
    */
    TTime GetExpirationDate();
    
    /**
    * Setting the message delivery time interval
    * @param aInterval The new delivery time interval
    * @return None
    */
    void SetDeliveryTimeInterval( TTimeIntervalSeconds aInterval );
    
    /**
    * Getting the message delivery time interval
    * @return The delivery time interval
    */
    TTimeIntervalSeconds GetDeliveryTimeInterval();
    
    /**
    * Setting the message delivery date
    * @param aDate The new delivery date
    * @return None
    */
    void SetDeliveryDate( TTime aDate );
    
    /**
    * Getting the message delivery date
    * @return The delivery date
    */
    TTime GetDeliveryDate();

	/**
    * Setting the message priority
    * @param aPriority The new priority
    * @return None
    */
    void SetMessagePriority( TMmsMessagePriority aPriority );
    
    /**
    * Getting the message priority
    * @return The priority
    */
    TInt GetMessagePriority();
    
    /**
    * Setting the sender visibility
    * @param aVisibility The new visibility
    * @return None
    */
    void SetSenderVisibility( TMmsMessageSenderVisibility aVisibility );
    
    /**
    * Getting the sender visibility
    * @return The visibility
    */
    TInt GetSenderVisibility();
    
    /**
    * Setting the delivery report request
    * @param aRequest The new request value
    * @return None
    */
    void SetDeliveryReportRequest( TMmsYesNo aRequest );
    
    /**
    * Getting the delivery report request
    * @return The request value
    */
    TInt GetDeliveryReportRequest();
    
    /**
    * Setting the read reply request
    * @param aRequest The new request value
    * @return None
    */
    void SetReadReplyRequest( TMmsYesNo aRequest );
    
    /**
    * Getting the read reply request
    * @return The request value
    */
    TInt GetReadReplyRequest();
    
    /**
    * Getting the message sending date
    * @return The sending date
    */
    TTime GetSendingDate();
    
    /**
    * Getting the response text
    * @return The response text
    */
    TPtrC GetResponseText();
    
    /**
    * Getting the response status
    * @return The response status
    */
    TInt GetResponseStatus();
    
    /**
    * Getting the number of the previous senders of a message
    * @return The number of previous senders
    */
    TInt GetNumberOfPreviousSenders();
    
    /**
    * Getting the address of a previous sender of a message
    * @param aIndex The index of the sender whose address is to be returned
    * @return The address of the sender with the specified index
    */
    TPtrC GetPreviousSender( TInt aIndex );
    
    /**
    * Getting the date and time of a previous sending of a message
    * @param aIndex The index of the sending whose date is to be returned
    * @return The sending date with the specified index
    */
    TTime GetPreviousSendingDate( TInt aIndex );

	/**
	* Getting the message receive time
	* @return The receive time
	*/
	TTime GetReceiveTime();

	/**
	* Getting the message transfer size
	* @return The transfer size
	*/
	TInt GetTransferSize();

	/**
	* Getting the message content location
	* @return The location
	*/
	TPtrC8 GetContentLocation();

	/**
    * Setting the message root part
    * @param aRoot The entry id of the new root
    * @return Result error code (KErrNone if all went well)
    */
    TInt SetMessageRootPart( TMsvAttachmentId aRoot );
    
    /**
    * Getting the message root part
    * @return The id of the root part entry
    */
    TMsvAttachmentId GetMessageRootPart();
    
    /**
    * Setting the maximum image size
    * @param aWidth The new maximum width
    * @param aHeight The new maximum height
    * @return None
    */
    void SetMaximumImageSize( TInt aWidth, TInt aHeight );
    
    /**
    * Getting the maximum image size
    * @param aWidth On return, the maximum width
    * @param aHeight On return, the maximum height
    * @return None
    */
    void GetMaximumImageSize( TInt& aWidth, TInt& aHeight );
    
    /**
    * Getting the message size
    * @return The message size
    */
    TInt32 GetMessageSize();
    
    /**
    * Setting the message description
    * @param aDescription The new description
    * @return Result error code (KErrNone if all went well)
    */
    TInt SetMessageDescription( const TDesC& aDescription );
    
    /**
    * Adding a new attribute
    * @param aName The name of the new attribute
    * @param aValue The value of the new attribute
    * @return Result error code (KErrNone if all went well)
    */
    TInt AddNewAttribute( const TDesC& aName, const TDesC& aValue );
    
    /**
    * Getting an attribute
    * @param aName The name of the wanted attribute
    * @param aValue On return, the attribute's value
    * @return Result error code (KErrNone if all went well)
    */
    TInt GetAttribute( const TDesC& aName, TPtrC& aValue );
    
    /**
    * Checking if an attribute exists
    * @param aName The name of the attribute to be searched for
    * @return ETrue if the attribute exists, EFalse if it does not
    */
    TBool FindAttribute( const TDesC& aName );
    
    /**
    * Deleting an attribute
    * @param aName The name of the attribute which is to be deleted
    * @return None
    */
    void DeleteAttribute( const TDesC& aName );
    
    /**
    * Resetting all attributes
    * @return None
    */
    void ResetAttributes();
    
    /**
    * Adding an attachment synchronously
    * @param aFileType The file type of the new attachment
    * @param aFilePath The location of the attachment file
    * @param aType The type of the attachment operation
    */
    TInt AddAttachmentSyncL( const TDesC& aFileType, const TDesC& aFilePath,
                             TMMSClientMTMBCTesterAttaType aType );
    
    /**
    * Adding a text attachment synchronously
    * @param aText The content of the new attachment
    * @param aFileName File name suggestion for the attachment
    * @param aType The type of the attachment operation
    */
    TInt AddTextAttachmentL( const TDesC& aText, const TDesC& aFileName,
                             TMMSClientMTMBCTesterAttaType aType );

	/**
	* Sending a message using default service settings
	* @return None
	*/
	void SendMessageL();
	
	/**
	* Sending a selection of messages using default service settings
	* @param aFolder The location of the messages that are to be sent
	* @return None
	*/
	void SendMessageSelectionL( TMsvId aFolder );
	
	/**
	* Fetching all available messages to inbox
	* @return None
	*/
	void FetchAllMessagesL();
	
	/**
	* Getting the type of the MTM
	* @return The type
	*/
	TUid GetType();

	/**
	* Setting the context to the given entry
	* @param aEntry Pointer to the entry to which the context should be set
	* @return Result error code (KErrNone if all went well)
	*/
	TInt SetContext( CMsvEntry* aEntry );
	
	/**
	* Switching the context to the given entry
	* @param aEntry The entry id of the entry to which the context is set
	* @result Result error code (KErrNone if all went well)
	*/
	TInt SwitchContext( TMsvId aEntry );
	
	/**
	* Check if the context has been set
	* @return ETrue if the context has been set, EFalse if it hasn't
	*/
	TBool HasContext();
	
	/**
	* Getting the current entry
	* @return Reference to the current entry
	*/
	CMsvEntry& GetCurrentEntry();
	
	/**
	* Getting an entry with the specified id
	* @param aEntryId The entry id of the wanted entry
	* @param aEntry On return, the wanted entry
	* @return Result error code (KErrNone if all went well)
	*/
	TInt GetEntry( TMsvId aEntryId, CMsvEntry*& aEntry );
	
	/**
	* Saving a message (storing current entry data)
	* @return Result error code (KErrNone if all went well)
	*/
	TInt SaveMessage();
	
	/**
	* Loading a message (restoring current entry data)
	* @return Result error code (KErrNone if all went well)
	*/
	TInt LoadMessage();
	
	/**
	* Validating selected parts of the current message
	* @param aPartList Flags specifying which parts are to be validated
	* @return Result error code (KErrNone if all went well)
	*/
	TInt ValidateMessage( TMsvPartList aPartList );
	
	/**
	* Searching for the given text in specified parts of the current message
	* @param aText The text to be searched for
	* @param aPartList Flags specifying which parts are to be searched
	* @return Flags specifying in which parts the text was found
	*/
	TMsvPartList SearchForText( const TDesC& aText, TMsvPartList aPartList );
	
	/**
	* Creating a reply message to the current message
	* @param aDestination Destination folder for the new message
	* @return The message id of the new message
	*/
	TMsvId CreateReplyL( TMsvId aDestination );
	
	/**
	* Creating a forward message based on the current message
	* @param aDestination Destination folder for the new message
	* @return The message id of the new message
	*/
	TMsvId CreateForwardL( TMsvId aDestination );
	
	/**
	* Adding an addressee for the message
	* @param aAddressee The address of the new recipient
	* @return Result error code (KErrNone if all went well)
	*/
	TInt AddAddressee( const TDesC& aAddressee );

	/**	
	* Adding an addressee with an alias for the message
	* @param aAddressee The address of the new recipient
	* @param aAlias The alias of the new recipient
	* @return Result error code (KErrNone if all went well)
	*/
	TInt AddAddressee( const TDesC& aAddressee, const TDesC& aAlias );
	
	/**
	* Adding a typed addressee for the message
	* @param aRecipientType The type of the new recipient
	* @param aAddressee The address of the new recipient
	* @return Result error code (KErrNone if all went well)
	*/
	TInt AddAddressee( TMsvRecipientType aRecipientType,
	                   const TDesC& aAddressee );
	
	/**
	* Adding a typed addressee with an alias for the message
	* @param aRecipientType The type of the new recipient
	* @param aAddressee The address of the new recipient
	* @param aAlias The alias of the new recipient
	* @return Result error code (KErrNone if all went well)
	*/
	TInt AddAddressee( TMsvRecipientType aRecipientType,
	                   const TDesC& aAddressee, const TDesC& aAlias );
	
	/**
	* Removing an addressee from the message
	* @param aIndex The index of the addressee which is to be removed
	* @return None
	*/
	void RemoveAddressee( TInt aIndex );
	
	/**
	* Getting access to the message body
	* @return A reference to the rich text body of the message
	*/
    CRichText& GetMessageBody();
    
    /**
    * Getting access to the message body, const version
    * @return A const reference to the rich text body of the message
    */
    const CRichText& GetMessageBodyConst();
    
    /**
    * Setting the message subject
    * @param aSubject The new subject
    * @return Result error code (KErrNone if all went well)
    */
    TInt SetMessageSubject( const TDesC& aSubject );
    
    /**
    * Getting the message subject
    * @param On return, the message subject
    * @result Result error code (KErrNone if all went well)
    */
    TInt GetMessageSubject( TPtrC& aSubject );
	
	/**
	* Asking for the existence and the value of a capability
	* @param aCapability The capability to be queried
	* @param aResponse On return, the value describing the capability
	* @return KErrNone if the capability is known to the MTM,
	*         KErrNotSupported if it is not
	*/
	TInt QueryCapability( TUid aCapability, TInt& aResponse );
	
	/**
	* Invoking a synchronous function
	* @param aFunction The function code of the function to be invoked
	* @param aEntries Array of message entries to be operated on
	* @return None
	*/
	void InvokeSyncFunctionL( TInt aFunction,
	                          const CMsvEntrySelection& aEntries );
	
	/**
	* Invoking an asynchronous function
	* @param aFunction The function code of the function to be invoked
	* @param aEntries Array of message entries to be operated on
	* @return None
	*/
	void InvokeAsyncFunctionL( TInt aFunction,
	                           const CMsvEntrySelection& aEntries );
	
	/**
	* Getting access to the session
	* @return A reference to the session
	*/
	CMsvSession& GetSession();
	
	/**
	* Adding a file attachment by its file path to the current message
	* @param aFileType The file type of the attachment file
	* @param aFilePath The location of the attachment file
	* @param aType The type of the adding operation
	* @return None
	*/
	void AddAttachmentL( const TDesC& aFileType, const TDesC& aFilePath,
	                     TMMSClientMTMBCTesterAttaType aType );
	
	/**
	* Adding a file attachment by an open file handle
	* @param aFileType The file type of the attachment file
	* @param aFilePath The location of the attachment file
	* @param aType The type of the adding operation
	*/
	void AddAttachmentByHandleL( const TDesC& aFileType, const TDesC& aFilePath,
	                             TMMSClientMTMBCTesterAttaType aType );
	
	/**
	* Adding a file attachment as a linked file
	* @param aFileType The file type of the attachment file
	* @param aFilePath The location of the attachment file
	* @param aType The type of the adding operation
	*/
	void AddAttachmentAsLinkedL( const TDesC& aFileType, const TDesC& aFilePath,
	                             TMMSClientMTMBCTesterAttaType aType );
	
	/**
	* Adding a message entry as an attachment
	* @param aAttachment The id of the message entry to be added
	* @return None
	*/
	void AddAttachmentMsgEntryL( TMsvId aAttachment );
	
	/**
	* Adding an attachment asynchronously
	* @param aFileType The file type of the attachment file
	* @param aFilePath The location of the attachment file
	* @param aType The type of the adding operation
	* @return None
	*/
    void AddAttachmentAsyncL( const TDesC& aFileType, const TDesC& aFilePath,
                              TMMSClientMTMBCTesterAttaType aType );
	
	/**
	* Creating a new empty message entry
	* @param aServiceId The service id for the new entry
	* @return Result error code (KErrNone if all went well)
	*/
	TInt CreateMessage( TMsvId aServiceId );
	
	/**
	* Notifying of a changed BIO type
	* @aType The BIO type
	* @return Result error code (KErrNone if all went well)
	*/
	TInt BIOTypeChanged( TUid aType );
	
	/**
	* Removing the default service
	* @return Result error code (KErrNone if all went well)
	*/
	TInt RemoveDefaultService();
	
	/**
	* Changing the default service
	* @param aService The new default service
	* @return Result error code (KErrNone if all went well)
	*/
	TInt ChangeDefaultService( TMsvId& aService );

	/**
	* Determining if the given item is found in the addressee list
	* @param aItem The addressee candidate
	* @return KErrNone if the given item is found in the addressee list,
	*         KErrNotFound if it is not
	*/
	TInt AddresseeExists( const TDesC& aItem );
	
	/**
	* Cleaning up: removing all messages from all the folders in use
	* @return None
	*/
	void Cleanup();
	
	/**
	* Removing all messages from the given folder
	* @param aFolder The folder to be emptied
	* @return None
	*/
	void ClearFolderL( TMsvId aFolder );

	// Helper functions for using the configuration file
	// Definitions in MMSClientMTMBCTesterTools.cpp
	void ReadConfigFileL( TFileName& aIniFile );
	HBufC* GetItemFromFile( const TDesC8& aID, HBufC* iBuf );


private:

	// Private member functions

	/*
	* First phase constructor
	*/
	CMMSClientMTMBCTesterController();

	/*
	* Second phase constructor
	*/
	void ConstructL();
	

	// Class attributes
	
	// STIF Test Framework logger
	CStifLogger* iLogger;
	
	// Message server session
	CMsvSession* iSession;
	
	// MTM registry
	CClientMtmRegistry* iRegistry;
	
	// MMS Client
	CMmsClientMtm* iClient;
	
	// For using the configuration file
	TInt iExtraSpace;
	TInt iBufSize;
	HBufC8* iFileBuf;
		
};


#endif // MMSCLIENTMTMBCTESTERCONTROLLER_H
            
// End of file: MMSClientMTMBCTesterController.h
