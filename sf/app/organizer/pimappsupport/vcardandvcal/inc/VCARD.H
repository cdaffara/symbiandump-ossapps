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
 
#ifndef __VCARD_H__
#define __VCARD_H__

#ifndef __VERSIT_H__
#include <versit.h>
#endif

#include <vobserv.h>


/** This constant is for internal use only.
@internalTechnology
*/
_LIT(KVersitTokenVCardVersionNo, "2.1");


class CVCard3ParserPlugIn;

//
// CParserVCard
//
class CParserVCard : public CVersitParser
/** A vCard parser. 

Adds support for property groups (see CParserGroupedProperty) and agents 
(see CParserPropertyValueAgent) to the functionality of CVersitParser. 

Provides a constructor and overrides CVersitParser::InternalizeL(),   
ExternalizeL(), RecognizeToken(), ConvertAllPropertyDateTimesToMachineLocalL(), 
ParsePropertyL() and MakePropertyValueL().

The vCard data is read from or written to a stream or file, using the InternalizeL() 
and ExternalizeL() functions. Most users of this class will only need to use 
these functions.

If you are sequentially creating and destroying multiple parsers, a major performance 
improvement may be achieved by using thread local storage to store an instance of 
CVersitUnicodeUtils which persists and can be used by all of the parsers.

See CVersitTlsData for more information. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserVCard* NewL();
	static CParserVCard* NewL(TBool aParsingAgent);
	IMPORT_C CArrayPtr<CParserProperty>* GroupOfPropertiesL(const TDesC8& aName) const;
public: //from CVersitParser
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream);
	IMPORT_C void ConvertAllPropertyDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
protected:
	IMPORT_C virtual CParserPropertyValue* MakePropertyValueAgentL(TPtr16 aValue);
	IMPORT_C CDesC8Array* GetGroupNamesL(TPtr8& aGroupsAndName);
protected: // from CVersitParser
	IMPORT_C void ParsePropertyL();
	IMPORT_C CParserPropertyValue* MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue);
    IMPORT_C CParserPropertyValue* MakePropertyValueSoundL(HBufC16*& aValue);
public: //from CVersitParser
	IMPORT_C TUid RecognizeToken(const TDesC8& aToken) const;
	IMPORT_C void ConvertDateTimesToMachineLocalAndDeleteTZL();
protected:
	CParserVCard();
	CParserVCard(TBool aParsingAgent);
private: // from CVersitParser
	IMPORT_C virtual void Reserved1();
	IMPORT_C virtual void Reserved2();
	TBool iParsingAgent;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
NONSHARABLE_CLASS(CParserVCard3) : public CParserVCard
/** A vCard 3.0 parser. 

Overrides CParserVCard::InternalizeL(). Internalizing of vCard 3.0
objects is not supported

@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CParserVCard3* NewL();
	~CParserVCard3();
	
public: //from CParserVCard
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
private:
	CParserVCard3();
	void ConstructL();
	void CreateParserPlugInL();

private:
	CVCard3ParserPlugIn* iPlugInImpl;
	};
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS	
//
// CParserGroupedProperty
//
class CParserGroupedProperty : public CParserProperty
/** A grouped property.

This is a vCard property which may be a member of one or more property groups. 
It owns an array of descriptors, each of which identifies a group 
to which the property belongs.

An example of how grouped properties may be useful is for a person with two 
homes. Each home has an address and telephone number, both of which are to 
be stored on the vCard. The address and telephone number are grouped for each 
home. A prefix for the address and telephone number properties is provided: 
one for each home. Each prefix is stored as a property group.

When the property is externalised, the identities of all the groups to which 
the property belongs (i.e. the prefixes) are written to the stream before 
CParserProperty::ExternalizeL() is called.

The class also provides a function which can be used to test whether 
the property is a member of a given group.

Note that the vCard parser stores all properties using this class; if the 
vCard property is not grouped then the array of property groups is empty. 
Conversely, the vCalender parser does not have property groups, and so stores 
all properties using the CParserProperty base class. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserGroupedProperty* NewL(CParserPropertyValue& aPropertyValue, const TDesC8& aName, CDesC8Array* aArrayOfGroups, CArrayPtr<CParserParam>* aArrayOfParams);
	IMPORT_C CParserGroupedProperty(CDesC8Array* aArrayOfGroups, CArrayPtr<CParserParam>* aArrayOfParams);
	IMPORT_C ~CParserGroupedProperty();
	IMPORT_C TBool Group(const TDesC8& aGroup) const;
	inline void SetGroups(CDesC8Array* aArrayOfGroups);
	inline const CDesC8Array* Groups();
public: //from CParserProperty
	IMPORT_C TBool SupportsInterface(const TUid& aInterfaceUid) const;
	IMPORT_C void ExternalizeL(RWriteStream& aStream, CVersitParser* aVersitParser = NULL);
protected:
	CParserGroupedProperty(CParserPropertyValue& aPropertyValue, CDesC8Array* aArrayOfGroups, CArrayPtr<CParserParam>* aArrayOfParams);
private: //from CParserProperty
	IMPORT_C virtual void Reserved();
protected:
	CDesC8Array* iArrayOfGroups;
	};

//
// CParserPropertyValueAgent
//
class CParserPropertyValueAgent : public CParserPropertyValue
/** An agent property value parser. 

An agent property value contains information about a person who is not the 
main subject of the vCard. It is implemented as a vCard nested within another 
vCard. The agent's vCard is held in the property value of the parent vCard.

We do not support nested agents when parsing an agent's vcard.

The UID for an agent property value is KVCardPropertyAgentUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueAgent(CParserVCard* aValue);
	IMPORT_C ~CParserPropertyValueAgent();
	inline CParserVCard* Value() const;

public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/);
protected:
	CParserVCard* iValue;
private:
	};

#include <vcard.inl>

#endif
