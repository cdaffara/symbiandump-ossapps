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
//


#include <etelmm.h>
#include "SMCMMAIN.H"
#include "SMUTSET.H"

#include <commsdattypesv1_1.h>
#include <commsdat.h>
#include <txtetext.h>
#include <csmsemailfields.h>
#include <logwraplimits.h>
using namespace CommsDat;



// Compile time check to see if the enums RMobileSmsMessaging::TMobileSmsBearer and CSmsSettings::TMobileSmsBearer
// are equivalent.

#undef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

C_ASSERT(RMobileSmsMessaging::ESmsBearerPacketOnly==CSmsSettings::ESmsBearerPacketOnly);
C_ASSERT(RMobileSmsMessaging::ESmsBearerCircuitOnly==CSmsSettings::ESmsBearerCircuitOnly);
C_ASSERT(RMobileSmsMessaging::ESmsBearerPacketPreferred==CSmsSettings::ESmsBearerPacketPreferred);
C_ASSERT(RMobileSmsMessaging::ESmsBearerCircuitPreferred==CSmsSettings::ESmsBearerCircuitPreferred);

const TInt16 KMsgFileSmsMessageSettingsVersion	= 1;
const TInt16 KMsgFileSmsNumberVersion			= 2;
const TInt KSmcmSCAddressGranularity			= 8;


/**
Constructor.
*/
CSmsMessageSettings::CSmsMessageSettings()
:	CBase(),
	iValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet)
	{
	}

/** 
Internalises the settings from a read stream.

@param	aReadStream
The stream from which the settings should be internalised.
*/
EXPORT_C void CSmsMessageSettings::InternalizeL(RReadStream& aReadStream)
	{
	aReadStream.ReadInt16L(); // version - not used yet

	iValidityPeriod		= aReadStream.ReadInt32L();
	iValidityPeriodFormat = (TSmsFirstOctet::TSmsValidityPeriodFormat) aReadStream.ReadInt8L();
	iAlphabet			= (TSmsDataCodingScheme::TSmsAlphabet) aReadStream.ReadInt16L();
	iMsgFlags			= aReadStream.ReadUint32L();
	iMessageConversion	= (TSmsPIDConversion) aReadStream.ReadInt16L();
	}

/** 
Externalises the settings to a write stream.

@param	aWriteStream
The stream to which the settings should be externalised.
*/
EXPORT_C void CSmsMessageSettings::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(KMsgFileSmsMessageSettingsVersion);
	aWriteStream.WriteInt32L(iValidityPeriod.Int());
	aWriteStream.WriteInt8L(iValidityPeriodFormat);
	aWriteStream.WriteInt16L(iAlphabet);
	aWriteStream.WriteUint32L(iMsgFlags);
	aWriteStream.WriteInt16L(iMessageConversion);
	}

/** 
Copies the settings stored in the specified object into this object.

@param	aSettings
The settings to be copied.
*/
EXPORT_C void CSmsMessageSettings::CopyL(const CSmsMessageSettings& aSettings)
    {
	iMsgFlags			= aSettings.iMsgFlags;
	iMessageConversion	= aSettings.iMessageConversion;
	iAlphabet			= aSettings.iAlphabet;
	SetValidityPeriod(aSettings.ValidityPeriod());
	SetValidityPeriodFormat(aSettings.ValidityPeriodFormat());
    }

/**
Returns SMS message settings flag

@return
SMS message settings flag
*/     
TUint32 CSmsMessageSettings::MessageFlags() const
	{
	return iMsgFlags;
	}

/**
Sets SMS message settings flag

@param aMsgFlags
SMS message settings flag
*/   
void CSmsMessageSettings::SetMessageFlags(TUint32 aMsgFlags)
	{
	iMsgFlags = aMsgFlags;
	}

/*
 * CSmsSettings
 */

/** 
Allocates and constructs an SMS settings object.

Initialises all member data to their default values.

@return
The newly constructed SMS settings object.
*/
EXPORT_C CSmsSettings* CSmsSettings::NewL()
	{
	CSmsSettings* self = CSmsSettings::NewLC();
	CleanupStack::Pop(self); 
	return self;
	}

/** 
Allocates and constructs an SMS settings object.

Initialises all member data to their default values.

@return
The newly constructed SMS settings object.
*/
EXPORT_C CSmsSettings* CSmsSettings::NewLC()
	{
	CSmsSettings* self = new (ELeave) CSmsSettings();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Constructor.
*/
CSmsSettings::CSmsSettings()
:	CSmsMessageSettings(),
	iDefaultSC(-1),
	iStatusReportHandling(EDoNotWatchForReport),
	iSpecialMessageHandling(EDoNotWatchForReport),
	iCommDbAction(ENone),
	iSmsBearerAction(ENone),
	iSmsBearer(ESmsBearerCircuitPreferred),
	iClass2Folder(KMsvGlobalInBoxIndexEntryId),
	iDescriptionLength(KSmsDescriptionLength)
	{		
	}

/** 
Destructor. 
*/
EXPORT_C CSmsSettings::~CSmsSettings()
	{
	if (iServiceCenters)
		{
		iServiceCenters->ResetAndDestroy();
	    delete iServiceCenters;
		}
	}

/**
Second phase construction.
*/
void CSmsSettings::ConstructL()
	{
	iServiceCenters = new (ELeave) CArrayPtrFlat<CSmsServiceCenter>(KSmcmSCAddressGranularity);
	}


void CSmsSettings::StoreToCommDbL() const
	{
	if (iCommDbAction != EStoreToCommDb && iSmsBearerAction != EStoreToCommDb)
		return; // nothing to store to the Comm DB
	
	CMDBSession *dbSession = CMDBSession::NewL(KCDVersion1_2);	
	CleanupStack::PushL(dbSession);	

	//Create a CCDModemBearerRecord link field container to read the 
	//RecordID in the CCDGlobalSettingsRecord.iModemForPhoneServicesAndSMS
	CMDBRecordLink<CCDModemBearerRecord> *modemBearerRecordSet = new(ELeave) CMDBRecordLink<CCDModemBearerRecord>(KCDTIdGlobalSettingsRecord | KCDTIdModemPhoneServicesSMS); 
	CleanupStack::PushL(modemBearerRecordSet);

	//RecordID needs to be set to 1, as there is always just one GlobalSettings record 
	modemBearerRecordSet->SetRecordId(1); 
	
	//Load field container with data from database
	TRAPD(err, modemBearerRecordSet->LoadL(*dbSession));
	if(err != KErrNone)
		{
		User::Leave(KErrNotFound);				
		}

	if (iCommDbAction == EStoreToCommDb)
		{
		//Create a container for a ModemBearerRecord and read its contents
		CCDModemBearerRecord *modemRecord = static_cast<CCDModemBearerRecord*> (CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
		CleanupStack::PushL(modemRecord);

		//Set the record ID to that previously read from CCDGlobalSettingsRecord.iModemForPhoneServicesAndSMS
		modemRecord->SetRecordId(*modemBearerRecordSet);

		//Load record container with data from database
		modemRecord->LoadL(*dbSession);

		if (iDefaultSC > -1 && iServiceCenters->Count())
			{
			const TPtrC address = GetServiceCenter(iDefaultSC).Address();			
			modemRecord->iMessageCentreNumber.SetMaxLengthL(address.Length());
			modemRecord->iMessageCentreNumber = address;
			}
		else
			{			
			modemRecord->iMessageCentreNumber = KNullDesC;
			}

		const TUint32 valPeriod = (TUint32) ValidityPeriod().Int();		
		modemRecord->iMessageValidityPeriod = valPeriod;
		
		const TBool delReport = DeliveryReport() ? ETrue : EFalse;		
		modemRecord->iMessageDeliveryReport = delReport;		

		modemRecord->ModifyL(*dbSession);
		
		CleanupStack::PopAndDestroy(modemRecord);
		}

	if (iSmsBearerAction == EStoreToCommDb)				
		{
		CCDGlobalSettingsRecord* globalSettingsRecord = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
		CleanupStack::PushL(globalSettingsRecord);

		//RecordID needs to be set to 1, as there is always just one GlobalSettings record
		globalSettingsRecord->SetRecordId(1);
		
		TRAPD(err, globalSettingsRecord->LoadL(*dbSession));
		if(err != KErrNone)
			{
			User::Leave(KErrNotFound);				
			}	

		globalSettingsRecord->iSMSBearer = SmsBearer();
		globalSettingsRecord->iSMSBearer.ModifyL(*dbSession);
		CleanupStack::PopAndDestroy(globalSettingsRecord);
		}

	CleanupStack::PopAndDestroy(modemBearerRecordSet);
	CleanupStack::PopAndDestroy(dbSession);
	}


/** 
Gets the number of Service Centre addresses stored in this object.

@return
The number of addresses.
*/   
EXPORT_C TInt CSmsSettings::ServiceCenterCount() const
	{
	return iServiceCenters->Count();
	}

/** 
Gets the Service Centre address at the specified index in the list.

@param	aIndex
The index of the specified address.

@return
The indexed address. 
*/
EXPORT_C CSmsServiceCenter& CSmsSettings::GetServiceCenter(TInt aIndex) const
	{
	return *iServiceCenters->At(aIndex);	
	}

/**
Appends a service centre address (name and number) to the list.

@param	aName
The Service Centre name

@param	aNumber
The Service Centre number.
*/	
EXPORT_C void CSmsSettings::AddServiceCenterL(const TDesC& aName,const TDesC& aNumber)
	{
	
	CSmsServiceCenter* recipient=CSmsServiceCenter::NewL();
	CleanupStack::PushL(recipient);
	recipient->SetNameL(aName);
	recipient->SetAddressL(aNumber);

	iServiceCenters->AppendL(recipient);
	CleanupStack::Pop(recipient);

	if (iServiceCenters->Count() == 1 || iDefaultSC < 0)
		iDefaultSC = 0;	
	}

/** 
Removes the service centre address with index aIndex from the array of service 
centre addresses that are stored in the object. 

@param	aIndex
The index of the Service Centre address to remove. 
*/
EXPORT_C void CSmsSettings::RemoveServiceCenter(TInt aIndex)
	{
	delete iServiceCenters->At(aIndex);
	iServiceCenters->Delete(aIndex);

	if (iDefaultSC == aIndex)
		{
		iDefaultSC = iServiceCenters->Count() ? 0 : -1;
		}
	else if (iDefaultSC > aIndex)
		{
		iDefaultSC--;
		}	
	}

/**
Gets the index of the default service centre address for this service.

@return
The index of the default service centre address.
*/	
EXPORT_C TInt CSmsSettings::DefaultServiceCenter() const
	{
	return iDefaultSC;
	}

/** 
Sets the index of the default service centre address for this service.

@param	aDefaultSC
An index into the list of the default service centre address.

@panic	SMCM	7
The specified index is negative or beyond the maximum index (debug only).
*/	
EXPORT_C void CSmsSettings::SetDefaultServiceCenter(TInt aDefaultSC)
	{
	__ASSERT_DEBUG(aDefaultSC>=0, Panic(ESmscDefaultSCOutOfRange));
	__ASSERT_DEBUG(aDefaultSC<iServiceCenters->Count(), Panic(ESmscDefaultSCOutOfRange));

	iDefaultSC=aDefaultSC;
	}

void CSmsSettings::RemoveSCAddresses()
	{
	iServiceCenters->ResetAndDestroy();
	}

/**
Copies the SMS settings stored in the specified object into this object.

@param	aSmsSettings
The SMS Settings to be copied.
*/
EXPORT_C void CSmsSettings::CopyL(const CSmsSettings& aSmsSettings)
    {
	CSmsMessageSettings::CopyL(aSmsSettings);

    iServiceCenters->ResetAndDestroy();
	const TInt count = aSmsSettings.ServiceCenterCount();
    for (TInt i=0; i < count; i++)
        {
        CSmsServiceCenter* scNumber = CSmsServiceCenter::NewL(aSmsSettings.GetServiceCenter(i));
		CleanupStack::PushL(scNumber);
        iServiceCenters->AppendL(scNumber);
		CleanupStack::Pop(scNumber);
        }

	iDelivery				= aSmsSettings.iDelivery;
	iDefaultSC				= aSmsSettings.iDefaultSC;
	iSpecialMessageHandling	= aSmsSettings.iSpecialMessageHandling;
	iStatusReportHandling	= aSmsSettings.iStatusReportHandling;
	iSetFlags				= aSmsSettings.iSetFlags;
	iCommDbAction			= aSmsSettings.iCommDbAction;
	iSmsBearerAction		= aSmsSettings.iSmsBearerAction;
	iSmsBearer				= aSmsSettings.iSmsBearer;
	iClass2Folder			= aSmsSettings.iClass2Folder;
	iDescriptionLength		= aSmsSettings.iDescriptionLength;
    }
   
/**
Returns SMS settings flag

@return
SMS settings flag
*/  
TUint32 CSmsSettings::SettingsFlags() const
	{
	return iSetFlags;
	}

/**
Sets SMS settings flag

@param aSetFlags
SMS settings flag
*/  
void CSmsSettings::SetSettingsFlags(TUint32 aSetFlags)
	{
	iSetFlags = aSetFlags;
	}    

/*
 * CSmsNumber
 */
 
/**
Creates and allocates a new CSmsNumber object.

@return
The new object.
*/
EXPORT_C CSmsNumber* CSmsNumber::NewL()
	{
	return new(ELeave) CSmsNumber();
	}

/** 
Creates and allocates a new CSmsNumber object, copying an existing object.

@param	aSmsNumber
The object to be copied.

@return
The new object.
*/
EXPORT_C CSmsNumber* CSmsNumber::NewL(const CSmsNumber& aSmsNumber)
	{
	CSmsNumber* number=new(ELeave) CSmsNumber();
	CleanupStack::PushL(number);
	number->CopyL(aSmsNumber);
	CleanupStack::Pop(number);
	return number;
	}

/**
Destructor.
*/
EXPORT_C CSmsNumber::~CSmsNumber()
	{
	delete iNumber;
	delete iName;
	}

/**
Internalises the object from a read stream.

@param	aStream
The stream from which the object should be internalised.
*/
EXPORT_C void CSmsNumber::InternalizeL( RReadStream& aStream)
	{
	TInt16 version = aStream.ReadInt16L();

	CMsvRecipient::InternalizeL(aStream);

	HBufC* number = HBufC::NewL(aStream, KSmcmSmsNumberMaxNumberLength);
	delete iNumber;
	iNumber = number;

	HBufC* name = HBufC::NewL(aStream, KSmcmSmsNumberMaxNameLength);
	delete iName;
	iName = name;

	iLogId = aStream.ReadInt32L();

	if( version >= 2 )
		{
		iDeliveryStatus = (TSmsAckStatus) aStream.ReadInt8L();
		}
	else
		{
		iDeliveryStatus = ENoAckRequested;
		}
	}

/**
Externalises the object to a write stream.

@param	aStream
The stream to which the object should be externalised.
*/
EXPORT_C void CSmsNumber::ExternalizeL( RWriteStream& aStream) const
	{
	aStream.WriteInt16L(KMsgFileSmsNumberVersion);

	CMsvRecipient::ExternalizeL(aStream);

	aStream << Address();
	aStream << Name();

	aStream.WriteInt32L(iLogId);
	aStream.WriteInt8L(iDeliveryStatus);
	}

/**
Constructor.
*/
CSmsNumber::CSmsNumber()
: iLogId(KLogNullId)
	{
	}

/**
Copies settings from another CSmsNumber object.

@param	aSmsNumber
The object to be copied.

@return
This object with the updated settings.
*/
EXPORT_C CSmsNumber& CSmsNumber::CopyL(const CSmsNumber& aSmsNumber)
	{
	SetNameL(aSmsNumber.Name());
	SetAddressL(aSmsNumber.Address());
	SetLogId(aSmsNumber.LogId());
	SetAckStatus(ESmsAckTypeDelivery, aSmsNumber.AckStatus(ESmsAckTypeDelivery));
    return *this;
	}

/**
Gets the recipient address.

The address is a telephone number. It is limited to a maximum length specified
by KSmcmSmsNumberMaxNumberLength.

@see	KSmcmSmsNumberMaxNumberLength

@return	
A descriptor with the recipient address.
*/
EXPORT_C TPtrC CSmsNumber::Address() const
	{
	TPtrC number = iNumber ? iNumber->Des() : TPtrC();
	return number;
	}

/** 
Gets the recipient name.

@return
A descriptor with the recipient name.
*/
EXPORT_C TPtrC CSmsNumber::Name() const
	{
	TPtrC name = iName ? iName->Des() : TPtrC();
	return name;
	}

/**
Sets the recipient address.

The address is a telephone number. It is limited to a maximum length specified
by KSmcmSmsNumberMaxNumberLength.

@see	KSmcmSmsNumberMaxNumberLength

@param	aAddress
The recipient address.
*/
EXPORT_C void CSmsNumber::SetAddressL(const TDesC& aAddress)
	{
	TPtrC clippedNumber = LimitStringSize(aAddress, KSmcmSmsNumberMaxNumberLength);
	HBufC* number = clippedNumber.AllocL();
	delete iNumber;
	iNumber = number;
	}

/**
Sets the recipient email address, The address here is a email address.

@param	aAddress
The recipient address.
@param	aEmailFields
Encapsulates the address fields for Email sent over SMS.
@param	aAlias
*/

// For address information separation (start)
const TUint KMsgSmsAddressStartChar         ('<');
// For address information separation (end)
const TUint KMsgSmsAddressEndChar           ('>');

void CSmsNumber::SetEmailAddressL(const TDesC& aAddress, CSmsEmailFields* aEmailFields, const TDesC& aAlias)
	{
    if ( aAlias.Length() > 0 )
        {
        HBufC* aliasBuf = HBufC::NewLC( aAddress.Length() + aAlias.Length() + 3 );
        TPtr aliasTPtr = aliasBuf->Des();
        aliasTPtr.Append( aAlias );
        aliasTPtr.Append( CEditableText::ESpace );
        aliasTPtr.Append( KMsgSmsAddressStartChar );
        aliasTPtr.Append( aAddress );
        aliasTPtr.Append( KMsgSmsAddressEndChar );
        aEmailFields->AddAddressL( aliasTPtr );
       
        CleanupStack::PopAndDestroy( aliasBuf );
        }
    else
        {
        aEmailFields->AddAddressL( aAddress );
        }			
	}

/**
Sets the recipient name.

The name is limited to a maximum length specified by KSmcmSmsNumberMaxNameLength.

@see	KSmcmSmsNumberMaxNameLength

@param	aName
The recipient name.
*/
EXPORT_C void CSmsNumber::SetNameL(const TDesC& aName)
	{
	TPtrC clippedName = LimitStringSize(aName, KSmcmSmsNumberMaxNameLength);
	HBufC* name = clippedName.AllocL();
	delete iName;
	iName = name;
	}

/**
Gets the delivery status for this recipient.

@return
The current delivery status for this recipient.

@see	CSmsNumber::TSmsDeliveryStatus
*/
EXPORT_C CSmsNumber::TSmsAckStatus CSmsNumber::AckStatus(TSmsAckType aAckType) const
	{
	TSmsAckStatus status = ENoAckRequested;
	switch( aAckType )
		{
	case ESmsAckTypeDelivery:
		status = iDeliveryStatus;
		break;
	default:
		// Use the default...
		break;
		}
	return status;
	}
	
/**
Sets the delivery status for this recipient.

@param
aDeliveryStatus	The new delivery status for this recipient.

@see	CSmsNumber::TSmsDeliveryStatus
*/
EXPORT_C void CSmsNumber::SetAckStatus(TSmsAckType aAckType, TSmsAckStatus aAckStatus)
	{
	switch( aAckType )
		{
	case ESmsAckTypeDelivery:
		iDeliveryStatus = aAckStatus;
		break;
	default:
		// Do nothing - fail gracefully
		break;
		}
	}

/**
Returns a limited version of the input string.

The output string is limited by the maximum length specified or the actual#
length of the input string, withever is the shortest.

@param	aString
The input string to be limited.

@param	aMaxLength
The maximum length

@return
The input string limited by the shortest value between its original length or
the maximum length specified.
*/
TPtrC CSmsNumber::LimitStringSize(const TPtrC& aString, TInt aMaxLength) const
	{
	if (aString.Length() <= aMaxLength)
		return aString;
	else
		return aString.Left(aMaxLength);
	}
	
/**
Creates and allocates a new CSmsServiceCenter object.

@return
The new object.
*/
EXPORT_C CSmsServiceCenter* CSmsServiceCenter::NewL()
	{
	return new(ELeave) CSmsServiceCenter();
	}

/** 
Creates and allocates a new CSmsServiceCenter object, copying an existing object.

@param	CSmsServiceCenter
The object to be copied.

@return
The new object.
*/
EXPORT_C CSmsServiceCenter* CSmsServiceCenter::NewL(const CSmsServiceCenter& aSmsSc)
	{
	CSmsServiceCenter* sc = new(ELeave) CSmsServiceCenter();
	CleanupStack::PushL(sc);
	sc->CopyL(aSmsSc);
	CleanupStack::Pop(sc);
	return sc;
	}

/**
Destructor.
*/
EXPORT_C CSmsServiceCenter::~CSmsServiceCenter()
	{
	delete iNumber;
	delete iName;
	}
	
/**
Constructor.
*/
CSmsServiceCenter::CSmsServiceCenter()
	{
	}

/**
Copies settings from another CSmsServiceCenter object.

@param	aSmsSc
The object to be copied.

@return
This object with the updated settings.
*/
EXPORT_C CSmsServiceCenter& CSmsServiceCenter::CopyL(const CSmsServiceCenter& aSmsSc)
	{
	SetNameL(aSmsSc.Name());
	SetAddressL(aSmsSc.Address());
    return *this;
	}

/**
Gets the recipient address.

The address is a telephone number. It is limited to a maximum length specified
by KSmcmSmsNumberMaxNumberLength.

@see	KSmcmSmsNumberMaxNumberLength

@return	
A descriptor with the recipient address.
*/
EXPORT_C TPtrC CSmsServiceCenter::Address() const
	{
	TPtrC number = iNumber ? iNumber->Des() : TPtrC();
	return number;
	}

/** 
Gets the recipient name.

@return
A descriptor with the recipient name.
*/
EXPORT_C TPtrC CSmsServiceCenter::Name() const
	{
	TPtrC name = iName ? iName->Des() : TPtrC();
	return name;
	}

/**
Sets the recipient address.

The address is a telephone number. It is limited to a maximum length specified
by KSmcmSmsNumberMaxNumberLength.

@see	KSmcmSmsNumberMaxNumberLength

@param	aAddress
The recipient address.
*/
EXPORT_C void CSmsServiceCenter::SetAddressL(const TDesC& aAddress)
	{
	TPtrC clippedNumber = LimitStringSize(aAddress, KSmcmSmsNumberMaxNumberLength);
	HBufC* number = clippedNumber.AllocL();
	delete iNumber;
	iNumber = number;
	}

/**
Sets the recipient name.

The name is limited to a maximum length specified by KSmcmSmsNumberMaxNameLength.

@see	KSmcmSmsNumberMaxNameLength

@param	aName
The recipient name.
*/
EXPORT_C void CSmsServiceCenter::SetNameL(const TDesC& aName)
	{
	TPtrC clippedName = LimitStringSize(aName, KSmcmSmsNumberMaxNameLength);
	HBufC* name = clippedName.AllocL();
	delete iName;
	iName = name;
	}
	
/**
Returns a limited version of the input string.

The output string is limited by the maximum length specified or the actual#
length of the input string, withever is the shortest.

@param	aString
The input string to be limited.

@param	aMaxLength
The maximum length

@return
The input string limited by the shortest value between its original length or
the maximum length specified.
*/
TPtrC CSmsServiceCenter::LimitStringSize(const TPtrC& aString, TInt aMaxLength) const
	{
	if (aString.Length() <= aMaxLength)
		{
		return aString;
		}
	else
		{
		return aString.Left(aMaxLength);
		}
	}
	
