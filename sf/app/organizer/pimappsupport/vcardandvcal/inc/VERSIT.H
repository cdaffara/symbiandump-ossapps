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

#ifndef __VERSIT_H__
#define __VERSIT_H__

// System includes
#include <e32base.h>
#include <badesca.h>
#include <s32file.h>

// User includes
#include <vuid.h>
#include <vtoken.h>
#include <vprop.h>
#include <versittls.h>

// Classes referenced
class RFile;
class TVersitDateTime;
class MVersitObserver;
class MVersitPlugIn;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
// Panic enumeration
/** Versit panic numbers. The Versit panic numbers have a category of "Versit-Parser". 
@internalComponent
@released
*/
enum TVersitParserPanic
	{
	ECurrentTokenNotFound,		//Unused
	EUnicodeUtilsNotFound,		//Unused
	ENoPropertyValue,		//Unused
	/** A CWeekDayArray contains an invalid value (not between EMonday 
	and ESunday inclusive). */
	ENoWeekdayFound,
	/** The number of a week within a month is invalid. */
	EWeekNoOverTen,
	/** The repeat type for a recurrence rule property value is not one of the values 
	specified in CVersitRecurrence::TType. */
	ENoRecognizedRepeatType,
	EVersitPanicEscapedTextAlreadyExists,		//Unused
	/** 8-bit encoding is proposed as the default for a parser but may not be appropriate. */
	EVersitPanicCannotSetEightBitEncoding,
	/** 8-bit encoding is encountered or proposed where it is not expected. */
	EVersitPanicUnexpectedEightBitEncoding,
	/** A parser was not specified when externalising a property. */
	EVersitPanicNeedToSpecifyParser,
	/** The additional storage slot for the given property has already been used */
	EVersitPanicAdditionalStorageSlotAlreadyInUse,
	/** Attempting to assign a NULL value to an additional storage slot  */
	EVersitPanicNullValueAssignedToAdditionalStorageSlot,
	//
	EVersitPanicLast
	};

/**
Used as key into additional storage within tls object, for CVersitTLSContainer
Value should not conflict with genuine compiler generated pointer values
@internalComponent
@released
*/
const static TInt* const KTLSVars = reinterpret_cast<TInt*>(1);
/**
@internalComponent
@released
*/
GLREF_C void DestroyHBufC(TAny* aHBufC);
/** Versit parser panic
@internalComponent
@released
*/
IMPORT_C void Panic(TVersitParserPanic aPanic);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS



#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/** Extension mechanism for CLineReader
This is an internal class and is only for use by CLineReader
@internalComponent
@released
*/
NONSHARABLE_CLASS(CLineReaderExtension) : public CBase
	{
	friend class CLineReader;
private:
	CLineReaderExtension();
	~CLineReaderExtension();

	static CLineReaderExtension* NewL();
	
	void DeleteBase64ValueBuffer();
	HBufC8* CreateBase64ValueBufferL();
	HBufC8* Base64ValueBuffer();
private:
	TUint8 iBuf[1024];
	TInt iOffset;
	TInt iSize;

// Hold the line buffer temporarily. 
// It is only used when the client wants
// the base64 extended behaviour defined
// in MVersitPlugInExtensionBase64Ending
	HBufC8* iLineBase64Value;
	};
#else
//Forward declare class
class CLineReaderExtension;	
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class CLineReader : public CBase
/** Line reader for a vCalendar or vCard parser.

Reads in a line at a time for the parser. 

Also allows the first character of the next line to be checked to see if it 
is a space, which can indicate that the line is wrapped. This enables the 
reading of multi-line property values.

Used by CVersitParser to internalise streams. 
@publishedAll
@released
*/
	{
public:
	/** Defines the initial line size of, and the size of expansions to, the buffer 
	which stores the line being read. */
	enum
		{
		/** The initial size of the buffer (pointed to by iBuf). */
		EInitialLineSize=96,
		/** The size by which the buffer (pointed to by iBuf) 
		is expanded when it has run out of room. */
		EExpandSize=16,
		};
	/** Defines values which describe the content of a line that has been read.

	This is the return value from the function ReadLine(). */
	enum
		{
		/** The line has content (not white space). */
		ELineHasContent=0,
		/** The line has white space only. */
		ELineIsWhiteSpace=1,		//Doesn't include next case
		/** The line has no content, and so is just a carriage return and line 
		feed ("/r/n"). */
		ELineIsCRLFOnly=2,
		/** The line has a colon
		This is only used internally. When the line has a colon, ReadLineL will
		return  ELineHasContent for the sake of backwards compatibility. 
		*/
		ELineHasColon
		};
public:
	IMPORT_C static CLineReader* NewL(RReadStream& aStream);
	IMPORT_C ~CLineReader();
	IMPORT_C virtual TInt ReadLineL(TInt aPos,TInt& aErr);
	IMPORT_C TBool AppendLineIfSpaceNextL();
	IMPORT_C TBool IsSpaceNextL();
	IMPORT_C TInt AppendSpaceL();
	inline void SetPlugIn(MVersitPlugIn* aPlugIn);
	inline void SetSkipWhiteSpaceAtStart(TBool aDoSkip);

public:
	void ReadBase64ValueL(TInt aPopValueStart);

protected:
	inline CLineReader(RReadStream& aStream) :iReadStream(&aStream), iBufPtr(NULL,0), iFirstCharNextLine(-1) {}
	IMPORT_C void ConstructL();
	IMPORT_C void ExpandBufferL(TInt aCurrentSize);
	IMPORT_C TUint8 ReadChar(TInt& aErr);

private:
	IMPORT_C virtual void Reserved();
	
private:
	TInt ReadLineL(HBufC8*& aHBuf, TInt aPos,TInt& aErr);

public:
	/** A pointer to an RReadStream object, the ReadUint8L() function of which is used 
	to read single characters from the stream.
	
	This is passed into the NewL() function upon construction. */
	RReadStream* iReadStream;
	/** A pointer to a buffer which stores data read from the stream.
	
	Its size on construction is EInitialLineSize, and it is expanded by EExpandSize 
	when necessary.
	
	A copy of this value should not be stored, since the buffer location may change 
	if the buffer is expanded.
	
	Data in the buffer is not lost when the buffer is expanded, but is copied 
	to the new location. */
	TPtr8 iBufPtr;
protected:
	HBufC8* iLineBuf;
	TInt iFirstCharNextLine;
private:
	MVersitPlugIn* iPlugIn;
	TBool iSkipWhiteSpaceAtStart;
	CLineReaderExtension* iExtension;
	};

class CVersitParser : public CBase
/** A generic Versit parser. 

Provides generic functions which implement behaviour common to both vCalendar 
and vCard parsers. For instance:

- InternalizeL() and ExternalizeL() functions, for writing and reading 
data from a stream or file.

- adding/retrieving properties and sub-entities to/from an existing entity. 

- encoding and character set conversion capabilities.

Although this is not an abstract class, in practice you would create and use 
objects of a derived class instead (CParserVCal or CParserVCard), as these 
provide additional functionality needed for parsing vCalendars and vCards.

Note: a flag used in the class constructor indicates whether the entity needs 
a version property. The version property will be inserted at the start of 
the array of properties for the entity, and specifies the version of the vCard/vCalendar 
specification used by the data of this particular vCard/vCalendar. The versions 
that are currently supported are vCard v2.1 and vCalendar v1.0.

A typical vCard looks like this:

BEGIN VCARD

VERSION 2.1 ...

END VCARD

Note: if you are sequentially creating and destroying multiple 
parsers, a major performance improvement may be achieved 
by using thread local storage to store an instance of CVersitUnicodeUtils 
which persists and can be used by all of the parsers.

See CVersitTlsData for more details. 
@publishedAll
@released
*/
	{
	friend class CParserProperty;
public:
	IMPORT_C CVersitParser(TUint aFlags);
	IMPORT_C void ConstructL();
	IMPORT_C ~CVersitParser();
	IMPORT_C void								InternalizeL(RFile& aInputFile,TInt& aBytesThroughFile);
	IMPORT_C virtual void						InternalizeL(RReadStream& aStream);
	IMPORT_C virtual void						InternalizeL(HBufC* aEntityName,CLineReader* aLineReader);
	IMPORT_C void								ExternalizeL(RFile& aOutputFile);
	IMPORT_C virtual void						ExternalizeL(RWriteStream& aStream);
	IMPORT_C void								AddEntityL(CVersitParser* aEntity);
	IMPORT_C void								AddPropertyL(CParserProperty* aProperty,TBool aInternalizing=EFalse);
	IMPORT_C CArrayPtr<CVersitParser>*			EntityL(const TDesC& aEntityName,TBool aTakeOwnership=ETrue);
	IMPORT_C CArrayPtr<CVersitParser>*			ArrayOfEntities(TBool aTakeOwnership=ETrue);
	IMPORT_C CArrayPtr<CParserProperty>*		PropertyL(const TDesC8& aPropertyName,const TUid& aPropertyUid,TBool aTakeOwnership=ETrue) const;
	IMPORT_C CArrayPtr<CParserProperty>*		ArrayOfProperties(TBool aTakeOwnership=ETrue);
	IMPORT_C virtual void						ConvertAllPropertyDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void								AdjustAllPropertyDateTimesToMachineLocalL();
	IMPORT_C static TBool						IsValidParameterValue(TInt& aPos,const TDesC& aParamValue);
	IMPORT_C void								SetEntityNameL(const TDesC& aEntityName);
	IMPORT_C TPtrC								EntityName() const;
	IMPORT_C static TBool						IsValidLabel(const TDesC& aLabel, TInt& aPos);
	IMPORT_C static TInt						Val(const TDesC& aString, TInt& aNumber);
	IMPORT_C void								SetCharacterConverter(Versit::TEncodingAndCharset& encodingAndCharset);

	//
	// Set/Get the default settings for the [en|de]coding process
	//
	IMPORT_C Versit::TVersitEncoding			DefaultEncoding() const;
	IMPORT_C void								SetDefaultEncoding(const Versit::TVersitEncoding aEncoding);
	IMPORT_C Versit::TVersitCharSet				DefaultCharSet() const;
	IMPORT_C TUint								DefaultCharSetId() const;
	IMPORT_C void								SetDefaultCharSet(const Versit::TVersitCharSet aCharSet);
	IMPORT_C void								SetDefaultCharSetId(TUint aCharSetId);
	IMPORT_C void								SetAutoDetect(TBool aOn,const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* aAutoDetectCharSets=NULL);

	//
	// Set/Get Observers and PlugIn's
	//
	inline void									SetObserver(MVersitObserver* aObserver);
	inline MVersitObserver*						Observer();
	inline void									SetPlugIn(MVersitPlugIn* aPlugIn);
	inline MVersitPlugIn*						PlugIn();

public:

	IMPORT_C TInt LoadBinaryValuesFromFilesL();
	IMPORT_C TInt LoadBinaryValuesFromFilesL(RFs& aFileSession);
	IMPORT_C TInt SaveBinaryValuesToFilesL(TInt aSizeThreshold,const TDesC& aPath);
	IMPORT_C TInt SaveBinaryValuesToFilesL(TInt aSizeThreshold,const TDesC& aPath,RFs& aFileSession);
public:
	/** White space character codes: used while analysing the syntax of the received 
	data and while externalising data. 
	@publishedAll
	@released 
	*/
	enum TCharCodes
		{
		/** ' ' */
		ESpace				= ' ',
		/** 9 */
		EHTab				= 9,
		/** 10 */
		ELineFeed			= 10,
		/** 13 */
		ECarriageReturn		= 13,
		/** ':' */
		EColon				=':'
		};
	/** Flags that can be specified on construction. 
	@publishedAll
	@released 
	*/
	enum TVersitParserFlags
		{
		/** This entity does not need a version property. */
		ENoVersionProperty	= 0,
		/** This entity should have a version property. */
		ESupportsVersion	= 0x01,
		//Gaps since other flags have been used in past
		EImportSyncML		= 0x1000,	//Importing from a SyncML server
		/** Indicates whether the parser should use auto-detection of character sets when 
		one has not been explicitly specified. */
		EUseAutoDetection	= 0x4000,
		//versit Internal use only
		/** The current property has specified a character set. */
		ECharSetIdentified	= 0x8000,
		/** If the charset is not explicitly specified, the default charset will be used instead of US-ASCII as required
		by the Versit specification . */
		EUseDefaultCharSetForAllProperties = 0x2000		
		};

	//
	// Unicode support conversion functions
	//
	IMPORT_C static TInt						ConvertFromUnicodeToISOL(TDes8& aIso, const TDesC16& aUnicode, CCnvCharacterSetConverter* aConverter);
	IMPORT_C TVersitDateTime*					DecodeDateTimeL(TDes& aToken) const;

protected:
	IMPORT_C TInt								ConvertToUnicodeFromISOL(TDes16& aUnicode, const TDesC8& aIso, TUint aCharacterSet);

	//
	// Parsing high level functions
	//
	IMPORT_C void								ParsePropertiesL();
	IMPORT_C void								ParseBeginL();
	IMPORT_C void								ParseEndL();
	void										ParseEndL(HBufC16& aEntityName);
	IMPORT_C TBool								ParseEntityL();
	IMPORT_C virtual void						ParsePropertyL();
	IMPORT_C CArrayPtr<CParserParam>*			ReadLineAndDecodeParamsLC(TInt& aValueStart,TInt& aNameLen);
	IMPORT_C void								MakePropertyL(TPtr8& aPropName,TInt aValueStart);
	IMPORT_C CArrayPtr<CParserParam>*			GetPropertyParamsLC(TPtr8 aParams);
	IMPORT_C void								ParseParamL(CArrayPtr<CParserParam>* aArray,TPtr8 aParam);
	IMPORT_C void								AnalysesEncodingCharset(CArrayPtr<CParserParam>* aArrayOfParams);
	IMPORT_C void								ReadMultiLineValueL(TPtr8& aValue,TInt aValueStart,TBool aBinaryData);
	inline TPtr8&								BufPtr();

	//
	// Append standard versit tokens to streams
	//
	IMPORT_C void								AppendBeginL();
	IMPORT_C void								AppendEndL();
	void										AppendEntityNameL();
	IMPORT_C void								DoAddPropertyL(CParserProperty* aProperty);

	//
	// Dispatcher functions to create entities/properties based upon a Versit identifying Uid
	//
	IMPORT_C virtual CVersitParser*				MakeEntityL(TInt aEntityUid,HBufC* aEntityName);
	CParserPropertyValueHBufC*					MakeDefaultPropertyValueL(HBufC16*& aValue);
	IMPORT_C virtual CParserPropertyValue*		MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue);
	IMPORT_C HBufC*								DecodePropertyValueL(const TDesC8& aValue);
	IMPORT_C void								DecodePropertyValueL(const TDesC8& aValue,const TUid& aEncodingUid);
	HBufC*										ConvertToUnicodeL(const TDesC8& aValue);
	IMPORT_C CDesCArray*						MakePropertyValueCDesCArrayL(TPtr16 aStringValue);
	IMPORT_C CArrayPtr<TVersitDateTime>*		MakePropertyValueMultiDateTimeL(TPtr16 aDateTimeGroup);
	IMPORT_C CVersitDaylight*					MakePropertyValueDaylightL(TPtr16 aDaylightValue);
	IMPORT_C TBool								FindFirstField(TPtr16& aField,TPtr16& aRemaining, TBool aTrimSpace=ETrue);
	IMPORT_C void								FindRemainingField(TPtr16& aField,TPtr16& aRemaining);

	//
	// Helper methods to decode versit dates, times, and time periods
	//
	IMPORT_C TTimeIntervalSeconds				DecodeTimeZoneL(const TDesC& aToken) const;
	IMPORT_C TTime*								DecodeTimePeriodL(const TDesC& aToken) const;
	IMPORT_C TInt								GetNumberL(const TDesC& aToken,TInt& aNumChars) const;

public:
	IMPORT_C virtual TUid						RecognizeToken(const TDesC8& aToken) const;
	IMPORT_C virtual TInt						RecognizeEntityName() const;
	//
	// Cleanup support methods
	//
	IMPORT_C static void						ResetAndDestroyArrayOfParams(TAny* aObject);
	IMPORT_C static void						ResetAndDestroyArrayOfProperties(TAny* aObject);
	IMPORT_C static void						ResetAndDestroyArrayOfEntities(TAny* aObject);
	IMPORT_C static void						ResetAndDestroyArrayOfDateTimes(TAny* aObject);

	inline void									SetFlags(TUint aFlags);

	

protected:
	//
	// Enquiry functions
	//
	static TBool								IsPunctuationToken(TUint aChar);
	inline TBool								SupportsVersion() const;
	inline void									SetSupportsVersion();
	inline void									ClearSupportsVersion();

	//
	// Set the settings for the [en|de]coding of the current property
	//
	IMPORT_C void								RestoreLineCodingDetailsToDefault();
	IMPORT_C void								SetLineEncoding(Versit::TVersitEncoding aLineEncoding);
	IMPORT_C void								SetLineEncoding(TUint aVersitEncodingUid);
	IMPORT_C void								SetLineCharacterSet(Versit::TVersitCharSet aLineCharSet);
	IMPORT_C void								SetLineCharacterSetId(TUint aLineCharSetId);
	IMPORT_C void								SetLineCoding(Versit::TVersitCharSet aLineCharSet, Versit::TVersitEncoding aLineEncoding);

	//
	// Return the settings for the current property
	//
	IMPORT_C Versit::TVersitEncoding			LineEncoding() const;
	IMPORT_C Versit::TVersitCharSet				LineCharSet() const;
	IMPORT_C TUint								LineEncodingId() const;
	IMPORT_C TUint								LineCharSetId() const;

	inline CVersitUnicodeUtils&					UnicodeUtils();

public:
	//
	// Static utility functions to aid with the Unicode conversion process
	//
	static TUint								MapVersitCharsetToCharConvCharset(Versit::TVersitCharSet aVersitSet);
	static TUint								MapVersitEncodingToConArcUid(Versit::TVersitEncoding aVersitEncoding);

private:
	void										SetLineCharsetDetailsToDefault();
	void										SetLineEncodingDetailsToDefault();

private:	//To fix TimeZone SyncML bug
	void										ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	void										ConvertUTCDateTimeToMachineLocal(TVersitDateTime* aDateTime,const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	//void										AddTimeZonePropertyL();

protected:
	struct TParserCodingDetails
		{
		Versit::TVersitEncoding		iEncoding;
		TUint						iEncodingUid;
		Versit::TVersitCharSet		iCharSet;
		TUint						iCharSetUid;
		};

protected:
	// Default settings & internal flags
	TInt							iFlags;
	TParserCodingDetails			iDefaultCodingDetails;
	TParserCodingDetails			iCurrentPropertyCodingDetails;
	TBuf<KVersitMaxVersionLength>	iDefaultVersion;
	const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* iAutoDetectCharSets;

	// Member data relating to the current item being parsed
	HBufC*							iEntityName;
	CArrayPtr<CVersitParser>*		iArrayOfEntities;
	CArrayPtr<CParserProperty>*		iArrayOfProperties;
	CParserProperty*				iCurrentProperty;
	CLineReader*					iOwnedLineReader;
	CLineReader*					iLineReader;
	HBufC8*							iDecodedValue;
	CBufSeg*						iLargeDataBuf;

	// In memory buffers
	RWriteStream*					iWriteStream;

	// General utility class
	CVersitTlsData*					iStaticUtils;

	// Plug-in classes
	MVersitObserver*				iObserver;
	MVersitPlugIn*					iPlugIn;

private:
	void DoInternalizeL();
	IMPORT_C virtual void Reserved1();
	IMPORT_C virtual void Reserved2();
private:
	TInt iParseBegin;
	TInt iReserved2;
	};
	
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
NONSHARABLE_CLASS( CVersitTLSContainer ): public CBase	
/**
Wrapper class for static variables to be stored in TLS
@internalComponent
@released
*/
	{
public:
	static CVersitTLSContainer *NewLC(const TInt aSize);
	~CVersitTLSContainer();
public:
	HBufC * iShiftJisEscape;//Store shift-jis escape charcter, as generated by relevant charconv plugin	
	};
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <versit.inl>

#endif
