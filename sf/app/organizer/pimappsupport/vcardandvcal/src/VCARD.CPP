// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include <vcard.h>

#include <s32mem.h>

#include <vutil.h>
#include <vstaticutils.h>
#include "verror.h"
#include <vobserv.h>
#include "vpbapplugin.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "vcard3.h"
#endif

_LIT(KVersitTokenVCard3VersionNo, "3.0");



_LIT8(KVCard3LineBreakAndWSP,	"\r\n ");
_LIT8(KVCard3TokenB, "b");

const TUint KTokenNewLineVal = 'n';
const TUint KTokenCarriageReturnVal = '\r';
const TUint KTokenLineFeedVal = '\n';

const TInt KMaxVCard3LineLength = 75; //line length recommended in RFC2426

//
// CParserVCard
//
EXPORT_C CParserVCard* CParserVCard::NewL()
/** Allocates and constructs a vCard parser.

@return A pointer to the newly constructed vCard parser. */
	{
	CParserVCard* self=new(ELeave) CParserVCard();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CParserVCard* CParserVCard::NewL(TBool aParsingAgent)
/* Allocates and constructs a vCard parser.
   This is used internally to limit the number of recursive AGENT vcards that
   may be found within an AGENT property */
	{
	CParserVCard* self=new(ELeave) CParserVCard(aParsingAgent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CParserVCard::CParserVCard()
	: CVersitParser(ESupportsVersion), iParsingAgent(EFalse)
	{
	iDefaultVersion = KVersitTokenVCardVersionNo;
	}

CParserVCard::CParserVCard(TBool aParsingAgent)
	: CVersitParser(ESupportsVersion), iParsingAgent(aParsingAgent)
	{
	iDefaultVersion = KVersitTokenVCardVersionNo;
	}

EXPORT_C CArrayPtr<CParserProperty>* CParserVCard::GroupOfPropertiesL(const TDesC8& aGroupName) const
/** Gets an array of all properties in the named property group from the vCard entity.

A property group is a collection of related properties, identified by a group name.

Ownership of the properties is not transferred.

@param aGroupName The name of the property group of interest.
@return An array of all properties in the specified group. NULL if the named property 
group was not found. */
	{
	if	(!iArrayOfProperties)
		return NULL;

	TInt properties=iArrayOfProperties->Count();
	CArrayPtr<CParserProperty>* arrayOfGroupedProperties=new(ELeave) CArrayPtrFlat<CParserProperty>(5);
	CleanupStack::PushL(arrayOfGroupedProperties);
	TUid uid;
	CParserGroupedProperty* groupedProperty;
	for (TInt ii=0; ii<properties; ii++)
		{
		uid.iUid=KVersitGroupedPropertyUid;
		if ((*iArrayOfProperties)[ii]->SupportsInterface(uid))
			{
			groupedProperty=STATIC_CAST(CParserGroupedProperty*,(*iArrayOfProperties)[ii]);
			if(groupedProperty->Group(aGroupName))
				arrayOfGroupedProperties->AppendL((*iArrayOfProperties)[ii]);
			}
		}
	CleanupStack::Pop(arrayOfGroupedProperties);
	if (arrayOfGroupedProperties->Count())
		return arrayOfGroupedProperties;
	delete arrayOfGroupedProperties;
	return NULL;
	}

EXPORT_C void CParserVCard::InternalizeL(RReadStream& aStream)
/** Internalises a vCard entity from a read stream. 

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

@param aStream Stream from which the vCard entity should be internalised. 
@see CVersitParser::InternalizeL() */
	{
	// Generic Internalize that loads properties\values with no extra processing
	CVersitParser::InternalizeL(aStream);
	}

EXPORT_C void CParserVCard::ConvertDateTimesToMachineLocalAndDeleteTZL()
/** Converts all date/time property values contained in the vCard entity into machine 
local values (including all date/time values contained in agent properties).

This conversion is needed because of differences in universal and local times 
due to time zones.

Finds the necessary increment to compensate for the time zone, using the value 
given by the first time zone property (named KVersitTokenTZ) in the array 
of properties. This increment is passed as a parameter to ConvertAllPropertyDateTimesToMachineLocalL(). 
The CVersitDaylight* parameter passed to ConvertAllPropertyDateTimesToMachineLocalL() 
is NULL, so no compensation can be made by this function for any daylight 
saving (seasonal time shift). 
@deprecated since 9.1
*/
	{
	CArrayPtr<CParserProperty>* arrayOfTimeZones = PropertyL(KVersitTokenTZ, TUid::Uid(KVersitPropertyTimeZoneUid),ETrue);
	if	(!arrayOfTimeZones)
		{
		if (iFlags&EImportSyncML)
			AdjustAllPropertyDateTimesToMachineLocalL();
		return;
		}

	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfProperties, arrayOfTimeZones));
	CParserPropertyValueTimeZone* timeZone = STATIC_CAST(CParserPropertyValueTimeZone*, (*arrayOfTimeZones)[0]->Value());
	TTimeIntervalSeconds increment = timeZone->Value().Int();
	ConvertAllPropertyDateTimesToMachineLocalL(increment,NULL);
	CleanupStack::PopAndDestroy(arrayOfTimeZones);
	}

EXPORT_C void CParserVCard::ExternalizeL(RWriteStream& aStream)
/** Externalises a vCard entity (and all sub-entities) to a write stream.

Sets the entity name to KVersitVarTokenVCARD if it hasn't already been set.

Adds a version property to the start of the current entity's array of properties 
if the entity supports this. (If there isn't an array of properties then one 
is made).

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream Stream to which the vCard entity should be externalised. 
@see CVersitParser::ExternalizeL() */
	{
	if	(!iEntityName)
		SetEntityNameL(KVersitVarTokenVCARD);
	CVersitParser::ExternalizeL(aStream);
	}

EXPORT_C void CParserVCard::ConvertAllPropertyDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
/** Converts all date/time property values contained in the vCard entity into machine 
local values (including all date/time values contained in agent properties).

This conversion is needed because of differences in universal and local times 
due to time zones and daylight savings (seasonal time shifts).

If there is a daylight savings rule associated with the date/time (held in 
the aDaylight parameter) then this will be used to compensate for differences 
between universal and local times due to both time zones and the daylight 
savings rule. Otherwise, the aIncrement parameter is used to compensate for 
any difference due to time zones alone.

@param aIncrement A time interval in seconds which represents the negative 
of the time zone of the originating machine. For instance, if the time zone 
is +04:30, aIncrement should be set to -04:30.
@param aDaylight Pointer to the specification for daylight saving. If the date/time 
value is within the period for daylight saving, the value is modified by the 
daylight saving offset (which accounts for both the time zone and daylight 
saving rule). 
@deprecated since 9.1
*/
	{
	CVersitParser::ConvertAllPropertyDateTimesToMachineLocalL(aIncrement,aDaylight);
	CArrayPtr<CParserProperty>* properties = PropertyL(KVersitTokenAGENT, TUid::Uid(KVCardPropertyAgentUid), EFalse);
	if	(!properties)
		return;
	CleanupStack::PushL(properties);
	TInt count = properties->Count();
	CParserPropertyValueAgent* agent;
	for (TInt ii = 0; ii < count; ii++)
		{
		agent = STATIC_CAST(CParserPropertyValueAgent*, (*properties)[ii]->Value());
		if (agent && agent->Value())
			agent->Value()->ConvertAllPropertyDateTimesToMachineLocalL(aIncrement, aDaylight);
		}
	CleanupStack::PopAndDestroy(properties);
	}

EXPORT_C CParserPropertyValue* CParserVCard::MakePropertyValueAgentL(TPtr16 aValue)
	{
	if(!iParsingAgent)
		{
		HBufC* entityName = NULL;
		TInt start=aValue.Find(KVersitVarTokenBEGIN);
		if (start>KErrNotFound)
			{		//BEGIN is on the same line as AGENT
			start+=KVersitVarTokenBEGIN.iTypeLength;
			TInt length=aValue.Length()-start;
			aValue.Set(&aValue[start],length,length);
			start=aValue.Find(KVersitTokenColonUnicode);
			if (start==KErrNotFound)
				User::Leave(KErrNotFound);
			start+=KVersitTokenColonUnicode.iTypeLength;
			length=aValue.Length()-start;
			if (length==0)
				User::Leave(KErrNotFound);
			aValue.Set(&aValue[start],length,length);
			VersitUtils::StripWhiteSpace(aValue);
			entityName=aValue.AllocLC();
			}
		else
			{
			TBool moreCharsOnLine = VersitUtils::IsNoneWhiteSpaceWideChar(aValue);
			if( EFalse == moreCharsOnLine )
				{		// ... rest of line is empty, so BEGIN is on a following line
				CParserProperty* agentProperty=iCurrentProperty;
				iCurrentProperty=NULL;
				CleanupStack::PushL(agentProperty);
				ParsePropertyL();
				if (iCurrentProperty->NameUid().iUid!=KVersitTokenBeginUid)
					User::Leave(KVersitErrNestedVcardExpected);
				entityName=STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->TakeValueOwnership();
				delete iCurrentProperty;
				CleanupStack::Pop(agentProperty);
				iCurrentProperty=agentProperty;
				CleanupStack::PushL(entityName);
				}
			else
				{	// ... rest of line contains non-standard data, ignore it.
			User::Leave(KVersitErrNestedVcardExpected);
				}
			}
	
		//	create a parser setting iAgentParsing member variable to true
		CParserVCard* parser=CParserVCard::NewL(ETrue);
		CleanupStack::PushL(parser);
		if (iObserver)
			{
			iObserver->NewParser(parser);
			}
		
		parser->SetAutoDetect(iFlags&EUseAutoDetection,iAutoDetectCharSets);
			
		parser->CVersitParser::InternalizeL(entityName, iLineReader);
		CleanupStack::Pop(2, entityName);
		CleanupStack::PushL(parser);
	
		CParserPropertyValue* propertyValue=new(ELeave)CParserPropertyValueAgent(parser);
		CleanupStack::Pop(parser);
		return propertyValue;
		}

	// we leave if iParsingAgent is ETrue because we do not support nested agent properties
	delete iCurrentProperty;
	iCurrentProperty=NULL;
	User::Leave(KErrNotSupported);
	return NULL; // satisfy compiler
	}
		


EXPORT_C CDesC8Array* CParserVCard::GetGroupNamesL(TPtr8& aGroupsAndName)
	{
	CDesC8Array* arrayOfGroups=NULL;
	TInt periodPos;
	TInt len;
	FOREVER
		{
		periodPos=aGroupsAndName.Find(KVersitTokenPeriod);
		if (periodPos==KErrNotFound)
			break;
		if (!arrayOfGroups)
			{
			arrayOfGroups=new(ELeave) CDesC8ArrayFlat(5);
			CleanupStack::PushL(arrayOfGroups);
			}
		arrayOfGroups->AppendL(TPtrC8(&aGroupsAndName[0],periodPos));
		len=aGroupsAndName.Length()-(++periodPos);
		
		// INC113960:vCard crash when parsing random data
		//data cause crash:"ariat@etsi.fr=0D=0ATel.:"
		//In the case there is no data after the KVersitTokenPeriod, don't set aGroupsAndName.
		if (len ==0)
			{
			break;
			}
		
		aGroupsAndName.Set(&aGroupsAndName[periodPos],len,len);
		}
	if (arrayOfGroups)
		CleanupStack::Pop(arrayOfGroups);
	return arrayOfGroups;
	}

EXPORT_C void CParserVCard::ParsePropertyL()
	{
	TInt valueStart;
	TInt lenNameGroup;
	CArrayPtr<CParserParam>* arrayOfParams=ReadLineAndDecodeParamsLC(valueStart,lenNameGroup);
	TPtr8 propName(&BufPtr()[0],lenNameGroup,lenNameGroup);
	CDesC8Array* arrayOfGroups=GetGroupNamesL(propName);
	if (arrayOfGroups)
		CleanupStack::PushL(arrayOfGroups);
	iCurrentProperty=new(ELeave) CParserGroupedProperty(arrayOfGroups,arrayOfParams);
	if (arrayOfGroups)
		CleanupStack::Pop(arrayOfGroups);
	CleanupStack::Pop(arrayOfParams);
	// Change according to defect DEF022049. If a vCard property does not have a name,
	// it should not create a property for this field.
	if (propName.Length()>0)
		{
		MakePropertyL(propName,valueStart);
		}
	else
		{
		User::Leave(KVersitErrNoPropertyName);
		}
	}

EXPORT_C CParserPropertyValue* CParserVCard::MakePropertyValueL(const TUid& aPropertyUid, HBufC16*& aValue)
/** Creates a property value, by parsing the specified descriptor.

This function overrides CVersitParser::MakePropertyValueL() to ensure that agent and 
sound properties are correctly recognised and constructed.

Note that aValue may be set to NULL on return from this method.

@param aPropertyUid The UID type associated with the raw property string.
@param aValue The raw property value descriptor. The function removes anything that 
is not part of the property.
@return A property value. */
	{
	if(!aValue && (aPropertyUid.iUid != KVersitPropertyBinaryUid || !iLargeDataBuf))
		{
		return NULL;
		}
		
	switch (aPropertyUid.iUid)
		{
		case KVCardPropertyAgentUid:
            {
			return MakePropertyValueAgentL(aValue->Des());
            }
		case KVCardPropertySoundUid:
            {
			return MakePropertyValueSoundL(aValue);
            }
		default:
            {
			return CVersitParser::MakePropertyValueL(aPropertyUid,aValue);
            }
		};
	}

EXPORT_C CParserPropertyValue* CParserVCard::MakePropertyValueSoundL(HBufC16*& aValue)
/** Creates a property value object from a SOUND property value. 

SOUND property values can either contain a single string (using HBufC storage) 
or, following the addition of Japanese pronunciation support to Symbian's 
Versit API, an array (using CDesCArray storage). This method allocates 
the additional storage needed to support array-based sound properties, when 
necessary.

Note that aValue will be set to NULL on return from this method.

@param aValue The raw property value descriptor.
@return A constructed sound property value. */
    {
    // Allocate additional storage if necessary
 	if(!aValue)
		return NULL;
 	        
	HBufC16 *buf  = HBufC16::NewLC(aValue->Length());
	*buf = *aValue;//copy avalue into buf
	
    TPtr16 pValue(buf->Des());
    //pass copy of avalue into AllocateAdditionalPropertyStorageL so that RemoveEscapeChars isnt applied twice to the string
    VersitUtils::AllocateAdditionalPropertyStorageL(*iStaticUtils, *iCurrentProperty, pValue, LineCharSetId());

    // Now we remove any escaping
	VersitUtils::RemoveEscapeChars(*aValue, LineCharSetId());
	
	CleanupStack::PopAndDestroy( buf );
    
    // ... and create a property value
    CParserPropertyValueHBufC* returnedValue = new(ELeave) CParserPropertyValueHBufC(aValue);
	aValue = NULL;
	return returnedValue;
    }

EXPORT_C TUid CParserVCard::RecognizeToken(const TDesC8& aToken) const
// From CVersitParser - extra recognition capablities are added by overriding this
// virtual function. These properties are specific to vCards
/** Returns a UID that identifies a specified token's type.

For example, if aToken contains the property name BDAY, the function returns 
KVersitPropertyDateUid. If the token is not recognized as vCard-specific, the 
function calls CVersitParser::RecognizeToken(), which recognizes generic Versit 
tokens.

@param aToken The token to be recognized.
@return A defined UID value if the token is recognized and KVersitTokenUnknownUid 
if not. */
	{
	TUid uid = KNullUid;
	TChar firstChar(aToken.Ptr()[0]);
	firstChar=firstChar.GetUpperCase();
	switch (firstChar)
		{
	case 'A':
		if (!aToken.CompareF(KVersitTokenADR))
			uid.iUid=KVersitPropertyCDesCArrayUid;
		else if (!aToken.CompareF(KVersitTokenAGENT))
			uid.iUid=KVCardPropertyAgentUid;
		break;
	case 'B':
		if (!aToken.CompareF(KVersitTokenBDAY))
			uid.iUid=KVersitPropertyDateUid;
		break;
	case 'E':
		if (!aToken.CompareF(KVersitTokenEMAIL))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'F':
		if (!aToken.CompareF(KVersitTokenFN))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'G':
		if (!aToken.CompareF(KVersitTokenGEO))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'K':
		if (!aToken.CompareF(KVersitTokenKEY))
			{
			Versit::TVersitEncoding encodingValue = LineEncoding();
			if(encodingValue == Versit::EBase64Encoding)
				{
				uid.iUid = KVersitPropertyBinaryUid;
				}
			else
				{
 				uid.iUid=KVersitPropertyHBufCUid;
				}
			}
 		break;
	case 'L':
		if (!aToken.CompareF(KVersitTokenLABEL))
			uid.iUid=KVersitPropertyHBufCUid;
		else if (!aToken.CompareF(KVersitTokenLOGO))
			uid.iUid=KVersitPropertyBinaryUid;
		break;
	case 'M':
		if (!aToken.CompareF(KVersitTokenMAILER))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'N':
		if (!aToken.CompareF(KVersitTokenN))
			uid.iUid=KVersitPropertyCDesCArrayUid;
		else if (!aToken.CompareF(KVersitTokenNOTE))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'O':
		if (!aToken.CompareF(KVersitTokenORG))
			uid.iUid=KVersitPropertyCDesCArrayUid;
		break;
	case 'P':
		if (!aToken.CompareF(KVersitTokenPHOTO))
			uid.iUid=KVersitPropertyBinaryUid;
		break;
	case 'R':
		if (!aToken.CompareF(KVersitTokenREV))
			uid.iUid=KVersitPropertyDateTimeUid;
		else if (!aToken.CompareF(KVersitTokenROLE))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'S':
		if (!aToken.CompareF(KVersitTokenSOUND))
			uid.iUid=KVCardPropertySoundUid;
		break;
	case 'T':
		if (!aToken.CompareF(KVersitTokenTZ))
			uid.iUid=KVersitPropertyTimeZoneUid;
		else if ((!aToken.CompareF(KVersitTokenTEL))
			||(!aToken.CompareF(KVersitTokenTITLE)))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'U':
		if ((!aToken.CompareF(KVersitTokenURL))
			||(!aToken.CompareF(KVersitTokenUID)))
			uid.iUid=KVersitPropertyHBufCUid;
		break;
	case 'V':
		if (!aToken.CompareF(KVersitTokenVERSION))
			uid.iUid=KVersitTokenVersionUid;
		break;
	case 'X':
		if (!aToken.CompareF(KVersitTokenSECONDNAME))
			{
			uid.iUid=KVersitPropertyHBufCUid;
			}
		else if (!aToken.CompareF(KVersitTokenAssistant))
			{
			uid.iUid=KVersitPropertyHBufCUid;
			}
		else if (!aToken.CompareF(KVersitTokenAssistantTel))
			{
			uid.iUid=KVersitPropertyHBufCUid;
			}
		else if (!aToken.CompareF(KVersitTokenAnniversary))
			{
			uid.iUid=KVersitPropertyDateUid;
			}
		else if (!aToken.CompareF(KVersitTokenSpouse))
			{
			uid.iUid=KVersitPropertyHBufCUid;
			}
		else if (!aToken.CompareF(KVersitTokenChildren))
			{
			uid.iUid=KVersitPropertyCDesCArrayUid;
			}
		else if (!aToken.CompareF(KVersitTokenClass))
			{
			uid.iUid=KVersitPropertyHBufCUid;
			}
		break;
	default:
		break;
		}
	if (uid == KNullUid)		// If a vCard specific property was not recognised then get the base class to see if it recognises the token.
		return CVersitParser::RecognizeToken(aToken);
	return uid;
	}

EXPORT_C void CParserVCard::Reserved1()
	{}

EXPORT_C void CParserVCard::Reserved2()
	{}

//
// CParserVCard3
//
EXPORT_C CParserVCard3* CParserVCard3::NewL()
/** Allocates and constructs a vCard 3.0 parser.

@return A pointer to the newly constructed vCard parser. */
	{
	CParserVCard3* self = new(ELeave) CParserVCard3();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void CParserVCard3::ConstructL()
/** Second phase constructor.*/
	{
	CVersitParser::ConstructL();
	CreateParserPlugInL();
	}
	
CParserVCard3::~CParserVCard3()
/** Frees all resources owned by the vCard parser, prior to its destruction. */
	{
	delete iPlugInImpl;
	}

CParserVCard3::CParserVCard3()
	{
	iDefaultVersion = KVersitTokenVCard3VersionNo;
	}

void CParserVCard3::CreateParserPlugInL()
/** Create Versit plugin to enable vCard 3.0 format export as defined
by RFC2425 and RFC2426 */
	{
	iPlugInImpl = new(ELeave) CVCard3ParserPlugIn;
	SetPlugIn(iPlugInImpl);
	}
	
EXPORT_C void CParserVCard3::InternalizeL(RReadStream& /*aStream*/)
/** Internalises a vCard 3.0 entity from a read stream. 
Always leaves with KErrNotSupported because this class should only
be used for vCard 3.0 export

@param aStream Stream from which the vCard entity should be internalised. 
*/
	{
	User::Leave(KErrNotSupported);
	}


//
// CVCard3ParserPlugIn
//
TBool CVCard3ParserPlugIn::AddSpace() 
	{
	return EFalse;
	}

TBool CVCard3ParserPlugIn::DeleteAllSpaces()
	{
	return EFalse;
	}

TBool CVCard3ParserPlugIn::NeedsBlankLine()
	{
	return EFalse;
	}

void CVCard3ParserPlugIn::RemoveEscaping(TPtr16& /*aText*/) 
	{
	//import not supported so do nothing
	}
	
void CVCard3ParserPlugIn::AddEscaping(HBufC16*& aText)
/** Plugin provides escaping of comma characters and also replacement of CRLF 
sequence with '\n' in property values. */
	{	
	HBufC16* targetText = NULL;
	//allocate a buffer large enough to cope with all characters in the string being
	//escaped. If this allocation fails no escaping is attempted which will result in
	//the creation of an invalid vCard
	TRAPD(error, targetText = HBufC16::NewL(2*aText->Length()));
	if (error || !targetText)
		{
		return;
		}

 	//escape comma characters and replace CRLF with '\n'
	TPtr targetPtr(targetText->Des());
	const TUint16* pSource = aText->Ptr();
	TInt length = aText->Length();
	for(TInt i = 0; i<length; i++, pSource++) 
		{
		switch (*pSource) 
			{
			case KVersitTokenCommaVal:
				{
				//replace "," with "\,"
				targetPtr.Append(KVersitTokenBackslashVal);
				targetPtr.Append(*pSource);
				}
				break;
			case KTokenCarriageReturnVal:
				{
				//CR is skipped and the next LF is replaced with '\n'
				break;
				}
			case KTokenLineFeedVal:
				{
				//replace CRLF with "\n"	
				targetPtr.Append(KVersitTokenBackslashVal);
				targetPtr.Append(KTokenNewLineVal);
				}
				break;
			default:
				targetPtr.Append(*pSource);
				break;	
			}
		}
	delete aText;
	aText = targetText;				
	}
	

TBool CVCard3ParserPlugIn::WrapLine(RWriteStream& aStream, TInt& aCurrentLineLength, const TPtr8& aText)
/** Plugin provides line folding as defined by RFC2425 and RFC2426*/
	{
	//this is a non-leaving function so all writes to the stream (which can leave)
	//must be called in a TRAP. Any leaving errors are ignored because there is no
	//way to report them to the client. This may result in a partial property value
	//being written to the stream  
	TRAP_IGNORE(DoWrapLineL(aStream, aCurrentLineLength, aText));
	return ETrue;
	}

void CVCard3ParserPlugIn::DoWrapLineL(RWriteStream& aStream, TInt& aCurrentLineLength, const TPtr8& aText)
	{
	TPtr8 text(aText);
	TPtr8 line(NULL,0,0);
	TInt lineLength = Max(KMaxVCard3LineLength-aCurrentLineLength, 0);
	TInt remaining = text.Length();

	while (remaining > lineLength)
		{
		line.Set(&text[0], lineLength, lineLength);
		aStream.WriteL(line);
		aStream.WriteL(KVCard3LineBreakAndWSP);
		aCurrentLineLength = 1;
		remaining -= lineLength;
		text.Set(&text[lineLength], remaining, remaining);
		lineLength = KMaxVCard3LineLength-1;
		}
	aCurrentLineLength += text.Length();
	aStream.WriteL(text);
	}
	
TBool CVCard3ParserPlugIn::EncodingType(Versit::TVersitEncoding& aEncoding,
						TBool /*aRequiresEncoding*/,
						Versit::TVersitEncoding /*aDefaultEncoding*/,
						TUid aPropertyUid, TUint /*aPropertyCharsetId*/) 
	{
	//quoted-printable encoding is performed by the transport wrapper so all
	//non binary property values can be set to have no encoding here
	aEncoding = Versit::ENoEncoding;

	if (aPropertyUid == TUid::Uid(KVersitPropertyBinaryUid))
		{
		//inline binary property values are always encoded to base64
		aEncoding = Versit::EBase64Encoding;
		}
	return ETrue;
	}

const TDesC8& CVCard3ParserPlugIn::EncodingName(Versit::TVersitEncoding aEncoding) 
	{
	if (aEncoding == Versit::EBase64Encoding)
		{
		//base64 encoding parameter value changed from "BASE64" to "B" 
		return KVCard3TokenB;
		}
	return VersitUtils::IANAEncodingName(aEncoding);
	}

void CVCard3ParserPlugIn::GetInterface(TUid aInterfaceUid, TAny*& aInterface)
/** Returns interface extension. */
	{
	if (aInterfaceUid == KUidVersitPlugInExtension)
		{
		aInterface = static_cast<MVersitPlugInExtension*>(this);
		}
	}
	
TBool CVCard3ParserPlugIn::DisableCharsetParam()
	{
	return ETrue;
	}

TBool CVCard3ParserPlugIn::DisableBlankLineAfterBinaryValue()
	{
	return ETrue;
	}

TBool CVCard3ParserPlugIn::WrapBinaryLinesL(CBufBase& aBuffer, TInt& aCurrentLineLength)
/** Plugin provides line folding as defined by RFC2425 and RFC2426*/
	{
	TInt lineLength = Max(KMaxVCard3LineLength-aCurrentLineLength, 0);
	TInt remaining = aBuffer.Size();
	TInt pos = 0;
	while (remaining>lineLength)
		{
		pos += lineLength;
		aBuffer.InsertL(pos, KVCard3LineBreakAndWSP);
		pos += KVCard3LineBreakAndWSP().Length();
		remaining -= lineLength;
		lineLength = KMaxVCard3LineLength-1;
		aCurrentLineLength = 1;
		}
	aCurrentLineLength += remaining;
	return ETrue;
	}
	
TBool CVCard3ParserPlugIn::FoldParam()
	{
	return ETrue;
	}
	
	
//
// CParserGroupedProperty
//

CParserGroupedProperty::CParserGroupedProperty(CParserPropertyValue& aPropertyValue,CDesC8Array* aArrayOfGroups,CArrayPtr<CParserParam>* aArrayOfParams)
	: CParserProperty(aPropertyValue, aArrayOfParams), iArrayOfGroups(aArrayOfGroups)
	{}

EXPORT_C CParserGroupedProperty::CParserGroupedProperty(CDesC8Array* aArrayOfGroups, CArrayPtr<CParserParam>* aArrayOfParams)
	: CParserProperty(aArrayOfParams), iArrayOfGroups(aArrayOfGroups)
/** Allocates and partially constructs a new grouped property using the array of 
grouped property names and array of property parameters specified.

@param aArrayOfGroups Pointer to an array of descriptors. Each one specifies 
a group name. The grouped property takes ownership of this array. 
(May be NULL).
@param aArrayOfParams The property parameters. The grouped property 
takes ownership of this array. (May be NULL). */
	{}

EXPORT_C CParserGroupedProperty* CParserGroupedProperty::NewL(CParserPropertyValue& aPropertyValue, const TDesC8& aName, CDesC8Array* aArrayOfGroups, CArrayPtr<CParserParam>* aArrayOfParams)
/** Allocates and constructs a new grouped property from the value, name, property 
parameters and groups specified.

Note: the property value, name and array of property parameters are generic 
to all properties. The array of grouped property names is specific to the CParserGroupedProperty 
class.

@param aPropertyValue The property value. The grouped property takes 
ownership of this.
@param aName The property name. The grouped property takes ownership 
of this.
@param aArrayOfGroups Pointer to an array of descriptors. Each one specifies 
a group name. The grouped property takes ownership of this array. (May be NULL).
@param aArrayOfParams The property parameters. The grouped property 
takes ownership of this array. (May be NULL).
@return Pointer to the newly created grouped property. */
	{
	// coverity [alloc_fn]
	CParserGroupedProperty* self = new(ELeave) CParserGroupedProperty(aPropertyValue,aArrayOfGroups,aArrayOfParams);
	ConstructSelfL(*self,aName);
	return self;
	}

EXPORT_C CParserGroupedProperty::~CParserGroupedProperty()
/** Frees all resources owned by the property, prior to its destruction. */
	{
	delete iArrayOfGroups;
	}

EXPORT_C TBool CParserGroupedProperty::Group(const TDesC8& aGroup) const
/** Tests whether the property is a member of the specified property group.

@param aGroup The name of the property group. 
@return ETrue if the property is a member of the specified property group. 
EFalse if not. */
	{
	if (iArrayOfGroups)
		{
		TInt notUsed;
		if (iArrayOfGroups->Find(aGroup,notUsed)==0)
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C TBool CParserGroupedProperty::SupportsInterface(const TUid& aInterfaceUid) const
/** Tests whether the property supports the given interface.

Returns true if the given interface UID is KVersitGroupedPropertyUid.

@param aInterfaceUid An interface UID.
@return ETrue if aInterfaceUid is KVersitGroupedPropertyUid, and EFalse if not. */
	{
	if (aInterfaceUid==TUid::Uid(KVersitGroupedPropertyUid))
		return ETrue;
	return CParserProperty::SupportsInterface(aInterfaceUid);
	}

EXPORT_C void CParserGroupedProperty::ExternalizeL(RWriteStream& aStream, CVersitParser* aVersitParser)
/** Externalises the property into aStream.

Externalises the list of groups to which the property belongs, if there are 
any, then calls CParserProperty::ExternalizeL() to write the rest of the property 
data to the stream.

@param aStream Stream to which the value should be externalised.
@param aVersitParser Versit parser which contains the objects to be externalised. 
This must not be NULL or a panic occurs. */
	{
	if (iArrayOfGroups)
		{
		TInt count=iArrayOfGroups->Count();
		for (TInt ii = 0; ii < count; ii++)
			{
#if defined _UNICODE
			for(TInt i = 0; i < (*iArrayOfGroups)[ii].Length(); i++)
				aStream.WriteUint8L((*iArrayOfGroups)[ii][i]);
#else
			aStream.WriteL((*iArrayOfGroups)[ii]);
#endif
			aStream.WriteL(KVersitTokenPeriod);
			}
		}
	CParserProperty::ExternalizeL(aStream, aVersitParser);
	}

EXPORT_C void CParserGroupedProperty::Reserved()
	{}

//
// CParserPropertyValueAgent
//

EXPORT_C CParserPropertyValueAgent::CParserPropertyValueAgent(CParserVCard* aValue)
: CParserPropertyValue(TUid::Uid(KVCardPropertyAgentUid))
, iValue(aValue)	
/** Constructs an agent property value, using a pointer to an agent. 

Sets the property value UID to KVCardPropertyAgentUid.

@param aValue A pointer to the agent object to assign to the property value. 
The property value takes ownership of the pointer. */
	{}

EXPORT_C CParserPropertyValueAgent::~CParserPropertyValueAgent()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueAgent::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/)
/** Externalises the agent property value to aStream.

@param aStream Stream to which the value should be externalised.
@param aEncodingCharset Contains character set and encoding information. Not used as it is not 
relevant to this function.
@param aLengthOutput The line wrapping offset. Not used as it is not relevant to this function. */
	{
	if (!iValue)
		return;
	aStream.WriteL(KVersitTokenCRLF);
	iValue->ExternalizeL(aStream);
	}

