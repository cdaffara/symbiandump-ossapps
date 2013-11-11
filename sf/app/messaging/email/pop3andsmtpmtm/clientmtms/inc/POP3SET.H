// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined (__POP3SET_H__)
#define __POP3SET_H__

 


#include <msvuids.h>
#include <miutset.h>
#include <miut_err.h>



/** Default POP3 server port number. */
const TUint32 KPOP3DefaultPortNumber = 110;

/** @deprecated */
const TInt32 KImPop3SynchroniseAll	 = -1;

/** @deprecated */
const TInt32 KImPop3SynchroniseNone	 = 0;

/**
Specifies which parts of a POP3 message to download. Used when configuring the
POP3 service settings.

@see
CImPop3Settings 

@publishedAll
@released
*/
enum TPop3GetMailOptions
	{
	/** Message headers only. */
	EGetPop3EmailHeaders,
	/** Complete message. */
	EGetPop3EmailMessages
	};			   


class TPop3Progress
/**
Progress information for a POP3 operation. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C TInt ConnectionState() const;
	IMPORT_C TInt ConnectionIAP() const;

public:
	/** Defines types of POP3 operations. */
	enum TPop3ProgressType 
		{
	/** Refreshing view of remote mail box. */
		EPopRefreshing,
	/** Copying messages. */
		EPopCopying,
	/** Deleting messages. */
		EPopDeleting,
	/** Connecting to remote mail box. */
		EPopConnecting,
	/** Internal reorganisation. */
		EPopTidying,
	/** Connected to remote mail box and no current operations. */
		EPopConnectedAndIdle,
	/** Disconnecting from remote mail box. */
		EPopDisconnecting,
	/** Disconnected from remote mail box. */
		EPopDisconnected,
	/** Moving messages. */
		EPopMoving,
	/** Copying new messages. */
		EPopCopyNewMail,
	/** Moving new messages. */
		EPopMoveNewMail,
	/** Copying a selection of messages. */
		EPopCopyMailSelection,
	/** Moving a selection of messages. */
		EPopMoveMailSelection,
	/** Copying all messages. */
		EPopCopyAllMail,
	/** Moving all messages. */
		EPopMoveAllMail,
	/** Populating view of remote mail box. */
		EPopPopulating,
	/** Cancelling all off-line operations. */
		EPopCancellingOfflineOps,
	/** Populating mailbox with top command */
		EPopTopPopulating,
	/** Defines the maximum value for this enumeration. */
		EPopMaxProgressValue=EPopTopPopulating
		};

	/** Type of operation for which progress is being reported. */
	TPop3ProgressType	iPop3Progress;	// holds the progress for a 'super operation'
	/** When in the EPopConnecting state, this member holds the connection 
	stage. Otherwise it holds the total number of messages for the operation. */
	TInt				iTotalMsgs;
	/** Remaining number of messages to process. */
	TInt				iMsgsToProcess;
	/** Number of bytes processed by the operation. */
	TInt				iBytesDone;
	/** When in the EPopConnecting state, this member holds the connection 
	iap value. Otherwise it holds the total number of bytes to progress for the
	operation. */
	TInt				iTotalBytes;
	/** Operation error code. */
	TInt				iErrorCode; // client side needs to be informed of an error
	/** Type of the sub-operation, where a nested operations is occurring. */
	TPop3ProgressType	iPop3SubStateProgress; // holds the progress for the substate of the 'super operation'
	/** Service performing the operation. */
	TMsvId				iServiceId;
	/** The total size of messages to be downloaded. */
	TInt				iTotalSize; // The total size of messages to be downloaded.
	};


class TImPop3GetMailInfo
/**
Use to specify the maximum message size and the destination folder ID for
certain POP3 operations.

@see
CPop3ClientMtm

@publishedAll
@released
*/
	{
public:
	/** Maximum message size for the operation. */
	TInt32		iMaxEmailSize;
	/** ID of the destination folder for the operation. */
	TMsvId		iDestinationFolder;
	};

class TImPop3PopulateOptions
/**
Use to specify the population limit for the POP3 top command

@see
CPop3ClientMtm

@publishedAll
@released
*/
	{
private:
	/** Version of this GetMailInfo class, breaks coding standard by putting a private member at 
	    the beginning to enable unpacking code to check the version before unpacking the rest of the class */
	TInt32	iVersion;

public:
	IMPORT_C TImPop3PopulateOptions();
	IMPORT_C static void UnpackL(const TDesC8& aDesc, TImPop3PopulateOptions &aUnpackTo);

	IMPORT_C TInt MaxEmailSize();
	IMPORT_C void SetMaxEmailSize(TInt aMaxEmailSize);
	IMPORT_C TInt PopulationLimit();
	IMPORT_C void SetPopulationLimit(TInt aPopulationLimit);
	
private:
	/** Maximum message size for the operation. */
	TInt32		iMaxEmailSize;
	/** Population limit for Populate operation, set to -1 for total populate */
	TInt32		iPopulationLimit;
	/** Construction utility function */
	};


class CImPop3Settings : public CImBaseEmailSettings
/**
Run-time configuration settings for POP3.

Messaging clients should use an instance of this class to specify
and retrieve configuration settings that are used by the POP3 service when
executing email operations.  

Service settings such as the user name and password, whether to authenticate
using APOP or plain text, the maximum size of an email to download, and the
maximum number of messages to synchronise can be specified using this class.
Storing and restoring from the message store is also supported.

To use this class to change a setting:
1) Set the current context to the POP3 service entry using CMsvStore.
2) Create an instance of CImPop3Settings and put it on the cleanup stack.
3) Retrieve the existing settings by calling CImPop3Settings::RestoreL().
4) Change the inbox synchronisation limit setting by calling CImPop3Settings::SetInboxSynchronisationLimit().
5) Save the new settings by calling CImPop3Settings::StoreL().
6) Pop and destroy the CImPop3Settings instance.

@see CMsvStore
@publishedAll
@released
*/
	{
public:
	IMPORT_C CImPop3Settings();
	IMPORT_C virtual ~CImPop3Settings();
	IMPORT_C void Reset();
	IMPORT_C const TPtrC8 LoginName() const;
	IMPORT_C void SetLoginNameL(const TDesC8&);
	IMPORT_C const TPtrC8 Password() const;
	IMPORT_C void SetPasswordL(const TDesC8&);
	IMPORT_C TBool AutoSendOnConnect() const;
	IMPORT_C void SetAutoSendOnConnect(TBool aFlag);
	IMPORT_C TBool Apop() const;
	IMPORT_C void SetApop(TBool aFlag);
	IMPORT_C TBool DisconnectedUserMode() const;
	IMPORT_C void SetDisconnectedUserMode(TBool aFlag);
	IMPORT_C TBool DeleteEmailsWhenDisconnecting() const;
	IMPORT_C void SetDeleteEmailsWhenDisconnecting(TBool aFlag);
	IMPORT_C TBool AcknowledgeReceipts() const;
	IMPORT_C void SetAcknowledgeReceipts(TBool aFlag);
	IMPORT_C TInt MaxEmailSize() const;
	IMPORT_C void SetMaxEmailSize(const TInt aMaxEmailSize);
	IMPORT_C TPop3GetMailOptions GetMailOptions() const;
	IMPORT_C void SetGetMailOptions(TPop3GetMailOptions aGetMailOptions);
	IMPORT_C CImPop3Settings& CopyL(const CImPop3Settings& aCImPop3Settings);
	IMPORT_C TBool operator==(const CImPop3Settings& aCImPop3Settings) const;
	IMPORT_C TInt32 InboxSynchronisationLimit() const;
	IMPORT_C void SetInboxSynchronisationLimit(const TInt32 aInboxSyncLimit);
	IMPORT_C TInt32 PopulationLimit() const;
	IMPORT_C void SetPopulationLimitL(const TInt32 aPopulationLimit);
	IMPORT_C void SetTlsSslDomainL(const TDesC8& aDomainName);
	IMPORT_C TPtrC8 TlsSslDomain() const;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	IMPORT_C void SetPOP3Auth(TBool aFlag);
	IMPORT_C TBool POP3Auth() const;
	IMPORT_C void SetFallBack(TBool aFlag);
	IMPORT_C TBool FallBack() const;
#endif

private:
	class TImPop3SettingsExtension
		{
	public:
		inline TImPop3SettingsExtension();
	public:
		HBufC8* iLoginName;
		HBufC8* iPassword;
		HBufC8* iTlsSslDomain;
		TInt32 iPopulationLimit;
		};
	inline TImPop3SettingsExtension* Extension() const;
	inline void CheckExtensionExistsL();

	enum TImPop3EmailSettings
		{
		KPop3SettingsClearFlag					= 0x00000000,
		KPop3BaseEmailSettingsLastUsedFlag		= CImBaseEmailSettings::EBaseEmailSettingsLastUsedFlag,	//0x00000002
		KPop3ApopFlag							= KPop3BaseEmailSettingsLastUsedFlag << 1,		//0x00000004
		KPop3AutoSendFlag						= KPop3ApopFlag << 1,							//0x00000008
		KPop3DisconnectedModeFlag				= KPop3AutoSendFlag << 1,						//0x00000010
		KPop3DeleteEmailsWhenDisconnectingFlag	= KPop3DisconnectedModeFlag << 1,				//0x00000020
		KPop3AcknowledgeReceipts				= KPop3DeleteEmailsWhenDisconnectingFlag << 1,	//0x00000040
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)		
		KPop3AuthFlag							= KPop3AcknowledgeReceipts << 1,				//0x00000080
		KPop3FallbackFlag						= KPop3AuthFlag << 1,							//0x00000100
		KPop3EmailSettingsLastUsedFlag			= KPop3FallbackFlag								//0x00000100
#else		
		KPop3EmailSettingsLastUsedFlag			= KPop3AcknowledgeReceipts						//0x00000040
#endif		
		};

	TImPop3SettingsExtension*	iExtension; // renamed iReceiptAddress
	TInt32  iInboxSyncLimit;
	TInt32	iMaxEmailSize;
	TPop3GetMailOptions iGetMailOptions;
	};

inline CImPop3Settings::TImPop3SettingsExtension* CImPop3Settings::Extension() const
	{
	return iExtension;
	}

inline void CImPop3Settings::CheckExtensionExistsL()
	{
	if (!iExtension)
		iExtension=new (ELeave) CImPop3Settings::TImPop3SettingsExtension;
	}

inline CImPop3Settings::TImPop3SettingsExtension::TImPop3SettingsExtension() : iLoginName(NULL) , iPassword(NULL), iTlsSslDomain(NULL)
	{
	}

#endif // #define __POP3SET_H__
