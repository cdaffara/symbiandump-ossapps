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

#include <versit.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <versitlinereaderext.h>
#include "versittlscontainer.h"
#include "versit_internal.h"
#endif

// System includes
#include <s32file.h>
#include <s32mem.h>
#include <utf.h>
#include <charconv.h>
#include <confndr.h>
#include <concnf.h>
#include <conlist.h>
#include <e32base.h>

// User includes
#include <vutil.h>
#include <vstaticutils.h>
#include <versittls.h>
#include "verror.h"
#include <vobserv.h>
#include <vcal.h>
#include "VersitAdditionalStorage.h"

// Constants
_LIT8(KReplacementChars, "?");
const TInt KVersitDatePropertySymbianConnectMessageMaximumLenght=1;

#define UNUSED_VAR(a) a = a

//
// CLineReader
//

EXPORT_C CLineReader* CLineReader::NewL(RReadStream& aStream)
/** Constructs and returns a CLineReader.

@param aStream The stream to read the lines from. */
	{
	CLineReader* self=new(ELeave) CLineReader(aStream);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CLineReader::ConstructL()
	{
	iSkipWhiteSpaceAtStart=ETrue;
	iLineBuf=HBufC8::NewL(EInitialLineSize);
	iBufPtr=iLineBuf->Des();
#if defined(__VC32__)
	iBufPtr.Set(iLineBuf->Des());    // workaround for VC6 bug
#endif
	iExtension=CLineReaderExtension::NewL();
	}

EXPORT_C CLineReader::~CLineReader()
/** Frees all resources owned by the line reader prior to its destruction. */
	{
	delete iLineBuf;
	
	if (iExtension && iReadStream)
		{
		if (iExtension->iSize - iExtension->iOffset > 0)
			{
			// there is data in the buffer that has not yet been parsed
			// so we must adjust the seek pointer for the input stream to
			// point exactly to the end of the data that has been parsed
			// so that the rest will be re-read when the file is next accessed.
			// The traps are neccesary because a destructor can't leave.
			MStreamBuf* source = iReadStream->Source();
			if (source != NULL)
				{
				TStreamPos pos(0);
				TRAPD(err, pos = source->TellL(MStreamBuf::ERead));
				if (err == KErrNone)
					{
					pos -= (iExtension->iSize - iExtension->iOffset);
					TRAP(err, source->SeekL(MStreamBuf::ERead, pos));
					}
				}
			}
		}

	delete iExtension;
	}
	
/** Read base64 multiple lines

This should be only called when parsing multi-line base64 data, and the interface 
MVersitPlugInExtensionBase64Ending::BlankLineAndLeadingSpaceNotRequired returns ETrue 

If the character ':' is not contained in the current line,we know it is base64 value.It is therefore appended to
iBufPtr. Otherwise we know it is a new property which will be held in CLineReaderExtension::iLineBase64Value 
temporarily until the next propery is to be parsed.

@param aPosValueStart The position in the iLineBuffer where base64 value starts 
*/	
void CLineReader::ReadBase64ValueL(TInt aPosValueStart)
	{
	HBufC8* base64Buffer = iExtension->CreateBase64ValueBufferL();//Creat a buffer to hold a
	 
	iBufPtr.Delete(0, aPosValueStart);//To make sure iBufPtr only contains base64 value	
	TInt err = KErrNone;
	TInt ret = ELineIsWhiteSpace;
	TPtr8 ptr = base64Buffer->Des();
	while (ret != ELineHasColon)
		{
		__ASSERT_DEBUG(ret==ELineHasContent ||ret==ELineIsWhiteSpace||ret==ELineIsCRLFOnly, Panic(ECurrentTokenNotFound));
		if (ret == ELineHasContent)
			{
			iLineBuf = iLineBuf->ReAllocL(iLineBuf->Length() + ptr.Length());
			iBufPtr.Set(iLineBuf->Des());	
			iBufPtr.Append(ptr);
			}
		
		ptr.SetLength(0);
		ret = ReadLineL(base64Buffer, 0, err);
		ptr.Set(base64Buffer->Des());
		}
	}
	
/** Reads in a line from the stream.

The line is stored in a buffer pointed to by iBufPtr. Data from the stream 
is appended to the buffer, starting at buffer position aPos, until the next 
'CRLF' ("/r/n") is reached. The line feed and carriage return are not added 
to the buffer. Using only linefeeds LF(\n) as the line delimiters is also supported.

The buffer will be expanded if necessary to fit the line into it. 

If the end of stream is reached, aErr will contain KErrEof upon return.

It would be normal to set aPos to the start of the buffer unless reading a 
multi-line value.

@param aPos The position in the buffer pointed to by iBufPtr at which to begin 
appending data to the buffer. Allows lines to be appended to the buffer when 
reading multi-line values.
@param aErr On return, this will be KErrEof if the end of the stream is reached 
and KErrNone otherwise.
@return ELineHasContent if the line has content; ELineIsWhiteSpace if the line 
has only white space before the CRLF; ELineIsCRLFOnly if the line is only 
a CRLF. 
*/
EXPORT_C TInt CLineReader::ReadLineL(TInt aPos,TInt& aErr)
	{
	TInt ret = ELineHasContent;
	HBufC8* base64Buffer = iExtension->Base64ValueBuffer();
	if(aPos==0 && base64Buffer && base64Buffer->Length()>0)
		{//If the base64Buffer is not empty, we know the current line has
		// already been read and the data is held in CLineReaderExtension::iLineBase64Value
		// for a new property (see ReadBase64ValueL)
		if(iBufPtr.MaxLength() < base64Buffer->Length())
			{
			iLineBuf = iLineBuf->ReAllocL(base64Buffer->Length());
			}
		
		*iLineBuf = *base64Buffer;
		iBufPtr.Set(iLineBuf->Des());
		iBufPtr.SetLength(base64Buffer->Length());
		iExtension->DeleteBase64ValueBuffer();
		}
	else //Otherwise read the current line
		{
		ret = ReadLineL(iLineBuf, aPos, aErr);
		if(ret == ELineHasColon)
			{
			ret = ELineHasContent;//For the sake of backwards compatability - ELineHasColon is only used by ReadBase64ValueL. 
			}
		iBufPtr.Set(iLineBuf->Des());
		}
		
	return ret;
	}

TInt CLineReader::ReadLineL(HBufC8*& aHBuf, TInt aPos,TInt& aErr)
/** Reads in a line from the stream.

The line is stored in a buffer pointed to by aHBuf. Data from the stream 
is appended to the buffer, starting at buffer position aPos, until the next 
'CRLF' ("/r/n") is reached. The line feed and carriage return are not added 
to the buffer. Using only linefeeds LF(\n) as the line delimiters is also supported.

The buffer will be expanded if necessary to fit the line into it. 

If the end of stream is reached, aErr will contain KErrEof upon return.

It would be normal to set aPos to the start of the buffer unless reading a 
multi-line value.

@param aPos The position in the buffer pointed to by aHBuf at which to begin 
appending data to the buffer. Allows lines to be appended to the buffer when 
reading multi-line values.
@param aErr On return, this will be KErrEof if the end of the stream is reached 
and KErrNone otherwise.
@return 
ELineHasContent if the line has content;
ELineIsWhiteSpace if the line has only white space before the CRLF;
ELineIsCRLFOnly if the line is only a CRLF.
EELineHasColon If character ':' contained in the line
*/
	{
	TInt crPos=-1;
	TUint8 ch=STATIC_CAST(TUint8,iFirstCharNextLine);
	TInt whiteSpaceLine=ELineIsCRLFOnly;
	TBool truncateLine=ETrue;
	TBool skipWhiteSpaceAtStart=iSkipWhiteSpaceAtStart;
	TPtr8 ptrHbuf = aHBuf->Des();
	ptrHbuf.SetLength(aPos);
	if (iFirstCharNextLine>=0)
		{
		iFirstCharNextLine=-1;
		goto CharacterRead;
		}
	FOREVER
		{
		ch=ReadChar(aErr);
		if (aErr!=KErrEof)
			User::LeaveIfError(aErr);
		else
			{
			crPos=aPos-1;
			while (crPos>=0 && (ptrHbuf[crPos]==CVersitParser::ELineFeed || ptrHbuf[crPos]==CVersitParser::ECarriageReturn))
				{
				--crPos;
				}
			truncateLine=(++crPos<aPos);
			break;
			}
	CharacterRead:
		if (ch==CVersitParser::ECarriageReturn)
			{
			if (crPos<0)
				crPos=aPos;
			skipWhiteSpaceAtStart=EFalse;
			// next character should be a LF...
			}
		else if (ch==CVersitParser::ELineFeed)
			{
			if (crPos<0)
				{
				// LF without CR
				crPos=aPos;
				skipWhiteSpaceAtStart=EFalse;
				}		
			break; // The end of a line.
			}
		else 
			{
			crPos=-1;
			if (whiteSpaceLine == ELineIsWhiteSpace || whiteSpaceLine == ELineIsCRLFOnly )
				{
				whiteSpaceLine=(VersitUtils::IsWhiteSpace(ch) ? ELineIsWhiteSpace:ELineHasContent);
				if (!whiteSpaceLine)
					skipWhiteSpaceAtStart=EFalse;
				}
				
			if (ch == CVersitParser::EColon)
				{
				whiteSpaceLine = ELineHasColon;
				}
			}
		if (skipWhiteSpaceAtStart)
			{
			if (iPlugIn)
				skipWhiteSpaceAtStart=iPlugIn->DeleteAllSpaces();
			}
		else
			{
			if (aPos==ptrHbuf.MaxLength())
				{
				aHBuf=aHBuf->ReAllocL(aPos+EExpandSize);
				ptrHbuf.Set(aHBuf->Des());
				}

			++aPos;
			ptrHbuf.Append(ch);
			}
		}
	if (truncateLine)
		ptrHbuf.SetLength(crPos);
	return whiteSpaceLine;
	}

EXPORT_C TBool CLineReader::AppendLineIfSpaceNextL()
/** Checks the first character of the next line and, if it is a white space, reads 
the next line into the buffer (pointed to by iBufPtr).

The line is appended to the buffer rather than overwriting data already in 
the buffer.

A plug-in option can determine that no space is to be written to the buffer 
between the lines. By default, a space will be written.

Used by the Versit parser while it is reading multi-line property values.

@return ETrue if a line was read (because the first character of the next 
line is a space); EFalse if a line wasn't read (because the first character 
of the next line isn't a space or an error occured). */
	{
	TInt err=KErrNone;
	if (iFirstCharNextLine<0)
		iFirstCharNextLine=ReadChar(err);
	if (err!=KErrNone || !VersitUtils::IsWhiteSpace(iFirstCharNextLine))
		return EFalse;
	TInt len;
	if (iPlugIn && !iPlugIn->AddSpace())
		len=iBufPtr.Length();
	else
		len=AppendSpaceL();
	ReadLineL(len,err);
	return ETrue;
	}

EXPORT_C TBool CLineReader::IsSpaceNextL()
/** Checks to see if the first character of the next line is white space.

This function should not be called more than once without a line being read 
in.

Used by the Versit parser while it is reading multi-line property values.

@return ETrue if the next line starts with a white space character; EFalse 
if it doesn't or an error occurs. */
	{
	TInt err;
	iFirstCharNextLine=ReadChar(err);
	return (err==KErrNone && VersitUtils::IsWhiteSpace(iFirstCharNextLine));
	}

EXPORT_C TInt CLineReader::AppendSpaceL()
/** Appends a space to the end of the buffer (pointed to by iBufPtr).

The buffer will be expanded if necessary.

This is called by AppendLineIfSpaceNextL() to create a space in the buffer 
between the lines being read (as long as there is no plug-in option to to 
indicate there should be no space).

@param The length of the buffer after the space has been added. */
	{
	TInt len=iBufPtr.Length();
	if (len==iBufPtr.MaxLength())
		ExpandBufferL(len);
	iBufPtr.Append(CVersitParser::ESpace);
	return len+1;
	}

EXPORT_C void CLineReader::ExpandBufferL(TInt aCurrentSize)
	{
	iLineBuf=iLineBuf->ReAllocL(aCurrentSize+EExpandSize);
	iBufPtr.Set(iLineBuf->Des());
	}

EXPORT_C TUint8 CLineReader::ReadChar(TInt& aErr)
	{
	// Return the next character from the read buffer. If it is the
	// end of the buffer or it is empty then refill the buffer.
	aErr = KErrNone;
	if (iExtension->iOffset >= iExtension->iSize)
		{
		iExtension->iOffset = 0;
		MStreamBuf* source = iReadStream->Source();
		TRAP(aErr, iExtension->iSize = source->ReadL(iExtension->iBuf, 1024));
		if (aErr != KErrNone)
			{
			return 0;
			}
		if (iExtension->iSize == 0)
			{
			aErr = KErrEof;
			return 0;
			}
		}
	return iExtension->iBuf[iExtension->iOffset++];
	}

EXPORT_C void CLineReader::Reserved()
	{}


//
// CVersitParser
//

EXPORT_C CVersitParser::CVersitParser(TUint aFlags) : iFlags(aFlags)
/** The first phase constructor for a Versit parser. 

Sets the default encoding to Versit::ENoEncoding and the default character 
set to Versit::EUSAsciiCharSet.

Note: this function is called by the CParserVCal and CParserVCard constructors. It 
should only be called directly by a user if creating a new parser

@param aFlags The flag to indicate whether this entity needs a version property. 
The possible values for this flag are given in the TVersitParserFlags enumeration. 
If a version property is needed, one will be created and appended to the 
start of the array of properties. */
	{
	SetDefaultEncoding(Versit::ENoEncoding);
	SetDefaultCharSet(Versit::EUSAsciiCharSet);
	
	RestoreLineCodingDetailsToDefault();
	}

void CloseTlsData(TAny *aData)
	{
	CVersitTlsData *data = static_cast<CVersitTlsData *>(aData);
	data->CloseVersitTlsData();
	}

EXPORT_C void CVersitParser::ConstructL()
/** The second phase constructor for a Versit parser.

Stores a pointer to a CVersitTlsData (thread local storage data class). This 
is used to allow an instance of CVersitUnicodeUtils to be shared by all co-existing 
parsers, which provides a major performance improvement.

Called by the CParserVCal and CParserVCard constructors.

Should only be called directly by a user if creating a new parser. */
	{
	_LIT8(KEscapeCharacter8, "\\");
	iStaticUtils=&CVersitTlsData::VersitTlsDataL();
	
	//query charconv for the plugin dependant shift-jis escape character, 
	//store in TLS to be accessed by versit static utils
	CCnvCharacterSetConverter::TAvailability avail=UnicodeUtils().SetCurrentCharSetL(KCharacterSetIdentifierShiftJis);
	if (avail == CCnvCharacterSetConverter::EAvailable)
		{
		// Do the conversion
		CCnvCharacterSetConverter& converter=UnicodeUtils().CharacterSetConverter();
		TInt ignore = 0;
		converter.SetReplacementForUnconvertibleUnicodeCharactersL(KReplacementChars);

		CVersitTlsData& data = CVersitTlsData::VersitTlsDataL();
		CleanupStack::PushL( TCleanupItem( CloseTlsData, &data ) );

		CVersitAdditionalStorage& additionalStorage = data.AdditionalStorage();
		CBase* storedValue = additionalStorage.FromStorage(KTLSVars);

		if( NULL == storedValue )
			{
			CVersitTLSContainer *tlsContainer = CVersitTLSContainer::NewLC(2);
			TPtrC8 ptr8 = KEscapeCharacter8();
			TPtr ptr16( tlsContainer->iShiftJisEscape->Des() );
			converter.ConvertToUnicode(ptr16, ptr8, ignore);
			additionalStorage.AddToStorageL(KTLSVars, tlsContainer);
			CleanupStack::Pop( tlsContainer );
			}

		CleanupStack::PopAndDestroy();//data
		}
	}

EXPORT_C CVersitParser::~CVersitParser()
// Note: CVersitParser never owns iReadStream/iWriteStream
/** Frees all resources owned by the object, prior to its destruction. */
	{
	if (iArrayOfEntities)
		{
		iArrayOfEntities->ResetAndDestroy();
		delete iArrayOfEntities;
		}
	if (iArrayOfProperties)
		{
		iArrayOfProperties->ResetAndDestroy();
		delete iArrayOfProperties;
		}
	delete iCurrentProperty;
	delete iEntityName;
	delete iOwnedLineReader;
	delete iLargeDataBuf;
	delete iDecodedValue;

	// Close any TLS storage
	if (iStaticUtils)
		iStaticUtils->VersitTlsDataClose();
	}

EXPORT_C void CVersitParser::InternalizeL(RFile& aInputFile,TInt& aBytesThroughFile)
/** Internalises a versit entity (vCard or vCalendar) and all of its
sub-entities and properties from a file.

Stores all date/time in same format as received

@param aInputFile  The file from which to internalise the entity.
@param aBytesThroughFile The number of bytes into the file at which to begin 
reading. On return, is updated to contain the last position in the file which 
was read.
@leave KErrEof The file position specified is beyond the end of the file. */
	{
	TInt size;
	if (aInputFile.Size(size)==KErrNone)
		{
		RFileReadStream stream(aInputFile,aBytesThroughFile);
		TInt size=stream.Source()->SizeL();
		TRAPD(err,InternalizeL(stream));
		if (err==KErrNone)
			TRAP(err,aBytesThroughFile+=(size-stream.Source()->SizeL()));
		STATIC_CAST(RFileBuf*,stream.Source())->Reset();
		stream.Close();
		User::LeaveIfError(err);
		}
	}

EXPORT_C void CVersitParser::InternalizeL(RReadStream& aStream)
/** Internalises a Versit entity (vCard or vCalendar) contained in the incoming stream 
and parses it, breaking it down into its constituent sub-entities (e.g. to-dos, 
events, and nested vCards) and properties.

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

This virtual function serves as a base function for derived classes to internalise an entity.

@param aStream Stream from which the vCalendar should be internalised. */
	{
	iOwnedLineReader=CLineReader::NewL(aStream);
	iOwnedLineReader->SetPlugIn(iPlugIn);
	iLineReader=iOwnedLineReader;
	
	TRAPD(err,DoInternalizeL());

	delete iDecodedValue;
	iDecodedValue=NULL;
	delete iOwnedLineReader;
	iOwnedLineReader=NULL;
	iLineReader=NULL;
	
	User::LeaveIfError(err);
	}

void CVersitParser::DoInternalizeL()
	{
	iParseBegin = TRUE;
	ParseBeginL();
	iParseBegin = FALSE;
	
	do
		{
		delete iCurrentProperty;
		iCurrentProperty=NULL;
		ParsePropertiesL();
		} while(ParseEntityL());
	ParseEndL();
	}

EXPORT_C void CVersitParser::InternalizeL(HBufC* aEntityName,CLineReader* aLineReader)
/** Internalises a vCalendar or vCard sub-entity.

(Assumes "BEGIN : <EntityName> CRLF" has been parsed).

This virtual function serves as a base function for derived classes to parse a 
sub-entity.

@param aEntityName The entity name to be parsed. Ownership of aEntityName is taken by 
the parser.
@param aLineReader Pointer to a line reader which is used by the super-entity. */
	{
	iLineReader=aLineReader;

	ParsePropertiesL();
	ParseEndL(*aEntityName);
	
	delete iEntityName;
	iEntityName = aEntityName;

	iLineReader=NULL;
	delete iDecodedValue;
	iDecodedValue=NULL;
	}

EXPORT_C void CVersitParser::ExternalizeL(RFile& aOutputFile)
/** Externalises a Versit entity (vCard or vCalendar) and all of its sub-entities 
and properties to a file. 

Adds a version property to the start of the current entity's array of properties 
if the entity supports this.

This is a thin layer over the CVersitParser::ExternalizeL(RWriteStream& aStream) 
function to enable a versit entity to be externalised into an RFile.

@param aOutputFile The file to which to write the entity. */
	{
	RFileWriteStream stream(aOutputFile);
	CleanupClosePushL(stream);
	ExternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	}

EXPORT_C void CVersitParser::ExternalizeL(RWriteStream& aStream)
/** Externalises a Versit entity (vCard or vCalendar) and all of its sub-entities 
and properties to a write stream. 

Adds a version property to the start of the current entity's array of properties 
if the entity supports this. (If there isn't an array of properties then one is made).

The presence of this function means that the standard templated operator<<() (defined 
in s32strm.h) is available to externalise objects of this class.

This virtual function serves as a base function for derived classes to externalise 
an entity.

@param aStream Stream to which the entity should be externalised. */
	{
	iWriteStream=&aStream;

	if (SupportsVersion())
		{
		if (!iArrayOfProperties)
			iArrayOfProperties = new(ELeave)CArrayPtrFlat<CParserProperty>(5);
		TUid uid = TUid::Uid(KVersitPropertyHBufCUid);
		
		// get properties but don't take ownership of the elements of the array
		CArrayPtr<CParserProperty>* properties = PropertyL(KVersitTokenVERSION, uid, EFalse);
		if (properties)
			delete properties;
		else	//Need to add a version property
			{
			CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(iDefaultVersion);
			value->SetPlugIn(iPlugIn);
			CleanupStack::PushL(value);
			CParserProperty* property = CParserProperty::NewL(*value, KVersitTokenVERSION, NULL);
			CleanupStack::Pop(value);
			CleanupStack::PushL(property);
			iArrayOfProperties->InsertL(0,property);
			CleanupStack::Pop(property);
			}
		}
	AppendBeginL();
	if (iArrayOfProperties)
		{
		TInt noOfProperties=iArrayOfProperties->Count();
		for (TInt ii=0; ii<noOfProperties; ii++)
			{
			(*iArrayOfProperties)[ii]->ExternalizeL(aStream, this);
			}
		}
	if (iArrayOfEntities)
		{
		const TInt noOfEntities = iArrayOfEntities->Count();
		for (TInt ii=0; ii<noOfEntities; ii++)
			{
			// Ensure that the entities defaults match this parser's
			CVersitParser& entity = *iArrayOfEntities->At(ii);
			//
			entity.SetDefaultCharSet(DefaultCharSet());
			entity.SetDefaultEncoding(DefaultEncoding());
			//
			entity.ExternalizeL(aStream);
			}
		}
	AppendEndL();
	iWriteStream = NULL;
	}

/*void CVersitParser::AddTimeZonePropertyL()		//Function that may be useful one day
	{
	TLocale locale;
	TTimeIntervalSeconds offSet(locale.UniversalTimeOffset());
	CParserPropertyValue* timeZone=new(ELeave)CParserPropertyValueTimeZone(offSet);
	CleanupStack::PushL(timeZone);
	CParserProperty* property = CParserProperty::NewL(*timeZone,KVersitTokenTZ,NULL);
	CleanupStack::Pop(timeZone);
	CleanupStack::PushL(property);
	iArrayOfProperties->InsertL(0,property);
	CleanupStack::Pop(property);
	}*/

EXPORT_C void CVersitParser::AddPropertyL(CParserProperty* aProperty,TBool aInternalizing)
/** Appends a property to the current entity's array of properties. 

This function may be used when building up a Versit parser object from 
a client application. It is not called internally.

@param aProperty Pointer to the property to add to the entity. 
@param aInternalizing This parameter is used to distinguish between reading 
an entity from a stream or file (i.e. internalising), in which case the argument 
should have a value of ETrue, and building an entity "by hand" (e.g. creating 
a vCard in the Contacts application), in which case the argument should have 
a value of EFalse. */
	{
	if	(!aProperty)
		return;

	TUid tokenId=RecognizeToken(aProperty->Name());
	CleanupStack::PushL(aProperty);
	
	if ((tokenId.iUid==KVersitTokenUnknownUid) && aProperty->Name().CompareF(KVersitTokenATTACH)==0)
		{
		tokenId.iUid = KVersitPropertyBinaryUid;//In vCal standard, default value is binary
		CParserParam* valueParam = aProperty->Param(KVersitTokenVALUE);
		if (valueParam)
			{
			TPtrC8 pParameterValue(valueParam->Value());
			
			if (pParameterValue.CompareF(KVersitTokenINLINE) && pParameterValue.CompareF(KVersitTokenBINARY))
				{//If VALUE isn't specified as INLINE or BINARY, value is text type.
				tokenId.iUid = KVersitPropertyHBufCUid;			
				}
			}
		}
 	
 	aProperty->SetNameUid(tokenId);

	if ((tokenId.iUid==KVersitTokenUnknownUid) && !aInternalizing)
		{
		//Do not prefix X-EPOC incase the property already has a prefix X- at the begining		
		if(aProperty->NameBuf()->Find(KVersitTokenXDash)!=0)
			{	
			HBufC8*& name=aProperty->NameBuf();
			HBufC8* newName=HBufC8::NewL(name->Length()+KVersitTokenXDashEPOC.iTypeLength);
			TPtr8 ptr=newName->Des();
			ptr=KVersitTokenXDashEPOC;
			ptr+=*name;
			delete name;
			name=newName;
			}
		}

	DoAddPropertyL(aProperty);
	CleanupStack::Pop(aProperty);
	}

EXPORT_C void CVersitParser::DoAddPropertyL(CParserProperty* aProperty)
	{
	if	(!aProperty)
		return;

	if (!iArrayOfProperties)
		iArrayOfProperties = new(ELeave)CArrayPtrFlat<CParserProperty>(5);

	if (aProperty->Value())
		aProperty->Value()->SetPlugIn(iPlugIn);
	iArrayOfProperties->AppendL(aProperty);
	}


EXPORT_C void CVersitParser::AddEntityL(CVersitParser* aEntity)
/** Adds a sub-entity (e.g. a to-do, event or a nested vCard) to the current entity.

Sets the default encoding and character set to the default ones of the current 
Versit parser, then adds the entity to the array of entities owned by the 
parser.

Note: This function may be used when building up a Versit parser object from 
a client application.

Called by ParseEntityL().

@param aEntity The entity to be added into the array of entities. Ownership 
of aEntity is taken at beginning of the function. */
	{
	CleanupStack::PushL(aEntity);

	if	(!iArrayOfEntities)
		iArrayOfEntities = new(ELeave)CArrayPtrFlat<CVersitParser>(5);

	if	(aEntity)
		{
		// Ensure that the entity contains the same defaults as this parser
		aEntity->SetDefaultCharSet(DefaultCharSet());
		aEntity->SetDefaultEncoding(DefaultEncoding());
		//
		aEntity->SetLineCharacterSet(LineCharSet());
		aEntity->SetLineEncoding(LineEncoding());
		//
		iArrayOfEntities->AppendL(aEntity);
		}

	CleanupStack::Pop(); //aEntity
	}

EXPORT_C CArrayPtr<CVersitParser>* CVersitParser::EntityL(const TDesC& aEntityName,TBool aTakeOwnership)
/** Gets all sub-entities in the current entity, whose name matches the 
name specified.

@param aEntityName The sub-entity name of interest, e.g. KVersitVarTokenVEVENT or 
KVersitVarTokenVTODO. All sub-entities which match this name are returned in the 
array of pointers.
@param aTakeOwnership If ETrue, the calling code takes ownership of each matching 
sub-entity, in which case the sub-entities are deleted from the current object's 
array of entities. If EFalse, ownership remains with the Versit parser.
@return An array of pointers to all sub-entities with the specified name. 
(Ownership is taken by the calling code). */
	{
	CArrayPtr<CVersitParser>* arrayOfNamedEntities = NULL;
	if (iArrayOfEntities)
		{
		TInt entities=iArrayOfEntities->Count();
		arrayOfNamedEntities=new(ELeave) CArrayPtrFlat<CVersitParser>(5);
		//CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfEntities,arrayOfNamedEntities));
		CleanupStack::PushL(arrayOfNamedEntities);
		for (TInt ii=0; ii<entities; ii++)
			{
			if ((*iArrayOfEntities)[ii]->EntityName()==aEntityName)
				{
				arrayOfNamedEntities->AppendL((*iArrayOfEntities)[ii]);
				}
			}
		CleanupStack::Pop(); //arrayOfNamedEntities
		if (arrayOfNamedEntities->Count() == 0)
			{
			delete arrayOfNamedEntities;
			arrayOfNamedEntities = NULL;
			}
		else if(aTakeOwnership)	
			{
			for (TInt ii = entities-1; ii>=0; --ii)
				{
				if ((*iArrayOfEntities)[ii]->EntityName()==aEntityName)
					{
					iArrayOfEntities->Delete(ii);
					} // if
				} // for
			} // else if
		}
	return arrayOfNamedEntities;
	}

EXPORT_C CArrayPtr<CVersitParser>* CVersitParser::ArrayOfEntities(TBool aTakeOwnership)
/** Gets the current entity's array of sub-entities. 

Note: the current entity may be a top level entity, or may itself be a sub-entity.

@param aTakeOwnership If ETrue, the calling code takes ownership of the array; 
the array of entities owned by the current entity is then deleted. If EFalse, 
ownership remains with the Versit parser. 
@return Array of pointers to the current entity's array of sub-entities. */
	{
	CArrayPtr<CVersitParser>* arrayOfEntities=iArrayOfEntities;
	if (aTakeOwnership)
		iArrayOfEntities=NULL;
	return arrayOfEntities;
	}

EXPORT_C CArrayPtr<CParserProperty>* CVersitParser::PropertyL(const TDesC8& aPropertyName,const TUid& aPropertyUid,TBool aTakeOwnership) const
/** Gets all properties in the current entity's array of properties whose name and 
value match the name and value specified.

@param aPropertyName The property name of interest. Property names are defined 
in vtoken.h. 
@param aPropertyUid The ID of the property value of interest (which has the 
format "K<Versit/Card/Cal>Property<Type>Uid"). The values are defined in file vuid.h.
@param aTakeOwnership If ETrue, the calling code takes ownership of each matching 
property; the property is then deleted from the current entity's array. If 
EFalse, ownership remains with the entity. 
@return An array of pointers to all properties with the name and value specified, 
or NULL if there are none. */
	{
	CArrayPtr<CParserProperty>* arrayOfNamedProperties = NULL;
	if (iArrayOfProperties)
		{
		const TInt properties = iArrayOfProperties->Count();
		arrayOfNamedProperties = new(ELeave) CArrayPtrFlat<CParserProperty>(5);
		//CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfProperties, arrayOfNamedProperties));
		CleanupStack::PushL(arrayOfNamedProperties);
		CParserProperty* parserProperty = NULL;
		for (TInt ii = 0; ii < properties; ++ii)
			{
			parserProperty = (*iArrayOfProperties)[ii];
			if (parserProperty->Value() && parserProperty->Name().CompareF(aPropertyName)==0)
				{
				if(parserProperty->Value()->Uid()==aPropertyUid || aPropertyUid == KNullUid)
					{
					arrayOfNamedProperties->AppendL(parserProperty);
					}
				}
			}
		CleanupStack::Pop(arrayOfNamedProperties);
		if (arrayOfNamedProperties->Count() == 0)
			{
			delete arrayOfNamedProperties;
			arrayOfNamedProperties = NULL;
			}
		else if (aTakeOwnership)
			{
			for (TInt ii = properties-1; ii>=0; --ii)
				{
				parserProperty = (*iArrayOfProperties)[ii];
				if (parserProperty->Name().CompareF(aPropertyName)==0)
					{
					if (parserProperty->Value()->Uid()==aPropertyUid || aPropertyUid == KNullUid)
						{
						iArrayOfProperties->Delete(ii);
						}
					}
				}
			}
		}
	return arrayOfNamedProperties;
	}

EXPORT_C CArrayPtr<CParserProperty>* CVersitParser::ArrayOfProperties(TBool aTakeOwnership)
/** Gets the current entity's array of properties.

@param aTakeOwnership If ETrue, the calling code takes ownership of the array; 
the entity's array of properties is then deleted . If EFalse, ownership 
remains with the entity. 
@return Array of pointers to the entity's properties. */
	{
	CArrayPtr<CParserProperty>* arrayOfProperties=iArrayOfProperties;
	if (aTakeOwnership)
		iArrayOfProperties=NULL;
	return arrayOfProperties;
	}

EXPORT_C void CVersitParser::SetEntityNameL(const TDesC& aEntityName)
/** Sets the name for the current entity to one of: VCARD, VCALENDAR, VEVENT or 
VTODO.

@param aEntityName The new name for the current parser entity. Any existing 
name is first deleted. 
@leave KErrNoMemory Memory is allocated for the new entity name on the heap, 
so it can leave if there is insufficient memory available. */
	{
	delete iEntityName;
	iEntityName = NULL;
	iEntityName = aEntityName.AllocL();
	}

EXPORT_C TPtrC CVersitParser::EntityName() const
/** Gets the current entity's name. 

If no name has been set, returns an empty descriptor.

@return The current entity's name. */
	{
	if (iEntityName)
		return iEntityName->Des();
	return KVersitTokenEmpty();
	}

EXPORT_C TBool CVersitParser::IsValidLabel(const TDesC& aLabel, TInt& aPos)
/** Tests whether a property name is valid.

The property name is invalid if it contains any of the following characters:-

[] (left or right square bracket)

= (equals sign)

: (colon)

. (dot)

, (comma)

@param aLabel The property name to test. 
@param aPos On return, contains the character position within the property 
name of the first invalid character found. 
@return ETrue if valid, EFalse if invalid. */
	{
	return !(((aPos = aLabel.Locate(KVersitTokenLSquareBracketVal)) != KErrNotFound) ||
		((aPos = aLabel.Locate(KVersitTokenRSquareBracketVal)) != KErrNotFound) ||
		((aPos = aLabel.Locate(KVersitTokenEqualsVal)) != KErrNotFound) ||
		((aPos = aLabel.Locate(KVersitTokenColonVal)) != KErrNotFound) ||
		((aPos = aLabel.Locate(KVersitTokenPeriodVal)) != KErrNotFound) ||
		((aPos = aLabel.Locate(KVersitTokenCommaVal)) != KErrNotFound));
	}

EXPORT_C CArrayPtr<CParserParam>* CVersitParser::ReadLineAndDecodeParamsLC(TInt& aValueStart,TInt& aNameLen)
	{
	//Read in line from stream skipping lines made up only of white space TChar
	TInt err;
	do
		{
		const TInt readLineResponse = iLineReader->ReadLineL(0,err);
		if  (readLineResponse == CLineReader::ELineIsWhiteSpace || readLineResponse == CLineReader::ELineIsCRLFOnly)
			{
			aValueStart = 0;
			}
		else
			{
			TBool continueSearch;
			do
				{
				continueSearch = EFalse;
				aValueStart = BufPtr().Find(KVersitTokenColon) + 1;
				if (aValueStart == 0)	
					{
					continueSearch = iLineReader->AppendLineIfSpaceNextL();
					}
				}
			while (continueSearch && aValueStart == 0);
			}
		}
	while (aValueStart==0 && err!=KErrEof);
	//
	if (aValueStart==0)
		User::Leave(err);
	iFlags&=~ECharSetIdentified;
	
	if(iFlags & EUseDefaultCharSetForAllProperties)
		{
		SetLineCharacterSet(DefaultCharSet());
		iFlags|= ECharSetIdentified;
		}
		
	//Scan for semi-colon and split off params
	CArrayPtr<CParserParam>* arrayOfParams=NULL;
	aNameLen=BufPtr().Find(KVersitTokenSemiColon);		//First ";" can't be escaped
	TInt lenParams=aValueStart-aNameLen-2;
	if (aNameLen==KErrNotFound || lenParams<=0)
		{
		aNameLen=(lenParams==0&&aNameLen>=0 ? aNameLen:aValueStart-1);
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
		}
	else
		{
		TPtr8 params(&BufPtr()[aNameLen+1],lenParams,lenParams);
		arrayOfParams=GetPropertyParamsLC(params);
		AnalysesEncodingCharset(arrayOfParams);
		}
	return arrayOfParams;
	}

EXPORT_C void CVersitParser::MakePropertyL(TPtr8& aPropName,TInt aValueStart)
	{
	VersitUtils::StripWhiteSpace(aPropName);
	TUid propertyUid=RecognizeToken(aPropName);
	iCurrentProperty->SetNameL(aPropName);
	//Deal with the value
	TInt lenValue=BufPtr().Length()-aValueStart;
	TPtr8 propValue(NULL,0);
	if (lenValue>0)
		propValue.Set(&BufPtr()[aValueStart],lenValue,lenValue);
	TBool beginOrEnd=VersitUtils::IsBeginOrEnd(propertyUid);
	TBool readMultiLine=!beginOrEnd;
	Versit::TVersitEncoding encoding=Versit::ENoEncoding;
	if (!readMultiLine)
		{
		encoding=LineEncoding();
		readMultiLine=(encoding==Versit::EBase64Encoding || encoding==Versit::EQuotedPrintableEncoding);
		}
	/**
		Ignore junk lines between vCal entities.
		Skip readMultiLine within ParseBegin.
	*/
	if (readMultiLine && !iParseBegin)

		{
		CParserParam* valueParam = iCurrentProperty->Param(KVersitTokenVALUE);
		if (valueParam)
			{
			TPtrC8 pParameterValue(valueParam->Value());
			
			if (!pParameterValue.CompareF(KVersitTokenINLINE))
				{
				encoding = Versit::EBase64Encoding;				
				}
			}

		TBool isBinary = ((propertyUid.iUid==KVersitPropertyBinaryUid) ||
						  ((propertyUid.iUid==KVCalPropertyExtendedAlarmUid) && (encoding==Versit::EBase64Encoding)));
		ReadMultiLineValueL(propValue, aValueStart, isBinary);
		}
	if (beginOrEnd)
		{
		VersitUtils::StripWhiteSpace(propValue);
		if (encoding!=Versit::EBase64Encoding)
			propValue.UpperCase();
		}
	iCurrentProperty->SetNameUid(propertyUid);
	
	HBufC* valueBuf=DecodePropertyValueL(propValue);
	CleanupStack::PushL(TCleanupItem(DestroyHBufC, &valueBuf));
	if (iPlugIn && (valueBuf != NULL))
		{
		TPtr ptr(valueBuf->Des());
	#if defined(__VC32__)    // workaround for VC6 bug
		ptr.Set(valueBuf->Des());
	#endif
		iPlugIn->RemoveEscaping(ptr);
		}
	CParserPropertyValue* value = MakePropertyValueL(propertyUid,valueBuf);
	delete iLargeDataBuf;
	iLargeDataBuf=NULL;
	iCurrentProperty->SetValue(value);
	CleanupStack::PopAndDestroy(&valueBuf);

	}

EXPORT_C void CVersitParser::ParsePropertyL()
	{
	TInt valueStart;
	TInt lenName;
	CArrayPtr<CParserParam>* arrayOfParams=ReadLineAndDecodeParamsLC(valueStart,lenName);
	delete iCurrentProperty;
	iCurrentProperty=NULL;
	iCurrentProperty=new(ELeave) CParserProperty(arrayOfParams);
	CleanupStack::Pop(arrayOfParams);
	if (lenName>0)
		{
		TPtr8 propName(&BufPtr()[0],lenName,lenName);
		MakePropertyL(propName,valueStart);
		}
	}

EXPORT_C void CVersitParser::ReadMultiLineValueL(TPtr8& aValue,TInt aValueStart,TBool aBinaryData)
	{
	if (iLargeDataBuf)
		iLargeDataBuf->Reset();
	Versit::TVersitEncoding encoding=LineEncoding();
	TInt lineLen=BufPtr().Length();
	TInt err;
	TBool secondLine=ETrue;
	__ASSERT_DEBUG(encoding!=Versit::EEightBitEncoding, Panic(EVersitPanicUnexpectedEightBitEncoding));
	if (encoding!=Versit::EBase64Encoding)
		{
		if (encoding==Versit::EQuotedPrintableEncoding)
			{
			iLineReader->SetSkipWhiteSpaceAtStart(EFalse); // avoid space skiping for Quoted Printable encoding
			}
		TBool moreLinesToCome=ETrue;
		if (encoding!=Versit::EQuotedPrintableEncoding)
			moreLinesToCome=iLineReader->IsSpaceNextL();
		while (moreLinesToCome && (BufPtr()[lineLen-1]==KVersitTokenEqualsVal || encoding!=Versit::EQuotedPrintableEncoding))
			{
			if (secondLine)
				{
				lineLen-=aValueStart;
				BufPtr()=BufPtr().Mid(aValueStart,lineLen);
				secondLine=EFalse;
				}
			if (encoding==Versit::EQuotedPrintableEncoding)
				{
				--lineLen;
				moreLinesToCome=!iLineReader->ReadLineL(lineLen,err);
				}
			else if (lineLen==0)
				{
				iLineReader->ReadLineL(lineLen,err);
				moreLinesToCome=iLineReader->IsSpaceNextL();
				}
			else
				{
				moreLinesToCome=iLineReader->AppendLineIfSpaceNextL();
				}
			lineLen=BufPtr().Length();
			}
		iLineReader->SetSkipWhiteSpaceAtStart(ETrue); //The SetSkipWhiteSpaceAtStart function as been added for a defect fixing this line ensure that every other code calling ReadLineL behave the same than before. Stephane Lenclud.
		}
	else		// encoding == Versit::EBase64Encoding
		{
		if (aBinaryData)
			{
			if (!iLargeDataBuf)
				iLargeDataBuf=CBufSeg::NewL(128);
			}
		TInt bufLen=0;		//Initialise to stop warning "local variable may be used without having been initialized"
		TBool moreLinesToCome=ETrue;

		if (iPlugIn)
			{
			TAny* extPlugIn  = NULL;
			iPlugIn->GetInterface(KUidVersitPluginExtensionBase64Termination, extPlugIn);
			if (extPlugIn && static_cast<MVersitPlugInExtensionBase64Ending*>(extPlugIn)->BlankLineAndLeadingSpaceNotRequired())
				{
				iLineReader->ReadBase64ValueL(aValueStart);
				if(aBinaryData)
					{
					iLargeDataBuf->InsertL(bufLen,BufPtr());
					}
				lineLen=BufPtr().Length();
				moreLinesToCome = EFalse;
				secondLine=EFalse;
				}
			}

		while (moreLinesToCome)
			{
			if (secondLine)
				{
				lineLen-=aValueStart;
				TPtrC8 data=BufPtr().Mid(aValueStart,lineLen);
				if (aBinaryData)
					{
					iLargeDataBuf->InsertL(0,data);
					bufLen=lineLen;
					lineLen=0;
					}
				else
					BufPtr()=data;
				secondLine=EFalse;
				}
				
			if (iPlugIn && !iPlugIn->NeedsBlankLine())
				{
				moreLinesToCome=iLineReader->AppendLineIfSpaceNextL();
				}
			else
				{
				TPtrC8 lineData = BufPtr().Mid(0); //Grab a copy of last line, used to check if the end has been reached if only whitespace is present
				TInt lineEval = iLineReader->ReadLineL(lineLen,err);
				moreLinesToCome=(lineEval !=CLineReader::ELineIsCRLFOnly);
				if (lineEval == CLineReader::ELineIsWhiteSpace)
					{
					//White space lines are not really part of the spec, however can be present
					//through transmission errors. For the sake of robustness it is ignored, however if
					//it is present after the last line instead of CRLF then it is a problem. We can check 
					//for the other delimiter mentioned in the spec to see if the last line has been reached
					//by checking for the '=' character. This is not perfect as these are padding characters and
					//will not be present if the length of the data is a multiple of 3 but as the data is not
					//conformant with the spec then this is the best we can do to recover from the error 
					//which is present.

					if ( lineData.LocateReverse('=') != KErrNotFound)
						{
						//Found an '=' so the end has been reached.
						moreLinesToCome = EFalse;
						}
					}
				}
			if (aBinaryData)
				{
				iLargeDataBuf->InsertL(bufLen,BufPtr());
				bufLen+=BufPtr().Length();
				}
			else
				lineLen=BufPtr().Length();
			}//while
		if (!aBinaryData && lineLen==0)
			{		//The bad ending, to the Base64 data, in other situations does not cause a crash
			BufPtr().Append(KVersitLineBreak);
			BufPtr().Append(KVersitTokenCRLF);
			lineLen=BufPtr().Length();
			}
		}//base64
	if (aBinaryData)
		{
		if (!iLargeDataBuf)
			iLargeDataBuf=CBufSeg::NewL(128);
		if (secondLine)
			iLargeDataBuf->InsertL(0,aValue);
		else if (encoding!=Versit::EBase64Encoding)
			iLargeDataBuf->InsertL(0,BufPtr());
		aValue.Set(NULL,0,0);
		}
	else
		{
		if (!secondLine && BufPtr().Length() > 0)
			aValue.Set(&BufPtr()[0],lineLen,lineLen);
		delete iLargeDataBuf;
		iLargeDataBuf=NULL;
		}
	}



//
// Parsing
//


EXPORT_C void CVersitParser::ParseBeginL()
// Parse start of entity (and extract iEntityName)
	{
	ParsePropertyL();
	// Parse until token Begin or EOF is reached.	
	while (iCurrentProperty->NameUid().iUid!=KVersitTokenBeginUid)
		{
		delete iCurrentProperty;
		iCurrentProperty=NULL;

		ParsePropertyL();
		}

	delete iEntityName;
	iEntityName=NULL;
	iEntityName=STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->TakeValueOwnership();
	}

EXPORT_C void CVersitParser::ParseEndL()
// Parse end of entity
	{
	ParseEndL(*iEntityName);
	}
	
void CVersitParser::ParseEndL(HBufC16& aEntityName)
// Parse end of entity
	{
	if (iCurrentProperty->NameUid().iUid!=KVersitTokenEndUid)
		{
		User::Leave(KErrNotFound);
		}

	TBool result = (aEntityName==STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->Value());
	delete iCurrentProperty;
	iCurrentProperty=NULL;
	if	(!result)
		User::Leave(KErrNotFound);
	}

EXPORT_C TBool CVersitParser::ParseEntityL()
//
//	Parse next entity in stream, and add to iArrayOfEntities
//
	{
	TBool entityWasFound = EFalse;
	TInt32 uid=0;
	if (iCurrentProperty)
		uid=iCurrentProperty->NameUid().iUid;
	if (uid==KVersitTokenBeginUid)
		{
		TInt uid = RecognizeEntityName();
		HBufC* entityName=STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->TakeValueOwnership();
		CleanupStack::PushL(entityName);
		CVersitParser* entity = MakeEntityL(uid,entityName);
		CleanupStack::Pop(entityName);
		AddEntityL(entity);
		
		entityWasFound = ETrue;
		}

	// Returns whether or not an entity was found
	return entityWasFound;
	}

EXPORT_C void CVersitParser::ParsePropertiesL()
//Parse all properties in stream
	{
#if defined(_DEBUG)
	TInt propCount = 0;
#endif
	FOREVER
		{
#if defined(_DEBUG)
		propCount++;
#endif
		// For every single property that is parsed, any decoding or character set conversion
		// will use the default character set and encoding type set during the call to this
		// class' constructor.
		// To ensure (after a property is read) that the default is restored, a call to this
		// method is made before each property is read.
		RestoreLineCodingDetailsToDefault();
		TRAPD(err, ParsePropertyL());
		if (VersitUtils::CheckAndIgnoreCustomErrorL(err))
			{
			delete iCurrentProperty;
			iCurrentProperty = NULL;
			}
		else
			{
			if (iCurrentProperty)
				{
				if (VersitUtils::IsBeginOrEnd(iCurrentProperty->NameUid()))
					return;
				DoAddPropertyL(iCurrentProperty);
				if (iObserver && iCurrentProperty->Name()==KVersitTokenVERSION)
					iObserver->VersionSet(this,STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->Value());
				iCurrentProperty=NULL;
				}
			else
				{
				User::Leave(KErrEof);
				}
			}
		}
	}



EXPORT_C void CVersitParser::ConvertAllPropertyDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
/** Converts all date/time property values contained in the current entity's array 
of properties into machine local values.

This conversion is needed because of differences in universal and local times 
due to time zones and daylight savings (seasonal time shifts).

First, all of the date/times are converted to universal time, if they are 
not already:

If daylight saving is in effect then the daylight savings rule (held in the 
aDaylight parameter) will be used to compensate for differences between universal 
and local times due to both time zones and the daylight saving. Otherwise, 
the aIncrement parameter is used to compensate for any difference due to time 
zones alone.

Then, these universal time values are converted into machine local 
times by adding the universal time offset for the machine's locale.

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
	TUid uid;
	uid.iUid = KVersitTimePropertyUid;
	if (iArrayOfProperties)
		{
		CParserTimePropertyValue* timeValue;
		TInt properties = iArrayOfProperties->Count();
		for (TInt ii = 0; ii < properties; ii++)
			{
			CParserPropertyValue* value=(*iArrayOfProperties)[ii]->Value();
			if (value && value->SupportsInterface(uid))
				{
				timeValue = STATIC_CAST(CParserTimePropertyValue*, value);
				timeValue->ConvertAllDateTimesToUTCL(aIncrement, aDaylight);
				}
			}
		TLocale locale;
		TTimeIntervalSeconds localeIncrement(locale.UniversalTimeOffset());
		if ((iFlags&EImportSyncML) && aDaylight && localeIncrement.Int()==-aIncrement.Int())	//If Importing from a SyncML Server and working in same time zone which has a daylight rule
			ConvertAllUTCDateTimesToMachineLocalL(localeIncrement,aDaylight);
		else
			{
			AdjustAllPropertyDateTimesToMachineLocalL();
			}
		}
	}


/*
 * Converts all date/time property values contained in the current
 * array of properties into machine local values.
 @deprecated since 9.1
 */
EXPORT_C void CVersitParser::AdjustAllPropertyDateTimesToMachineLocalL()
	{
	if (iArrayOfProperties)
		{
		TUid uid;
		uid.iUid = KVersitTimePropertyUid;
		TInt properties = iArrayOfProperties->Count();
		CParserTimePropertyValue* timeValue;
		TLocale locale;
		TTimeIntervalSeconds localeIncrement(locale.UniversalTimeOffset());
		for (TInt jj = 0; jj < properties; jj++)
			{
			CParserPropertyValue* value=(*iArrayOfProperties)[jj]->Value();
			if (value && value->SupportsInterface(uid))
				{
				timeValue = STATIC_CAST(CParserTimePropertyValue*, value);
				timeValue->ConvertAllUTCDateTimesToMachineLocalL(localeIncrement);
				}
			}
		}
	}


void CVersitParser::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
	{
	CParserPropertyValue* value;
	CParserTimePropertyValue* timeValue;
	TInt properties=iArrayOfProperties->Count();
	TUid uid;
	uid.iUid = KVersitTimePropertyUid;
	for (TInt jj = 0; jj < properties; jj++)
		{
		value=(*iArrayOfProperties)[jj]->Value();
		if (value && value->SupportsInterface(uid))
			{
			timeValue = STATIC_CAST(CParserTimePropertyValue*, value);
			switch (timeValue->Uid().iUid)
				{
			case KVCalPropertyAlarmUid:
				{
				CVersitAlarm* alarm=STATIC_CAST(CParserPropertyValueAlarm*,timeValue)->Value();
				if (alarm && alarm->iRunTime)
					ConvertUTCDateTimeToMachineLocal(alarm->iRunTime,aIncrement,aDaylight);
				}
				break;
			case KVersitPropertyDateTimeUid:
				{
				TVersitDateTime* dateTime=STATIC_CAST(CParserPropertyValueDateTime*,timeValue)->Value();
				if (dateTime)
					ConvertUTCDateTimeToMachineLocal(dateTime,aIncrement,aDaylight);
				}
				break;
			case KVersitPropertyMultiDateTimeUid:
				{
				CArrayPtr<TVersitDateTime>* values=STATIC_CAST(CParserPropertyValueMultiDateTime*,timeValue)->Value();
				if (values)
					{
					TInt count=values->Count();
					for (TInt ii=0;ii<count; ii++)
						ConvertUTCDateTimeToMachineLocal((*values)[ii],aIncrement,aDaylight);
					}
				}
				break;
			default:
				timeValue->ConvertAllUTCDateTimesToMachineLocalL(aIncrement);
				}
			}
		}
	}

void CVersitParser::ConvertUTCDateTimeToMachineLocal(TVersitDateTime* aDateTime,const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
	{
	if (aDateTime && aDateTime->iRelativeTime==TVersitDateTime::EIsUTC)
		{
		CParserTimePropertyValue::ConvertDateTime(aDateTime->iDateTime,aIncrement,aDaylight,EFalse);
		aDateTime->iRelativeTime=TVersitDateTime::EIsMachineLocal;
		}
	}


//
// Recognition
//

EXPORT_C TBool CVersitParser::IsValidParameterValue(TInt& aPos,const TDesC& aParamValue)
/** Tests whether a property parameter name or value is valid.

If the string aParamValue contains any punctuation characters, the string 
is invalid. Otherwise, it is valid. Punctuation characters are defined as 
any of the following:- 

[] (left or right square bracket)

= (equals sign)

: (colon)

; (semi colon)

. (dot)

, (comma)

@param aPos On return, contains the character position of the first invalid 
character in the property parameter name or value. 
@param aParamValue The property parameter name or value to test. 
@return ETrue if valid, EFalse if invalid. */
	{
	TInt length = aParamValue.Length();
	for (aPos = 0; aPos < length; aPos++)
		{
		if (IsPunctuationToken(aParamValue[aPos]))
			return EFalse;
		}
	return ETrue;
	}

EXPORT_C CArrayPtr<CParserParam>* CVersitParser::GetPropertyParamsLC(TPtr8 aParams)
	{
	CArrayPtr<CParserParam>* arrayOfParams=new(ELeave)CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
	TUint8* currentChar=&aParams[0];
	TUint8* dest;
	TUint8* afterLastChar=currentChar+aParams.Length();
	TUint8* startField;
	TBool escaping;
	TInt len;
	while (currentChar<=afterLastChar)
		{
		startField=currentChar;
		escaping=EFalse;
		while (currentChar<afterLastChar && *currentChar!=KVersitTokenSemiColonVal && *currentChar!=KVersitTokenBackslashVal)
			++currentChar;
		dest=currentChar;
		if (*currentChar==KVersitTokenBackslashVal && currentChar<afterLastChar)
			{
			do	{
				if (!escaping)
					{
					if (*currentChar==KVersitTokenSemiColonVal)
						break;
					escaping=(*currentChar==KVersitTokenBackslashVal);
					if (escaping)
						{
						++currentChar;
						continue;
						}
					}
				else
					{
					if (*currentChar!=KVersitTokenBackslashVal && *currentChar!=KVersitTokenSemiColonVal)
						*dest++=KVersitTokenBackslashVal;
					escaping=EFalse;
					}
				*dest++=*currentChar++;
				} while(currentChar<afterLastChar);
			if (escaping)
				*dest++=KVersitTokenBackslashVal;
			}
		len=dest-startField;
		if (len>0)
			ParseParamL(arrayOfParams,TPtr8(startField,len,len));
		++currentChar;
		}
	return arrayOfParams;
	}

EXPORT_C void CVersitParser::ParseParamL(CArrayPtr<CParserParam>* aArray,TPtr8 aParam)
	{
	TPtr8 paramName(aParam);
	TPtr8 paramValue(NULL,0);
	TInt equalsPos=aParam.Find(KVersitTokenEquals);
	if (equalsPos>KErrNotFound)
		{
		paramName.Set(&aParam[0],equalsPos,equalsPos);
		++equalsPos;
		TInt valLen=aParam.Length()-equalsPos;
			if (valLen>0)
				{
				paramValue.Set(&aParam[equalsPos],valLen,valLen);
				VersitUtils::StripWhiteSpace(paramValue);
			}
		}
	VersitUtils::StripWhiteSpace(paramName);
	CParserParam* newParam = CParserParam::NewL(paramName, paramValue);
	CleanupStack::PushL(newParam);
	aArray->AppendL(newParam);
	CleanupStack::Pop(newParam);
	}

EXPORT_C void CVersitParser::AnalysesEncodingCharset(CArrayPtr<CParserParam>* aArrayOfParams)
//
// This method checks each parameter in turn to establish if any of the parameters (should some
// exist) contain references to the CHARSET parameter or the ENCODING parameter.
//
// Should references exist to CHARSET then a character set override is applied to the current
// property value during the decoding process.
//
// Note that it is  also valid syntax to simply indicate that the property value is encoded
// using QUOTED-PRINTABLE  simply by specifying QUOTED-PRINTABLE without following the normal
// PARAMETER_NAME=VALUE syntax (i.e the parameter might just consist of the PARAMETER_NAME
// part (in which case VALUE will be  null and PARAMETER_NAME will be QUOTED-PRINTABLE etc).
// 
// Method could leave while creating a UID 
//
	{
	CParserParam* parameter;
	TPtrC8 pParameterName;
	TPtrC8 pParameterValue;
	TInt noOfParams = (aArrayOfParams)? aArrayOfParams->Count() : 0;
	TInt tokenUidValue = 0;

	//
	// Check each parameter for coding related tags, such as CHARSET, ENCODING, etc and
	// set up state ready for the decoding of the parameter value
	//
	for(TInt ii = 0; ii < noOfParams; ii++)
		{
		parameter = aArrayOfParams->At(ii);
		pParameterName.Set(parameter->Name());
		pParameterValue.Set(parameter->Value());

		// Attempt to recognize this property parameter
		tokenUidValue = RecognizeToken(pParameterName).iUid;

		// The parameter NAME is CHARSET
		if	(tokenUidValue == KVersitParamCharsetUid)
			{
			// Charset identifier found, so establish the encoding type (e.g UTF8, ISO8859-X etc).
			TUint charSetUid=KCharacterSetIdentifierAscii;
			charSetUid=UnicodeUtils().ConvertStandardNameL(pParameterValue); // Method could leave while creating a UID 
			if (charSetUid!=0)
				{
				SetLineCharacterSetId(charSetUid);
				iFlags|=ECharSetIdentified;
				}

			// The CHARSET parameter of this property is no longer required so remove it from the array.
			delete parameter;
			aArrayOfParams->Delete(ii);
			--ii;
			--noOfParams;
			}
		// The parameter NAME is ENCODING
		else if (tokenUidValue == KVersitParamEncodingUid)
			{
			const TUint conversionUid = RecognizeToken(pParameterValue).iUid;
			SetLineEncoding(conversionUid);
			}
		// The parameter NAME is QUOTED-PRINTABLE
		else if (tokenUidValue == KVersitParamEncodingQuotedPrintableUid)
			{
			SetLineEncoding(Versit::EQuotedPrintableEncoding);
			}
		// The parameter NAME is BASE64
		else if (tokenUidValue == KVersitParamEncodingBase64Uid)
			{
			SetLineEncoding(Versit::EBase64Encoding);
			}
		} // end FOR
	}

HBufC* CVersitParser::ConvertToUnicodeL(const TDesC8& aValue)
	{
	if (!(iFlags&ECharSetIdentified))
		{
		if (iFlags&EUseAutoDetection && !VersitUtils::IsBeginOrEnd(iCurrentProperty->NameUid())
																	&& iCurrentProperty->NameUid().iUid!=KVersitTokenVersionUid)
			SetLineCharacterSetId(UnicodeUtils().AutoDetectCharSetL(aValue,iAutoDetectCharSets));
		else
			SetLineCharacterSet(Versit::EUSAsciiCharSet);
		}
	if	(LineCharSet() != Versit::EUSAsciiCharSet)
		{
		HBufC* value = HBufC::NewLC(aValue.Size());
		TPtr ptr(value->Des());

		if	(aValue.Length())
			{
			User::LeaveIfError(ConvertToUnicodeFromISOL(ptr, aValue, LineCharSetId()));
			__ASSERT_DEBUG(ptr.Length(), User::Invariant());
			}

		CleanupStack::Pop(value);
		return value;
		}
	else
		return UnicodeUtils().WidenL(aValue);
	}

EXPORT_C HBufC* CVersitParser::DecodePropertyValueL(const TDesC8& aValue)
//
// This method is used to decode the property value given that any
// parameters to the property have been established.
//
	{
	// If this property has an overridden encoding type, or the default encoding is not ENoEncoding
	// then do the decode here... otherwise leave it up to MakeXXXX instead.
	if	(LineEncoding() != Versit::ENoEncoding)
		{
		const TUid propertyEncodingUid = { LineEncodingId() };
		TRAPD(err,DecodePropertyValueL(aValue,propertyEncodingUid));
		if	(err != KErrNotSupported)
			{
			User::LeaveIfError(err);
			if (iLargeDataBuf)
				return NULL;
			return ConvertToUnicodeL(*iDecodedValue);
			}
		}
	if (iLargeDataBuf)
		return NULL;
	return ConvertToUnicodeL(aValue);
	}

EXPORT_C void CVersitParser::DecodePropertyValueL(const TDesC8& aValue,const TUid& aEncodingUid)
	{
	CCnaConverterList* convList=CCnaConverterList::NewLC();
	CConverterBase* conv = convList->NewConverterL(aEncodingUid);
	if (!conv)
		User::Leave(KErrNotSupported);
	CleanupStack::PushL(conv);

	CBufSeg* destBuf=NULL;		//Initialise to stop warning "local variable may be used without having been initialized"
	RBufReadStream bufRead;
	RBufWriteStream bufWrite;
	RDesReadStream desRead;
	RDesWriteStream desWrite;
	RReadStream* readStream;
	RWriteStream* writeStream;
	if (iLargeDataBuf)
		{
		destBuf=CBufSeg::NewL(128);
		CleanupStack::PushL(destBuf);
		bufRead.Open(*iLargeDataBuf);
		bufWrite.Open(*destBuf);
		readStream=&bufRead;
		writeStream=&bufWrite;
		}
	else
		{
		if (!iDecodedValue)
			iDecodedValue=HBufC8::NewL(aValue.Length());
		else
			{
			TPtr8 value=iDecodedValue->Des();
			if(value.MaxLength()<aValue.Length())
				iDecodedValue=iDecodedValue->ReAllocL(aValue.Length());
			}
		desRead.Open(aValue);
		TPtr8 ptr=iDecodedValue->Des();		//To stop warning: nonstandard extension used : 'argument' : conversion from 'class TPtr8' to 'class TDes8 &'
		desWrite.Open(ptr);
		readStream=&desRead;
		writeStream=&desWrite;
		}
	//
	conv->ConvertObjectL(*readStream,*writeStream);
	//
	writeStream->CommitL();
	readStream->Close();
	writeStream->Close();
	//
	if (iLargeDataBuf)
		{
		CleanupStack::Pop(destBuf);
		delete iLargeDataBuf;
		iLargeDataBuf=destBuf;
		}
	CleanupStack::PopAndDestroy(2, convList); // conv, convList
	}


EXPORT_C void CVersitParser::AppendBeginL()
// Write beginning of entity to stream
	{
	iWriteStream->WriteL(KVersitTokenBEGIN);
	AppendEntityNameL();
	}

EXPORT_C void CVersitParser::AppendEndL()
// Write end of entity to stream
	{
	iWriteStream->WriteL(KVersitTokenEND);
	AppendEntityNameL();
	}

void CVersitParser::AppendEntityNameL()
	{
	iWriteStream->WriteL(KVersitTokenColon);
	if (iEntityName)
		{
		UnicodeUtils().SetCurrentCharSetL(KCharacterSetIdentifierCodePage1252);
		HBufC8* narrow = UnicodeUtils().NarrowLC(*iEntityName);
		iWriteStream->WriteL(*narrow);
		CleanupStack::PopAndDestroy(narrow);
		}
	iWriteStream->WriteL(KVersitTokenCRLF);
	}

EXPORT_C CVersitParser* CVersitParser::MakeEntityL(TInt /*aEntityUid*/,HBufC* /*aEntityName*/)
// Make an entity from iReadStream
	{
	User::Leave(KErrNotSupported); //entities not supported
	return NULL;
	}

EXPORT_C CParserPropertyValue* CVersitParser::MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue)
// Every property value has a specific property UID type which identifies the type of
// container object the property value (returned from this method) is derived from.
	{
	if(!aValue && (aPropertyUid.iUid != KVersitPropertyBinaryUid || !iLargeDataBuf))
		{
		return NULL;
		}

	CParserPropertyValue* value = NULL;

	switch(aPropertyUid.iUid)
		{
		case KVersitPropertyCDesCArrayUid:
			{
			CDesCArray* array=MakePropertyValueCDesCArrayL(aValue->Des());
			CleanupStack::PushL(array);
			value=new(ELeave)CParserPropertyValueCDesCArray(array);
			CleanupStack::Pop(array);
			break;
			}
		case KVersitPropertyTimeZoneUid:
			{
			TPtr16 ptr=aValue->Des();
			VersitUtils::StripWhiteSpace(ptr);
			TTimeIntervalSeconds timeZone=DecodeTimeZoneL(ptr);
			value=new(ELeave)CParserPropertyValueTimeZone(timeZone);
			break;
			}
		case KVersitPropertyDaylightUid:
			{
			CVersitDaylight* daylight=MakePropertyValueDaylightL(aValue->Des());
			CleanupStack::PushL(daylight);
			value=new(ELeave)CParserPropertyValueDaylight(daylight);
			CleanupStack::Pop(daylight);
			break;
			}
		case KVersitPropertyDateTimeUid:
			{
			TPtr16 ptr=aValue->Des();
			if (ptr.Length()<=KVersitDatePropertySymbianConnectMessageMaximumLenght)
				{
				//date is not valid, exception implemented to satisfy symbian connect protocol
				value=MakeDefaultPropertyValueL(aValue);
				}
			else
				{
				TVersitDateTime* dateTime=DecodeDateTimeL(ptr);
				CleanupStack::PushL(dateTime);
				value=new(ELeave)CParserPropertyValueDateTime(dateTime);
				CleanupStack::Pop(dateTime);
				}
			break;
			}
		case KVersitPropertyDateUid:
			{
			TPtr16 ptr=aValue->Des();
			if (ptr.Length()<=KVersitDatePropertySymbianConnectMessageMaximumLenght)
				{
				//date is not valid, exception implemented to satisfy symbian connect protocol
				value=MakeDefaultPropertyValueL(aValue);
				}
			else
				{	
				TVersitDateTime* dateTime=DecodeDateTimeL(ptr);
				CleanupStack::PushL(dateTime);
				value=new(ELeave)CParserPropertyValueDate(dateTime);
				CleanupStack::Pop(dateTime);
				}
			break;
			}
		case KVersitPropertyMultiDateTimeUid:
			{
			CArrayPtr<TVersitDateTime>*	multiDateTime=MakePropertyValueMultiDateTimeL(aValue->Des());
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfDateTimes,multiDateTime));
			value=new(ELeave)CParserPropertyValueMultiDateTime(multiDateTime);
			CleanupStack::Pop(multiDateTime);
			break;
			}
		case KVersitPropertyIntUid:
			{
			TInt number;
			Val(*aValue, number);
			value=new(ELeave)CParserPropertyValueInt(number);
			break;
			}
		case KVersitPropertyBinaryUid:
			value=new(ELeave) CParserPropertyValueBinary(*iLargeDataBuf);
			iLargeDataBuf=NULL;
			break;
		default://KVersitPropertyHBufCUid
			{
			value=MakeDefaultPropertyValueL(aValue);
			}
		}
	return value;
	}

CParserPropertyValueHBufC* CVersitParser::MakeDefaultPropertyValueL(HBufC16*& aValue)
/**
 * Instantiates and gives ownership of a CParserPropertyValueHBufC object.
 */
	{
	if(!aValue && !iLargeDataBuf)
		return NULL;

	VersitUtils::RemoveEscapeChars(*aValue,LineCharSetId());
	CParserPropertyValueHBufC* returnedValue=new(ELeave) CParserPropertyValueHBufC(aValue);
	aValue=NULL;
	return returnedValue;
	}


EXPORT_C CDesCArray* CVersitParser::MakePropertyValueCDesCArrayL(TPtr16 aStringValue)
/** Parses a compound property value string. 

The sub-values found are appended to an array, after removal of escape characters. 
The array is returned, and ownership is transferred to the caller.

@param aStringValue Compound property value string.
@return Array of property values found in the string. */
	{
    CDesCArray* arrayOfValues = VersitUtils::ParseForArrayPropertiesL(aStringValue, LineCharSetId());
	return arrayOfValues;
	}

EXPORT_C TBool CVersitParser::FindFirstField(TPtr16& aField,TPtr16& aRemaining, TBool aTrimSpace)
	{
	TInt len=aRemaining.Length();
	TInt posSemicolon=aRemaining.Locate(KVersitTokenSemiColonVal);
	TInt startRest=posSemicolon+1;
	TBool ret=(posSemicolon==KErrNotFound);
	if (ret)
		{
		if (len==0)
			{
			aField.Set(NULL,0,0);
			return ETrue;
			}
		posSemicolon=len;
		startRest=len;
		}
	aField.Set(&aRemaining[0],posSemicolon,posSemicolon);
	len-=startRest;
	if (len==0)
		startRest=0;
	aRemaining.Set(&aRemaining[startRest],len,len);
	if(aTrimSpace)
		VersitUtils::StripWhiteSpace(aField);
	return ret;
	}
	
/** Sets up a pointer to the remaining field. Sets the original remaining field pointers length to 0. 
@param aField on return points to the data and length that aRemaining originally pointed to. 
@param aRemaining as input references a location that the descriptor is to represent. On return length is set to 0.
*/
EXPORT_C void CVersitParser::FindRemainingField(TPtr16& aField,TPtr16& aRemaining)
	{
	TInt len=aRemaining.Length();
	aField.Set(&aRemaining[0],len,len);
	aRemaining.Set(&aRemaining[0], 0, 0);
	}

EXPORT_C CVersitDaylight* CVersitParser::MakePropertyValueDaylightL(TPtr16 aDaylightValue)
	{
	if(aDaylightValue.MatchF(KVersitVarTokenFALSE)>KErrNotFound)
		return CVersitDaylight::NewL(EFalse, NULL, NULL, NULL, KVersitTokenEmpty, KVersitTokenEmpty);

	TPtr16 field(NULL,0);
	FindFirstField(field,aDaylightValue);
	if(field.CompareC(KVersitVarTokenTRUE))
		return NULL;

	FindFirstField(field,aDaylightValue);
	TTimeIntervalSeconds offset=0;
	if(field.Length()>0)
		offset = DecodeTimeZoneL(field);

	FindFirstField(field,aDaylightValue);
	TVersitDateTime* startTime = NULL;
	if(field.Length()>0)
		startTime = DecodeDateTimeL(field);
	CleanupStack::PushL(startTime);

	FindFirstField(field,aDaylightValue);
	TVersitDateTime* endTime = NULL;
	if(field.Length()>0)
		endTime = DecodeDateTimeL(field);
	CleanupStack::PushL(endTime);

	FindFirstField(field,aDaylightValue);
	TPtr16 edt(NULL,0);
	FindFirstField(edt,aDaylightValue);

	if(!startTime || !endTime)
		User::Leave(KVersitErrBadDateTime);
			
	CVersitDaylight* daylight = CVersitDaylight::NewL(ETrue, offset, startTime, endTime, field, edt);
	CleanupStack::Pop(2,startTime);
	return (daylight);
	}

EXPORT_C CArrayPtr<TVersitDateTime>* CVersitParser::MakePropertyValueMultiDateTimeL(TPtr16 aDateTimeGroup)
// Make a property from aDateTimeGroup, with a CArrayPtr<TDateTime> value
	{
	VersitUtils::StripWhiteSpace(aDateTimeGroup);
	if(aDateTimeGroup.Length()==0)
		return NULL;
	CArrayPtr<TVersitDateTime>* arrayOfDateTimes=new(ELeave)CArrayPtrFlat<TVersitDateTime>(5);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfDateTimes,arrayOfDateTimes));
	TPtr16 field(NULL,0);
	do
		{
		FindFirstField(field,aDateTimeGroup);
		if(field.Length()>0)
			{
			TVersitDateTime* dateTime = DecodeDateTimeL(field);
			CleanupStack::PushL(dateTime);
			arrayOfDateTimes->AppendL(dateTime);
			CleanupStack::Pop(dateTime);
			}
		}while (aDateTimeGroup.Length()!=0);

	CleanupStack::Pop(arrayOfDateTimes); //arrayOfDateTimes
	return arrayOfDateTimes;
	}

EXPORT_C TInt CVersitParser::Val(const TDesC& aString, TInt& aNumber)
// static
/** Converts a string into an integer, for example, it converts "438" to 438.

@param aString The string to be converted. Every character contained in 
the string needs to be number presentable, i.e. between '0' and '9'.
@param aNumber On return, the integer corresponding to aString. 
@return KErrNone if the number has been converted from the string; 
KVersitErrNumberExpected otherwise. */
	{
	aNumber = 0;
	TInt length=aString.Length();
	for(TInt i=0;i<length;i++)
		{
		if(aString[i]<'0'||aString[i]>'9')
			return(KVersitErrNumberExpected);
		aNumber = aNumber * 10 + aString[i] - '0';
		}
	return KErrNone;
	}


EXPORT_C TVersitDateTime* CVersitParser::DecodeDateTimeL(TDes& aToken) const
// Extracts the date and time portion from aToken and returns it as a
// TVersitDateTime (Format YYYY(:)MM(:)DD(:)(THH(:)MM(:)SS(:)(Z))
	{
	const TInt KSymbianOSDefaultMidnightValue = 0;
	const TInt KVersitAlternativeMidnightValue = 24;

	aToken.TrimAll();
	TInt length = aToken.Length();
	if (length==0)
		User::Leave(KVersitErrBadDateTime);
	TInt position = 0;
	if (length < 8)
		User::Leave(KVersitErrBadDateTime);
	TVersitDateTime::TRelativeTime relativeTime = TVersitDateTime::EIsVCardLocal;
	TInt year;
	User::LeaveIfError(Val(aToken.Left(4), year));
	position += 4;
	if(aToken[position] == KVersitTokenMinusVal)
		position++;
	TInt month;
	User::LeaveIfError(Val(aToken.Mid(position, 2), month));
	position += 2;
	if(aToken[position] == KVersitTokenMinusVal)
		position++;
	if (length < (position + 2))
		User::Leave(KVersitErrBadDateTime);
	TInt day;
	User::LeaveIfError(Val(aToken.Mid(position, 2), day));
	position += 2;
	//
	// Parse time aspect
	//
	TBool adjustDayToNextAvailable = EFalse;
	TInt hour = 0;
	TInt minute = 0;
	TInt second = 0;

	//	position is now sitting left of the "T" in the date string YYYY(-)MM(-)DD(-)THH(:)MM(:SS)
	if (length >= (position + 5))
		{
		TBool parseSeconds = EFalse;
		
		// Time value can be in "HHMM" or "HHMMSS" format.
		if(length >= (position + 7))
			parseSeconds = ETrue;

		if(aToken[position++] != KVersitTimePeriodTimeVal)
			User::Leave(KVersitErrBadDateTime);
			
		User::LeaveIfError(Val(aToken.Mid(position, 2), hour));
		// If the hour value is "24" then, this is deemed to be at the end
		// of the specified day. This can be thought of as synonymous with the
 		// start of the next day.
 		if	(hour == KVersitAlternativeMidnightValue)
 			{
 			adjustDayToNextAvailable = ETrue;
 			hour = KSymbianOSDefaultMidnightValue;
 			}

		position += 2;
		if(aToken[position] == KVersitTokenColonVal)
			position++;
		
		if(length < position+2)
			User::Leave(KVersitErrBadDateTime);
		
		User::LeaveIfError(Val(aToken.Mid(position, 2), minute));
		position += 2;

		if(parseSeconds)
			{
			if(aToken[position] == KVersitTokenColonVal)
				position++;
			if (length < (position + 2))
				User::Leave(KVersitErrBadDateTime);
			User::LeaveIfError(Val(aToken.Mid(position, 2), second));
			position += 2;
			}

		if ((length > position) && (aToken[position] == KVersitTokenUniversalTimeVal))
			relativeTime = TVersitDateTime::EIsUTC;
		}
	TDateTime date;
	const TInt error = date.Set(year, (TMonth)(month - 1), day - 1, hour, minute, second, 0);
	if(error!=KErrNone)
 		User::LeaveIfError(KVersitErrBadDateTime);

 	// In the instance whereby we need to treat 24 as being the start of the next
 	// day, we should increment the day value by one.
 	if	(adjustDayToNextAvailable)
 		{
 		const TInt KVersitSingleDay = 1;
 		//
 		TTime time(date);
 		time += TTimeIntervalDays(KVersitSingleDay);
 		date = time.DateTime();
 		}
	return new(ELeave) TVersitDateTime(date, relativeTime);
	}

EXPORT_C TTimeIntervalSeconds CVersitParser::DecodeTimeZoneL(const TDesC& aToken) const
// Extracts the date and time portion from aToken and returns it as a
// TVersitDateTime Format: (-)(+)HH((:)MM)
	{
	TInt length = aToken.Length();
	if (length < 1)
		User::Leave(KVersitErrBadTimeZone);
	TInt position = 0;
	TInt sign = 1;
	TInt hour = 0,minute = 0;
	if(aToken[position] == KVersitTokenMinusVal)
		{
		position++;
		sign = -1;
		}
	else if(aToken[position] == KVersitTokenPlusVal)
		position++;
	if(length-position<1)
		User::Leave(KVersitErrBadTimeZone);
	if((length-position)==1 || aToken.Mid(position+1,1)==KVersitTokenColonUnicode)
		{
		User::LeaveIfError(Val(aToken.Mid(position, 1), hour));
		position += 1;
		}
	else
		{
		User::LeaveIfError(Val(aToken.Mid(position, 2), hour));
		position += 2;
		}

	if (length >= (position + 1))
		{
		if(aToken[position] == KVersitTokenColonVal)
			position++;
		if(length-1==position)
			User::LeaveIfError(Val(aToken.Mid(position, 1), minute));
		else if(length-1>position)
			User::LeaveIfError(Val(aToken.Mid(position, 2), minute));
		}
	return TTimeIntervalSeconds(sign * (hour * 3600 + minute * 60));
	}

EXPORT_C TTime* CVersitParser::DecodeTimePeriodL(const TDesC& aToken) const
// Extracts the date and time portion from aToken and returns it as a
// TDateTime time period
	{
	TInt length = aToken.Length();
	TInt ii = 0;
	if (!length || (aToken[ii++] != KVersitTimePeriodBeginVal))
		User::Leave(KVersitErrBadTimePeriod);
	TInt number = 0;
	TInt year = 0;
	TInt month = 0;
	TInt week = 0;
	TInt day = 0;
	TInt hour = 0;
	TInt minute = 0;
	TInt second = 0;
	TBool isTimeOfDay = EFalse;
	while (ii < length)
		{
		TChar tokenChar = aToken[ii];
		if ((tokenChar >= '0') && (tokenChar <= '9'))
			{
			TInt numChars;
			number = GetNumberL(aToken.Right(length - ii),numChars);
			ii+=numChars;
			}
		else
			{
			if (isTimeOfDay)
				{
				if (tokenChar == KVersitTimePeriodHourVal)
					hour = number;
				else if (tokenChar == KVersitTimePeriodMinuteVal)
					minute = number;
				else if (tokenChar == KVersitTimePeriodSecondVal)
					second = number;
				}
			else
				{
				if (tokenChar == KVersitTimePeriodTimeVal)
					isTimeOfDay = ETrue;
				else if (tokenChar == KVersitTimePeriodYearVal)
					year = number;
				else if (tokenChar == KVersitTimePeriodMonthVal)
					month = number;
				else if (tokenChar == KVersitTimePeriodWeekVal)
					week = number;
				else if (tokenChar == KVersitTimePeriodDayVal)
					day = number;
				}
			ii++;
			number = 0;
			}
		}
	//TBuf<KVersitDefaultBufferSize> time;
	//_LIT(formatString, "%04d%02d%02d:%02d%02d%02d.");
	//time.Format(formatString, year, month, day, hour, minute, second);
	TDateTime dateTime(year,REINTERPRET_CAST(TMonth&,month),day,hour,minute,second,0);
	//TTime* timeVal = new(ELeave) TTime(time);
	TTime* timeVal = new(ELeave) TTime(dateTime);
	*timeVal += TTimeIntervalDays(week * 7);
	return timeVal;
	}

EXPORT_C TInt CVersitParser::GetNumberL(const TDesC& aToken,TInt& aNumChars) const
// Converts aToken to an integer
	{
	TInt length = aToken.Length();
	TPtrC numBuf(aToken);
	TInt num;
	aNumChars = 0;
	while ((aNumChars < length) && (aToken[aNumChars] >= '0') && (aToken[aNumChars] <= '9'))
		++aNumChars;
	numBuf.Set(aToken.Left(aNumChars));
	User::LeaveIfError(Val(numBuf, num));
	return num;
	}

EXPORT_C TUid CVersitParser::RecognizeToken(const TDesC8& aToken) const
/** Returns a UID for the specified token. 

This function only recognizes generic Versit tokens. For example, if aToken 
contains the property name KVersitTokenBEGIN, the function returns KVersitTokenBeginUid. 
More specific recognition should occur in derived classes which implement 
this function, using this as the base recogniser.

@param aToken The token to be recognized.
@return A defined UID value if the token has been recognized; KVersitTokenUnknownUid 
otherwise. */
	{
	TUid uid = TUid::Uid(KVersitTokenUnknownUid);
	TChar firstChar(aToken.Ptr()[0]);
	firstChar=firstChar.GetUpperCase();
	switch (firstChar)
		{
	case 'B':
		if (!aToken.CompareF(KVersitTokenBEGIN))
			uid.iUid = KVersitTokenBeginUid;
		else if (!aToken.CompareF(KVersitTokenBASE64))
			uid.iUid = KVersitParamEncodingBase64Uid;
		break;
	case 'C':
		if (!aToken.CompareF(KVersitTokenCHARSET))
			uid.iUid = KVersitParamCharsetUid;
		break;
	case 'E':
		if (!aToken.CompareF(KVersitTokenEND))
			uid.iUid = KVersitTokenEndUid;
		else if (!aToken.CompareF(KVersitTokenENCODING))
			uid.iUid = KVersitParamEncodingUid;
		break;
	case 'Q':
		if (!aToken.CompareF(KVersitTokenQUOTEDPRINTABLE))
			uid.iUid = KVersitParamEncodingQuotedPrintableUid;
		break;
	default:
		break;
		}
	return uid;
	}

EXPORT_C TInt CVersitParser::RecognizeEntityName() const
// From CVersitParser
/** Tests the current entity to see if it is a vEvent or vTodo.

This function is virtual. Actual testing only occurs in derived classes which 
implement this function.

@return Zero. Derived classes' implementations would return a value that indicates 
the current entity's type. */
	{
	return 0;
	}

EXPORT_C void CVersitParser::ResetAndDestroyArrayOfParams(TAny* aObject)
/** Destroys an array of parameters.

@param aObject Pointer to the array of parameters to be destroyed. */
	{
	CArrayPtr<CParserParam>* array=REINTERPRET_CAST(CArrayPtr<CParserParam>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

EXPORT_C void CVersitParser::ResetAndDestroyArrayOfProperties(TAny* aObject)
/** Destroys an array of properties.

@param aObject Pointer to the array of properties to be destroyed. */
	{
	CArrayPtr<CParserProperty>* array=REINTERPRET_CAST(CArrayPtr<CParserProperty>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

EXPORT_C void CVersitParser::ResetAndDestroyArrayOfEntities(TAny* aObject)
/** Destroys an array of entities. 

@param aObject Pointer to the array of entities to be destroyed. */
	{
	CArrayPtr<CVersitParser>* array=REINTERPRET_CAST(CArrayPtr<CVersitParser>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

EXPORT_C void CVersitParser::ResetAndDestroyArrayOfDateTimes(TAny* aObject)
/** Destroys an array of Versit dates and times.

@param aObject Pointer to the array of Versit dates and times to be destroyed. */
	{
	CArrayPtr<TVersitDateTime>* array=REINTERPRET_CAST(CArrayPtr<TVersitDateTime>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

TBool CVersitParser::IsPunctuationToken(TUint aChar)		//static function
	{
	return (aChar == KVersitTokenColonVal ||
			aChar == KVersitTokenSemiColonVal ||
			aChar == KVersitTokenEqualsVal ||
			aChar == KVersitTokenPeriodVal ||
			aChar == KVersitTokenCommaVal ||
			aChar == KVersitTokenLSquareBracketVal ||
			aChar == KVersitTokenRSquareBracketVal);
	}

EXPORT_C TInt CVersitParser::ConvertFromUnicodeToISOL(TDes8& aIso, const TDesC16& aUnicode, CCnvCharacterSetConverter* aConverter)
/** Converts text in the Unicode character set (UCS-2) into a 
non-unicode (International Standards Organisation) character set.

Which ISO character set the string is converted to is determined by 
the value of the character set identifier in the aConverter parameter.

@param aIso On return, the converted text string in the specified ISO 
character set. 
@param aUnicode The Unicode text string to be converted.
@param aConverter The character set converter.
@return If no error occurs, this is the number of unconverted characters 
left at the end of the input descriptor. This is zero if the whole string 
is converted, but may be greater than zero (e.g. because the output 
descriptor is not long enough to hold all the text). Otherwise, one of 
the error values defined in TError in the CCnvCharacterSetConverter class, or 
KErrNotFound if aConverter does not point to a valid character 
set converter object. */
	{
	if	(aConverter)
		{
		// Do the conversion
		aConverter->SetReplacementForUnconvertibleUnicodeCharactersL(KReplacementChars);
		return aConverter->ConvertFromUnicode(aIso, aUnicode);
		}
	return KErrNotFound;
	}

EXPORT_C TInt CVersitParser::ConvertToUnicodeFromISOL(TDes16& aUnicode, const TDesC8& aIso,TUint aCharacterSet)
/** Converts text in a non-Unicode character set into Unicode (UCS-2).

@param aUnicode On return, the converted text string in the Unicode character set.
@param aIso  The non-Unicode text string to be converted.
@param aCharacterSet The non-Unicode character set in which aIso is encoded.
@return If no error occurs, the number of unconverted bytes left at the end of 
the input descriptor. This is zero if the whole string is converted; but it may 
be greater than zero, e.g. because the output descriptor is not long enough to 
hold all the text. Otherwise, one of the error values defined in TError in the 
CCnvCharacterSetConverter class, or KErrNotFound if aCharacterSet cannot be 
converted. */
	{
	CCnvCharacterSetConverter::TAvailability avail=UnicodeUtils().SetCurrentCharSetL(aCharacterSet);
	if	(avail == CCnvCharacterSetConverter::EAvailable)
		{
		// Do the conversion
		CCnvCharacterSetConverter& converter=UnicodeUtils().CharacterSetConverter();
		TInt ignore = 0;
		converter.SetReplacementForUnconvertibleUnicodeCharactersL(KReplacementChars);
		return converter.ConvertToUnicode(aUnicode, aIso, ignore);
		}
	return KErrNotFound;
	}

EXPORT_C void CVersitParser::SetDefaultEncoding(Versit::TVersitEncoding aEncoding)
/** Sets the parser's default encoding to aEncoding.

@param aEncoding An encoding. Cannot be 8-bit.
@panic Versit-Parser 7 aEncoding is Versit::EEightBitEncoding. */
	{
	// 8-bit is reserved for versit - it uses it when it needs to, but it's not
	// settable by 3rd parties.
	__ASSERT_ALWAYS(aEncoding!=Versit::EEightBitEncoding, Panic(EVersitPanicCannotSetEightBitEncoding));
	//
	iDefaultCodingDetails.iEncoding		= aEncoding;
	iDefaultCodingDetails.iEncodingUid	= MapVersitEncodingToConArcUid(aEncoding);
	}

EXPORT_C void CVersitParser::SetDefaultCharSet(Versit::TVersitCharSet aCharSet)
/** Sets the default character set or transformation format. 

This may be used to represent property values in Versit objects.
	
@param aCharSet The default character set. */
	{
	iDefaultCodingDetails.iCharSet		= aCharSet;
	iDefaultCodingDetails.iCharSetUid	= MapVersitCharsetToCharConvCharset(aCharSet);
	}

EXPORT_C void CVersitParser::SetDefaultCharSetId(TUint aCharSetId)
/** Sets the default character set or transformation format.

This may be used to represent property values in Versit objects. 

@param aCharSetId The UID for the character set. These are defined in charconv.h. */
	{
	iDefaultCodingDetails.iCharSet		= VersitUtils::CharSet(aCharSetId);
	iDefaultCodingDetails.iCharSetUid	= aCharSetId;
	}

EXPORT_C void CVersitParser::RestoreLineCodingDetailsToDefault()
	{
	// Set the [en|de]coding details for the current line back to the default values specified
	SetLineCharsetDetailsToDefault();
	SetLineEncodingDetailsToDefault();
	}

EXPORT_C void CVersitParser::SetLineEncoding(Versit::TVersitEncoding aLineEncoding)
	{
	iCurrentPropertyCodingDetails.iEncoding		= aLineEncoding;
	iCurrentPropertyCodingDetails.iEncodingUid	= MapVersitEncodingToConArcUid(aLineEncoding);
	}

EXPORT_C void CVersitParser::SetLineEncoding(TUint aVersitEncodingUid)
	{
	switch (aVersitEncodingUid)
		{
		case KQuotedPrintableToTextConverter:
		case KVersitParamEncodingQuotedPrintableUid:
			iCurrentPropertyCodingDetails.iEncoding		= Versit::EQuotedPrintableEncoding;
			iCurrentPropertyCodingDetails.iEncodingUid	= KQuotedPrintableToTextConverter;
			break;
		case KBase64ToTextConverter:
		case KVersitParamEncodingBase64Uid:
			iCurrentPropertyCodingDetails.iEncoding		= Versit::EBase64Encoding;
			iCurrentPropertyCodingDetails.iEncodingUid	= KBase64ToTextConverter;
			break;
		default:
			iCurrentPropertyCodingDetails.iEncoding		= Versit::ENoEncoding;
			iCurrentPropertyCodingDetails.iEncodingUid	= 0;
			break;
		}
	}

EXPORT_C void CVersitParser::SetLineCharacterSet(Versit::TVersitCharSet aLineCharSet)
	{
	iCurrentPropertyCodingDetails.iCharSet		= aLineCharSet;
	iCurrentPropertyCodingDetails.iCharSetUid	= MapVersitCharsetToCharConvCharset(aLineCharSet);
	}

EXPORT_C void CVersitParser::SetLineCharacterSetId(TUint aLineCharSetId)
	{
	iCurrentPropertyCodingDetails.iCharSet		= VersitUtils::CharSet(aLineCharSetId);
	iCurrentPropertyCodingDetails.iCharSetUid	= aLineCharSetId;
	}

EXPORT_C void CVersitParser::SetLineCoding(Versit::TVersitCharSet aLineCharSet,Versit::TVersitEncoding aLineEncoding)
	{
	SetLineCharacterSet(aLineCharSet);
	SetLineEncoding(aLineEncoding);
	}


EXPORT_C Versit::TVersitEncoding CVersitParser::DefaultEncoding() const
/** Gets the parser's default encoding. 

This value is initialised on construction, to ENoEncoding.

@return The parser's default encoding. */
	{
	return iDefaultCodingDetails.iEncoding;
	}

EXPORT_C Versit::TVersitCharSet CVersitParser::DefaultCharSet() const
/** Gets the default character set or transformation format. This may be used to 
represent property values in Versit objects.

@return The default character set. */
	{
	return iDefaultCodingDetails.iCharSet;
	}

EXPORT_C TUint CVersitParser::DefaultCharSetId() const
/** Gets the default character set or transformation format. 

This may be used to represent property values in Versit objects.

@return The default character set identifier. Possible values are defined in charconv.h. */
	{
	return iDefaultCodingDetails.iCharSetUid;
	}

EXPORT_C Versit::TVersitEncoding CVersitParser::LineEncoding() const
	{
	return iCurrentPropertyCodingDetails.iEncoding;
	}

EXPORT_C Versit::TVersitCharSet CVersitParser::LineCharSet() const
	{
	return iCurrentPropertyCodingDetails.iCharSet;
	}

EXPORT_C TUint CVersitParser::LineEncodingId() const
	{
	return iCurrentPropertyCodingDetails.iEncodingUid;
	}

EXPORT_C TUint CVersitParser::LineCharSetId() const
	{
	return iCurrentPropertyCodingDetails.iCharSetUid;
	}


TUint CVersitParser::MapVersitCharsetToCharConvCharset(Versit::TVersitCharSet aVersitSet)
// Converts between the Versit character set enumeration and the constant character set identifier that CharConv
// uses during the conversion process
	{
	return VersitUtils::CharConvCharSetUid(aVersitSet).iUid;
	}


TUint CVersitParser::MapVersitEncodingToConArcUid(Versit::TVersitEncoding aVersitEncoding)
// Map from a versit specific encoding enumeration...
	{
	switch(aVersitEncoding)
		{
		case Versit::EQuotedPrintableEncoding:
			return KQuotedPrintableToTextConverter;
		case Versit::EBase64Encoding:
			return KBase64ToTextConverter;
		default:
			return 0;
		}
	}



void CVersitParser::SetLineCharsetDetailsToDefault()
	{
	iCurrentPropertyCodingDetails.iCharSet		= iDefaultCodingDetails.iCharSet;
	iCurrentPropertyCodingDetails.iCharSetUid	= iDefaultCodingDetails.iCharSetUid;
	}


void CVersitParser::SetLineEncodingDetailsToDefault()
	{
	iCurrentPropertyCodingDetails.iEncoding		= iDefaultCodingDetails.iEncoding;
	iCurrentPropertyCodingDetails.iEncodingUid	= iDefaultCodingDetails.iEncodingUid;
	}

EXPORT_C void CVersitParser::SetCharacterConverter(Versit::TEncodingAndCharset& aEncodingAndCharset)
/** Sets a character converter suitable for converting strings between 
Unicode and the specified character set.

The function finds a suitable converter for the character set 
specified in aEncodingAndCharset.iCharSetId, if one is available, 
and assigns it to aEncodingAndCharset.iConverter.

If there is no converter available for the specified character set then iConverter 
is set to NULL.

This function is only of use if executing a major change to the externalisation 
behaviour, particularly if overriding CParserProperty::ExternalizeL() - the 
function from which this function is called.

@param aEncodingAndCharset A utility class from which the character set 
information is extracted (from its member iCharSetId) and to which the 
suitable character converter is set (to its member iConverter). */
	{
	CCnvCharacterSetConverter::TAvailability avail=CCnvCharacterSetConverter::ENotAvailable;
	TRAPD(err,avail=UnicodeUtils().SetCurrentCharSetL(aEncodingAndCharset.iCharSetId));
	UNUSED_VAR(err); // used to suppress build warnings
	if (avail==CCnvCharacterSetConverter::EAvailable)
		aEncodingAndCharset.iConverter=&UnicodeUtils().CharacterSetConverter();
	else
		aEncodingAndCharset.iConverter=NULL;
	}

EXPORT_C void CVersitParser::SetAutoDetect(TBool aOn,const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* aAutoDetectCharSets/*=NULL*/)
/** Turns auto detection of character sets on or off. 

If a property does not specify a character set, then it is possible to 
guess its character set. This function turns this behaviour on or off. 
When the behaviour is on, it also lets the caller specify a restricted 
set of character sets to be considered.

Auto-detection of character sets is used (if auto detection is on) 
when converting a property value to Unicode while internalising a stream.

@param aOn If ETrue, auto detection is turned on; if EFalse (the default value), 
auto detection is turned off
@param aAutoDetectCharSets If specified then auto detection will only consider 
the character sets listed. If NULL, all available character sets are considered. */
	{
	if (aOn)
		{
		iAutoDetectCharSets=aAutoDetectCharSets;
		iFlags|=EUseAutoDetection;
		}
	else
		iFlags&=~EUseAutoDetection;
	}


EXPORT_C TInt CVersitParser::SaveBinaryValuesToFilesL(TInt aSizeThreshold,const TDesC& aPath)
/** Saves all binary property values larger than a specified threshold to files, 
and sets each property value to be a URI representing the file, rather than 
the binary data iself.

The files are created in the folder identified by aPath, and are assigned 
unique filenames that consist of the property name and some random numbers.

Each new URI property value is prefixed with file:// and contains the path 
and filename of the file created.

If a vCard contains any agent property values and if they contain binary data 
whose size exceeds the threshold, these property values are replaced with 
URI property values.

The function sets up its own file server session, which is needed to create 
the files. It leaves if there is a problem creating any of the files.

@param aSizeThreshold The threshold number of bytes for the binary data, above 
which the binary data is stored in a file.
@param aPath The path identifying the location in which the files are created. 
This must not be greater than 240 characters long or the function leaves with 
KErrArgument. If it doesn't end in a slash, then one is appended.
@return The number of files created. */
	{
	
	TInt files = 0;
	// create a file server session
	RFs fs;	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	// parse through all the properties		
	const TInt countofproperties = iArrayOfProperties->Count();
	for (TInt i = 0; i < countofproperties; i++)
		{
			files += iArrayOfProperties->At(i)->SaveBinaryValuesToFilesL(aSizeThreshold,aPath,fs);
		}
	
	// this never gets executed for a vcard since there are no entities for a vcard.
	// But since this implementation is in the generic parser class, the code had been added.
	if(iArrayOfEntities)
		{
			const TInt countofentities = iArrayOfEntities->Count();
			for (TInt i = 0; i < countofentities; i++)
				{
					files += iArrayOfEntities->At(i)->SaveBinaryValuesToFilesL(aSizeThreshold,aPath,fs);
				}
		}

	CleanupStack::PopAndDestroy(&fs);
			
	return files;
	}


EXPORT_C TInt CVersitParser::LoadBinaryValuesFromFilesL()
/** Loads all files represented by URI property values and sets the binary data 
contained in the files to be the property values instead of the URIs.

For every property in the parser, if its value is a URI containing file:// 
followed by a path and filename, then the file is opened and the binary data 
it contains is read into a CParserPropertyValueBinary object. This replaces 
the URI as the property value. The function also operates on any agents in 
the vCard that contain URI property values.

The function creates its own file server session, which is needed to open 
the files. It leaves if there is a problem opening any of the files.

@return The number of files that were read. */
	{
	
	TInt files = 0;
	// create a file server session
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
		
	// parse through all the properties	
	const TInt countofproperties = iArrayOfProperties->Count();
	for (TInt i = 0; i < countofproperties; i++)
		{
			files += iArrayOfProperties->At(i)->LoadBinaryValuesFromFilesL(fs);
		}

    // this never gets executed for a vcard since there are no entities for a vcard.
	// But since this implementation is in the generic parser class, the code had been added.
	if(iArrayOfEntities)
		{
			const TInt countofentities = iArrayOfEntities->Count();
			for (TInt i = 0; i < countofentities; i++)
				{
					files += iArrayOfEntities->At(i)->LoadBinaryValuesFromFilesL(fs);
				}
		}
		
	CleanupStack::PopAndDestroy(&fs);
			
	return files;
	}


EXPORT_C TInt CVersitParser::SaveBinaryValuesToFilesL(TInt aSizeThreshold,const TDesC& aPath,RFs& aFileSession)
/** Saves all binary property values larger than a specified threshold to files, 
and sets each property value to be a URI representing the file rather than 
the binary data iself.

The files are created in the folder identified by aPath, and are assigned 
unique filenames that consist of the property name and some random numbers.

Each new URI property value is prefixed with file:// and contains the path 
and filename of the file created.

If a vCard contains any agent property values and if they contain binary data 
whose size exceeds the threshold, these property values are replaced with 
URI property values.

The function uses the file server session supplied, which is needed to create 
the files. It leaves if there is a problem creating any of the files.

@param aSizeThreshold The threshold number of bytes for the binary data, above 
which the binary data is stored in a file.
@param aPath The path identifying the location in which the files are created. 
This must not be greater than 240 characters long or the function leaves with 
KErrArgument. If it doesn't end in a slash, then one is appended.
@param aFileSession The file server session used to create the files.
@return The number of files created. */
	{
	
	TInt files = 0;
	
	const TInt countofproperties = iArrayOfProperties->Count();
	for (TInt i = 0; i < countofproperties; i++)
		{
			files += iArrayOfProperties->At(i)->SaveBinaryValuesToFilesL(aSizeThreshold,aPath,aFileSession);
		}
	 // this never gets executed for a vcard since there are no entities for a vcard.
	// But since this implementation is in the generic parser class, the code had been added.
	if(iArrayOfEntities)
		{
			const TInt countofentities = iArrayOfEntities->Count();
			for (TInt i = 0; i < countofentities; i++)
				{
					files += iArrayOfEntities->At(i)->SaveBinaryValuesToFilesL(aSizeThreshold,aPath,aFileSession);
				}
		}

	return files;
	}


EXPORT_C TInt CVersitParser::LoadBinaryValuesFromFilesL(RFs& aFileSession)
/** Loads all files represented by URI property values and sets the binary data 
contained in the files to be the property values instead of the URIs.

For every property in the parser, if its value is a URI containing file:// 
followed by a path and filename, then the file is opened and the binary data 
it contains is read into a CParserPropertyValueBinary object. This replaces 
the URI as the property value. The function also operates on any agents in 
the vCard that contain URI property values.

The function uses the file server session supplied, which is needed to open 
the files. It leaves if there is a problem opening any of the files.

@param aFileSession The file server session used to open the files.
@return The number of files that were read. */
	{
	
	TInt files = 0;
			
	const TInt countofproperties = iArrayOfProperties->Count();
	for (TInt i = 0; i < countofproperties; i++)
		{
			files += iArrayOfProperties->At(i)->LoadBinaryValuesFromFilesL(aFileSession);
		}
	 // this never gets executed for a vcard since there are no entities for a vcard.
	// But since this implementation is in the generic parser class, the code had been added.
	if(iArrayOfEntities)
		{
			const TInt countofentities = iArrayOfEntities->Count();
			for (TInt i = 0; i < countofentities; i++)
				{
					files += iArrayOfEntities->At(i)->LoadBinaryValuesFromFilesL(aFileSession);
				}
		}
		
	return files;
	}

CVersitTLSContainer *CVersitTLSContainer::NewLC(const TInt aSize)
	{
	CVersitTLSContainer *ret = new (ELeave) CVersitTLSContainer();
	CleanupStack::PushL( ret );
	ret->iShiftJisEscape = HBufC::NewL(aSize);
	return ret;
	}

CVersitTLSContainer::~CVersitTLSContainer()
	{
	delete iShiftJisEscape;
	iShiftJisEscape = NULL;
	}

EXPORT_C void CVersitParser::Reserved1()
	{}

EXPORT_C void CVersitParser::Reserved2()
	{}


//
// MVersitObserver
//

EXPORT_C void MVersitObserver::Reserved1()
	{}

EXPORT_C void MVersitObserver::Reserved2()
	{}


//
// MVersitPlugIn
//
EXPORT_C void MVersitPlugIn::GetInterface(TUid /*aInterfaceUid*/, TAny*& /*aInterface*/)
	{}

EXPORT_C void MVersitPlugIn::Reserved2()
/**
Reserved function.

@internalComponent
*/
	{}


//
// Dll entry point
//

EXPORT_C void Panic(TVersitParserPanic aPanic)
	{
	_LIT(KCategory,"Versit-Parser");
	User::Panic(KCategory,aPanic);
	}

//
// class CLineReaderExtension
//
CLineReaderExtension::CLineReaderExtension()
	{}
	
CLineReaderExtension::~CLineReaderExtension()
	{
	delete iLineBase64Value;
	}
	
void CLineReaderExtension::DeleteBase64ValueBuffer()
	{
	delete iLineBase64Value;
	iLineBase64Value = NULL;
	}

HBufC8* CLineReaderExtension::CreateBase64ValueBufferL()
	{
	if (!iLineBase64Value)
		{
		iLineBase64Value = HBufC8::NewL(CLineReader::EInitialLineSize);
		}
	return iLineBase64Value;
	}
	
HBufC8* CLineReaderExtension::Base64ValueBuffer()
	{
	return iLineBase64Value;
	}
	
CLineReaderExtension* CLineReaderExtension::NewL()
	{
	CLineReaderExtension* self=new(ELeave) CLineReaderExtension();
	return self;
	}

