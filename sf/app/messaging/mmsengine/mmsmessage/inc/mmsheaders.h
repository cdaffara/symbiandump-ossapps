/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mmsheaders  declaration
*
*/


 
#ifndef CMMSHEADERS_H
#define CMMSHEADERS_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <mtclbase.h>       // TMsvPartList
#include "mmsconst.h"
#include "mmsgenutils.h"
#include "mmssendingchain.h"
#include "mmsdeleteresultarray.h"

// CONSTANTS
// Maximum size for Multimedia message field string value.
const TInt KMaxHeaderStringLength = 1000;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsSettings;
class CMmsElementDescriptor;
class CMmsMMBoxMessageHeaders;
class CMmsMMBoxViewHeaders;

// CLASS DECLARATION

/**
*  This class provides access to MMS specific header data.
*  The class can be used from both Client MTM and Server MTM.
*/
class CMmsHeaders:public CBase
    {
    public:  // Constructors and destructor
        
// <DEPRECATED>
        /**
        * Two-phased constructor.
        * This is a legacy function and sets the default mms version
        * to a hard coded value.
        * All client should use the NewL version that gives the version
        * as parameter. The version value should come form CMmsSettings class
        */
        IMPORT_C static CMmsHeaders* NewL();
// </DEPRECATED>
        
        /**
        * Two-phased constructor.
        * @param aMmsVersion The version of MMS encapsulation that should be
        *     used unless the actual message contains some other value
        */
        IMPORT_C static CMmsHeaders* NewL( TInt16 aMmsVersion );
        
        /**
        * Destructor.
        */
        virtual ~CMmsHeaders();

    public: // New functions
        
        /**
        * Reset.
        * @param aSettings MMS settings to be used for initial values. Can be NULL.
        */
        IMPORT_C void Reset( CMmsSettings* aSettings = NULL);

        /**
        * Internalize the headers.
        * @param aStore CMsvStore
        */
        IMPORT_C void RestoreL( CMsvStore& aStore );

        /**
        * Externalize the headers.
        * Caller must commit the store to allow several store operations
        *     before committing the store
        * @param aStore CMsvStore in edit mode.
        */
        IMPORT_C void StoreL( CMsvStore& aStore );

        /**
        * Subject mutator. Note that Client MTM should update explicitly 
        * TMsvEntry.iDescription too.
        * @param aSubject subject string,
        */
        IMPORT_C void SetSubjectL( const TDesC& aSubject );

        /**
        * Subject accessor.
        * @return Subject string.
        */
        IMPORT_C TPtrC Subject() const;

        /**
        * Returns recipients that are of specified type (To, Cc, Bcc).
        * @param aType specifies recipient type (types defined in MMSCONST.H)
        * @return array of recipient addresses.
        */
        IMPORT_C const CDesCArray& TypedAddresseeList( TMmsRecipients aType );

        /**
        * Adds recipient with specified type (To, Cc, Bcc).<br>
        * This function also adds all recipients into the internal list <br>
        * that contains all addressees regardless of addressee type.<br>
        * @param aRealAddress recipient address.
        * @param aType specifies recipient type (types defined in MMSCONST.H)
        */
        IMPORT_C void AddTypedAddresseeL( const TDesC& aRealAddress, 
                                          TMmsRecipients aType );

        /**
        * Remove a given address from the right typed list.
        * @param aRealAddress recipient address.
        * @return ETrue is address is found and deleted.
        */
        IMPORT_C TBool RemoveAddressee( const TDesC& aRealAddress );

        /**
        * Sender mutator
        * @param aAlias
        */
        IMPORT_C void SetSenderL( const TDesC& aAlias );

        /**
        * Sender accessor
        * @return Real address of sender (for example phone number). <br>
        *     Alias can be searched from contacts database.
        */
        IMPORT_C TPtrC Sender() const;

        /**
        * Message class accessor
        * @return Message class: Personal/Advertisement/Informational. <br>
        *     values defined in MMSCONST.H
        */
        inline TInt MessageClass() const;

        /**
        * Message class mutator
        * values defined in MMSCONST.H
        */
        inline void SetMessageClass( TInt aClass );

        /**
        * Expiration interval mutator
        * @param aInterval The length of time after which the message
        *     is discarded by MMSC. Expressed in seconds.
        */
        IMPORT_C void SetExpiryInterval( TInt aInterval );

        /**
        * Expiration interval accessor.
        * @return Storage time of the message in MMSC (in seconds).
        */
        IMPORT_C TInt ExpiryInterval();

        /**
        * Expiration date mutator.
        * @param aDate The date and time the message will expire in seconds from 1970-01-01.
        *     Date must be in UTC time zone
        */
        IMPORT_C void SetExpiryDate( TInt64 aDate );

        /**
        * Expiration date accessor.
        * @return The date and time the message will expire in seconds from 1970-01-01.
        *     Date will be in UTC time zone
        */
        IMPORT_C TInt64 ExpiryDate();

        /**
        * Delivery time interval mutator.
        * @param aInterval The length of time after which the message will<br>
        *     be delivered to the recipient by the MMSC.<br>
        *     Expressed in seconds.
        */
        IMPORT_C void SetDeliveryTimeInterval( TInt aInterval );

        /**
        * Delivery time interval accessor.
        * @return delivery time interval of the message (in seconds).
        */
        IMPORT_C TInt DeliveryTimeInterval();

        /**
        * Delivery date mutator.
        * @param aDate The date and time the message will be delivered <br>
        *     to the recipient by the MMSC in seconds from 1970-01-01.
        *     Date must be in UTC time zone
        */
        IMPORT_C void SetDeliveryDate( TInt64 aDate );

        /**
        * Delivery date accessor.
        * @return The date and time the message will be delivered to the
        *     recipient by the MMSC in seconds from 1970-01-01.
        *     Date will be in UTC time zone
        */
        IMPORT_C TInt64 DeliveryDate();

        /**
        * Message priority mutator (Low/Normal/High). Default: Normal.
        * @param aPriority Message priority, values defined in MMSCONST.H
        */
        inline void SetMessagePriority( TInt aPriority );

        /**
        * Message priority accessor.
        * @return Message priority, values defined in MMSCONST.H
        */
        inline TInt MessagePriority() const;

        /**
        * Sender visibility mutator.
        * @param aVisibility Hide or Show (values defined in MMSCONST.H)
        */
        inline void SetSenderVisibility( TInt aVisibility );

        /**
        * Sender visibility accessor.
        * @return Hide or Show (values defined in MMSCONST.H)
        */
        inline TInt SenderVisibility();

        /**
        * Delivery Report Request mutator.
        * @param aRequest Yes or No (values defined in MMSCONST.H)
        */
        inline void SetDeliveryReport( TInt aRequest );

        /**
        * Delivery Report Request accessor.
        * @return Yes or No (values defined in MMSCONST.H)
        */
        inline TInt DeliveryReport();

        /**
        * Read Reply Request mutator.
        * @param aRequest Yes or No (values defined in MMSCONST.H)
        */
        inline void SetReadReply( TInt aRequest );

        /**
        * Read Reply Request accessor.
        * @return Yes or No (values defined in MMSCONST.H)
        */
        inline TInt ReadReply();

        /**
        * Message root part mutator.
        * @param aId Id of the message part that should be displayed
        *     first.<br>
        *     Should point to the SMIL part that starts the message display.
        */
        inline void SetMessageRoot( const TMsvAttachmentId aId );

        /**
        * Message root part accessor.
        * @return Id of the message part that should be displayed
        *     first.<br>
        *     Should point to the SMIL part that starts the message display.
        */
        inline TMsvAttachmentId MessageRoot( ) const;
               
        /** 
        * Give const access to the "To" recipient list
        * @return recipient list
        */
        inline const CDesCArray& ToRecipients() const;

        /** 
        * Give const access to the "Cc" recipient list
        * @return recipient list
        */
        inline const CDesCArray& CcRecipients() const;

        /** 
        * Give const access to the "Bcc" recipient list
        * @return recipient list
        */
        inline const CDesCArray& BccRecipients() const;
        
        /**
        * Content-Location mutator. Used in a Message Notification.
        * @param aURL URL of the message.
        */
        IMPORT_C void SetContentLocationL( const TDesC8& aURL );

        /**
        * Content-Location accessor. Used in a Message Notification.
        * @return URL of the message
        */
        IMPORT_C TPtrC8 ContentLocation() const;

        /**
        * Set Message-Size header.
        * @param aSize Message-Size in bytes. Used in a Message Notification.
        */
        inline void SetMessageSize( TInt aSize );

        /**
        * Get Message-Size. Used in a Message Notification.
        * @return Message-Size in bytes.
        */
        inline TInt MessageSize() const;

        /**
        * Size of the MMS headers object in memory. 
        * The multipart objet size is not included.
        * @return object size
        */
        IMPORT_C TInt Size() const;

        /**
        * Set Transaction-ID
        * @param aCid transaction ID
        */
        IMPORT_C void SetTidL( const TDesC8& aCid ); 

        /**
        * Get Transcation-ID
        * @return transaction id
        */
        IMPORT_C TPtrC8 Tid() const;

        /**
        * Set Message-Type
        * @param aType message type 
        */
        inline void SetMessageType( TInt aType ); 

        /**
        * Get Message-Type
        * @return message type.
        */
        inline TInt MessageType();

        /**
        * Set MMS-Version
        * @param aVersion MMS version
        */
        inline void SetMmsVersion( TInt16 aVersion );

        /**
        * Get MMS-Version
        * @return MMS-Version
        */
        inline TInt16 MmsVersion();

        /**
        * Set Date
        * @param aTime seconds from 1.1. 1970
        *     Date must be in UTC time zone
        */
        inline void SetDate( TInt64 aTime );

        /**
        * Get Date
        * @return seconds from 1.1. 1970
        *     Date will be in UTC time zone
        */
        inline TInt64 Date();

        /**
        * Report-Allowed accessor
        * @return Report-Allowed field value 
        */
        inline TInt32 ReportAllowed() const;

        /**
        * Report-Allowed mutator
        * @param aValue Report-Allowed value
        */
        inline void SetReportAllowed( TInt32 aValue );

        /**
        * Response-Status accessor
        * @return Response-Status field value 
        */
        inline TInt32 ResponseStatus() const;

        /**
        * Response-Status mutator
        * @param aValue Response-Status value
        */
        inline void SetResponseStatus( TInt32 aValue );

        /**
        * Message-ID mutator
        * @param aID Message-ID
        */
        IMPORT_C void SetMessageIdL( const TDesC8& aId ); 

        /**
        * Message-ID accessor
        * @return Message-ID
        */
        IMPORT_C TPtrC8 MessageId() const;

        /**
        * Status mutator
        * @param aStatus status value. See mmsconst.h.
        */
        inline void SetStatus( const TInt aStatus );

        /**
        * Status accessor
        * @return status value. See mmsconst.h.
        */
        inline TInt Status() const;

        // DEPRECATED
        // Legacy function, new version needs RFs (file system handle)
        /**
        * Make a copy of MmsHeaders
        * @param aParts message part list
        * @param aReply if the new message is a reply message. This has meaning
        *               for recipient handling.
        * @return MmsHeader object
        */
        IMPORT_C CMmsHeaders* CopyL( 
            TMsvPartList aParts,
            TBool aReply );

        /**
        * Set initial setting values from service settings
        * @param aSettings MMS service settings. Optional.
        */
        IMPORT_C void SetSettings(
            CMmsSettings* aSettings);

        /**
        * Maximum image size mutator.
        * @param aWidth image width
        * @param aHeight image height
        */
        IMPORT_C void SetMaximumImage( TInt aWidth, TInt aHeight );

        /**
        * Maximum image size accessor.
        * @param aWidth image width (OUT)
        * @param aHeight image height (OUT)
        */
        IMPORT_C void GetMaximumImage( TInt& aWidth, TInt& aHeight ) const;

        /**
        * Root Content-ID mutator.
        * @param aID Content-ID for message presentation part 
        */
        IMPORT_C void SetRootContentIdL( const TDesC8& aId ); 

        /**
        * Root Content-ID accessor.
        * @return Content-ID for message presentation part 
        */
        IMPORT_C TPtrC8 RootContentId() const;

        // new for MMS encapsulation version 1.1
        /**
        * Read status mutator.
        * @param aReadStatus status value. See mmsconst.h.
        */
        inline void SetReadStatus( const TInt aReadStatus );

        /**
        * Read status accessor.
        * @return Read status field value.
        */
        inline TInt ReadStatus() const;

        /**
        * ReplyCharging mutator.
        * @param aReplyCharging value. See mmsconst.h
        */
        inline void SetReplyCharging( const TInt aReplyCharging );

        /**
        * ReplyCharging accessor.
        * @return ReplyCharging status field
        */
        inline TInt ReplyCharging() const;

        /**
        * ReplyChargingSize mutator.
        * @param aReplyChargingSize maximum size of message for which
        *     reply charging applies.
        */
        inline void SetReplyChargingSize( const TInt aReplyChargingSize );

        /**
        * ReplyChargingSize accessor.
        * @return maximum size of message for which reply charging applies.
        */
        inline TInt ReplyChargingSize() const;

        /**
        * ReplyChargingID mutator.
        * @param aId original message ID to which this is a reply to
        */
        IMPORT_C void SetReplyChargingIdL( const TDesC8& aId ); 
         
        /**
        * ReplyChargingId accessor.
        * @return original message ID to which this is a reply to
        */
        IMPORT_C TPtrC8 ReplyChargingId() const;

        /**
        * ReplyCharging interval mutator.
        * @param aInterval The length of time during which the sender
        *     pays charges for reply. Expressed in seconds.
        */
        IMPORT_C void SetReplyChargingInterval( TInt aInterval );

        /**
        * ReplyCharging interval accessor.
        * @return The length of time during which the sender
        *     pays charges for reply. Expressed in seconds.
        */
        IMPORT_C TInt ReplyChargingInterval();

        /**
        * ReplyCharging date mutator.
        * @param aDate The date and time until which the sender pays charges
        * for reply in seconds from 1970-01-01.
        *     Date must be in UTC time zone
        */
        IMPORT_C void SetReplyChargingDate( TInt64 aDate );

        /**
        * ReplyCharging date accessor.
        * @return The date and time until which the sender pays charges for reply
        * in seconds from 1970-01-01.
        *     Date will be in UTC time zone
        */
        IMPORT_C TInt64 ReplyChargingDate();

        /*
        * Insert previous sender.
        * If corresponding entry is found, just insert sender to entry,
        * otherwise create new CMmsSendingChain item and insert it into array
        * @param aOrder the order number of the sending event
        * @param aSender the previous sender.
        */
        IMPORT_C void InsertPreviouslySentByL( TInt aOrder, const TDesC& aSender );

        /*
        * Insert previous sending date.
        * If corresponding entry is found, just inseret date to entry,
        * otherwise create new CMmsSendingChain item and insert it into array
        * @param aOrder the order number of the sending event
        * @param aDate the previous sending date, seconds from 1.1.1970 (UTC time).
        */
        IMPORT_C void InsertPreviouslySentDateL( TInt aOrder, TInt64 aDate );

        /*
        * Append new item to previously sent list
        * @param aDate the previous sending date, seconds from 1.1.1970 (UTC time).
        * @param aSender the previous sender.
        */
        IMPORT_C void AppendPreviouslySentItemL( TInt64 aDate, const TDesC& aSender );

        /*
        * Accessor to previously sent chain for looking at the content.
        * The items should normally be accessed in sequential order by outsiders.
        * @return reference to the sending chain array.
        */
        inline const RPointerArray<CMmsSendingChain>& PreviouslySentList();

        /**
        * Response text mutator.
        * Same string is used for Response text, Retrieve text and Status text
        * as all strings are information messages coming in from the MMSC and
        * only one of them can be present in any PDU
        * @param aText Response text string
        */
        IMPORT_C void SetResponseTextL( const TDesC& aText );

        /**
        * Response text accessor.
        * Same string is used for Response text, Retrieve text and Status text
        * as all strings are information messages coming in from the MMSC and
        * only one of them can be present in any PDU
        * @return Response text string.
        */
        IMPORT_C TPtrC ResponseText() const;

        /**
        * Creates a ContentId to be used in mime headers
        * @param aCid the created contentId (OUT)
        *     The caller must reserve a buffer that is at least 12 bytes long
        *     ( 10 bytes for a 32 bit integer converted to a string + angle brackets )
        */
        IMPORT_C static void CreateContentId( TDes8& aCid );

        /**
        * Distribution indicator accessor 
        * @since 2.6
        * @return X-Mms-DistributionIndicator value KMmsYes/KMmsNo
        */
        inline TInt DistributionIndicator() const;

        /**
        * Distribution indicator mutator
        * @since 2.6
        * @param aDistributionIndicator X-Mms-DistributionIndicator value KMmsYes/KMmsNo
        */
        inline void SetDistributionIndicator( const TInt aDistributionIndicator );

        /**
        * Related entry accessor
        * @since 2.6
        * @return Id of an entry that is connected to this one by some relationship
        * For example original notification entry in case of forward without retrieval
        */
        inline TMsvId RelatedEntry() const;

        /**
        * Related entry mutator
        * @since 2.6
        * @param aRelatedEntry Id of an entry that is connected to this one by some relationship
        */
        inline void SetRelatedEntry( const TMsvId aRelatedEntry );

        /**
        * Multipart type accessor
        * @since 2.6
        * @return type of multipart data saved to this entry (WSP assigned number)
        * 0 = unknown or not defined
        */
        inline TInt MultipartType() const;

        /**
        * Multipart type mutator
        * @since 2.6
        * @param aMultipartType type of multipart data saved to this entry (WSP assigned number)
        */
        inline void SetMultipartType( const TInt aMultipartType );

        // Read only accessors should be used when one must determine if data exists.
        // Editable accessors should be used when data will be stored to the members
        // No mutators are defined, these members are fully under control of CMmsHeaders

        /**
        * Element descriptor accessor for read only usage.
        * @since 2.6
        * @return pointer to element descriptor member for read only access.
        *     If no data has been defined for element descriptor, returns NULL.
        */
        inline const CMmsElementDescriptor* ReadOnlyElementDescriptor() const;

        /**
        * MMBox message headers accessor for read only usage
        * @since 2.6
        * @return pointer to MMBox message headers member for read only access.
        *     If no data has been defined for MMBox message headers, returns NULL.
        */
        inline const CMmsMMBoxMessageHeaders* ReadOnlyMMBoxMessageHeaders() const;

        /**
        * MMBox view headers accessor for read only usage
        * @since 2.6
        * @return pointer to MMBox view headers member for read only access.
        *     If no data has been defined for MMBox view headers, returns NULL.
        */
        inline const CMmsMMBoxViewHeaders* ReadOnlyMMBoxViewHeaders() const;

        /**
        * Element descriptor accessor for edit usage.
        * @since 2.6
        * @return reference to element descriptor member for edit access.
        *     If member pointer does not exist, it is created.
        *     This function must be used when data is to be added to CMmsElementDescriptor.
        */
        IMPORT_C CMmsElementDescriptor& ElementDescriptorL();

        /**
        * MMBox message headers accessor for edit usage
        * @since 2.6
        * @return reference to MMBox message headers member for edit access.
        *     If member pointer does not exist, it is created.
        *     This function must be used when data is to be added to CMmsMMBoxMessageHeaders.
        */
        IMPORT_C CMmsMMBoxMessageHeaders& MMBoxMessageHeadersL();

        /**
        * MMBox view headers accessor for edit usage
        * @since 2.6
        * @return reference to MMBox view headers member for edit access.
        *     If member pointer does not exist, it is created.
        *     This function must be used when data is to be added to CMmsMMBoxViewHeaders.
        */
        IMPORT_C CMmsMMBoxViewHeaders& MMBoxViewHeadersL();

        /**
        * Extended Notification mutator.
        * @since 2.6
        * @param aText Extended notification text.
        */
        IMPORT_C void SetExtendedNotificationL( const TDesC& aText );

        /**
        * Extended Notification accessor.
        * @since 2.6
        * @return Extended notification text.
        */
        IMPORT_C TPtrC ExtendedNotification() const; 

        /**
        * Message complete indicator mutator
        * @since 2.6
        * @param aExtensionStatus indicator that tells if the  whole message fits into
        *     extension text string.
        *     0x20 = Message body size > text, message retrieval uncomplete
        *     0x30 = Message retrieval complete (whole message body in IExtendedNotificationText)
        *     other values reserved.
        */
        inline void SetMessageComplete( TInt aExtensionStatus );

        /**
        * Message complete indicator accessor.
        * @since 2.6
        * @return status that tells if the  whole message fits into extension text string.
        *     0x20 = Message body size > text, message retrieval uncomplete
        *     0x30 = Message retrieval complete (whole message body in IExtendedNotificationText)
        *     other values reserved.
        */
        inline TInt MessageComplete();

        /**
        * Accessor to delete result array for looking at the content.
        * The items should normally be accessed in sequential order by outsiders.
        * @return reference to the delete result array.
        */
        inline const RPointerArray<CMmsDeleteResultArray>& DeleteResultArray();

        /**
        * Insert delete status.
        * If corresponding entry is found, just insert delete status to entry,
        * otherwise create new CMmsDeleteResultArray item and insert it into array
        * @param aOrder the order number of the not deleted entry
        * @param aStatus the previous sender.
        */
        IMPORT_C void InsertDeleteStatusL( TUint aOrder, const TInt32 aStatus );

        /**
        * Insert delete result content location.
        * If corresponding entry is found, just insert content location to entry,
        * otherwise create new CMmsDeleteResultArray item and insert it into array
        * @param aOrder the order number of the not deleted entry
        * @param aContentlocation content location.
        */
        IMPORT_C void InsertDeleteContentLocationL( TUint aOrder, const TDesC8& aContentLocation );

        /**
        * Insert delete result response text.
        * If corresponding entry is found, just insert response text to entry,
        * otherwise create new CMmsDeleteResultArray item and insert it into array
        * @param aOrder the order number of the not deleted entry
        * @param aResponseText content location.
        */
        IMPORT_C void InsertDeleteResponseTextL( TUint aOrder, const TDesC& aResposeText );
        
        /**
        * Save the time when the message was received.
        *     This header allows the iDate field in TMsvEntry to contain
        *     either the receiving time or the sending time. If the TMsvEntry
        *     shows the sending time, the receiving time can be accessed through
        *     this field.
        * @param aTime time in UTC time zone when the message has been received.
        */
        inline void SetReceivingTime( TTime aTime );
        
        /**
        * The time the message was received in the terminal.
        * @return Message receiving time
        */
        inline TTime ReceivingTime() const; 
        
        /**
        * Identification of the destination application mutator.
        * @param aApplicId application ID
        */
        IMPORT_C void SetApplicIdL( const TDesC16& aApplicId );

        /**
        * Identification of the destination application accessor.
        * @return Application ID.
        */
        IMPORT_C TPtrC16 ApplicId() const;
        
        /**
        * Identification of an application to which replies are addressed mutator.
        * @param aApplicId reply to application ID
        */
        IMPORT_C void SetReplyApplicIdL( const TDesC16& aApplicId );

        /**
        * Identification of an application to which replies are addressed accessor.
        * @return Reply to Application ID.
        */
        IMPORT_C TPtrC16 ReplyApplicId() const;
        
        /**
        * Auxiliary application information for internal use of the destination
        * application mutator.
        * @param aAuxApplicInfo auxiliary application info
        */
        IMPORT_C void SetAuxApplicInfoL( const TDesC8& aAuxApplicInfo );

        /**
        * Auxiliary application information for internal use of the destination
        * application accessor.
        * @return auxiliary application info.
        */
        IMPORT_C TPtrC8 AuxApplicInfo() const;

        /**
        * Make a copy of MmsHeaders
        *
        * @since 3.1
        * @param aParts message part list
        * @param aReply if the new message is a reply message. This has meaning
        *               for recipient handling.
        * @param aFs file system handle. Needed for alias handling
        * @return MmsHeader object
        */
        IMPORT_C CMmsHeaders* CopyL( 
            TMsvPartList aParts,
            TBool aReply,
            RFs& aFs );
            
        /**
        * Set the content class of the message  as specified in MMS encapsulation 1.3-
        * @param aContentClass content class value from mmsconst.h
        * - KMmsContentClassText
        * - KMmsContentClassImageBasic
        * - KMmsContentClassImageRich
        * - KMmsContentClassVideoBasic
        * - KMmsContentClassVideoRich
        * - KMmsContentClassMegaPixel
        * - KMmsContentClassContentBasic
        * - KMmsContentClassContentRich
        */
        inline void SetContentClass( TInt aContentClass );
        
        /**
        * Return the content class value as specified in MMS encapsulation 1.3.
        * @return Content class value
        * - 0 content class not defined
        * - KMmsContentClassText ... KMmsContentClassContentRich defined content class.
        */
        inline TInt ContentClass() const;
        
        /**
        * Set DRM Content indicator header.
        * @param aDrmContent Messag has DRM content. Possible values:
        * - KMmsYes
        * - KMmsNo
        */
        inline void SetDrmContent( TInt aDrmContent );
        
        /**
        * Get DRM content indicator (encapsulation 1.3)
        * @return DRM indicator header. Possible values:
        * - KMmsYes
        * - KMmsNo
        */
        inline TInt DrmContent() const;
        
        /**
        * Set the header that tells if MMSC is allowed to alter the message (make it smaller).
        * @param aAdaptationAllowed Adaptation allowed header. Possible values:
        * - KMmsYes
        * - KMmsNo
        */
        inline void SetAdaptationAllowed( TInt aAdaptationAllowed );
        
        /**
        * Get header that tells if MMSC is allowed to alter the message (make it smaller).
        * @return Adaptation allowed header. Possible values:
        * - KMmsYes
        * - KMmsNo
        */
        inline TInt AdaptationAllowed() const;
        
        /**
        * Set recommended retrieval mode.
        * This header appears in a notification and may recommend manual retrieval.
        * @param aRetrievalMode Suggested retrieval mode. Possible values
        * - KMmsRecommendedRetrievalModeManual
        */
        inline void SetRecommendedRetrievalMode( TInt aRetrievalMode );
        
        /**
        * Get recommended retrieval mode
        * This header appears in a notification and may recommend manual retrieval.
        * @return Suggested retrieval mode. Possible values
        * - KMmsRecommendedRetrievalModeManual
        * 0 not defined
        */
        inline TInt RecommendedRetrievalMode() const;
        
        /**
        * Set explanation why retrival mode is recommended
        * @param explanantion for recommended retrieval mode
        */
        IMPORT_C void SetRecommendedRetrievalModeTextL(
            const TDesC16& aRecommendedRetrievalModeText );

        /**
        * Get explanation why retrival mode is recommended
        * @return explanantion for recommended retrieval mode
        */
        IMPORT_C TPtrC16 RecommendedRetrievalModeText() const;

        /**
        * Set id of message to be replaced or cancelled
        * @param id of message to be replaced or cancelled
        */
        IMPORT_C void SetReplaceCancelIdL( const TDesC8& aReplaceCancelId );

        /**
        * Get id of message to be replaced or cancelled
        * @return id of message to be replaced or cancelled
        */
        IMPORT_C TPtrC8 ReplaceCancelId() const;
        
        /**
        * Set status of cancel operation
        * @param aCancelStatus Status of cancel operation. Possible values:
        * - KMmsCancelRequestSuccessfullyReceived
        * - KMmsCancelRequestCorrupted
        */
        inline void SetCancelStatus( TInt aCancelStatus );
        
        /**
        * Get status of cancel operation
        * @return aCancelStatus Status of cancel operation. Possible values:
        * - KMmsCancelRequestSuccessfullyReceived
        * - KMmsCancelRequestCorrupted
        * 0 not defined
        */
        inline TInt CancelStatus() const;
        

    public: // Functions from base classes

        
    protected:  // New functions
        

    protected:  // Functions from base classes
        

    private:

        /**
        * C++ default constructor.
        */
        CMmsHeaders();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TInt16 aMmsDefaultVersion );

        /**
        * Internalize.
        * @param aStream read stream
        */
        IMPORT_C void InternalizeL( RMsvReadStream& aStream );

        /**
        * Externalize.
        * @param aStream write stream
        */
        IMPORT_C void ExternalizeL( RMsvWriteStream& aStream ) const;

        /**
        * Remove a addressee from a given addressee list.
        * @param aList addressee list
        * @param aAddress addressee to match
        * @return ETrue if matched and deleted.
        */
        TBool RemoveAddressee(
            CDesCArray& aList,
            const TDesC& aAddress );

        /**
        * Externalize Array. 
        * @param anArray the array to be externalized
        * @param aStream stream where to write.
        */
        void ExternalizeArrayL(
            CDesC16Array& anArray,
            RWriteStream& aStream ) const;

        /**
        * Internalize Array.
        * @param anArray the array where to write data
        * @param aStream a stream where to read data from
        */
        void InternalizeArrayL(
            CDesC16Array& anArray,
            RReadStream& aStream );
 
        /**
        * Copy addressee list from source headers
        * @param aArray addressee list
        * @param aType recipient lins type
        * @param aExclude Address to be excluded from the list (sender)
        * @param aFs file system handle
        */
        void CopyAddresseeListL(
            const CDesCArray& aArray,
            TMmsRecipients aType,
            const TPtrC& aExclude,
            RFs& aFs );

        /**
        * Generate aliased address from an address that may be pure or not <br>
        * Caller must free buffer when no lenger needed
        * @param aOriginalAddress original address may be pure or not
        * @param aFs file system handle
        * @return combined address in format: <br>
        *    alias<real_address>
        */
        HBufC* AliasedAddressL(
            const TDesC& aOriginalAddress,
            RFs& aFs ); 
            
            
        /**
        * Generate array of aliased addresses from an arrays of addresses
        * that may be pure or not <br>
        * Caller must free the array when no lenger needed
        * @param aOriginalAddress array of original addresses, may be pure or not
        * @return array of addresses in format: <br>
        *    alias<real_address>
        */
        CDesCArray* AliasedAddressL(
            const CDesCArray& aOriginalAddress, 
            RFs& aFs );
            
        /**
        * Limit string size to maximum length
        * @param aString input string
        * @param aMaxSize maximum length of the string
        */
        static TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);
        
        /**
        * Limit string size to maximum length
        * @param aString input string
        * @param aMaxSize maximum length of the string
        */
        static TPtrC16 LimitStringSize(const TPtrC16& aString, TInt aMaxSize);
    


    public:     // Data
    
    protected:  // Data

    private:    // Data
        TInt32          iMsgType;       // Message-Type;
        HBufC8*         iTID;           // Transaction ID
        TInt16          iVersion;       // MMS-version
        TInt64          iDate;          // Date

        CDesCArray*     iToArray;       // TO headers
        CDesCArray*     iCcArray;       // Cc headers
        CDesCArray*     iBccArray;      // Bcc headers
        HBufC*          iSender;        // From header

        HBufC*          iSubject;       // Subject

        TInt32          iMsgClass;      // Message-Class

        TInt64          iExpiry;        // Expiry
        TBool           iExpiryAbs;     // Is the iExpiry time absolute time
        TInt64          iDelivery;      // Delivery
        TBool           iDeliveryAbs;   // Is iDelivery absolute time
        TInt32          iPriority;      // Priority
        TInt32          iSenderVisi;    // Sender-Visibility
        TInt32          iDelivReport;   // Delivery-Report
        TInt32          iReadReply;     // Read-Reply

        TMsvAttachmentId iStart;         // root part of the message

        HBufC8*         iContentLocation;   // Content-Location
        TUint32         iMessageSize;       // Message-Size 

        TInt32          iReportAllowed;     // Report-Allowed
        TInt32          iResponseStatus;    // Response-Status
        HBufC8*         iMessageId;         // Message-ID

        TInt32          iStatus;            // Status

        TInt32          iMaxImageHeight;    // maximum image height
        TInt32          iMaxImageWidth;     // maximum image width

        HBufC*          iOperatorResponseText; // Operator response in send-conf or retrieve-conf
        HBufC8*         iRootContentID;  // content-id of the root message part.

        // new fields for MMS encapsulation version 1.1
        // Retrieve status is stored into same variable as response status
        // response status comes in send confirmation,
        // retrieve status comes in retrieve confirmation.
        // Both have the same meaning indicating success or failure of the operation

        TInt32          iReadStatus;        // status from a read report
        TInt32          iReplyCharging;
        TInt64          iReplyChargingDeadline;
        TBool           iReplyChargingAbs;   // Is iReplyChargingDeadline absolute time
        TInt32          iReplyChargingSize;
        HBufC8*         iReplyChargingId;
        // previously sent by
        // previously sent date
        RPointerArray<CMmsSendingChain> iPreviouslySentArray;
        TLinearOrder<CMmsSendingChain> iLinearOrder;

        // New fields for MMS encapsulation version 1.2

        TInt32           iDistributionIndicator; // Yes/No
        // UID of an entry that is related to this one. 
        // For example id of the original notification when a notification is
        // being forwarded. The result of the operation affects the original entry
        TMsvId           iRelatedEntry;
        // Type of multipart data stored in this entry
        // (multipart/mixed, multipart/related, multipart/alternative)
        // saved as WSP assigned number
        TInt32           iMultipartType;

        TTime            iReceivingTime;

        // Reserved fields have been taken into use to store version 1.3 headers        
        // Content class values defined in mmsconst.h
        TInt32           iContentClass; 
        TInt32           iDrmContent; // Yes/No
        TInt32           iAdaptationAllowed; // Yes/No

        // Pointer to separate classes that contain rarer 1.2 headers
        // These are stored to separate streams to help preserving
        // compatibility.
        // The data for these classes is restored only when the
        // corresponding stream is present.

        CMmsElementDescriptor* iElementDescriptor;
        CMmsMMBoxMessageHeaders* iMmBoxMessageHeaders;
        CMmsMMBoxViewHeaders* iMmBoxViewHeaders;

        // Extended notification data
        // These are stored in a separate stream
        // End of message flag is not saved.
        // If the whole message is contained in the notification, the notification is converted
        // into a message.
        HBufC*          iExtendedNotificationText; // Operator response in send-conf or retrieve-conf
        // Extension status coding:
        // 0x20 = Message body size > text, message retrieval uncomplete
        // 0x30 = Message retrieval complete (whole message body in IExtendedNotificationText)
        TInt            iExtensionStatus;

        // result array from MMBox delete confirmation.
        // These are never saved to disk
        RPointerArray<CMmsDeleteResultArray> iDeleteResultArray;
        TLinearOrder<CMmsDeleteResultArray> iDeleteResultOrder;
        
        HBufC*          iApplicationId; // receiving application
        HBufC*          iReplyToApplicationId; // application to receive replies
        HBufC8*         iApplicationInfo; // info for the receiving application

        // Default MMS encapsulation version.
        // This is used to initialize iVersion member
        // The value is given in NewL, and should be the value from CMmsSettings class
        TInt16          iMmsDefaultVersion;
        
        // New headers from version 1.3
        // These are actually not part of a message but control the transactions.
        // These are stored on disk in case the transaction data is created by one
        // component and later sent by another
        
        // Recommended retrieval mode (notification)
        TInt32          iRecommendedRetrievalMode;
        // Explanation why manual retrieval is recommended
        HBufC*          iRecommendedRetrievalModeText;
        // Id of a message that is to be replaced or cancelled
        HBufC8*         iReplaceCancelId;
        // cancel status
        TInt32          iCancelStatus;


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#include "mmsheaders.inl"

#endif      // CMMSHEADERS_H
            
// End of File
