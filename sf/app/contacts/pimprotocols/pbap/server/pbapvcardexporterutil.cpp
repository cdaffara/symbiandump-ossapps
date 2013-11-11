// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// pbapvcardexporterimpl.cpp
// 
//

#include "pbapvcardexporterutil.h"

#include <cntitem.h>
#include <s32strm.h>
#include <vprop.h>
#include <vcard.h>
#include <centralrepository.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "vcard3.h"
#include "cntdb_internal.h"
#endif
#include <vtoken.h>
#include "pbaplogeng.h"
#include "btaccesshostlog.h"

/* These constants are properly defined in telconfigcrkeys.h, however we 
are not allowed to include that from here.  As a temporary fix the constants 
from that file are re-defined here. */
const TUid KCRUidTelConfiguration = {0x102828B8};
const TUint32 KTelMatchDigits = 0x00000001;
/* And similarly this constant is defined in ccsdefs.h */
const TInt KDefaultGsmNumberMatchLength = 7;

//constants
_LIT8(KVersitTokenCALLDATETIME,"X-IRMC-CALL-DATETIME");
_LIT8(KVersitParamMISSED,"MISSED");
_LIT8(KVersitParamRECEIVED,"RECEIVED");
_LIT8(KVersitParamDIALED,"DIALED");	// US English spelling to conform with spec


/*static*/ CPbapVCardExporterUtil* CPbapVCardExporterUtil::NewL(CContactDatabase& aDatabase, 
																CPbapLogWrapper* aLogWrapper)
	{
	LOG_STATIC_FUNC
	CPbapVCardExporterUtil* self = new(ELeave) CPbapVCardExporterUtil(aDatabase, aLogWrapper);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbapVCardExporterUtil::CPbapVCardExporterUtil(CContactDatabase& aDatabase, CPbapLogWrapper* aLogWrapper)
: iDatabase(aDatabase), iLogWrapper(aLogWrapper)
	{
	LOG_FUNC
	}

void CPbapVCardExporterUtil::ConstructL()
	{
	LOG_FUNC
	CVersitTlsData::VersitTlsDataL();		
	User::LeaveIfError(iTzClient.Connect());
	
	// Read the amount of digits to be used in contact matching
	// The key is properly owned by PhoneApp, however we cannot include
	// that header file from here, so a temporary fix has been done to 
	// use locally defined versions.  If there is a problem here it is 
	// likely because these values have gone out of sync.
	CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
    if ( repository->Get(KTelMatchDigits, iMatchDigitCount) == KErrNone )
        {
        // Min is 7
        iMatchDigitCount = Max(iMatchDigitCount, KDefaultGsmNumberMatchLength);
        }
    else
        {
        iMatchDigitCount = KDefaultGsmNumberMatchLength;
        }
	CleanupStack::PopAndDestroy(repository);
	}

CPbapVCardExporterUtil::~CPbapVCardExporterUtil()
	{
	LOG_FUNC
	iTzClient.Close();
	CVersitTlsData::CloseVersitTlsData();
	}
	
/**
 Export a contact as vCard.

 @param aContactId Contact item ID to export. If this value is set to KNullContactId an empty vCard
       containing only the mandatory properties defined in the PBAP specification will be exported
 @param aWriteStream Stream to externalize vCard data to
 @param aFormat Version of vCard specification 2.1 or 3.0
 @param aFilter Defines properties to be exported
 */
void CPbapVCardExporterUtil::ExportContactL(TContactItemId aContactId, 
											RWriteStream& aWriteStream,
											TVCardVersion aFormat, 
											TUint64 aFilter)
	{
	LOG_FUNC
	CContactIdArray* contactIdArray = CContactIdArray::NewLC();
	contactIdArray->AddL(aContactId);
	iDatabase.ExportSelectedContactsL(TUid::Uid(KUidPBAPVCardConvImpl), *contactIdArray, aWriteStream, 
										0, aFilter, NULL, aFormat, ETrue);
	CleanupStack::PopAndDestroy(contactIdArray);
	}


/**
 Export log engine event as a vCard.

 @param aEvent Log engine event to be exported
 @param aWriteStream Stream to externalize vCard data to
 @param aFormat Version of vCard specification 2.1 or 3.0
 @param aFilter Defines properties to be exported
 */
void CPbapVCardExporterUtil::ExportCallHistoryL(const CLogEvent& aLogEvent, 
												RWriteStream& aWriteStream,
												TVCardVersion aFormat, 
												TUint64 aFilter)
	{
	LOG_FUNC
	TContactItemId contactId = aLogEvent.Contact();	
	
	if (contactId == KNullContactId && aLogEvent.RemoteParty() != KNullDesC)
		{
		// The S60 phonebook uses the remote party for the name, so if there isn't a name
		// there won't be a contact, so we only look up the contact if there is a Remote Party
		contactId = FindContactIdFromNumberL(aLogEvent.Number());
		}
	
	if (ContactExistsL(contactId))
		{
		// store the log event, format and filter parameters to use in the callback
		// from the contacts vCard converter
		iLogEvent = &aLogEvent;
		iLogEventContactId = contactId;
		iFormat = aFormat;
		iFilter = aFilter;
	
		CContactIdArray* contactIdArray = CContactIdArray::NewLC();
		contactIdArray->AddL(contactId);
		iDatabase.ExportSelectedContactsL(TUid::Uid(KUidPBAPVCardConvImpl), *contactIdArray, aWriteStream, 0, aFilter, this, aFormat, EFalse);
		CleanupStack::PopAndDestroy(contactIdArray);
		}
	else
		{
		//no associated contact so only export properties which map to log entry fields
		CParserVCard* vCard = CreateVCardParserLC(aFormat);
		vCard->SetDefaultCharSet(Versit::EUTF8CharSet);

		// N property mandated by vCard 2.1 and 3.0 specifications
		vCard->AddPropertyL(EmptyNamePropertyL());
		
		// FN property mandated by vCard 3.0 specification
		if (aFormat==EPBAPVCard30)
			{
			//add empty FN property
			vCard->AddPropertyL(DesPropertyL(KVersitTokenFN, KNullDesC));
			}

		// add mandatory TEL property using number from call log
		vCard->AddPropertyL(DesPropertyL(KVersitTokenTEL, aLogEvent.Number()));

		// add the X-IRMC-CALL-DATETIME property
		if (aFilter == SymbianPBAP::KPbapAttributeAll || aFilter&SymbianPBAP::KPbapAttributeMaskCallDateTime)
			{
			vCard->AddPropertyL(CallDateTimePropertyL(aLogEvent, aFormat));
			}

		vCard->ExternalizeL(aWriteStream);
		CleanupStack::PopAndDestroy(vCard);
		}
	}

TContactItemId CPbapVCardExporterUtil::FindContactIdFromNumberL(const TDesC& aNumber)
	{
	TContactItemId ret = KNullContactId;
	CContactIdArray* contactIdArray = iDatabase.MatchPhoneNumberL(aNumber, iMatchDigitCount);
	if (contactIdArray->Count() > 0)
		{
		ret = (*contactIdArray)[0];
		}
	delete contactIdArray;
	return ret;
	}

/**
Writes a vCard to the stream containing only the mandatory properties defined in the PBAP specification
with all properties set to empty (null) values
*/
void CPbapVCardExporterUtil::ExportEmptyVCardL(RWriteStream& aWriteStream, TVCardVersion aFormat)
	{
	LOG_FUNC
	CParserVCard* vCard = CreateVCardParserLC(aFormat);
	vCard->SetDefaultCharSet(Versit::EUTF8CharSet);
	
	vCard->AddPropertyL(EmptyNamePropertyL());

	if (aFormat == EPBAPVCard30)
		{
		vCard->AddPropertyL(DesPropertyL(KVersitTokenFN, KNullDesC));
		}

	vCard->AddPropertyL(DesPropertyL(KVersitTokenTEL, KNullDesC));

	vCard->ExternalizeL(aWriteStream);
	CleanupStack::PopAndDestroy(vCard);
	}

/**
PBAP clients are supposed to provide any information regarding the contact item represented by aContactId.
This information should be in form of standard vCard property, all such properties should be appended to the array aPropertyList.
*/
void CPbapVCardExporterUtil::AddIntraContactPropertiesL(const TContactItemId& aContactId, 
														CArrayPtr<CParserProperty>* aPropertyList)
	{
	LOG_FUNC
	if(iLogEvent && iLogEventContactId == aContactId)
		{
		CParserProperty* property;

 		// add mandatory TEL property using number from log event
		property = DesPropertyL(KVersitTokenTEL, iLogEvent->Number());
		CleanupStack::PushL(property);
		aPropertyList->AppendL(property);
		CleanupStack::Pop(property); //ownership passed
		
		// add X-IRMC-CALL-DATETIME property
		if (iFilter == SymbianPBAP::KPbapAttributeAll || iFilter&SymbianPBAP::KPbapAttributeMaskCallDateTime)
			{
			property = CallDateTimePropertyL(*iLogEvent, iFormat);
			CleanupStack::PushL(property);  
			aPropertyList->AppendL(property);
			CleanupStack::Pop(property); //ownership passed
			}
							
		iLogEvent = NULL;
		}		
	}
	
TBool CPbapVCardExporterUtil::ContactExistsL(TContactItemId aContactId)
	{
	LOG_FUNC
	TBool exists = EFalse;
	if (aContactId != KNullContactId)
		{
		CContactItem* contactItem=NULL;
		TRAPD(error, contactItem=iDatabase.ReadMinimalContactL(aContactId));
		if (error==KErrNone)
			{
			if (contactItem && (contactItem->Type()== KUidContactCard || contactItem->Type()== KUidContactOwnCard))
				{
				// the contact item exists in the pb phone book
				exists = ETrue;
				delete contactItem;	
				}
			}
		else if (error != KErrNotFound && error!=KErrNotReady)
			{
			User::Leave(error);			
			}
		}
	return exists;		
	}

CParserVCard* CPbapVCardExporterUtil::CreateVCardParserLC(TVCardVersion aFormat)
	{
	LOG_FUNC
	CParserVCard* parser=NULL;
	switch (aFormat)
		{
		case EPBAPVCard21:
			parser=CParserVCard::NewL(); //create vCard 2.1 parser
			break;
		case EPBAPVCard30:
			parser=CParserVCard3::NewL(); //create vCard 3.0 parser
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	CleanupStack::PushL(parser);
	
	return parser;
	}

CParserProperty* CPbapVCardExporterUtil::EmptyNamePropertyL()
	{
	LOG_FUNC
	CDesCArrayFlat* desArray =new (ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);
	desArray->AppendL(KNullDesC);
	CParserPropertyValue* value = new(ELeave) CParserPropertyValueCDesCArray(desArray);
	CleanupStack::Pop(desArray); //ownership passed
	CleanupStack::PushL(value);
	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenN, NULL, NULL);
	CleanupStack::Pop(value); //ownership passed
	return property;
	}

		
CParserProperty* CPbapVCardExporterUtil::DesPropertyL(const TDesC8& aToken, const TDesC& aDes)
	{
	LOG_FUNC
	CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(aDes);	
	CleanupStack::PushL(value);
	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, aToken, NULL, NULL);
	CleanupStack::Pop(value); //ownership passed
	return property;
	}
		
CParserProperty* CPbapVCardExporterUtil::CallDateTimePropertyL(const CLogEvent& aLogEvent, TVCardVersion aFormat)
	{
	LOG_FUNC
	TTime localTime = aLogEvent.Time();
	TVersitDateTime::TRelativeTime relativeTime = TVersitDateTime::EIsUTC;
	if (iTzClient.ConvertToLocalTime(localTime) == KErrNone)
		{
		relativeTime = TVersitDateTime::EIsMachineLocal;
		}

	TVersitDateTime* dateTime= new(ELeave) TVersitDateTime(localTime.DateTime(), relativeTime);
	CleanupStack::PushL(dateTime);
	CParserPropertyValueDateTime* value = new(ELeave) CParserPropertyValueDateTime(dateTime);
	CleanupStack::Pop(dateTime); //ownership passed
	CleanupStack::PushL(value);
	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenCALLDATETIME, NULL, NULL);
	CleanupStack::Pop(value); //ownership passed
	CleanupStack::PushL(property);

	TPtrC8 paramValue(ParameterValueFromEventL(aLogEvent));
	
	if (paramValue.Length())
		{
		CParserParam* param;
		if (aFormat == EPBAPVCard30)
			{
			//nameless parameters not allowed for vCard 3.0, so add "TYPE" parameter name
			param= CParserParam::NewL(KVersitTokenTYPE, paramValue);
			}
		else
			{
			param= CParserParam::NewL(paramValue, KNullDesC);
			}
		CleanupStack::PushL(param);
		property->AddParamL(param);
		CleanupStack::Pop(param); //ownership passed
		}
		
	CleanupStack::Pop(property);
	return property;
	}


TPtrC8 CPbapVCardExporterUtil::ParameterValueFromEventL(const CLogEvent& aLogEvent)
	{
	LOG_FUNC
	if (iLogWrapper)
		{		
		TLogString logString;
		User::LeaveIfError(iLogWrapper->GetString(logString, R_LOG_DIR_MISSED));
		if (aLogEvent.Direction()==logString)
			{
			return KVersitParamMISSED();
			}
		User::LeaveIfError(iLogWrapper->GetString(logString, R_LOG_DIR_IN));
		if (aLogEvent.Direction()==logString)
			{
			return KVersitParamRECEIVED();
			}
		User::LeaveIfError(iLogWrapper->GetString(logString, R_LOG_DIR_OUT));
		if (aLogEvent.Direction()==logString)
			{
			return KVersitParamDIALED();
			}
		}
	//unknown log type, just set parameter value to empty string	
	return KNullDesC8();
	}
