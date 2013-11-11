// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMAPBODYSTRUCTURE_H__
#define __CIMAPBODYSTRUCTURE_H__

#include <e32base.h>

class CImapEnvelope;

/**
Represents an IMAP body structure as a tree.

The iEmbeddedBodyStructureList allows body structure to contain embedded body structures, 
as is required for multi-part and message/rfc822 type bodystructures.

iEmbeddedBodyStructureList owns the bodystructure objects it points to, and a CImapFetchResponse
object owns the root CImapBodyStructure that represents the entire body structure tree.

Consequentl, destroying the root CImapBodyStructure owned by CImapFetchResponse will cause the entire
tree of CImapBodyStructure objects to be destroyed.


This class provides a set of TPtrC8 items each of which will be set to point at the correct 
part of the incoming message during parsing. 

This class does not own the data that it points to.  So the TPtrC8 items will only be valid
while the HBufC8 buffer they are pointing to is valid.  

Consequently, this class should be used in conjunction with a valid HBufC8 object.
This object is owned by CImapFetchResponse, which also owns the root CImapBodyStructure object.

The following is a visual guide to bodystructures.
A formal definition of bodystructure can be found in Section 9 of RFC 3501.
The second line of each category below indicates "extension data".

bodyTypeMpart
	((-body-)(-body-)(-body-) "MIXED" 
	(n1 v1 n2 v2) (dispname (n1 v1 n2 v2)) extLang extLocation xExt1...)
	
bodyTypeText: all TEXT types
	("TEXT" "PLAIN" (n1 v1 n2 v2) id "description" "encoding" "size" lines 
	extMD5 (dispname (n1 v1 n2 v2)) extLang extLocation xExt1...)
	
bodyTypeMsg: only MESSAGE/RFC822
	("MESSAGE" "RFC822" (n1 v1 n2 v2) "id" "description" "encoding" "size" (-envelope-) (-body-) lines 
	extMD5 (dispname (n1 v1 n2 v2)) extLang extLocation xExt1...)
	
bodyTypeBasic: all other forms of bodyType1part
	("OTHER" "SUB" (n1 v1 n2 v2) "id" "description" "encoding" "size" 
	extMD5 (dispname (n1 v1 n2 v2)) extLang extLocation xExt1...)

----------------
envelope
	(date subject from (sender1 sender2) (replyTo1 replyTo2) (to1 to2) (cc1 cc2) (bcc1 bcc2) inReplyTo MessageId)

@internalTechnology
@prototype
*/
class CImapBodyStructure : public CBase
	{
public:
	
	/**
	The type of this bodystructure.
	RFC3501 defines four bodystructure types.
	For each type, a different set of CImapBodyStructure fields will be valid.
	*/
	enum TBodyStructureType 
		{
		/**
		This is the default type for a bodystructure.  It is the type given to all 
		bodystructures that are not rfc822, text or multipart.  Typically, this will
		be used for attachments.
		It has no embedded bodystructures, no envelope and BodyLines value.  All other fields are valid.
		In RFC3501, this is "body-type-basic"
		*/
		ETypeBasic,
		/**
		A message with MIME type and subtype "MESSAGE/RFC822", representing a MIME 
		header for an embedded message.
		All CImapBodyStructure fields are valid, including the envelope.
		There will be exactly one embedded bodystructure.
		In RFC3501, this is "body-type-msg"
		*/
		ETypeMessageRfc822,
		/**
		This is a message whose MIME type is "TEXT" together with and MIME subtype.
		This represents a text message.
		It has no embedded bodystructures and no envelope.  All other fields are valid.
		In RFC3501, this is "body-type-text"
		*/
		ETypeText,
		/** 
		This is a multipart message, with one or more embedded bodystructures.
		It has no envelope, and none of the following fields are valid:
			> BodyId
			> BodyDescription
			> BodyEncoding
			> BodySizeOctets
			> BodyLines
		All other fields are valid
		In RFC3501, this is "body-type-mpart"
		*/
		ETypeMultipart
		};

	/**
	Maps an attribute name to a value
	@internalTechnology
	@prototype
	*/
	struct TAttributeValuePair
		{
		TPtrC8 iAttribute;
		TPtrC8 iValue;
		};
	
	/**
	An array of pointers to CImapBodyStructure structure objects.
	*/
	typedef RPointerArray<CImapBodyStructure> RBodyStructureList;
	/**
	An array of attribute-value pairs.
	*/
	typedef RArray<TAttributeValuePair> RAttributeValuePairList;
	/**
	An array of 8-bit pointer-descriptors.
	*/
	typedef RArray<TPtrC8> RArrayTPtrC8;

	IMPORT_C TBodyStructureType BodyStructureType() const;
	IMPORT_C const RBodyStructureList EmbeddedBodyStructureList() const;
	
	IMPORT_C const TDesC8& Type() const;
	IMPORT_C const TDesC8& SubType() const;
		
	IMPORT_C const RAttributeValuePairList& ParameterList() const;
	
	IMPORT_C const TDesC8& BodyId() const;
	IMPORT_C const TDesC8& BodyDescription() const;
	IMPORT_C const TDesC8& BodyEncoding() const;
	IMPORT_C const TDesC8& BodySizeOctets() const;
	IMPORT_C const TDesC8& BodyLines() const;
	
	IMPORT_C const TDesC8& ExtMD5() const;
	IMPORT_C const TDesC8& ExtDispositionName() const;
	IMPORT_C const RAttributeValuePairList& ExtDispositionParameterList() const;
	IMPORT_C const RArrayTPtrC8& ExtLanguageList() const;
	IMPORT_C const TDesC8& ExtLocation() const;

	IMPORT_C CImapEnvelope& GetRfc822EnvelopeStructureL();

	static CImapBodyStructure* NewL();
	~CImapBodyStructure();
	
	void SetBodyStructureType(TBodyStructureType aBodyStructureType);
	void AppendEmbeddedBodyStructureL(const CImapBodyStructure& aBodyStructure);
	
	void SetType(const TDesC8& aType);
	void SetSubType(const TDesC8& aSubType);
		
	void AppendParameterListL(const TAttributeValuePair& aAttributeValuePair);
	
	void SetBodyId(const TDesC8& aBodyId);
	void SetBodyDescription(const TDesC8& aBodyDescription);
	void SetBodyEncoding(const TDesC8& aBodyEncoding);
	void SetBodySizeOctets(const TDesC8& aBodySizeOctets);
	void SetBodyLines(const TDesC8& aBodyLines);
	
	void SetExtMD5(const TDesC8& aExtMD5);
	void SetExtDispositionName(const TDesC8& aExtDispositionName);
	void AppendExtDispositionParameterListL(const TAttributeValuePair& aAttributeValuePair);
	void AppendExtLanguageListL(const TDesC8& aExtLanguage);
	void SetExtLocation(const TDesC8& aExtLocation);
	
private:
	CImapBodyStructure();
	
private:
	/**
	The type of this bodystructure.  
	It can be a Basic, Text, Message/Rfc822 or Multipart bodystructure.
	Each type is defined in section 9 of RFC3501.
	*/
	TBodyStructureType iBodyStructureType;
	
	/**
	For Multipart bodystructures, pointers to CBodyStructure objects representing each of the sub-parts is stored in the array
	For MESSAGE/RFC822, a single pointer to a single CBodyStructure object representing the embedded sub-part is store in the array.
	For Basic and Text bodystructures, this array will be empty.
	This pointer array takes ownership of the objects it points to.
	*/
	RBodyStructureList iEmbeddedBodyStructureList;
	
	/**
	The MIME type for this message part
	When body-type-mpart detected, this is set to "MULTIPART"
	*/
	TPtrC8 iType;
	/**
	The MIME sub-type for this message part
	*/
	TPtrC8 iSubType;
	
	/**
	A list of parameter-value pairs.
	Corresponds to a top level "body-fld-param" (i.e. not "body-fld-dsp" which is stored in iExtDispositionParameterList)
	*/
	RAttributeValuePairList iParameterList;
	
	// single-part data
	/**
	Corresponds to "body-fld-id" of RFC3501
	*/
	TPtrC8 iBodyId;
	/**
	Corresponds to "body-fld-desc" of RFC3501
	*/
	TPtrC8 iBodyDescription;
	/**
	Corresponds to "body-fld-enc" of RFC3501
	*/
	TPtrC8 iBodyEncoding;
	/**
	Corresponds to "body-fld-octets" of RFC3501
	*/
	TPtrC8 iBodySizeOctets;
	
	/**
	Corresponds to "body-fld-lines" of RFC3501
	*/
	TPtrC8 iBodyLines;
	
	// extension data
	/**
	Corresponds to "body-fld-md5" of RFC3501
	NOTE: This extension data item only applies to single-part structures
	*/
	TPtrC8 iExtMD5;
	/**
	A list of parameter-value pairs.
	Corresponds to the "string" part of "body-fld-dsp" of RFC3501
	*/
	TPtrC8 iExtDispositionName;
	/**
	
	Corresponds to the "body-fld-param" in "body-fld-dsp" of RFC3501
	*/
	RAttributeValuePairList iExtDispositionParameterList;
	/**
	A list of strings each representing a language
	Corresponds to "body-fld-lang" of RFC3501
	*/
	RArrayTPtrC8 iExtLanguageList;
	/**
	Corresponds to "body-fld-loc" of RFC3501
	*/
	TPtrC8 iExtLocation;
	
	// Note "future expansion" extension data is ignored safely by the parser.
	
private:
	/**
	If the bodystructure has an envelope (i.e. it is of type MESSAGE/RFC822)
	then a CImapEnvelope will be required to point to envelope items.
	GetRfc822EnvelopeStructureL() will construct an object of this type when first called.
	It is likely that CImapEnvelope will later be used to populate CImMimeHeader - which will cause string copies to occur.
	*/
	CImapEnvelope* iRfc822EnvelopeStructure;
	};

#endif // __CIMAPBODYSTRUCTURE_H__
