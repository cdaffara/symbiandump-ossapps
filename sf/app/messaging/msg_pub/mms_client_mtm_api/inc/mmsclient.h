/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Client Mtm for multimedia messaging.
*     This is the API for accessing multimedia messaging engine.
*
*/



#ifndef MMSCLIENT_H
#define MMSCLIENT_H

//  INCLUDES
#include  <mtclbase.h> // base client mtm
#include  <e32std.h>   // TTimeInterval & TTime
#include  "mmsconst.h" // common constants

// CONSTANTS

// MACROS

// DATA TYPES
typedef struct
    {
    const TUint SymbianCharsetUID;
    const TUint IANAMIBEnum;
    }TMmsCharacterSetLookup;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsSettings;
class CMmsHeaders;
class CMsvMimeHeaders;
class CMsvFindText;
class CMmsAttachmentWaiter;

// CLASS DECLARATION

/**
*  Client Mtm for multimedia messaging subsystem. 
*
*  This class will be the interface to the UI component and other 
*  messaging component that might want to handle multimedia messages 
*  (For example SendAs interface).
* 
*  This class provides access to MMS specific headers in the message.
*
*  Note: new functions are added as the last virtual functions in order
*  not to to break the vtable
*
* @code
*  // Example of getting access to this class:
*
*  // Called by an application that implements
*  // MMsvSessionObserver interface
*
*  iSession = CMsvSession::OpenSyncL(*this);
*  CleanupStack::PushL(iSession);
*  iClientMtmRegistry = CClientMtmRegistry::NewL(*iSession);
*  CleanupStack::PushL(iClientMtmRegistry);
*  iMmsClient = (CMmsClientMtm *) iClientMtmRegistry->
*               NewMtmL(KUidMsgTypeMultimedia);
*  CleanupStack::PushL(iMmsClient);
*
*  // - do other initialization
*
*  CleanupStack::Pop(3); //iSession, iClientMtmRegistry, iMmsClient
*
*  // - call any public functions in CMmsClientMtm
*
*  // When the application finishes,
*  // it must delete the objects in reverse order:
*  delete iMmsClient;
*  delete iClientMtmRegistry;
*  delete iSession;
* @endcode
*/

class CMmsClientMtm :public CBaseMtm
    {
    public:  // Constructors and destructor

        /**
        * Factory function. 
        *
        * The only function exported by this polymorphic interface dll. 
        * This function is not directly called by the application that needs 
        * access, it is called by an instance of CClientMtmRegistry class.
        *
        * @param[in] aRegisteredMtmDll Mtm Dll registry class
        * @param[in] aSession Message Server session.
        * @return Pointer to CMmsClientMtm class.
        */
        IMPORT_C static CMmsClientMtm* NewL(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvSession& aSession );

        /**
        * Destructor.
        */
        virtual ~CMmsClientMtm();

    public:  // New functions

        // ----------------------------------------------------------
        // Functions to create and modify message entries

        /**
        * Create a new message entry.
        *
        * @param[in] aDestination Target folder.
        * @param[in] aCompletionStatus Reference to the status of an active object. 
        *     This status will contain the relevant error code when the operation 
        *     completes.
        * @return Pointer to a message server operation (active object). 
        *     When the message has been created the progress information of the 
        *     operation provides the id of the created message in an 8-bit package 
        *     buffer. While the operation is in progress the package will contain a 
        *     null id (KMsvNullIndexEntryId). If there is an error while 
        *     creating the message the message will be deleted and the 
        *     package will contain a null id.
        *
        * This function is suitable when the caller is an active object or the 
        * caller does not want to immediately change current context to the 
        * new message entry.
        *
        * If the caller is not an active object and the caller wants the context 
        * of CMmsClientMtm to be immediately set to the new entry, it is simpler 
        * to call CreateMessageL.
        * @see CMmsClientMtm::CreateMessageL
        *
        * @code
        * // This example shows usage with a caller that is not an active object,
        * // so a separate waiter is needed. If the caller is an active object,
        * // the caller gives its own status to the function and continues 
        * // execution in RunL function.
        *
        * CMsvOperation* myOperation = NULL;
        * CMsvOperationActiveSchedulerWait* wait = 
        *     CMsvOperationActiveSchedulerWait::NewLC();
        *
        * // destinationId specifies the destination folder.
        * myOperation = iMmsClient->CreateNewEntryL( destinationId, wait->iStatus );
        *
        * CleanupStack::PushL( myOperation );
        *
        * wait->Start();
        *
        * if ( wait->iStatus.Int() != KErrNone )
        *     { 
        *     // error handling, e.g. leave
        *     }
        *
        * // Get the message id
        * TPckgBuf<TMsvId> pkg;
        * pkg.Copy( myOperation->ProgressL() );
        * TMsvId progress = pkg();
        * CleanupStack::PopAndDestroy(2); // myOperation, wait;
        *
        * // Load the new message
        * iMmsClient->SwitchCurrentEntryL( progress );
        *
        * // load the default values that were already initialized
        * // when the message was created 
        * iMmsClient->LoadMessageL();
        * 
        * // continue adding data to the message
        * // ...
        * @endcode
        */
        virtual CMsvOperation* CreateNewEntryL(
            TMsvId aDestination,
            TRequestStatus& aCompletionStatus);


        // -------------------------------------------------------------------
        // FUNCTIONS TO HANDLE MMSC SETTINGS
        //
        // Only one MMS service entry may be created!
        //
        // The Client MTM maintains cached MMS service settings, referred to 
        // as current service below. Some of those cached settings are used 
        // as template values when a new multimedia message is created. 
        //
        // Use Base MTM functions to get default service id.

// <DEPRECATED>
        /**
        * Create new service entry.
        *
        * Context is set to the new service entry. 
        * Currently a maximum of one service is created, and further requests 
        * do not create a new service entry.
        * @deprecated Only one MMS service is supported and it is automatically created. 
        * Use DefaultServiceL() to get the id for the default service.
        */
        virtual void CreateServiceL();
// </DEPRECATED>

        // Functions to load, save, and access MMS Settings.
        // There is no need to change the context when these functions are used.
        //

        /**
        * Get a reference to CMmsSettings class.
        *
        * This method should be used by MMS UI only. Other applications should 
        * not touch the MMS settings. The methods are needed in MMS Client API 
        * to allow MMS UI to inform MMS Client MTM about changed settings. 
        *
        * The contents of the class are those used by MMS Client MTM 
        * when constructing a message. If the settings must be changed, 
        * a copy must be made, and SetSettingsL function used to deliver 
        * the settings to MMS Client MTM.
        *
        * @return constant reference to iMmsSettings member of CMmsClientMtm.
        *
        * @code
        * // Usage:
        * 
        * CMmsSettings* settings = CMmsSettings::NewL();
        * CleanupStack::PushL( settings );
        * iMmsClient->RestoreSettingsL();
        * settings->CopyL( iMmsClient->MmsSettings() );
        * 
        * // change settings here...
        * // The settings are changed using CMmsSettings API
        * 
        * iMmsClient->SetSettingsL( *settings );
        * iMmsClient->StoreSettingsL();
        * CleanupStack::PopAndDestroy(); // settings
        *
        * @endcode
        */
        virtual const CMmsSettings& MmsSettings();

        /**
        * Copy the values from aSettings to CMmsClientMtm.
        *
        * This method should be used by MMS UI only. Other applications should 
        * not touch the MMS settings. 
        *
        * Used to provide new settings to MMS Client MTM when settings have 
        * been changed. Will affect only messages created after the settings  
        * have been changed. 
        *
        * Use function StoreSettingsL to save the settings on disk.
        *
        * @param[in] aSettings New values for CMmsSettings
        */
        virtual void SetSettingsL( const CMmsSettings& aSettings );

        /**
        * Save settings to disk.
        *
        * This method should be used by MMS UI only. Other applications should 
        * not touch the MMS settings. 
        */
        virtual void StoreSettingsL();

        /**
        * Load settings from disk.
        *
        * This method should be used by MMS UI only. Other applications should 
        * not touch the MMS settings.
        */
        virtual void RestoreSettingsL();

// <DEPRECATED>
        /**
        * Restore factory settings.
        *
        * Restore settings from ROM to the default service entry and select it 
        * as the current service entry.
        * @param aLevel Defines the operations to be done. 
        * @deprecated MMS UI should restore the factory settings directly.
        */
        virtual void RestoreFactorySettingsL(
            TMmsFactorySettingsLevel aLevel = EMmsFactorySettingsLevelNormal );
// </DEPRECATED>

        /**
        * Validate service.
        *
        * Checks that access point refers to a valid entry in comms database.
        *
        * @param[in] aServiceId Id of the MMS service
        * @return Error code. 
        * - KErrNone: The service is OK.
        * - KErrNotFound: The service id is incorrect.
        * - KMmsErrorInvalidSettings: The settings contain invalid values. 
        * This error is possible only if the settings file has been corrupted.
        * - KMmsErrorNoWAPAccessPoint: No access point has been defined.
        * - KMmsErrorAP1Invalid: MMS access point refers to an invalid entry in comms 
        * database.
        * - KMmsErrorNoURI1: Home page has not been defined for MMS access point
        */
        virtual TInt ValidateService( TMsvId aServiceId );

        // -------------------------------------------------------------------
        // FUNCTIONS TO HANDLE MMS HEADERS

        // Accessors and mutators (getters and setters) for header fields.
        // Some of these header fields have default values that are assigned
        // from cached service settings when a new header is allocated.
        // Some header fields are needed by protocol only.
        // Those headers don't have accessors and mutators here,
        // as they are used by Server MTM who accesses them directly
        // through functions offered by CMmsHeaders.

        /**
        * Set the sender of the message.
        *
        * If the sender of the message is not explicitly defined, the Proxy-Relay 
        *     or MMS Service Centre will add the sender's phone number.
        *
        * @param[in] aAlias The phone number of the sender of the message. 
        *     Maximum length 256 characters. This string will be included 
        *     as the sender of the message when the message is sent, but 
        *     the MMS Service Centre will check the validity of the 
        *     value and replace it with the actual phone number of the 
        *     sender if it is not correct.
        */
        virtual void SetSenderL( const TDesC& aAlias );

        /**
        * Get the sender of the message.
        *
        * @return Address of the sender of the message (for example phone number). 
        *     If the sender has not been defined, returns an empty string.
        */
        virtual const TPtrC Sender() const;

        /**
        * Set the message class.
        *
        * If message class is not explicitly set, the message will have 
        *     class "Personal" by default.
        *
        * @param [in] aMessageClass Message class code. Possible values: 
        * - EMmsClassPersonal: Message is a normal person-to-person message (default).
        * - EMmsClassAdvertisement: Message contains an advertisement.
        * - EMmsClassInformational: Message contains data from an information service.
        * - EMmsClassAuto: Message has been automatically generated by the phone. 
        *   This class is only valid for a message that is a read report to another message.
        */
        virtual void SetMessageClass( TMmsMessageClass aMessageClass );

        /**
        * Get the message class.
        *
        * @return Message class. Possible values: 
        * - EMmsClassPersonal: Message is a normal person-to-person message (default).
        * - EMmsClassAdvertisement: Message contains an advertisement.
        * - EMmsClassInformational: Message contains data from an information service.
        * - EMmsClassAuto: Message has been automatically generated by the phone. 
        *   This class is only valid for a message that is a read report to another message.
        * - 0: Message class has not been defined. Handle as EMmsClassPersonal.
        */
        virtual TInt MessageClass() const;

        /**
        * Set the validity period of the message.
        *
        * If the validity period is not defined for the message, default 
        * validity period from MMS settings or MMS Service Centre will be used.
        *
        * @param[in] aInterval The length of time in seconds after which the 
        *     message will be discarded by MMS Service Centre. 
        *     MMSC may limit the maximum length of the validity period. 
        */
        virtual void SetExpiryInterval( TTimeIntervalSeconds aInterval );

        /**
        * Get the validity period of the message.
        *
        * @return Storage time of the message in MMS Service Centre (in seconds). 
        *     If the message cannot be delivered to the recipient within the 
        *     validity period, it will be discarded. If the validity period is 0, 
        *     it has not been defined.
        */
        virtual TTimeIntervalSeconds ExpiryInterval() const;

        /**
        * Set the expiration date of the message.
        *
        * @param[in] aDate The date and time when the message will expire 
        *     (In UTC time). The date must be later than 1970-01-01, 00:00:00 GMT 
        *     If the MMS Service Centre cannot deliver the message to the recipient 
        *     before the expiration date, the message will be discarded. If expiration 
        *     date or validity period have not been defined, default is used.
        */
        virtual void SetExpiryDate( TTime aDate );

        /**
        * Get the expiration date of the message.
        *
        * @return The date and time when the message will expire. (in UTC time). 
        *      If the expiration date has not been defined, TTime(0) will be 
        *      returned.
        */
        virtual TTime ExpiryDate() const;

        /**
        * Set the delivery time interval for the message.
        *
        * @param[in] aInterval The length of time in seconds after which the message 
        *     will be delivered to the recipient by the MMS Service Centre. 
        *     If neither delivery time interval or delivery date have been defined,
        *     MMS Service Centre will deliver the message immediately.
        */
        virtual void SetDeliveryTimeInterval( TTimeIntervalSeconds aInterval );

        /**
        * Get the delivery time interval of the message.
        *
        * @return The length of time in seconds after which the message will be 
        *     delivered to the recipient by MMS Service Centre. If the delivery 
        *     time interval is 0, it has not been defined.
        */
        virtual TTimeIntervalSeconds DeliveryTimeInterval() const;

        /**
        * Set the delivery date for the message.
        *
        * @param[in] aDate The date and time when the message will be delivered 
        *     to the recipient by the MMSC (in UTC time). The date must be 
        *     later than 1970-01-01, 00:00:00 GMT. If neither delivery time 
        *     interval or delivery date have been defined, MMS Service Centre 
        *     will deliver the message immediately.
        */
        virtual void SetDeliveryDate( TTime aDate );

        /**
        * Get the delivery date of the message.
        *
        * @return The date and time when the message will be (or was) delivered 
        *     to the  recipient by the MMSC (in UTC time). If the delivery date 
        *     has not been defined, TTime(0) is returned.
        */
        virtual TTime DeliveryDate() const;

        /**
        * Set the priority of the message. 
        *
        * If the priority of the message is not set, the default priority will be 
        *     "normal".
        *
        * @param[in] aPriority Message priority, possible values:
        * - EMmsPriorityLow:     Low priority.
        * - EMmsPriorityNormal:  Normal priority.
        * - EMmsPriorityHigh:    High priority.
        */
        virtual void SetMessagePriority( TMmsMessagePriority aPriority );

        /**
        * Get the priority of the message.
        *
        * @return Message priority, possible values:
        * - EMmsPriorityLow:     Low priority.
        * - EMmsPriorityNormal:  Normal priority.
        * - EMmsPriorityHigh:    High priority.
        * - 0: Priority has not been defined, treat as EMmsPriorityNormal
        */
        virtual TInt MessagePriority() const;

        /**
        * Set the sender visibility setting for the message.
        *
        * Indicates whether the MMS Service Centre should hide the sender's phone 
        *     number from the recipient. If the value is not defined, default is 
        *     used. The default is to show the sender's number unless the sender 
        *     has a secret number.
        *
        * @param[in] aVisibility Visibility of the sender's phone number to the 
        *    recipient. Possible values:
        * - EMmsSenderVisibilityDefault: Use default visibility.
        * - EMmsSenderVisibilityHide: Hide the sender's number.
        * - EMmsSenderVisibilityShow: Show the sender's number even if it is a 
        *     secret number.
        */
        virtual void SetSenderVisibility(
            TMmsMessageSenderVisibility aVisibility );

        /**
        * Get the sender visibility setting of the message.
        *
        * Indicates whether the MMS Service Centre should hide the sender's phone 
        *     number from the recipient. The default is show the sender's number 
        *     unless the server has a secret number.
        *
        * @return visibility setting. Possible values:
        * - EMmsSenderVisibilityDefault: Default visibility.
        * - EMmsSenderVisibilityHide: Hide the sender's number.
        * - EMmsSenderVisibilityShow: Show the sender's number even if it is a 
        *     secret number.
        * - 0: Sender visibilty has not been defined, use default.
        */
        virtual TInt SenderVisibility() const;

        /**
        * Set the delivery report request setting value for the message.
        *
        * If the value is not set, default value from MMS settings will be used.
        *
        * @param[in] aRequest tells if the user wants a delivery report for this 
        *    message. Possible values: 
        * - EMmsYes: The user wants a delivery report.
        * - EMmsNo:  The user does not want a delivery report.
        */
        virtual void SetDeliveryReport(
            TMmsYesNo aRequest );

        /**
        * Get the delivery report request setting of the message.
        *
        * If the value is not defined, default value from MMS settings is used.
        *
        * @return delivery report request setting. Possible values: 
        * - EMmsYes: The user wants a delivery report.
        * - EMmsNo:  The user does not want a delivery report.
        * - 0: Setting has not been defined.
        */
        virtual TInt DeliveryReport() const;

        /**
        * Set the read report request setting value for the message.
        *
        * Specifies if the user wants a read report for the current message. 
        * If this value is Yes, the recipient phone should send a read report  
        *    when the user opens the message for the first time.
        *
        * @param[in] aRequest read report request setting. Possible values:
        * - EMmsYes: The user wants a read report.
        * - EMmsNo:  The user does not want a read report.
        */
        virtual void SetReadReply( TMmsYesNo aRequest );

        /**
        * Get the read report request setting of the message.
        *
        * Specifies if the sender wants a read report for current message. 
        * If this value is yes and the message has been received by the phone 
        *     (has "KMmsMessageMobileTerminated" flag) a read report should be 
        *     sent to the sender of this message when the message is opened 
        *     for the first time.
        *
        * @return read report request setting. Possible values:
        * - EMmsYes: The user wants a read report.
        * - EMmsNo:  The user does not want a read report.
        * - 0: Setting has not been defined. Do not send a read report.
        */
        virtual TInt ReadReply() const;

        /**
        * Get the sending date and time of the message. 
        * Valid only for messages received by the phone.
        * @return the time when MMS Service Centre has received the message  
        *     from sender (in UTC time). 
        *     If the time has not been defined, returns TTime(0).
        */
        virtual TTime SendingDate() const;

        /**
        * Get the response text from the message.
        *
        * Valid only in cases a response text has been obtained from MMS Service 
        *     Centre. Possible cases are received messages and messages whose 
        *     senging has failed. The text may explain the cause of the failure.
        *
        * @return Response text string. If text is not defined, returns an empty 
        *     string.
        * @since 2.0
        */
        virtual TPtrC ResponseText() const;

        /**
        * Get the response status value from the message.
        *
        * This function returns the status MMS Service Centre has sent with a 
        *     retrieved message or as a response to a failure to send a message. 
        * The status code may be used in case of permanent failures to retrieve 
        *     or failures to send to indicate the reason of the failure.
        *
        * @return Status code sent by MMS Service Centre. Possible values are 
        *     defined in OMA MMS Encapsulations specifications, and depend on 
        *     the version of the MMS Service Centre sending the response. 
        * - Error codes 128 - 136 denote legacy errors from MMS encapsulation 
        *     version 1.0
        * - Error codes 192 - 223 denote transient failures.
        * - Error codes 224 - 255 denote permanent failures.
        * - 0 means the response status has not been set. Either the operation was 
        *     successful or the cause of the failure was not set by MMS Service Centre.
        * @since 3.0
        */
        virtual TInt ResponseStatus() const;

        /**
        * Get number of times the message has been forwarded.
        *
        * Returns the number of previous senders in case of a message that 
        *     has been forwarded from one terminal to another based on the 
        *     MMS notification only without retrieving the actual message to 
        *     the terminal first.
        *
        * @return Number of times the message has been forwarded.
        * @since 3.0
        */
        virtual TInt NumberOfPreviousSenders() const;

        /**
        * Get the address of a previous sender.
        *
        * The addresses of the previous senders are defined for messages that 
        *     have been forwarded without fetching them to the terminal first.
        *
        * @param[in] aSequenceNumber Indicates the number of the sender in the 
        *     sequence. 1 is the first sender, a higher number indicates a later 
        *     sender.
        * @return Address of the specified previous sender. If the sequence number 
        *     exceeds the number of senders or is less than 1, an empty string is 
        *     returned.
        * @since 3.0
        */
        virtual TPtrC PreviousSender( TInt aSequenceNumber ) const;

        /**
        * Get the time when the message was previously sent (in UTC time).
        *
        * The function is valid only for messages that have been forwarded 
        *     without fetching them to the terminal first.
        *
        * @param[in] aSequenceNumber Indicates the number of the sender in the 
        *     sequence. 1 is the first sender, a higher number indicates a later 
        *     sender.
        * @return Time of the previous sending (in UTC time). If the sequence 
        *     number exceeds the number of senders or is less than 1, TTime(0)  
        *     is returned.
        * @since 3.0
        */
        virtual TTime PreviousSendingDate( TInt aSequenceNumber ) const;

        /**
        * Get the time when the message was received in the terminal.
        *
        * @return Time of the arrival of the message (in UTC time). 
        *    If the time has not been defined, TTime(0) is returned.
        * @since 3.0
        */
        virtual TTime MessageReceiveTime() const;

        /**
        * Get the incoming message size.
        *
        * This is valid only for a notification.
        *
        * @return Message size in octets as specified in MMS Notification.
        */
        virtual TInt MessageTransferSize() const;

        /**
        * Get the Uri from which the message can be fetched.
        *
        * This is valid only for a nofification.
        *
        * @return Content location of the actual message, the Uri from which 
        *    the message is fetched from MMS Service Centre.
        */
        virtual TPtrC8 MessageContentLocation() const;

        /**
        * Set id of the root part of the message.
        *
        * @param[in] aId Attachment Id of the message part which controls the 
        *     display of the message. Should point to the SMIL part if present.
        */
        virtual void SetMessageRootL( const TMsvAttachmentId aId );

        /**
        * Get the id of the root part of the message.
        *
        * @return Id of the attachment that starts the message display, 
        * KMsvNullIndexEntryId if the root part has not been defined.
        */
        virtual TMsvAttachmentId MessageRootAttachment() const;

        /**
        * Set the maximum size of the images that can be inserted in the message.
        *
        * @param[in] aHeight Image height in pixels.
        * @param[in] aWidth Image width in pixels.
        */
        virtual void SetMaximumImage( TInt aWidth, TInt aHeight );

        /**
        * Get the maximum size of the images that can be inserted in the message.
        *
        * The returned values are 0 if the maximum values have not been defined.
        * @param[out] aHeight image height in pixels
        * @param[out] aWidth image width in pixels
        */
        virtual void GetMaximumImage( TInt& aWidth, TInt& aHeight ) const;

        // -------------------------------------------------------------------
        // GENERAL MESSAGE INFORMATION METHODS

        /**
        * Get the message size. 
        *
        * SaveMessageL and LoadMessageL updates the value. This function returns 
        * the total amount of disk space the message takes. The actual message 
        * size in transmission is smaller due to binary encoding of the headers.
        *
        * @return size of all message parts in bytes including both attachments 
        *     and internal header structures.
        */
        virtual TInt32 MessageSize();

        /**
        * Set the message description string.
        * 
        * This provides a method to override the default message description. 
        * The next SaveMessageL saves the description text in the 
        * TMsvEntry::iDescription field. This field is shown in Message Centre  
        * message lists to describe the contents of the message. Normally it is  
        * the message subject, but if there is no subject in the message, the 
        * caller may set some text from a text part of the message as the 
        * description. 
        *
        * Note that this method does not check the text length, so avoid long 
        * descriptions to minimize memory usage.
        *
        * @param[in] aText Message description
        */
        virtual void SetMessageDescriptionL( const TDesC& aText );

        // ---------------------------------------------------------------------
        // FUNCTIONS TO HANDLE EXTRA MESSAGE ATTRIBUTES (FOR UI USE ONLY)

        /**
        * Add attribute to an attribute array (for the use of MMS UI only).
        *
        * No duplicates are allowed. If an attribute exists, its value is changed. 
        * The attributes and their values can be arbitrary strings. There are no 
        * restrictions. The purpose is to allow the UI to store some extra 
        * information with the message. The values of the attibutes are not included 
        * when the message is sent.
        * @param[in] aName Name of the attribute (case sensitive).
        * @param[in] aValue Value of the attribute.
        *
        * @leave KErrArgument if length of aName or aValue is 0.
        * @leave KErrNoMemory if memory runs out while adding the attribute.
        */
        virtual void AddAttributeL( const TDesC& aName, const TDesC& aValue );

        /**
        * Get value of an attribute (for the use of MMS UI only).
        *
        * @param[in] aName Name of the attribute (case sensitive).
        * @return Value of the attribute.
        * @leave KErrNotFound if attribute not found or the length of aName is 0.
        */
        virtual TPtrC GetAttributeL( const TDesC& aName );

        /**
        * Check if attribute is present (for the use of MMS UI only).
        *
        * @param[in] aName Name of the attribute (case sensitive).
        * @return ETrue if the attribute is found, EFalse otherwise.
        */
        virtual TBool FindAttribute( const TDesC& aName );

        /**
        * Delete named attribute from list (for the use of MMS UI only).
        *
        * @param[in] aName Name of the attribute (case sensitive).
        */
        virtual void DeleteAttribute( const TDesC& aName );

        /**
        * Reset all attributes (for the use of MMS UI only).
        *
        * Removes all attributes (names and values) from the message.
        */
        virtual void ResetAttributes();

        // -------------------------------------------------------------------
        // FUNCTIONS TO HANDLE MESSAGE ATTACHMENTS

        /**
        * Create attachment entry and copy specified file to message store.
        *
        * The user should call SaveMessageL after having added all attachments 
        *     to update TMsvEntry of the message entry.
        *
        * @param[in] aStore An open edit store for the message entry. 
        *     Caller must commit and close the store when ready. (Several 
        *     attachments can be added before committing the store.)
        * @param[in] aFile Open file handle, source of the attachment. 
        *     Caller must close the file afterwards.
        * @param[in] aMimeType Mime type (content type) of the attachmet 
        *     in format type/subtype, for example image/jpeg.
        * @param[in] aMimeHeaders Mime headers for the attachment. If the content 
        *     type is not defined in aMimeHeaders, the function adds the mime type 
        *     and subtype from aMimeType. Suggested filename in aMimeHeaders is 
        *     used as attachment name.
        * @param[in] aAttachmentInfo Attachment into structure, must be 
        *     initialized to CMsvAttachment::EMsvFile. If mime type is added 
        *     into the attachment info, it must be of format type/subtype, 
        *     for example image/jpeg. On return AttachmentInfo contains data 
        *     about the attachment. Ownership of attachmentinfo is transferred 
        *     to attachment manager, it must not be deleted by caller. It must 
        *     not be put on cleanup stack either. MMS engine keeps it safe until 
        *     the ownership has been transferred.
        * @param[out] aAttaId Attachment id of the newly created attachment.
        *
        * @pre A message entry must exist. It may be a new entry or an old entry 
        *     to be edited.
        * @pre CMsvMimeHeaders structure must have been filled in advantage. 
        *     The following values should be set:
        * - Content type, for example image 
        * - Content subtype, for example jpeg
        * - Character set IANA MIBEnum value, for example 106 (utf-8). Should be 
        *      defined only if the content type is text.
        * - Content-id if the presentation part refers to the attachments by 
        *      content-ids.
        * - Suggested filename (name only, no path), the name that should be 
        *      used to store the attachment and used as suggested filename 
        *      when sending the message. If the suggested filename is not set, the
        *      name of the attachment file will be used.
        * - Content-location if the presentation part refers to the attachments by 
        *      using content-location. The content-location string must contain only 
        *      us-ascii characters.
        * - X-type parameters (if needed). These are always handled as pairs of a 
        *      parameter name and parameter value. A descriptor at an even idex 
        *      in the array (0, 2, 4, ...) represents the parameter name and a 
        *      descriptor at an odd index (1, 3, 5, ...) represents the parameter 
        *      value. If a parameter has no value, it must be indicated by an empty 
        *      descriptor. The X-type parameter array must always contain an even 
        *      number of elements.
        *
        * @code
        * // The following code shows a short example of how the attachement
        * // creation proceeds.
        *
        * // Assume that the client entry is set to the message entry.
        * // Attachments are added to the message entry one by one
        * CMsvStore* store = iMmsClient->Entry().EditStoreL();
        * CleanupStack::PushL(store);
        *
        * CMsvAttachment* attaInfo = NULL;
        * TMsvAttachmentId attaId = 0;
        *
        * RFile attaFile;
        * // Set filename of attachment
        * TFileName name( _L("C:\\pictures\\picture123.jpg") );
        *
        * CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
        * CleanupStack::PushL( mimeHeaders );
        *
        * // Set values to mime headers
        * mimeHeaders->SetContentTypeL( _L8( "image") );
        * mimeHeaders->SetContentSubTypeL( _L8( "jpeg" ) );
        *
        * _LIT8(KMimeType, "image/jpeg");
        * // CreateAttachment2L will set the content type to attachment Info
        *
        * // Open the attachment file for reading
        * attaFile.Open( iFs, name, EFileShareReadersOnly | EFileRead );
        * CleanupClosePushL(attaFile);
        *
        * attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
        * // attaInfo ownerhip will be transferred to Attachment Manager.
        * // It must not be pushed onto the cleanupStack before calling 
        * // CreateAttachment2L.
        *
        * TMsvAttachmentId attaId = 0;
        *
        * iMmsClient->CreateAttachment2L(
        *     *store,   // edit store
        *     attaFile, // open file handle
        *     KMimeType, // combination type like image/jpeg
        *     *mimeHeaders,
        *     attaInfo,
        *     attaId);
        * // Now Attachment Manager owns the attaInfo
        * attaInfo = NULL;
        *
        * CleanupStack::PopAndDestroy(); // attaFile.Close()
        * CleanupStack::PopAndDestroy(); // mimeHeaders
        *
        * // Several attachments can be added before committing the store
        *
        * // Store must be committed before it is destroyed
        * store->CommitL();
        * CleanupStack::PopAndDestroy(); // store
        * @endcode
        */
        virtual void CreateAttachment2L(
            CMsvStore& aStore,
            RFile& aFile,
            TDesC8& aMimeType,
            CMsvMimeHeaders& aMimeHeaders,
            CMsvAttachment* aAttachmentInfo,
            TMsvAttachmentId& aAttaId);

        /**
        * Create a text/plain attachment.
        *
        * Creates a text attachment from text in a descriptor. 
        * Has option to convert all unicode paragraph separator marks to 
        *     line feeds. 
        * Converts text from unicode (ucs-2) to utf-8 before storing it.
        *
        * @param[in] aStore An open edit store. Caller must commit and close the 
        *     store (several attachments can be added before committing store).
        * @param[out] aAttachmentId Attachment id of the new attachment entry.
        * @param[in] aText Unicode text to be added as a text/plain attachment.
        * @param[in] aFile Suggested filename for the attachment.
        * @param[in] aConvertParagraphSeparator Flag that tells the function  
        *     to search for all 0x2029 characters (Unicode paragraph 
        *     separator) and to replace them with 0x000d 0x000a (carriage return, 
        *     line feed). 
        * Possible values:
        * - ETrue: Convert paragraph separators (default).
        * - EFalse: Do not convert paragraph separators.
        *
        * @pre A message entry must exist. It may be a new entry or an old entry 
        *     to be edited.
        *
        * @code
        *
        * TFileName attachmentFile( _L("story.txt") );
        *
        * CMsvStore* store = iMmsClient->Entry().EditStoreL();
        * CleanupStack::PushL(store);
        * TMsvAttachmentId attaId = 0;
        *
        * TBufC<12> story = _L( "Hello world!" );
        *
        * iMmsClient->CreateTextAttachmentL(
        *     *store,
        *     attaId,
        *     story,
        *     attachmentFile,
        *     ETrue )
        *
        * // When the call returns the id of the attachment will be strored in attaId
        *
        * // caller must commit the store as several attachments could be added berore
        * // committing the store.
        * store->CommitL();
        * CleanupStack::PopAndDestroy(); // store
        *
        * @endcode
        */
        virtual void CreateTextAttachmentL(
            CMsvStore& aStore,
            TMsvAttachmentId& aAttachmentId,
            const TDesC& aText,
            const TDesC& aFile,
            TBool aConvertParagraphSeparator = ETrue );

        // -------------------------------------------------------------------
        // MESSAGE HANDLING FUNCTIONS

        // NOTE: these are asynchronous functions

        /**
        * Send current message in the background.
        *
        * The message is automatically moved to Outbox folder before the 
        *     sending starts.
        *
        * @param[in] aCompletionStatus iStatus member of an active object. 
        *     It will be set as completed when the operating system has relayed 
        *     the request to the server side of Symbian Messaging System.
        * @param[in] aSendingTime Time at which the message is to be sent 
        *     given as UTC time. If aSending time is zero or in the past, the 
        *     message is scheduled to be sent as soon as possible.
        * @return Pointer to an operation active object. 
        *     The operation will complete when the sending has been successfuly 
        *     scheduled. The actual sending will happen in the background. 
        *     If scheduling the send fails, the status of CMsvOperation will 
        *     contain the relevant error code. The operation object must not 
        *     be deleted before it completes.
        *
        * @leave KErrNoMemory or other Symbian error code. If the function leaves  
        *     the owner of aCompletionStatus must not be set active because there 
        *     will be no pending request.
        */
        virtual CMsvOperation* SendL( TRequestStatus& aCompletionStatus,
            const TTime aSendingTime = TTime( 0 ) );

        /**
        * Send a selection of messages in the background.
        *
        * The messages are moved to Outbox folder before the sending starts. 
        *     All messages must be in the same place originally 
        *     (all in drafts, or all in outbox, for example).
        *
        * @param[in] aSelection List of messages to be sent.
        * @param[in] aCompletionStatus iStatus member of an active object. 
        *     It will be set as completed when the operating system has relayed 
        *     the request to the server side of Symbian Messaging System. 
        * @param aSendingTime Time at which the message selection is to be sent 
        *     given as UTC time. If aSending time is zero or in the past, the 
        *     message is scheduled to be sent as soon as possible.
        * @return Pointer to an operation active object. 
        *     The operation will complete when the sending has been successfully 
        *     scheduled. The actual sending will happen in the background. 
        *     If scheduling the send fails, the status of CMsvOperation will 
        *     contain the relevant error code. The operation object must not 
        *     be deleted before it completes.
        *
        * @leave KErrNotFound if aSelection is empty, or other Symbian error code. 
        *     If the function leaves the owner of aCompletionStatus must not be set  
        *     active because there will be no pending request.
        */
        virtual CMsvOperation* SendL(
            CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TTime aSendingTime = TTime( 0 ) );

        /**
        * Fetch pending MMS messages from MMS Service Centre to inbox.
        *
        * If there are notifications in postponed state they are all fetched. 
        * If there are notification in inbox, they are not touched.
        *
        * @param[in] aCompletionStatus iStatus member of an active object. 
        *     It will be set as completed when the operating system has relayed 
        *     the request to the server side of Symbian Messaging System.
        * @param[in] aForced indicates if the messages should be fetched 
        *     regardless of current mode settings.
        * - ETrue: User initiated fetch, use override.
        * - EFalse: Event triggered fetch, fetch only if settings allow.
        * @return Pointer to an operation active object. 
        *     The operation will complete when the retrieving has been successfuly 
        *     scheduled. The actual retrieving will happen in the background. 
        *     If scheduling the fetch fails, the status of CMsvOperation will 
        *     contain the relevant error code. The operation object must not 
        *     be deleted before it completes.
        *
        * @leave KErrNoMemory or other Symbian error code. If the function leaves 
        *     the owner of aCompletionStatus must not be set active because there 
        *     will be no pending request.
        *
        * @deprecated Postponed fetching mode is no longer supported by UI. In most 
        *     cases this function would not have any effect.
        */
        virtual CMsvOperation* FetchAllL( TRequestStatus& aCompletionStatus,
            TBool aForced = ETrue );
            
        /**
        * Send a read report to the sender of a message.
        *
        * This function should be called when a new message is opened and the 
        * sender of the message has specified that he wants a read report
        * for the message in question. This function should not be called 
        * if the settings indicate that sending read reports is not allowed.
        *
        * @param[in] aReadMessageId Id of the message for which a read report 
        *     should be sent. The message must not be locked and the caller 
        *     should not have CMsvStore open for the message as MMS Client Mtm 
        *     must be able to read header fields from the original message.
        * @param[in] aCompletionStatus iStatus member of an active object. 
        *     It will be set as completed when the operating system has relayed  
        *     the request to the server side of Symbian Messaging System.
        * @param[in] aReadStatus indicates if the message was read 
        *     Possible values:
        * - EMmsReadStatusRead: The message was read.
        * - EMmsReadStatusDeletedWithoutBeingRead: The message was deleted 
        *         without being read.
        * @return Pointer to an operation active object. 
        *     The operation will complete when the sending of the read report 
        *     has been successfully scheduled. The actual sending will happen 
        *     in the background. If scheduling the send fails, the status of 
        *     CMsvOperation will contain the relevant error code. 
        *     If the sender of the message has not requested a read report or 
        *     read report sending is not allowed, the operation completes 
        *     with error code KErrGeneral. 
        *     The operation object must not be deleted before it completes.
        *
        * @leave KErrLocked if the message entry cannot be accessed.
        */
        virtual CMsvOperation* SendReadReportL( TMsvId aReadMessageId,
            TRequestStatus& aCompletionStatus,
            TMmsReadStatus aReadStatus = EMmsReadStatusRead );
            
    public:  // FUNCTIONS FROM BASE CLASSES

        /**
        * From CBaseMtm: Return type of this Mtm.
        * @return Registered Mtm type.
        */
        inline TUid Type() const;


        // Context specific functions

        /**
        * From CBaseMtm: Set current context.
        * @param[in] aEntry Pointer to entry instance.
        */
        inline void SetCurrentEntryL( CMsvEntry* aEntry );

        /**
        * From CBaseMtm: Switch context to entry defined by aId.
        * @param[in] aId Entry id in message store.
        */
        inline void SwitchCurrentEntryL( TMsvId aId );

        /**
        * From CBaseMtm: Get reference to current entry.
        * @return Reference to entry instance.
        */
        inline CMsvEntry& Entry() const;

        /**
        * From CBaseMtm: Query if entry context has been set.
        * @return Status, possible values:
        * - ETrue:  Context has been set.
        * - EFalse: Context has not been set.
        */
        inline TBool HasContext() const;

        // Message specific functions

        /**
        * From CBaseMtm: Store current entry data.
        */
        void SaveMessageL();

        /**
        * From CBaseMtm: Restore current entry data.
        */
        void LoadMessageL();

        /**
        * From CBaseMtm: Checks that selected parts of current message are 
        *     legal.
        * @param[in] aPartList Flags specifying which parts to validate. 
        *     (defined in MTMDEF.H). Possible values:
        * - KMsvMessagePartPartBody: Ignored. MMS engine does not support 
        *       separate message body.
        * - KMsvMessagePartRecipient: Supported.
        * - KMsvMessagePartOriginator Supported.
        * - KMsvMessagePartDescription: Ignored. Description is always valid
        * - KMsvMessagePartDate: Ignored. Date is always valid
        * - KMsvMessagePartAttachments: Supported.
        * @return TMsvPartList bitmask identifies each invalid part. If all parts 
        *     are valid, returned value is 0.
        */
        TMsvPartList ValidateMessage( TMsvPartList aPartList );

        /**
        * From CBaseMtm: Searches for specified text in selected parts of 
        *     current message.
        * @param[in] aTextToFind Text to search for.
        * @param aPartList Flags specifying which parts to search. 
        *     (defined in MTMDEF.H). Possible values: 
        * - KMsvMessagePartPartBody: Ignored.
        * - KMsvMessagePartRecipient: Supported.
        * - KMsvMessagePartOriginator: Supported.
        * - KMsvMessagePartDescription: Supported.
        * - KMsvMessagePartDate: Ignored.
        * - KMsvMessagePartAttachments: Ignored.
        * @return TMsvPartList bitmask specifies in which of the specified parts the text
        *     was found.
        */
        TMsvPartList Find( const TDesC& aTextToFind, TMsvPartList aPartList );

        /**
        * From CBaseMtm: Send a reply to current message.
        *
        * @param[in] aDestination Id of the folder where the reply is generated.
        * @param[in] aPartlist Flags specifying which standard message parts
        *     are to be included in the response (defined in MTMDEF.H). Following 
        *     values are possible:
        * - KMsvMessagePartPartBody: Ignored.
        * - KMsvMessagePartRecipient: Causes reply-to-all. Otherwise reply-to-sender 
        *     only.
        * - KMsvMessagePartOriginator: Ignored.
        * - KMsvMessagePartDescription: Subject field is copied.
        * - KMsvMessagePartDate: Ignored.
        * - KMsvMessagePartAttachments: Ignored. Attachments are never copied to a reply.
        * @param[in] aCompletionStatus Status of an active object. This status 
        *     will be set as completed when the operation completes.
        * @return Pointer to an operation active object. The progress information 
        *     provides the id of the created message when the operation is complete. 
        *     If there was an error while creating the message, then the message 
        *     will be deleted and the result will contain a null id. 
        *     The operation object must not be deleted before it completes.
        */
        CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtm: Forward current message to new recipient.
        *
        * @param[in] aDestination Id of the folder where the new message 
        *     is generated. 
        * @param[in] aPartList Flags specifying which standard message parts 
        *     are to be included in the response. Possible values:
        * - KMsvMessagePartPartBody: Ignored.
        * - KMsvMessagePartRecipient: Ignored.
        * - KMsvMessagePartOriginator: Ignored.
        * - KMsvMessagePartDescription: Subject field is copied.
        * - KMsvMessagePartDate: Ignored.
        * - KMsvMessagePartAttachments: Ignored. Attachments are always 
        *       automatically included when forwarding a message.
        * @param[in] aCompletionStatus Status of an active object. This status 
        *     will be set as completed when the operation completes.
        * @return Pointer to an operation active object. The progress information 
        *     provides the id of the created message when the operation is complete. 
        *     If there was an error while creating the message, then the message 
        *     will be deleted and the result will contain a null id. 
        *     The operation object must not be deleted before it completes.
        */
        CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );

        /**
        * New recipient list function is not virtual, and the base MTM 
        * implementation must always be used. 
        * The function is shown here for reference only:
        *
        * const CMsvRecipientList& AddresseeList() const;
        */

        /**
        * From CBaseMtm: Adds an addressee, cannot distiguish To, Cc, and Bcc.
        *
        * New addresses are handled as To type of addresses. 
        * @param[in] aRealAddress Recipient address without alias.
        */
        void AddAddresseeL( const TDesC& aRealAddress );

        /**
        * From CBaseMtm: Adds an addressee, cannot distiguish To, Cc, and Bcc.
        *
        * New addresses are handled as To type of addresses.
        * @param[in] aRealAddress Recipient address.
        * @param[in] aAlias Descriptive name for the recipient.
        */
        void AddAddresseeL( const TDesC& aRealAddress, const TDesC& aAlias );

        /**
        * From CBaseMtm: Adds a typed addressee (To, Cc or Bcc).
        *
        * @param[in] aType recipient type. Possible values:
        * - EMsvRecipientTo: Normal recipient.
        * - EMsvRecipientCc: Recipient of a carbon copy.
        * - EMsvRecipientBcc: Recipient of a blind carbon copy.
        * @param[in] aRealAddress Address string without alias.
        */
        virtual void AddAddresseeL(
            TMsvRecipientType aType,
            const TDesC& aRealAddress);

        /**
        * From CBaseMtm: Adds a typed addressee (To, Cc or Bcc).
        *
        * @param[in] aType recipient type. Possible values:
        * - EMsvRecipientTo: Normal recipient.
        * - EMsvRecipientCc: Recipient of a carbon copy.
        * - EMsvRecipientBcc: Recipient of a blind carbon copy.
        * @param[in] aRealAddress Address string without alias.
        * @param[in] aAlias Descriptive name for the recipient.
        */
        virtual void AddAddresseeL(
            TMsvRecipientType aType,
            const TDesC& aRealAddress,
            const TDesC& aAlias);

        /**
        * From CBaseMtm: Removes an entry from addressee list.
        *
        * Cannot distinguish To, Cc and Bcc.
        * @param[in] aIndex Index to the array of addresses from 
        *     AddresseeList() function.
        */
        void RemoveAddressee( TInt aIndex );

        // Note: rich text body not supported in MMS Message encapsulation.

        /**
        * From CBaseMtm: Get rich text body of the message.
        *
        * MMS does not support separate message body. Body is ignored. 
        * All MMS message parts are attachments.
        * @return Rich text body from CBaseMtm.
        */
        inline CRichText& Body();

        /**
        * From CBaseMtm: Get rich text body.
        *
        * MMS does not support separate message body. Body is ignored. 
        * All MMS message parts are attachments.
        * @return Rich text body from CBaseMtm.
        */
        inline const CRichText& Body() const;

        /**
        * From CBaseMtm: Set message subject.
        * @param[in] aSubject Message subject.
        */
        void SetSubjectL( const TDesC& aSubject );

        /**
        * From CBaseMtm: Get message subject.
        * @return Message subject.
        */
        const TPtrC SubjectL() const;

        // General MTM-specific functionality

        /**
        * From CBaseMtm: Query capabilities of MTM.
        *
        * @param[in] aCapability UID specifying which capablity is queried. 
        *    For the possible Capability UIDs and types of return values 
        *    see mtmuids.h
        * @param[out] aResponse The value describing the capability at return.
        * @return error code, Possible values:
        * - KErrNone: Specified capability is supported and aResponse 
        *       contains the value of the capability if available.
        * - KErrNotSupported: Capability is not supported.
        */
        TInt QueryCapability( TUid aCapability, TInt& aResponse );

        /**
        * From CBaseMtm: Pass a request to MMS Server MTM.
        *
        * Pass a function code to Server MTM, wait until the 
        *     function returns. This function can be used to 
        *     invoke synchronous protocol-specific operations. 
        *     The supported functions are private and this function should 
        *     be called by MMS UI only.
        * @param[in] aFunctionId Enumeration constant defining the operation.
        * @param[in] aSelection Array of message entry ids to be operated on. 
        * @param[in] aParameter A descriptor that contains any parameters 
        *     required by function specified by aFunctionId.
        */
        void InvokeSyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter );

        /**
        * From CBaseMtm: Pass an asychronous request to Server MTM.
        *
        * Pass a function code to Server MTM. The operation will 
        *     run in the background. This function can be used to  
        *     invoke asynchronous protocol-specific operations. 
        *     The supported functions are private and this function should 
        *     be called by MMS UI only.
        * @param[in] aFunctionId Enumeration constant defining the operation.
        * @param[in] aSelection Array of message entry ids to be uperated on.
        * @param[in] aParameter A descriptor that contains any parameters 
        *     required by function specified by aFunctionId.
        * @param[in] aCompletionStatus Status of an active object. 
        *     This status will be set as completed when the operation completes
        * @return Pointer to a message server operation (active object). 
        */
        CMsvOperation*  InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtm: Return session that was set at initialization.
        * @return Reference to Message Server session object.
        */
        inline CMsvSession& Session();

        // Functions for SendAs support

        /**
        * From CBaseMtm: Add a file attachment to the current message entry.
        *
        * The attachment is referenced by its file path and is copied into the
        * message store. 
        * This function needs an edit store for the current entry. 
        * The caller should not keep the store open. 
        * The store is committed and closed after each attachment operation. 
        * Only one asynchronous operation can be running at any one time.
        *
        * If the file is a plain text file with ucs-2 character set MMS Engine 
        * will convert the character set to utf-8 and create a text attachment 
        * using this character set. The original file is not affected. This 
        * must be done because MMS text attachments should be sent using 
        * utf-8 character set.
        *
        * @param[in] aFilePath Full path specification of the attachment file.
        * @param[in] aMimeType Mime type of the attachment file.
        * @param[in] aCharset IANA MIBEnum of the character set of the attachment. 
        *        If character set is not relevant for current attachment type, 
        *        aCharset should be 0.
        * @param[in] aStatus The request status to complete.
        * @leave System-wide error codes.
        *
        * @code
        *
        * TFileName attachmentFile( _L("c:\\pictures\\picture123.jpg") );
        * TBufC8<20> mimeType = _L8( "image/jpeg" );
        * TUint charset = 0; // no character set needed for images
        *
        * CMsvOperationActiveSchedulerWait* wait = 
        *     CMsvOperationActiveSchedulerWait::NewLC();
        *
        * iMmsClient->AddAttachmentL(
        *     attachmentFile,
        *     mimeType,
        *     charset,
        *     wait->iStatus);
        *
        * wait->Start();
        *
        * if ( wait->iStatus.Int() != KErrNone )
        *     { 
        *     // error handling, e.g. leave
        *     }
        *
        * CleanupStack::PopAndDestroy(); // wait
        *
        * // The attachment has been added, store has been committed, and attachment data
        * // has been copied to the message store.
        * // If the original file is now modified, it does not affect the attachment file 
        * // in the message store any more.
        *
        * @endcode
        */
        void AddAttachmentL( const TDesC& aFilePath,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtm: Add a file attachment to the current message entry.
        *
        * The attachment is referenced by an open file handle and is copied
        * into the message store. 
        * This function needs an edit store for the current entry. 
        * The caller should not keep the store open. 
        * The store is committed and closed after each attachment operation. 
        *
        * If the file is a plain text file with ucs-2 character set MMS Engine 
        * will convert the character set to utf-8 and create a text attachment 
        * using this character set. The original file is not affected. This 
        * must be done because MMS text attachments should be sent using 
        * utf-8 character set.
        *
        * Only one asynchronous operation can be running at any one time.
        * @param[in] aFile An open file handle for the file attachment. The handle 
        *    is closed when the function completes.
        * @param[in] aMimeType Mime type of the attachment file.
        * @param[in] aCharset IANA MIBEnum of the character set of the attachment. 
        *        If character set is not relevant for current attachment type, 
        *        aCharset should be 0.
        * @param[in] aStatus The request status to complete.
        * @leave System-wide error codes.
        *
        * The function closes the file handle when done. The caller must not attempt 
        * to close the file handle afterwards.
        *
        * @code
        *
        * TFileName attachmentFile( _L("c:\\private\\privatedir\\picture123.jpg") );
        * RFile fileHandle;
        * TBufC8<20> mimeType = _L8( "image/jpeg" );
        * TUint charset = 0; // no character set needed for images
        *
        * fileHandle.Open( iFs, attachmentFile, EFileShareReadersOnly | EFileRead );
        * CleanupClosePush(fileHandle);
        *
        * CMsvOperationActiveSchedulerWait* wait = 
        *     CMsvOperationActiveSchedulerWait::NewLC();
        *
        * iMmsClient->AddAttachmentL(
        *     fileHandle,
        *     mimeType,
        *     charset,
        *     wait->iStatus);
        *
        * wait->Start();
        *
        * if ( wait->iStatus.Int() != KErrNone )
        *     { 
        *     // error handling, e.g. leave
        *     }
        *
        * CleanupStack::PopAndDestroy(); // wait
        * CleanupStack::Pop(); // file handle was closed if function did not leave
        *
        * @endcode
        */
        void AddAttachmentL( RFile& aFile,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtm: Add a file attachment to the current message entry 
        *     as a linked file.
        *
        * The attachment is referenced by its file path and is not copied 
        * into the message store. The attachment file is always used from 
        * its original location on disk indicated by the aFilePath 
        * parameter.
        *
        * This function needs an edit store for the current entry. 
        * The caller should not keep the store open. 
        * The store is committed and closed after each attachment operation. 
        * Only one asynchronous operation can be running at any one time. 
        * 
        * The file must be in some public directory so that MMS Engine can access 
        * the file when it is actually sent. If the file is a plain text attachment 
        * the character set cannot be converted to utf-8 as the original file cannot 
        * be changed. Text files should not be sent as linked attachmets unless the 
        * character set of the file is utf-8.
        *
        * @param[in] aFilePath Full path specification of the attachment file.
        * @param[in] aMimeType Mime type of the attachment file.
        * @param[in] aCharset IANA MIBEnum of the character set of the attachment. 
        *        If character set is not relevant for current attachment type, 
        *        aCharset should be 0.
        * @param[in] aStatus The request status to complete.
        * @leave System-wide error codes.
        */
        void AddLinkedAttachmentL( const TDesC& aFilePath,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtm: Add a message entry as an attachment to the current 
        *     message entry.
        *
        * Not supported. No Message attachments allowed in MMS.
        * @leave KErrNotSupported
        */
        void AddEntryAsAttachmentL( TMsvId aAttachmentId,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtm: Create an attachment and return an open file handle for it.
        *
        * This function needs an edit store for the current entry. 
        * The caller should not keep the store open. 
        * The store is committed and closed after each attachment operation. 
        * Only one asynchronous operation can be running at any one time.
        *
        * @param[in] aFileName Suggested filename.
        * @param[out] aAttachmentFile An open file handle for read/write 
        *     attachment file. The caller must close the handle.
        * @param[in] aMimeType Mime type of the attachment file.
        * @param[in] aCharset IANA MIBEnum of the character set of the attachment. 
        *        If character set is not relevant for current attachment type, 
        *        aCharset should be 0.
        * @param[in] aStatus The request status to complete.
        * @leave System-wide error codes.
        *
        * @code
        * TFileName attachmentFile( _L("picture123.jpg") );
        * RFile fileHandle;
        * TBufC8<20> mimeType = _L8( "image/jpeg" );
        * TUint charset = 0; // no character set needed for images
        *
        * CMsvOperationActiveSchedulerWait* wait = 
        *     CMsvOperationActiveSchedulerWait::NewLC();
        *
        * iMmsClient->CreateAttachmentL(
        *     attachmentFile,
        *     fileHandle,
        *     mimeType,
        *     charset,
        *     wait->iStatus);
        *
        * wait->Start();
        *
        * // When the function returns, the store has been committed
        *
        * // The attachment file handle is now open for writing the attachment data
        * CleanupClosePush(fileHandle);
        *
        * if ( wait->iStatus.Int() != KErrNone )
        *     { 
        *     // error handling, e.g. leave
        *     }
        *
        * // write file content to open handle
        * // ... 
        *
        * CleanupStack::PopAndDestroy(); // close file handle
        * CleanupStack::PopAndDestroy(); // wait
        *
        * @endcode
        */
        void CreateAttachmentL( const TDesC& aFileName,
            RFile& aAttachmentFile,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtm: Cancel the current attachment operation.
        */
        void CancelAttachmentOperation();

        // End of attachment functions to support SendAs

        /**
        * From CBaseMtm: Create an empty entry as the child of the current context.
        *
        * Sets the new entry as current context. 
        * The entry will be invisible and under construction.
        *
        * @param[in] aServiceId Service id for the new entry.
        *
        * @code
        * // Context must be set to parent folder for CreateMessageL
        * // This example creates the message to drafts folder
        *
        * TMsvId serviceId = iMmsClient->DefaultServiceL();
        * iMmsClient->SwitchCurrentEntryL( KMsvDraftEntryId );
        * iMmsClient->CreateMessageL( serviceId );
        *
        * // The message entry is invisible and in "In Preparation" state.
        * // The context of CMmsClientMtm has now been switched to the new message entry.
        * // The message entry is still completely empty.
        * // Continue by adding data to the message
        * // ...
        * @endcode
        */
        void CreateMessageL( TMsvId aServiceId );

        /**
        * From CBaseMtm: Inform Client MTM about bio type change.
        *
        * This function does nothing.
        */
        void BioTypeChangedL( TUid aBioTypeUid );

        /**
        * From CBaseMtm: Return id of default service for this MTM type.
        *
        * Only one MMS service is supported.
        * @return default service id.
        */
        TMsvId DefaultServiceL() const;

        /**
        * From CBaseMtm: Remove default service.
        *
        * Does nothing. Deletion of service not supported. 
        */
        void RemoveDefaultServiceL();

        /**
        * From CBaseMtm: Change default service.
        *
        * Does nothing. Changing of default service not supported. 
        */
        void ChangeDefaultServiceL(const TMsvId& aService);

    protected:  // New functions

        /**
        * Lists all visible and free MMS Notifications from inbox.
        * @return selection of Notifications in inbox.
        * @since 2.8
        */
        CMsvEntrySelection* ListNotificationsInInboxL();

    protected:  // Functions from base classes

        /**
        * From CBaseMtm: Called after the context of this instance
        * has been changed to another entry.
        */
        void ContextEntrySwitched();

        /**
        * From CBaseMtm: React to changes
        * @param[in] aEvent Code that tells which event has occurred. 
        *     Event codes defined in MSVAPI.H
        * @param[in] arg1 Depends on Event
        * @param[in] arg2 Depends on Event
        * @param[in] arg3 Depends on Event
        */
        void HandleEntryEventL(
            TMsvEntryEvent aEvent,
            TAny* arg1,
            TAny* arg2,
            TAny* arg3 );

        /**
        * By default Symbian OS constructor is private.
        * @param[in] aRegisteredMtmDll Reference to Mtm Dll registry class
        * @param[in] aSession Reference to a Message Server session.
        */
        CMmsClientMtm(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvSession& aSession );

        void ConstructL();

    private:

        /**
        * Build the iAddresseeList from the iMmsHeaders data.
        */
        void BuildAddresseeListL();

        /**
        * Add entries from the the specified array to iAddresseeList.
        * @param aArray recipient array.
        * @param aValue recipient type.
        */
        void BuildAddresseeListL(
            const CDesCArray& aArray, TMsvRecipientType aValue);

        /**
        * Attachments size
        * @return size of all attachments, binary data + mime headers.
        */
        TInt32 AttachmentsSizeL();

        /**
        * List notifications in MMS folder.
        * @return selection of notifications
        */
        CMsvEntrySelection* ListMmsFolderNotificationsL();

        /**
        * List notifications in inbox - only for mode switch fetch.
        * @return selection of notifications
        */
        CMsvEntrySelection* ListInboxNotificationsL();

        /**
        * Fetch messages corresponding to unhandled notifications in ibox
        * This function is needed only when the fetching mode is changed
        * @param[in] aCompletionStatus iStatus member of an active object.
        *     It will be set as completed when the request has finished.
        * @aparam[in] aForced indicates if the messages should be fetched
        *     regardless of current mode settings.
        *     ETrue = user initiated fetch, use override
        *     EFalse = event triggered fetch, fetch only if settings allow.
        * @return pointer to an operation active object.
        *     If successful, this is an asynchronously completing operation.
        *     If failed, this is a completed operation, with status set to
        *     the relevant error code.
        */
        CMsvOperation* FetchAllFromInboxL( TRequestStatus& aCompletionStatus,
            TBool aForced = ETrue );

        /**
        * Convert date time from UTC to local time.
        * @param aDate UTC date time
        * @return local time
        */
        // all times expressed in global time zone - no conversions
        /*
        TInt64 ConvertUTCDateToLocal( TInt64 aDate ) const;
        */

        /**
        * Find text from the given recipient list.
        * @param[in] aTextToFind a text to be searched
        * @param[in] aPartList message part list
        * @param[in] aRecipients the recipient list
        * @param[in] aFindText CMsvFindText object to help in the find
        * @return ETrue if match found.
        */
        TBool FindInRecipientL(
            const TDesC& aTextToFind,
            TMsvPartList aPartlist,
            const CDesCArray& aRecipients,
            CMsvFindText& aFindText );

        /**
        * Add an attachment from public location either as copied file or
        *     linked file.
        * @param[in] aFilePath The full path specification of the attachment file.
        * @param[in] aMimeType The mime type of the attachment file.
        * @param[in] aType CMsvAttachment::EMsvFile or
        *     CMsvAttachment::EMsvLinkedFile
        * @param[in] aStatus The request status to complete when request has
        *     completed.
        * @param[in] aCharacter set IANA MIBEnum of the character set for the
        *     attachment if needed.
        */
        void AddFilePathAttachmentL(const TDesC& aFilePath,
            const TDesC8& aMimeType,
            CMsvAttachment::TMsvAttachmentType aType,
            TRequestStatus& aStatus,
            const TUint aCharacterSet = 0 );

        /**
        * Store attribures to attribute stream in message entry
        * @param[in] aStore message store
        */
        void StoreAttributesL( CMsvStore& aStore );

        /**
        * Restore attribures from attribute stream in message entry
        * @param[in] aStore message store
        */
        void RestoreAttributesL( CMsvStore& aStore );

        /**
        * Checks whether given sample is UTF16 text.
        * @since    3.0
        * @param[in]    aSample text sample
        * @return   1015 if the sample starts with unicode BOM
        *           (unicode with explicit byte mark)
        *           0 otherwise
        */
        TUint GetUnicodeCharacterSet( TDesC8& aSample );

        /**
        * Reads bytes from so that sample is full or if file is shorter than
        *     sample then read whole file.
        * @since    3.0
        * @param[in]    aFile  open file handle to read bytes from
        * @param[out]   aSample sample buffer filled with data
        */
        void ReadBytesFromFileL( const RFile aFile, TDes8& aSample );

        /**
        * Tries to recognize character set from given sample.
        * @since    3.0
        * @param[in]   aFile  File to be recognized
        * @return   CharConv UID of the character set
        */
        TUint RecognizeCharSetL( RFile& aFile );


    public:     // Data

    protected:  // Data
        CMmsSettings* iMmsSettings;  // MMSC settings (access point etc.)

        CMmsHeaders*  iMmsHeaders;   // MMS message headers
        TMsvId        iServiceId;    // last selected service
        TBool         iFetchAll;     // All the messages are fetched when
                                     // settings are saved after certain fetch
                                     // mode change.
        TBool         iFetchOverride; // force fetching all messages.
        TInt          iMessageDrive; // messages are on C: drive by default,
                                     // may be moved to other drive
        TInt32        iHomeMode;     // receiving mode in the home network
        TInt32        iRoamingMode;  // receiving mode when roaming
        TInt          iAccessPointCount; // number of access points
        CDesCArrayFlat* iAttributes;     // zero or more attributes for UI.
                                         // Name, value pairs
    	CMsvSession& iOwnSession;    // copy of session because base class session is private


    private:    // Data
        // active object that commits the store when attachment operation
        // is complete
        CMmsAttachmentWaiter* iAttaWaiter;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// panic function
GLREF_C void gPanic( TMmsPanic aPanic );

#include "mmsclient.inl"

#endif      // MMSCLIENT_H

// End of File

