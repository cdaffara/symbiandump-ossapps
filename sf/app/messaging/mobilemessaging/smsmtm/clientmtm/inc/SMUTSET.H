// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SMUTSET.H
//
/**
 * @file 
 * @publishedAll
 * @released
 */
#ifndef __SMUTSET_H__
#define __SMUTSET_H__

#include <gsmumsg.h>
#include <msvrcpt.h>
#include <msvstore.h>
#include <logwrap.h>

class CSmsServiceCenter;
class CSmsEmailFields;

// Both the recipient's number  and SC number (TP-Destination-Address and RP-Destination-Address) 
// can be 12 octets which means that there can be 21 characters (including + sign) in the number string. 

/**
The maximum length of recipient's number.

The number returned by CSmsNumber::Address() and CSmsServiceCenter::Address() is limited this maximum length.

@see	CSmsNumber::Address
@see	CSmsServiceCenter::Address

@publishedAll
@released
*/
const TInt KSmcmSmsNumberMaxNumberLength = 21;

/**
The maximum length of recipient's name.

The number returned by CSmsNumber::Name() and CSmsServiceCenter::Name() is limited this maximum length.

@see	CSmsNumber::Name
@see	CSmsServiceCenter::Name

@publishedAll
@released
*/
const TInt KSmcmSmsNumberMaxNameLength = 256;

/** 
Defines the possible forms in which an SMS message should be delivered to the 
recipient by the service centre.

The specified format may or may not actually be supported by the service centre. 
Note that the format of the recipient address(es) may need to be different 
to a normal telephone number if the conversion is set to any other value than 
ESmsConvPIDNone, ESmsConvFax, ESmsConvPaging or ESmsConvErmes. 

@publishedAll
@released
*/
enum TSmsPIDConversion
		{
/**
No conversion (i.e. use SMS itself). 
*/
		ESmsConvPIDNone	= TSmsProtocolIdentifier::ESmsNoTelematicDevice,
/** 
Convert to fax. 
*/
		ESmsConvFax		= TSmsProtocolIdentifier::ESmsGroup3TeleFax,
/** 
Convert to X.400. 
*/
		ESmsConvX400	= TSmsProtocolIdentifier::ESmsX400MessageHandlingSystem,
/** 
Convert to pager message. 
*/
		ESmsConvPaging	= TSmsProtocolIdentifier::ESmsNationalPagingSystem,
/** 
Convert to Internet email. 
*/
		ESmsConvMail	= TSmsProtocolIdentifier::ESmsInternetElectronicMail,
/** 
Convert to ERMES paging. 
*/
		ESmsConvErmes	= TSmsProtocolIdentifier::ESmsERMES,
/**
Convert to sppech. 
*/
		ESmsConvSpeech	= TSmsProtocolIdentifier::ESmsVoiceTelephone
		};

/**
Defines the set of possible validity periods for mobile originated messages.

Mobile originated messages have a PDU type of SMS-SUBMIT.

@publishedAll
@released
*/
enum TSmcmValidityPeriod
	{
/**
Validity period of an hour.
*/
	ESmsVPHour			= TSmsValidityPeriod::EHalfHourUnitInMinutes*2,
/**
Validity period of six hours.
*/
	ESmsVPSixHours		= TSmsValidityPeriod::EHalfHourUnitInMinutes*12,
/**
Validity period of 24 hours.
*/
	ESmsVP24Hours		= TSmsValidityPeriod::EOneDayUnitInMinutes,
/**
Validity period of a week.
*/
	ESmsVPWeek			= TSmsValidityPeriod::EOneWeekUnitInMinutes,
/**
The maximum validity period - currently set as 63 weeks.
*/
	ESmsVPMaximum		= TSmsValidityPeriod::EOneWeekUnitInMinutes*63,
/**
An enum that specifies that a validity period is not supported.
*/
	ESmsVPNotSupported	= KErrNotSupported,
/**
The default validy period - currently set to a week.
*/
	ESmsVPDefault		= ESmsVPWeek
	}; 

/** 
Defines options for the preferred default sending schedule type of a new message
created using the SMS client MTM.

The option has no direct effect on the SMS MTM. When it is sending an SMS
message it automatically appends other SMS messages in the outbox whose send
states are marked as KMsvSendStatewaiting or KMsvSendSateUnknown.

Therefore it is the responsibility of the messaging client to set the correct
send state of new SMS messages correctly given the preferred sending schedule.

@publishedAll
@released
*/
enum TSmsDelivery
	{
/** 
Send message immediately. The message send state should be set to KMsvSendStateWaiting. 
*/
	ESmsDeliveryImmediately,
/** 
Send message when client requests. The message send state should be set to KMsvSendStateUponRequest.
*/
	ESmsDeliveryUponRequest,
/**
Send message at scheduled time. The message send state should be set to KMsvSendStateScheduled.
*/
	ESmsDeliveryScheduled
	};

/**
Defines the flags and masks for some of the message settings.

The settings classes CSmsMessageSettings and CSmsSettings use these flags to 
store the specified information.

@publishedAll
@released
*/
enum TSmsSettingsFlags
	{
/**
Mask that resets the flags.
*/
	ESmsSettingsNoFlags					=0x00000000,
/**
Flag that indicates if messages can be concatenated.

If true, the UI should pass a long message in one piece to the MTM. If false, 
it should split the message manually into 140 byte parts. The value of this 
flag does not affect the behaviour of the SMS MTM.
*/
	ESmsSettingsCanConcatenate			=0x00000001,
/**
Flag that indicates whether the original message should be included in relpy
messages.

If true, the received message should be quoted at the beginning of the reply 
message. If false, reply messages should be initially empty.
*/
	ESmsSettingsReplyQuoted				=0x00000002,
/**
Flag that indicates whether duplicate messages should be rejected.
*/
	ESmsSettingsRejectDuplicate			=0x00000004,
/**
Flag that indicates if a status report (TP-SRR in GSM spec 03.40) should be 
requested from the Service Centre.
*/
	ESmsSettingsDeliveryReport			=0x00000008,
/**
Flag that indicates if the reply path (Service Centre address) should be included
in the delivered message (TP-RP in GSM spec 03.40).
*/
	ESmsSettingsReplyPathRequested		=0x00000010,
/**  
Flag that indicates if a status report (TP-SRR in GSM spec 03.40) for the last segment
should be requested from the Service Centre.
*/
	ESmsSettingsLastSegmentDeliveryReport	=0x00000020	,
	
/**
Flag that indicates if a status report (TP-SRR in GSM spec 03.40) for the last segment 
is not requested from the Service Centre.
*/
	ENoSmsSettingsLastSegmentDeliveryReport	=0x00000040,
/**
Flag that indicates that the service center time stamp should be used for the date
field in any TMsvEntry that corresponds to a SMS message.
*/
	ESmsSettingsUseServiceCenterTimeStamp =0x00000080,
/**
Flag that indicates if message received from unknown port, 
discard the message without indicating the user. 
*/	
	ESmsSettingsDiscardUnknownPortMsgs =0x00000100,
/**
Mask for the set of flags defined.
*/
	ESmsSettingsMask                    =0x000000FF,
	};


/**
Defines the types of acknowledgements that can be received from recipients.
*/
enum TSmsAckType
	{
/**
Acknowlwdgement of delivery to a recipient.
*/
	ESmsAckTypeDelivery		=0	
	};

/** 
Progress information for SMS MTM operations.

This information is returned by CMsvOperation::ProgressL() in a TSmsProgressBuf 
package.

@see	CMsvOperation::ProgressL

@publishedAll
@released
*/
class TSmsProgress 
	{
public:
/** 
Defines type of operation for which progress information is being given.

These types correspond to the commands defined in TSmsMtmCommand.

@see	TSmsMtmCommand
*/
	enum TSmsProgressType
		{
/** 
The default operation type - no operation.
*/
		ESmsProgressTypeDefault,
/** 
Reading SIM parameters.
		
Pre v7.0, this was named ESmsProgressTypeReadingServiceCenterAddress.
*/
		ESmsProgressTypeReadSimParams,
/**
Writing SIM parameters.
		
Pre v7.0 this was named ESmsProgressTypeWritingServiceCenterAddress. 
*/
		ESmsProgressTypeWriteSimParams,
/**
Sending messages.
*/
		ESmsProgressTypeSending,
/**
Scheduling sending messages.
*/
		ESmsProgressTypeScheduling,
/**
Enumerating phone stores.
		
Pre v7.0, this was named ESmsProgressTypeEnumeratingSim.
*/
		ESmsProgressTypeEnumeratingPhoneStores, 
/**
Copying messages from phone stores.
		
Pre v7.0, this was named ESmsProgressTypeCopyFromSim.
*/
		ESmsProgressTypeCopyFromPhoneStore, 
/**
Moving messages from phone stores.
		
Pre v7.0, this was named ESmsProgressTypeMoveFromSim.
*/
		ESmsProgressTypeMoveFromPhoneStore, 
/**
Deleting messages from phone stores.
		
Pre v7.0, this was named ESmsProgressTypeDeleteFromSim.
*/
		ESmsProgressTypeDeleteFromPhoneStore, 
/**
Removing all messages from the task scheduler list.
*/
		ESmsProgressTypeDeleteSchedule,
/**
Checking the current scheduled status of the messages.
*/
		ESmsProgressTypeCheckSchedule,
/**
Recursively deleting messages.
*/
		ESmsProgressTypeDeleteAll,
/**
Copying messages to a phone store.
		
Pre v7.0, this was named ESmsProgressTypeCopyToSim.
*/
		ESmsProgressTypeCopyToPhoneStore,  
/**
Moving messages to a phone store.
		
Pre v7.0, this was named ESmsProgressTypeMoveToSim.
*/
		ESmsProgressTypeMoveToPhoneStore,
/**
Moving message entries.

This is the second part of ESmsProgressTypeMoveFromPhoneStore. The first part
deletes the messages from the phone storageSIM. This second part moves the 
messages from the folder that represents the phone storage to the specified
destination.
*/
		ESmsProgressTypeMovingEntries
		};
	
	inline TSmsProgress(TSmsProgressType aType = ESmsProgressTypeDefault);

public:
	
/** 
The error code.
*/
	TInt iError; 
/** 
The type of operation. 

@see	TSmsProgress::TSmsProgressType
*/
	TSmsProgressType iType;
/** 
The state of the operation.
*/
	TInt iState;
/** 
The number of recipients processed so far for the current message.
*/
	TInt iRcpDone;
/**
The total number of recipients in the current message.
*/
	TInt iRcpCount;
/** 
The nmber of messages processed so far for the current operation.
*/
	TInt iMsgDone;
/**
The total number of messages for the current operation.
*/
	TInt iMsgCount; 
/** 
The Service Centre address.
	
This is set by the command ESmsMtmCommandReadServiceCenter.

@removed
This is no longer used since v7.0.
*/
	TBuf<14> iServiceCenterAddress;
/** 
The invisible folder that contains the messages read from the SIM by the command
ESmsMtmCommandEnumeratePhoneStores.
*/
	TMsvId iEnumerateFolder;
	};

/** 
Package buffer for TSmsProgress. 

@see	TSmsProgress

@publishedAll
@released
*/
typedef TPckgBuf<TSmsProgress> TSmsProgressBuf;


/**
The SMS settings that are relevant on a per message basis.

Each SMS message has its own settings. Initially they have a default set of 
values derived from the SMS service settings. They can be changed on an
individual message basis. For instance, a messaging application may allow a
user to specify/not specify status reports individually for each message.

These settings are only relevant to mobile originated messages - those of 
SMS-SUBMIT type.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CSmsMessageSettings): public CBase
	{
public:
	IMPORT_C virtual void InternalizeL(RReadStream& aReadStream);
	IMPORT_C virtual void ExternalizeL(RWriteStream& aWriteStream) const;

	inline const TTimeIntervalMinutes& ValidityPeriod() const;
	inline void SetValidityPeriod(TTimeIntervalMinutes);

	inline void SetValidityPeriodFormat(TSmsFirstOctet::TSmsValidityPeriodFormat aValidityPeriodFormat);
	inline const TSmsFirstOctet::TSmsValidityPeriodFormat ValidityPeriodFormat() const;

	inline TBool RejectDuplicate() const;
	inline void SetRejectDuplicate(TBool);

	inline TBool DeliveryReport() const;
	inline void SetDeliveryReport(TBool);

	inline TBool ReplyPath() const;
	inline void SetReplyPath(TBool);

	inline TSmsPIDConversion MessageConversion() const;
	inline void SetMessageConversion(TSmsPIDConversion);

	inline TBool CanConcatenate() const;
	inline void SetCanConcatenate(TBool);

	inline enum TSmsDataCodingScheme::TSmsAlphabet CharacterSet() const;
	inline void SetCharacterSet(TSmsDataCodingScheme::TSmsAlphabet);

    IMPORT_C virtual void CopyL(const CSmsMessageSettings& aSettings);

	TUint32 MessageFlags() const;
	void SetMessageFlags(TUint32 aMsgFlags);
	inline TBool LastSegmentDeliveryReport() const;
	inline void SetLastSegmentDeliveryReport(TBool);

	inline void SetUseServiceCenterTimeStampForDate(TBool aUseServiceCenterTimestamp);
	inline TBool UseServiceCenterTimeStampForDate() const;

protected:

	CSmsMessageSettings();

private:

	TUint32 iMsgFlags;
	TSmsPIDConversion iMessageConversion;
	TSmsDataCodingScheme::TSmsAlphabet iAlphabet;
	TTimeIntervalMinutes iValidityPeriod;
	TSmsFirstOctet::TSmsValidityPeriodFormat iValidityPeriodFormat;
	};

/** 
The SMS Service settings.

An SMS service entry stores an object of this type in its message store. These
settings define the default settings for mobile originated messages (SMS-SUBMIT
type messages). They also provide some global settings that are applied to all
messages, both outward going and received messages.

The SMS service also uses this class to store the set of avilable Servive Centre
numbers. The default Service Centre is defined from this set.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CSmsSettings): public CSmsMessageSettings
	{
public:

/**
Defines the ways in which status reports that are received by the watcher may
be handled.

This can be applied to both SMS-STATUS-REPORT type messages and Special Messages
(e.g. Voice Mail Indication messages).
*/
	enum TSmsReportHandling
		{
/** 
The reports are put in the inbox and made invisible. 
*/
		EMoveReportToInboxInvisible,
/** 
The reports are put in the inbox and made visible.
*/
		EMoveReportToInboxVisible,
/**
The reports are deleted.
*/
		EDiscardReport,
/** 
Not supported.
*/
		EDoNotWatchForReport,
/**
Reports are put in the inbox and made invisible. The status report 
is matched against the sent mesasge and its summary info updated.
*/
		EMoveReportToInboxInvisibleAndMatch,
/**
Reports are put in the inbox and made visible. The status report 
is matched against the sent mesasge and its summary info updated.
*/
		EMoveReportToInboxVisibleAndMatch,
/**
Reports are deleted. The status report is matched against the sent
mesasge and its summary info updated.
*/
		EDiscardReportAndMatch
		};

/** 
Sets whether the service settings should be used to update the communications 
database. 

If the update is specified, it is done when the Service settings are stored into
the message store.
*/
	enum TSmsSettingsCommDbAction
		{
/** 
The comms database should not be updated.
*/
		ENone,
/** 
Use the Service settings to update the comms datebase.
*/
		EStoreToCommDb
		};

/** Defines the SMS bearer.
Modes: GSM */
enum TMobileSmsBearer
	{
	/** SMS messages will only be sent over a packet-switched (GPRS) network. */
		ESmsBearerPacketOnly,
	/** SMS messages will only be sent over a circuit-switched (GSM) network. */
		ESmsBearerCircuitOnly,
	/** SMS messages will be sent over the packet-switched (GPRS) network if possible, 
	otherwise over circuit-switched (GSM) network. */
		ESmsBearerPacketPreferred,
	/** SMS messages will be sent over the circuit-switched (GSM) network if possible, 
	otherwise over packet-switched (GPRS) network. */
		ESmsBearerCircuitPreferred
	};

public:

	IMPORT_C static CSmsSettings* NewL();
	IMPORT_C static CSmsSettings* NewLC();
	IMPORT_C virtual ~CSmsSettings();

	IMPORT_C TInt ServiceCenterCount() const;
	IMPORT_C CSmsServiceCenter& GetServiceCenter(TInt aIndex) const;
	IMPORT_C void AddServiceCenterL(const TDesC& aName,const TDesC& aNumber);
	IMPORT_C void RemoveServiceCenter(TInt aIndex);
	IMPORT_C TInt DefaultServiceCenter() const;
	IMPORT_C void SetDefaultServiceCenter(TInt aDefaultSC);
	inline TSmsDelivery Delivery() const;
	inline void SetDelivery(TSmsDelivery);

	inline TBool ReplyQuoted() const;
	inline void SetReplyQuoted(TBool);

    IMPORT_C void CopyL(const CSmsSettings& aSmsSettings);

	inline TSmsReportHandling StatusReportHandling() const;
	inline void SetStatusReportHandling(TSmsReportHandling aStatusReportHandling);
	inline TSmsReportHandling SpecialMessageHandling() const;
	inline void SetSpecialMessageHandling(TSmsReportHandling aSpecialMessageHandling);

	inline void SetCommDbAction(TSmsSettingsCommDbAction aCommDbAction);
	inline TSmsSettingsCommDbAction CommDbAction() const;

	inline void SetSmsBearerAction(TSmsSettingsCommDbAction aSmsBearerAction);
	inline TSmsSettingsCommDbAction SmsBearerAction() const;

	inline void SetSmsBearer(TMobileSmsBearer aSmsBearer);
	inline TMobileSmsBearer SmsBearer() const;

	inline void SetClass2Folder(TMsvId aId);
	inline TMsvId Class2Folder() const;

	inline void SetDescriptionLength(TInt aLength);
	inline TInt DescriptionLength() const;

	inline TBool DiscardUnknownPortMessage() const;
	inline void SetDiscardUnknownPortMessage(TBool aDeleteMessage);
	TUint32 SettingsFlags() const;
	void SetSettingsFlags(TUint32 aSetFlags);

	void RemoveSCAddresses();
	void StoreToCommDbL() const;

private:

	CSmsSettings();
	void ConstructL();

private:

	TUint32 iSetFlags;
	CArrayPtrFlat<CSmsServiceCenter> *iServiceCenters;
	TSmsDelivery iDelivery;
	TInt iDefaultSC;
	TSmsReportHandling iStatusReportHandling;
	TSmsReportHandling iSpecialMessageHandling;
	TSmsSettingsCommDbAction iCommDbAction;
	TSmsSettingsCommDbAction iSmsBearerAction;
	TMobileSmsBearer iSmsBearer;
	TMsvId iClass2Folder;
	TInt iDescriptionLength;
	};

/** 
Recipient information for an SMS message.

A SMS-SUBMIT message may have several recipients

@publishedAll
@released
*/
NONSHARABLE_CLASS(CSmsNumber): public CMsvRecipient
	{
public:

/**
Defines the status of an acknowledgement for a recipient.
*/
	enum TSmsAckStatus
		{
/**
An acknowledgement for this recipient has not been requested.
*/
		ENoAckRequested		= 0,
/**
The delivery status for this recipient has been requested, but the status report
has not yet been received.
*/
		EPendingAck,
/**
A successful acknowledgement for this recipient has been received.
*/
		EAckSuccessful,
/**
A failed acknowledgement for this recipient has been received.
*/
		EAckError
		};

public:

	IMPORT_C static CSmsNumber* NewL();
	IMPORT_C static CSmsNumber* NewL(const CSmsNumber& aSmsNumber);
	IMPORT_C virtual ~CSmsNumber();

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	IMPORT_C TPtrC Address() const;
	IMPORT_C void SetAddressL(const TDesC& aAddress);
	void SetEmailAddressL(const TDesC& aAddress, CSmsEmailFields* aEmailFields, const TDesC& aAlias);

	IMPORT_C TPtrC Name() const;
	IMPORT_C void SetNameL(const TDesC& aName);

	inline TLogId LogId() const;
	inline void SetLogId(TLogId aLogId);

	IMPORT_C CSmsNumber& CopyL(const CSmsNumber& aSmsNumber);

	IMPORT_C TSmsAckStatus AckStatus(TSmsAckType aAckType) const;
	IMPORT_C void SetAckStatus(TSmsAckType aAckType, TSmsAckStatus aAckStatus);
	
private:

	CSmsNumber();
	TPtrC LimitStringSize(const TPtrC& aString, TInt aMaxLength) const;

private:

	HBufC* iNumber;
	HBufC* iName;
	TLogId iLogId;
	TSmsAckStatus iDeliveryStatus;
	};

/** 
Used to store Service Centre numbers and their associated name.

@publishedAll
@released
*/
NONSHARABLE_CLASS (CSmsServiceCenter) : public CBase
	{
public:
	IMPORT_C static CSmsServiceCenter* NewL();
	IMPORT_C static CSmsServiceCenter* NewL(const CSmsServiceCenter& aSmsSC);
	IMPORT_C ~CSmsServiceCenter();

	IMPORT_C TPtrC Address() const;
	IMPORT_C void SetAddressL(const TDesC& aAddress);

	IMPORT_C TPtrC Name() const;
	IMPORT_C void SetNameL(const TDesC& aName);
	
	IMPORT_C CSmsServiceCenter& CopyL(const CSmsServiceCenter& aSmsSc);

private:
	CSmsServiceCenter();
	TPtrC LimitStringSize(const TPtrC& aString, TInt aMaxLength) const;
	
private:
	HBufC* iNumber;
	HBufC* iName;
	};

#include <smutset.inl>

#endif // __SMUTSET_H__
