/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store key definitions.
*
*/



#ifndef __MSG_STORE_PROPERTY_KEYS_H__
#define __MSG_STORE_PROPERTY_KEYS_H__

#include <e32std.h>

/*************************************************************************
 * 
 * Properties common to all plugins and sync engines
 * 
 ************************************************************************/

/** The property key for status flags field.  Its property value type is EMsgStoreTypeUint32.
 *  The values are defined in email fw's TFSMsgFlag enum (CFSMailCommon.h).
 * 
 *  The following code updates the "flags" field with the local read flag
 *
 *   <pre>
 *     TUint index;
 *     if ( message->FindProperty( KMsgStorePropertyFlags, index ) )
 *         {
 *         TUint flags = message->PropertyValueUint32L( index );
 *         flags |= EFSMsgFlag_Read_Locally;
 *         message->UpdatePropertyL( index, flags );
 *         }
 *     else
 *         {
 *         message->AddPropertyL( KMsgStorePropertyFlags, (TUint32)EFSMsgFlag_Read_Locally);
 *         }
 *   </pre>
 * 
 * @see TFSMsgFlag
 */
_LIT8( KMsgStorePropertyFlags,       "flags" );    
     

/** The property key for the received date.  It's property value type should be EMsgStoreTypeTime
 *  that represents a time stamp. 
 *
 *  The following code adds the "received" property to a message.
 *
 *   <pre>
 *     TTime time;
 * 	   time.HomeTime();  
 *     message->AddPropertyL( KMsgStorePropertyReceivedAt, time );               
 *   </pre>
 *
 *  The following code reads the "received" property from a message
 * 
 *   <pre>
 *     TUint index;
 *     TTime time;
 *     if ( message->FindPropertyL( KMsgStorePropertyReceivedAt, index ) )
 *         {
 *         message->PropertyValueTimeL( index, time );
 *         }
 *   </pre>
 * 
 *  Note: the message store needs to perform sorting based on the values of this field,
 *        so storing this field in different type of values may cause sorting to break.
 */
_LIT8( KMsgStorePropertyReceivedAt,  "received" ); 

/** The property key for the received date.  It's property value type should be EMsgStoreTypeTime
 *  that represents a time stamp. See KMsgStorePropertyReceivedAt for examples.
 * 
 *  @see         KMsgStorePropertyReceivedAt
 */
_LIT8( KMsgStorePropertySent, "sent" );

/** The property key for the total message size on the server; bodies + attachments.
 *  It's property value type should be EMsgStoreTypeUint32.
 */
_LIT8( KMsgStorePropertyMessageSizeOnServer, "size_on_server" );

/** The property key for the "sender" field. Its property value type should be EMsgStoreTypeAddress. 
 *
 *  The following code adds the "sender" property to a message:
 *
 *   <pre>
 *     RMsgStoreAddress addr;
 *     CleanupClosePushL( addr );
 *     addr.iEmailAddress.Create(_L("myAddres1"));
 *     addr.iDisplayName.Create(_L("My Name 1"));
 *     
 *     message->AddPropertyL( KMsgStorePropertySender, addr );
 *     CleanupStack::PopAndDestroy( &addr );
 *   </pre>
 * 
 *  Note: the message store needs to perform sorting based on the values of this field,
 *        so storing this field in different type of values may cause sorting to break.
 */
_LIT8( KMsgStorePropertySender,      "sender" );        


/** The property key for the "from" field.  Its property value type should be EMsgStoreTypeAddress.
 *
 *  @see  KMsgStorePropertySender
 */
_LIT8( KMsgStorePropertyFrom,        "from" );          


/** The property key for the "to" field. Its property value type should be EMsgStoreTypeAddress.
 *
 *  @see  KMsgStorePropertyFrom
 *
 *  Note that this field can be "duplicated" to store multiple addresses. So call FindProperty()
 *  in a loop to retrieve all the "to" fields.
 * 
 *  The following code adds multiple recipiants to a message:
 * 
 *  <pre>
 *  for ( int i = 0 ; i < aAddressArray.Count() ; i++ )
 *     {
 *     RMsgStoreAddress addr;
 *     CleanupClosePushL( addr );
 * 
 *     addr.iEmailAddress.Create( aAddressArray[i] );
 *     addr.iDisplayName.Create( aDisplayName[i] );
 *     
 *     message->AddPropertyL( KMsgStorePropertyTo, addr );
 * 
 *     CleanupStack::PopAndDestroy( &addr ):
 *     }
 *   </pre>
 *
 *  The following code retrieves ALL the recipiants in the "to" field:
 *
 *  <pre>
 *  TInt index = 0;                                                                  
 *  while ( message->FindProperty( KMsgStorePropertyTo, index, index ) )                
 *      {                                                                                 
 *      RMsgStoreAddress addr;
 *      CleanupClosePushL( addr );
 * 
 *      message->PropertyValueAddressL( index, addr );
 * 
 *      //do something with addr
 *                :
 * 
 *      CleanupStack::PopAndDestroy( &addr );
 *      }                                                                               
 *   </pre>
 */
_LIT8( KMsgStorePropertyTo,          "to" );            


/** The property key for the "cc" field. Its property value type should be EMsgStoreTypeAddress.
 *
 *  Note that this field can be "duplicated" to store multiple addresses. See KMsgStorePropertyTo
 *  for examples.
 * 
 *  @see  KMsgStorePropertyTo
 */
_LIT8( KMsgStorePropertyCc,          "cc" );            


/** The property key for the "bcc" field. Its property value type should be EMsgStoreTypeAddress.
 *
 *  Note that this field can be "duplicated" to store multiple addresses. See KMsgStorePropertyTo
 *  for examples.
 * 
 *  @see  KMsgStorePropertyTo
 */
_LIT8( KMsgStorePropertyBcc,         "bcc" ); 

/** The property key for the "replyTo" field. Its property value type should be EMsgStoreTypeAddress.
 *
 *  Note that this field can be "duplicated" to store multiple addresses. See KMsgStorePropertyTo
 *  for examples.
 * 
 *  @see  KMsgStorePropertyTo
 */
_LIT8( KMsgStorePropertyReplyTo, "replyto" );

/** Property for storing the subject line of a message.
 *  value type:  EMsgStoreTypeDes
 * 
 *  The following code adds the subject field to a message:
 * 
 *   </pre>
 *     message->AddPropertyL( KMsgStorePropertySubject, aSubject );
 *   </pre>
 */
_LIT8( KMsgStorePropertySubject, "subject" );

/** Generic property key for giving a container (folder, attachment, etc, a name).
 *  value type: EMsgStoreTypeDes
 */
_LIT8( KMsgStorePropertyName, "name" );

/** The character set property key.
 *  value type: EMsgStoreTypeDes8
 */
_LIT8( KMsgStorePropertyCharset, "charset" );

/** Property whose value indicates the content size of the message part.
 * value type:  EMsgStoreTypeUint32
 */
_LIT8( KMsgStorePropertySize, "size" );

/**
 * Property whose value indicates the downloaded size of a message part content.
 * value type:  EMsgStoreTypeUint32
 * 
 * If "retrieved_size" == "size" than the download of the message part content is complete.
 */
_LIT8( KMsgStorePropertyRetrievedSize, "retrieved_size" );

/*************************************************************************
 * 
 * RFC 2045, RFC 2183 and other content related properties
 * 
 ************************************************************************/

/** The property key for the "content-type" field.  
 *  Its property value type EMsgStoreTypeDes
 *
 */
_LIT8( KMsgStorePropertyContentType, "Content-Type" ); 

/** The property key for the "content-description" field.  
 *  Its property value type EMsgStoreTypeDes
 *
 */
_LIT8( KMsgStorePropertyContentDescription, "Content-Description" ); 

/** The property key for the "content-disposition" field.  
 *  Its property value type EMsgStoreTypeDes
 *
 */
_LIT8( KMsgStorePropertyContentDisposition, "Content-Disposition" ); 

/** The property key for the "content-class" field.  
 *  Its property value type EMsgStoreTypeDes
 *
 */
_LIT8( KMsgStorePropertyContentClass, "Content-Class" ); 

/** The property key for the "content-id" field.  
 *  Its property value type EMsgStoreTypeDes
 *
 */
_LIT8( KMsgStorePropertyContentId, "Content-Id" ); 

/*************************************************************************
 * 
 * Intellisync specific properties 
 * 
 ************************************************************************/

/** This is one of the Intellisync quick properties.  
 *  It's property value type should be EMsgStoreTypeTime.
 *
 *  @see KMsgStorePropertyReceivedAt
 */
_LIT8( KMsgStorePropertyReplyBy,     "replyby" );

/** This is one of the Intellisync quick properties.  
 *  It's property value type should be EMsgStoreTypeTime.
 *
 *  @see KMsgStorePropertyReceivedAt
 */
_LIT8( KMsgStorePropertyCompletedAt, "completed" );

/**
 * key:         mboxaddr.
 * duplicated:  Yes. Call FindProperty() in a loop to get all propertis with this key.
 * used By:     Message - the value contains the reply address 
 * value type:  EMsgStoreTypeAddress
 * 
 * @see         KMsgStorePropertyFrom
 */
_LIT8( KMsgStorePropertyMboxAddr, "mboxaddr" );

/**
 * key:         download.
 * used By:     Message Body or Attachment- the value indicates whether or not the content should be downloaded 
 * value type:  TBool
 * value range: ETrue or EFalse
 */
_LIT8( KMsgStorePropertyDownload, "download" );


/*************************************************************************
 * Message store internal properties. 
 ************************************************************************/

/** The property key for the "iEmailAddress" field in the RMsgStoreAddress. 
 *
 *  @see  KMsgStorePropertyTo
 *
 */
_LIT8( KMsgStorePropertyEmailAddress, "email_address" );           

/** The property key for the "iDisplayName" field in the RMsgStoreAddress. 
 *
 *  @see  KMsgStorePropertyTo
 *
 */
_LIT8( KMsgStorePropertyDisplayName, "display_name" );           


/** Applies to folders. Indicates whether the folder is a local only. 
 *  Its property value type can be EMsgStoreTypeBool.
 *
 */
_LIT8( KMsgStorePropertyLocal,       "local" ); 

/** Flag that indicates that whether a message part is an embedded message
 */
_LIT8( KMsgStorePropertyIsEmbeddedMsg, "is-embedded-msg");

/** A TUint32 folder type property to identify the mailbox root folders.
 * used by:     any MsgStore client, the MsgStore itself does not use it internally.
 * value type:  TUint32.
 * value range: whatever the TMsgStoreFolderType enum defines.
 */
_LIT8( KMsgStorePropertyFolderType, "folder_type");

/** A TDesC type property to identify the displyname of a non-root folder
 * used by:     any MsgStore client, the MsgStore itself does not use it internally.
 * value type:  TDesC.  
 * Note: only subfolders should have this property.
 */
_LIT8( KMsgStorePropertyFolderDisplayName, "folder_disp_name");

/**
 * key:         calendar.
 * duplicated:  No.
 * used By:     Message - the value contains the body 
 * value type:  CMsgStorePropertyContainer that represents a body part
 * @see         Part
 * @see         KMsgStorePropertyKeyCalendar
 */
_LIT8( KMsgStorePropertyMeetingRequest, "meetingRequest" );

/**
 * key:         m.guid.
 * used By:     Calendar - the value contains guid
 * value type:  binary
 */
_LIT8( KMsgStorePropertyMrGuid, "m.mrGuid" );

/**
 * key:         m.meetingApptId.
 * used By:     Calendar - the value contains meeting id
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrId, "m.mrId" );

/**
 * key:         m.to.
 * used By:     Calendar - the value contains meetingTo
 * value type:  addressList
 */
_LIT8( KMsgStorePropertyMrTo, "m.mrTo" );

/**
 * key:         m.cc.
 * used By:     Calendar - the value contains meetingCc
 * value type:  addressList
 */
_LIT8( KMsgStorePropertyMrCc, "m.mrCc" );

/**
 * key:         m.creationDate.
 * used By:     Calendar - the value contains creation date
 * value type:  Date
 */
_LIT8( KMsgStorePropertyMrCreationDate, "m.mrCreationDate" );

/**
 * key:         m.sentDate.
 * used By:     Calendar - the value contains sent date
 * value type:  Date
 */
_LIT8( KMsgStorePropertyMrSentDate, "m.mrSentDate" );

/**
 * key:         m.type.
 * used By:     Calendar - the value contains type
 * value type:  TUint32 of TMsgStoreICalMethod, same as CCalBase's TICalMethod.
 */
_LIT8( KMsgStorePropertyMrMethod, "m.mrMethod" );

enum TMsgStoreICalMethod
	{
	EMsgStoreMethodNone,
	EMsgStoreMethodPublish,
	EMsgStoreMethodRequest,
	EMsgStoreMethodReply, 
	EMsgStoreMethodAdd,
	EMsgStoreMethodCancel,
	EMsgStoreMethodRefresh,
	EMsgStoreMethodCounter,
	EMsgStoreMethodDeclineCounter
	};

/**
 * Meeting request acceptance status.
 * value type: TUint32 of TMsgStoreICalStatus, same as CCalEntry's TStatus.
 */
_LIT8( KMsgStorePropertyMrStatus, "m.mrStatus" );

enum TMsgStoreICalStatus
	{
	EMsgStoreStatusTentative, 
	EMsgStoreStatusConfirmed, 
	EMsgStoreStatusCancelled, 
	EMsgStoreStatusNullStatus
	};


/**
 * key:         m.readySyncGo.
 * used By:     Calendar - the value contains ready sync
 * value type:  TDesC
 */
_LIT8( KMsgStorePropertyMrReadySyncGo, "m.mrReadySyncGo" );

/**
 * key:         m.startDate.
 * used By:     Calendar - the value contains start date
 * value type:  Date
 */
_LIT8( KMsgStorePropertyMrStartDate, "m.mrStartDate" );

/**
 * key:         m.endDate.
 * used By:     Calendar - the value contains end date
 * value type:  Date
 */
_LIT8( KMsgStorePropertyMrEndDate, "m.mrEndDate" );

/**
 * key:         m.noTime.
 * used By:     Calendar - the value contains no time flag
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrNoTime, "m.mrNoTime" );

/**
 * key:         m.private.
 * used By:     Calendar - the value contains private
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrPrivate, "m.mrPrivate" );

/**
 * key:         m.reminder.
 * used By:     Calendar - the value contains reminder
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrReminder, "m.mrReminder" );

/**
 * key:         m.reminderTime.
 * used By:     Calendar - the value contains reminder time offset
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrReminderTime, "m.mrReminderTime" );

/**
 * key:         m.location.
 * used By:     Calendar - the value contains location
 * value type:  TDesC
 */
_LIT8( KMsgStorePropertyMrLocation, "m.mrLocation" );

/**
 * key:         m.recurType.
 * used By:     Calendar - the value contains recurrence type
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrRecurType, "m.mrRecurType" );

/**
 * key:         m.frequency.
 * used By:     Calendar - the value contains
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrFrequency, "m.mrFrequency" );

/**
 * Daily recurrence rule's day of week.
 * value type:  TUint32 of TMsgStoreDayOfWeek flags.
 */
_LIT8( KMsgStorePropertyMrDayOfWeekMask, "m.mrDayOfWeekMask" );

enum TMsgStoreDayOfWeek
    {
    KMsgStoreDaySunday    = 0x01,
    KMsgStoreDayMonday    = 0x02,
    KMsgStoreDayTuesday   = 0x04,
    KMsgStoreDayWednesday = 0x08,
    KMsgStoreDayThursday  = 0x10,
    KMsgStoreDayFriday    = 0x20,
    KMsgStoreDaySaturday  = 0x40
    };

/**
 * key:         m.recurEndDate.
 * used By:     Calendar - the value contains recurrence end date
 * value type:  Date
 */
_LIT8( KMsgStorePropertyMrRecurEndDate, "m.mrRecurEndDate" );

/**
 * Recurring for how many instances.
 * TUint32
 */
_LIT8( KMsgStorePropertyMrRecurCount, "m.mrRecurCount" );

/**
 * key:         m.busyStatus.
 * used By:     Calendar - the value contains busy status
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrBusyStatus, "m.mrBusyStatus" );

/**
 * key:         m.dayOfMonth.
 * used By:     Calendar - the value contains day of month
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrDayOfMonth, "m.mrDayOfMonth" );

/**
 * key:         m.contacts.
 * used By:     Calendar - the value contains contacts
 * value type:  TDesC[]
 */
_LIT8( KMsgStorePropertyMrContacts, "m.mrContacts" );

/**
 * key:         m.weekNumber.
 * used By:     Calendar - the value contains week numbers
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrWeekNo, "m.mrWeekNumber" );

/**
 * key:         m.timeZone.
 * used By:     Calendar - the value contains time zone
 * value type:  TDesC
 */
_LIT8( KMsgStorePropertyMrTimeZone, "m.mrTimeZone" );

/**
 * key:         m.additions.
 * used By:     Calendar - the value contains additions
 * value type:  Date[]
 */
_LIT8( KMsgStorePropertyMrAdditions, "m.mrAdditions" );

/**
 * key:         m.subfolder.
 * used By:     Calendar - the value contains subfolder
 * value type:  TDesC
 */
//_LIT8( KMsgStorePropertyKeySubfolder, "m.subfolder" );

/**
 * key:         m.silent.
 * used By:     Calendar - the value contains silent flag
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrSilent, "m.mrSilent" );

/**
 * key:         m.delegateMail.
 * used By:     Calendar - the value contains delegate mail flag
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrDelegateMail, "m.mrDelegateMail" );

/**
 * key:         m.exception.
 * used By:     Calendar - the value contains exception flag
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrException, "m.mrException" );

/**
 * key:         m.exceptions.
 * used By:     Calendar - the value contains exceptions of dates
 * value type:  Date[]
 */
_LIT8( KMsgStorePropertyMrExceptions, "m.mrExceptions" );

/**
 * key:         m.meetingUpdate.
 * used By:     Calendar - the value contains meeting update flag
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrMeetingUpdate, "m.mrMeetingUpdate" );

/**
 * key:         m.singleInvite.
 * used By:     Calendar - the value contains single invite flag
 * value type:  TBool
 */
_LIT8( KMsgStorePropertyMrSingleInvite, "m.mrSingleInvite" );

/**
 * key:         m.body.
 * used By:     Calendar - the value contains the body of meeting request
 * value type:  TDesC
 */
_LIT8( KMsgStorePropertyMrCalBody, "m.mrCalBody" );

/**
 * key:         m.dowPref.
 * used By:     Calendar - the value contains DOW pref
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyMrDowPref, "m.mrDowPref" );

/**
 * key:         m.unsupportedFields.
 * used By:     Calendar - the value contains unsupported meeting fields (round-tripped)
 * value type:  binary
 */
_LIT8( KMsgStorePropertyMrUnsupportedFields, "m.mrUnsupportedFields" );

/**
 * Sequence number, for iCal.
 * value type: TUint32
 */
_LIT8( KMsgStorePropertyMrSeqNo, "m.mrSeqNo" );

/**
 * iCal's Priority.
 * value type: TUint32
 */
_LIT8( KMsgStorePropertyMrPriority, "m.mrPriority" );

_LIT8( KMsgStorePropertyMrSubject, "m.mrSubject" );

//Added for ActiveSync on behalf of Sanket Lopes.
_LIT8( KMsgStorePropertyMrMonthOfYear, "m.mrMonthOfYear" );

_LIT8( KMsgStorePropertyMrMonthDay, "m.mrMonthDay" );

_LIT8( KMsgStorePropertyMrAllDay, "m.mrAllDayEvent" );

/**
 * value type: TTime
 */
_LIT8( KMsgStorePropertyMrRRID, "m.mrRecurrenceId" );

_LIT8( KMsgStorePropertyMrResReq, "m.mrRequestResponce" );

_LIT8( KMsgStorePropertyMrRequrrence, "m.mrRecurrence" );

_LIT8( KMsgStorePropertyMrType, "m.mrResponceType" );


/**
 * key:         v.VomitCode.
 * used By:     Smart Forwarding
 * value type:  TDesC
 */
_LIT8( KMsgStorePropertyVomitCode, "v.VomitCode" );

/**
 * key:         v.VomitCodePos.
 * used By:     Smart Forwarding
 * value type:  TUint32
 */
_LIT8( KMsgStorePropertyVomitCodePos, "v.VomitCodePos" );

/**
 * key:         isDefault
 * used By:     folder
 * value type:  TBool
 * value range: ETrue or EFalse
 */
_LIT8(KMsgStorePropertyIsDefault, "isDefault");

// Mailbox properties
_LIT8(KMsgStorePropertyMailboxEmailAddress, "email-address");
_LIT8(KMsgStorePropertyMailboxEmailPassword, "email-password");
_LIT8(KMsgStorePropertyMailboxMailboxName, "mailbox-name");
_LIT8(KMsgStorePropertyMailboxIncludeSignature, "include-signature");
_LIT8(KMsgStorePropertyMailboxSignature, "signature");
_LIT8(KMsgStorePropertyMailboxSyncReadStatus, "sync-read-status");

/**
 * key:         read_only_size.
 * used By:     Email - the value contains the read-only size of a message part
 * value type:  TInt
 */
_LIT8( KMsgStorePropertyReadOnlySize, "read_only_size" );


/*************************************************************************
 *  TEMPORARY
 * Properties used by both message store and the Intellisync
 * 
 ************************************************************************/

enum TEmailServerType
{
    ESrvUnknown,
    ESrvExchange,
    ESrvDomino,
    ESrvGroupWise,
    ESrvImap,
    ESrvPop
};

/** The mailbox's server type (TEmailServerType: unknown, Exchange, Domino, Pop, etc.) 
 *   for the backend account.  
 *  It's property value type should be EMsgStoreTypeUint32.
 */

_LIT8( KIsPropertyServerType, "is.mb.serverType" );

/** The mailbox's SourceId for the backend account.  
 * It's property value type should be EMsgStoreTypeInt.
 */
_LIT8( KIsPropertyEmailSourceId, "is.mb.emlSourceId" );

/** The mailbox's server capability (Flags of TBackendServerCapabilites) for the backend account.  
 *  It's property value type should be EMsgStoreTypeUint32.
 */
_LIT8( KIsPropertyCapabilities, "is.mb.capabilities" );

#endif
