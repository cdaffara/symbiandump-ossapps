/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CS general definitions
*
*/


#ifndef __C_CSSERVER_DEFS_H__
#define __C_CSSERVER_DEFS_H__

// INCLUDES FILES

// SYSTEM INCLUDES
#include <e32base.h>
#include <e32math.h>
#include <s32strm.h>
#include <s32mem.h>
#include <e32def.h>

//-----------------------------------------------------------------------------
// Conversation Server Details
//-----------------------------------------------------------------------------
/**
 * Name of the conversation server.
 */
_LIT(KCsServerName,    "CsServer");

/**
 * Name of the conversation server executable.
 */
_LIT(KCsServerExeName, "CsServer.exe");

/**
 * Conversation Server data plugin interface UID
 */
const TUid KCsPluginInterfaceUid = { 0x2002A545 };

//-----------------------------------------------------------------------------
// Conversation Server general constants
//-----------------------------------------------------------------------------
/**
 * Number of digits to compare (from right) for two phone numbers to match.
 */
const TInt KDefaultGsmNumberMatchLength = 7;

/**
 * Maximum internal buffer length used for IPC between conversation server and
 * clients.
 */
const TInt KBufferMaxLen = 16000; // 16K

/**
 * Maximum number of special conversations that are being reserved
 * 
 */
const TInt8 KMaxSpecialConversations = 3;

//-----------------------------------------------------------------------------
// Conversation Server event codes
//-----------------------------------------------------------------------------
const TUint32 KConversationEventNone             = 0x00000000;
const TUint32 KConversationListEventNew          = 0x00000001;
const TUint32 KConversationListEventUpdate       = 0x00000002;
const TUint32 KConversationListEventDelete       = 0x00000004;
const TUint32 KConversationEventNew              = 0x00000008;
const TUint32 KConversationEventUpdate           = 0x00000010;
const TUint32 KConversationEventDelete           = 0x00000020;
const TUint32 KConversationCachingEventStarted   = 0x00000040;
const TUint32 KConversationCachingEventCompleted = 0x00000080;
const TUint32 KConversationCachingEventError     = 0x00000100;
const TUint32 KConversationEventListRefresh      = 0x00000200;
const TUint32 KConversationEventRefresh          = 0x00000400;
const TUint32 KConversationListEventPartialDelete = 0x00000800;

//-----------------------------------------------------------------------------
// Conversation Server caching status codes
//-----------------------------------------------------------------------------
const TUint8 KCachingStatusUnknown    = 0x00;
const TUint8 KCachingStatusStarted    = 0x01;
const TUint8 KCachingStatusCompleted  = 0x02;
const TUint8 KCachingStatusError      = 0x04;

//-----------------------------------------------------------------------------
// Conversation Server special conversation codes
//-----------------------------------------------------------------------------
const TUint8 KUnknownConversationId       = 0x00;
const TUint8 KBluetoothMsgsConversationId = 0x01;
const TUint8 KInfraRedMsgsConversationId  = 0x02;

//-----------------------------------------------------------------------------
// Request commands to the conversation server
//-----------------------------------------------------------------------------
enum TCsServerRequestCommands
{
     EGetConversationList,
     EGetConversationUnreadList,
     EGetConversations,
     EGetTotalUnreadCount,
     ERequestChangeEvent,
     ERemoveChangeEvent,
     EGetCachingStatus,
     ESetConversationListChangeObserver,
     EResetConversationListChangeObserver,
     ESetConversationChangeObserver,
     EResetConversationChangeObserver,
     ESetCachingStatusObserver,
     EResetCachingStatusObserver,
     EShutdown,
     EUserDeleteConversation,
     EGetConversationId,
     EGetConversationIdFromAddress,
     EGetConversationFromMessageId,
     EUserMarkReadConversation,
     EGetConversationFromConversationId,
     EGetConversationFromConversationIdComplete
};

//-----------------------------------------------------------------------------
// Response commands from the conversation server
//-----------------------------------------------------------------------------
enum TCsServerResponseCommand
{
    EGetConversationListOperationComplete,
    EGetConversationOperationComplete,
    EGetConversationListBufferOverflow,
    EGetConversationBufferOverflow,
    EAddConversationListEvent,
    EDeleteConversationListEvent,
    EModifyConversationListEvent,
    EAddConversationEvent,
    EDeleteConversationEvent,
    EPartialDeleteConversationListEvent,
    EModifyConversationEvent,
    ECachingStartedEvent,
    ECachingCompletedEvent,
    ECachingErrorEvent,
    EInitRequestChange,  //initiate request change request to server
    EUserDeleteConversationComplete,
    ERefreshConversationListEvent,
    ERefreshConversationEvent,
    EGetConversationIdComplete,
    EGetConversationIdFromAddressComplete,
    EGetConversationFromMessageIdComplete,
    EUserMarkReadConversationComplete    
};

//-----------------------------------------------------------------------------
// Conversation Server data types
//-----------------------------------------------------------------------------

/**
 * Conversation Identifier
 */
typedef TInt TCsConversationEntryID;

/**
 * Direction of a conversation event.
 * @attention This enum can have values from 0 to 255 only.
 */
enum
{
ECsDirectionIncoming = 0,
ECsDirectionOutgoing,
ECsDirectionMissed,
ECsDirectionUnknown
};

/**
 * @typedef TCsDirection
 * typedef to denoting direction of a conversation event.
 */
typedef TUint8 TCsDirection;

/**
 * Type of a conversation event.
 * @attention This enum can have values from 0 to 255 only.
 */
enum
{
ECsSMS = 0,
ECsMMS,
ECsMmsNotification,
ECsAudio,
ECsBioMsg,
ECsBlueTooth,
ECsRingingTone,
ECsProvisioning,
ECsBioMsg_VCard,
ECsBioMsg_VCal,
ECsBioMgs_NokiaService,
ECsUnknown,
ECsBlueTooth_VCard,
ECsBlueTooth_VCal
};

/**
 * @typedef TCsType
 * typedef denoting type of a conversation event.
 */
typedef TUint8 TCsType;

/**
 * Attributes associated with a conversation event.
 * These are bitmask values.
 * Use CCsConversationEntry::ChangeAttributes() to set/ change
 * @attention This enum can have values from 0 to 65535 only.
 */
enum
{
ECsAttributeNone            = 0x0000,
ECsAttributeNew             = 0x0001,
ECsAttributeDraft           = 0x0002,
ECsAttributeAttachment      = 0x0004, // Attachment present
ECsAttributeHighPriority    = 0x0008, // Priority high
ECsAttributeLowPriority     = 0x0010, // Low high
ECsAttributeUnread          = 0x0020, // Unread
ECsAttributeSent            = 0x0040,
ECsAttributeNewEntryAdded   = 0x0080  // When a new entry added
};

/**
 * @typedef TCsAttribute
 * typedef denoting attributes associated with a conversation event.
 */
typedef TUint16 TCsAttribute;

/**
 * Transit sending state of a conversation event.
 * @attention This enum can have values from 0 to 255 only.
 */
enum
{
ECsSendStateUnknown = 0,
ECsSendStateUponRequest,
ECsSendStateWaiting,
ECsSendStateSending,
ECsSendStateScheduled,
ECsSendStateResend,
ECsSendStateSuspended,
ECsSendStateFailed,
ECsSendStateSent,
ECsSendStateNotApplicable
};

/**
 * @typedef TCsSendState
 * typedef denoting transit sending state of a conversation event.
 */
typedef TUint8 TCsSendState;

/**
 * @typedef TCsMmsNotificationMsgState
 * @attention This enum can have values from 0 to 255 only.
 */

enum 
{
EMsgStatusNull              = 0, // unknown status
EMsgStatusReadyForFetching  = 1, // first state when received in inbox
EMsgStatusRetrieving        = 2, // when MM is fetching ongoin
EMsgStatusForwarding        = 3, // forwarding without retrival
EMsgStatusForwarded         = 4, // state after successful forwarding
EMsgStatusWaiting           = 5, // when settings changed manual->automatic
EMsgStatusExpired           = 6, // expired time has exceed
EMsgStatusFailed            = 7, // when fetching has failed (forwarding?)
EMsgStatusDeleted           = 8  // when msg deleted from server
};

typedef TUint8 TCsMmsNotificationMsgState;

/**
 * Constant for defining the contacts db
 * This shall be used for converting contact link to id's
 */
_LIT(KVPbkDefaultCntDbURI, "cntdb://c:contacts.cdb"); //Phone Store

/**
 * Internal offset used to identify SIM contacts.
 */
const TInt KSimIdOffset = 99999;

/**
 * @typedef TCsMsgPreviewProperty
 */

enum 
{
  EPreviewNone           = 0x0000,
  EPreviewImage          = 0x0001,
  EPreviewAudio          = 0x0002,
  EPreviewVideo          = 0x0004,
  EPreviewAttachment     = 0x0008,
  EPreviewForward        = 0x0010,
  EPreviewProtectedImage = 0x0020,
  EPreviewProtectedAudio = 0x0040,
  EPreviewProtectedVideo = 0x0080,
  EPreviewCorruptedImage = 0x0100,
  EPreviewCorruptedAudio = 0x0200,
  EPreviewCorruptedVideo = 0x0400
};

typedef TUint16 TCsMsgPreviewProperty;

/**
 * @typedef TCsPreviewMsgProcessingState
 */
enum
{
    EPreviewMsgNotProcessed       = 0,
    EPreviewMsgProcessing         = 1,
    EPreviewMsgProcessed          = 2
};

/** 
@publishedAll
@released

Template class CleanupResetAndDestroy to clean up the array
of implementation information from the cleanup stack.
*/

template <class T>
class CleanupResetAndDestroy
	{
public:
	/**
	Puts an item on the cleanup stack.

	@param  aRef 
	        The implementation information to be put on the cleanup stack.
	*/
	inline static void PushL(T& aRef);
	inline static void PushL(T* aPointer);
private:
	static void ResetAndDestroy(TAny *aPtr);
	static void ResetAndDestroyDelete(TAny *aPtr);
	};
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

template <class T>
inline void CleanupResetAndDestroyPushL(T* aPointer);

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));}

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T* aPointer)
    {CleanupStack::PushL(TCleanupItem(&ResetAndDestroyDelete,aPointer));}

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{(STATIC_CAST(T*,aPtr))->ResetAndDestroy();}

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroyDelete(TAny *aPtr)
    {
    (STATIC_CAST(T*,aPtr))->ResetAndDestroy();
    delete (STATIC_CAST(T*,aPtr));
    }

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
	{CleanupResetAndDestroy<T>::PushL(aRef);}

template <class T>
inline void CleanupResetAndDestroyPushL(T* aPointer)
    {CleanupResetAndDestroy<T>::PushL(aPointer);}
	
#endif // __C_CSSERVER_DEFS_H__

// End of file
