// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __VOBSERV_H__
#define __VOBSERV_H__

#ifndef __E32DEF_H__
#include <e32def.h>
#endif
#ifndef __E32STD_H__
#include <e32std.h>
#endif
#ifndef __E32DES16_H__
#include <e32des16.h>
#endif
#ifndef __S32STRM_H__
#include <s32strm.h>
#endif
#ifndef __VUTIL_H__
#include <vutil.h>
#endif

class CVersitParser;

class MVersitObserver
/** A Versit parser observer.

This is a plug-in class and contains only pure virtual functions.

An implementator of this class can find out the version number of an entity 
being parsed. The version number specifies the version of the vCard/vCalendar 
specification used by the data of the vCard/vCalendar. This is for use in 
conjunction with the MVersitPlugin class, which adjusts the parser's behaviour 
according to the vCard/vCalendar version.

An implementator of this class can also respond to the creation of a new parser 
for an embedded sub-entity. This is so that the observer can set the MVersitPlugin, 
as well as itself, for each new parser that is created.

An observer is set up for a Versit parser using CVersitParser::SetObserver(). 
@publishedAll
@released
*/
	{
public:
	/** Called when the version property (a property of the name KVersitTokenVERSION) 
	of an entity is parsed during internalisation of a stream, if the Versit parser 
	has an observer.
	
	An implementation of this function can determine the version of an entity 
	being parsed.
	
	Called by CVersitParser::ParsePropertiesL().
	
	@param aParser A pointer to the parser object that detected the version.
	@param aVersion A unicode string containing the version number detected. */
	virtual void VersionSet(CVersitParser* aParser,const TDesC16& aVersion)=0;
	/** Called when a new Versit parser is created to parse an embedded object, 
	specifically a vEvent, a vTodo or an agent, if the Versit parser has an observer.
	
	@param aParser The newly created Versit entity. */
	virtual void NewParser(CVersitParser* aParser)=0;
private:
	IMPORT_C virtual void Reserved1();
	IMPORT_C virtual void Reserved2();
	};

class MVersitPlugIn
/** A Versit parser plug-in.

This is a plug-in class and contains only pure virtual functions.

An implementator of this class can override some of the low level behaviour 
of a Versit parser. For instance, options are provided to determine behaviour 
during line wrapping and unwrapping.

The use of this plug-in is optional, and when there is no plug-in the parser 
object will use default behaviour. However, vCard v3.0 has some differences 
to vCard v2.1, such as for line wrapping and unwrapping. Symbian OS supports 
vCard v2.1 in its default behaviour. Therefore this plug-in can be used to 
provide compatibility with vCard v3.0. 
@publishedAll
@released
*/
	{
public:
	/** Tests whether a space is to be added when merging (unwrapping) two lines 
	while internalising a stream. 
	
	If there is no plug-in then a space will be added.
	
	Used by the CLineReader class.
	
	@return ETrue if a space is to be added and EFalse if not. */
	virtual TBool AddSpace()=0;				//Unwrapping lines
	/** Tests whether white space at the start of a line, apart from the first space, 
	forms part of the data when internalising a stream.
	
	Note that the first space is always ignored and never included.
	
	If there is no plug-in then the rest of the white space at the start of a 
	line (tabs and spaces) is skipped and does not form part of the data when 
	internalising a stream.
	
	Used by the CLineReader class.
	
	@return EFalse if the spaces are to be part of the data and ETrue if not. */
	virtual TBool DeleteAllSpaces()=0;		//Unwrapping lines
	/** Tests how the end of Base64 data (data encoded using Versit::EBase64Encoding) 
	should be detected when internalising a stream. 
	
	To determine the end of Base64 data, either a blank line can be used, or a line 
	without a space at the start.
	
	If there is no plug-in then a blank line will be looked for. 
	
	Used by the CVersitParser class.
	
	@return ETrue if a blank line should be used and EFalse if a line without 
	a space at the start should be used. */
	virtual TBool NeedsBlankLine()=0;		//Unwrapping Base64 data
	/** Allows the removal of escape characters from a property value when internalising 
	from a stream.
	
	Versit deals with the escaping of semi-colons and the escape character itself 
	(that is, the Yen character for Shift-JIS or a backslash for other character 
	sets) without the help of a plug-in. Other characters, such as commas and 
	carriage returns, can be escaped and un-escaped using the plug-in's AddEscaping() 
	and RemoveEscaping().
	
	This function is needed as escaping is done differently in vCard v3.0: firstly, 
	commas are used as syntactical characters and so need to be escaped when they 
	are just part of the text; secondly, \\r\\n in plain (un-encoded) text is used 
	to mean a CRLF, whereas v2.1 forces you to use Quoted Printble encoding if 
	there is a CRLF in the data.
	
	Note that, although the string passed into this function can be changed, it 
	must not be made longer.
	
	Used by the CVersitParser class.
	
	@param aText The property value text from which escape characters are to be 
	removed. */
	virtual void RemoveEscaping(TPtr16& aText)=0;
	/** Allows the addition of escape characters to a property value when externalising 
	to a stream. 
	
	Versit deals with the escaping of semi-colons and the escape character itself 
	(that is, the Yen character for Shift-JIS or a backslash for other character 
	sets) without the help of a plug-in. Other characters, such as commas and 
	carriage returns, can be escaped and un-escaped using the plug-in's RemoveEscaping() 
	and AddEscaping().
	
	This function is needed as escaping is done differently in vCard v3.0: firstly, 
	commas are used as syntactical characters and so need to be escaped when they 
	are just part of the text; secondly, \\r\\n in plain (un-encoded) text is used 
	to mean a CRLF, whereas v2.1 forces you to use Quoted Printble encoding if 
	there is a CRLF in the data.
	
	If the string passed into this function needs to be made longer, then this 
	should be done with the following command, otherwise the cleanup stack will 
	eventually panic:
	
	@code
	aText=aText->ReAllocL(newSize); 
	@endcode
	
	Used by the CParserPropertyValue class.
	
	@param aText The property value text to which escape characters are to be 
	added. */
	virtual void AddEscaping(HBufC16*& aText)=0;
	/** Determines how an unencoded property value should be wrapped when externalising 
	to a stream.
	
	If there is no plug-in then line wrapping will follow vCal v1.0 and vCard 
	v2.1 wrapping rules. In this case, the text is split into lines with a maximum 
	length of KMaxExternalizedTokenLength (70) characters, and two spaces are inserted 
	at the beginning of each new line.
	
	Used by the CParserPropertyValue class.
	
	@param aStream The stream to write the text to.
	@param aCurrentLineLength The number of characters already written to the current 
	line, which needs to be taken into account when calculating where the next 
	line break should occur. This value should be updated before returning.
	@param aText The property value text to write to the stream, in the correct 
	character set and encoded as necessary.
	@return ETrue if the property value is wrapped using the method defined in 
	this (overloaded) function. EFalse if the property value text is not wrapped 
	by this function (in which case the default wrapping rules are implemented). */
	virtual TBool WrapLine(RWriteStream& aStream,TInt& aCurrentLineLength,const TPtr8& aText)=0;
	/** Determines how property values are encoded when externalising a property to 
	a stream.
	
	This function is called for each property in turn and can specify how encoding 
	should be implemented for the value of that property.
	
	If there is no plug-in, or this function returns EFalse, then the default 
	rules are used to determine how each property value is encoded.
	
	Used by the CVersitParser plug-in when externalising a property.
	
	@param aEncoding On return, specifies the encoding type used.
	@param aRequiresEncoding ETrue if encoding is required. This is the case if 
	either the default encoding is not Versit::ENoEncoding, or if the property 
	value contains characters that cannot be written out directly (e.g. equals, 
	CR, LF, tab or non-ASCII characters).
	@param aDefaultEncoding The default encoding specifed by the user of the parser.
	@param aPropertyUid The property UID of the property being externalised. These are 
	defined in vuid.h.
	@param aPropertyCharsetId The character set UID of the character set being 
	used to output the property.
	@return ETrue if the encoding type to be used is defined in this (overloaded) 
	function. EFalse if this function does not determine the encoding type (in 
	which case Versit's default method is used to decide the encoding type). */
	virtual TBool EncodingType(Versit::TVersitEncoding& aEncoding,TBool aRequiresEncoding,Versit::TVersitEncoding aDefaultEncoding
																						,TUid aPropertyUid,TUint aPropertyCharsetId)=0;
	/** Returns the encoding name to be used for a specified encoding type when externalising 
	a property to a stream, or allows the default name to be used.
	
	Can override the default name Versit would select if there was no plug-in ("BASE64", 
	"QUOTED-PRINTABLE", "8-BIT").
	
	The default names are selected using VersitUtils::IANAEncodingName().
	
	Used by the CVersitParser class when externalising a property.
	
	@param aEncoding The encoding type the name is required for.
	@return The name to use for the encoding type, or a zero length descriptor 
	if the default name should be used. */
	virtual const TDesC8& EncodingName(Versit::TVersitEncoding aEncoding)=0;
	
	/** Returns a pointer to a specified interface extension - to allow future extension
	of this class without breaking binary compatibility

	@param aInterfaceUid Identifier of the interface to be retrieved
	@param aInterface A reference to a pointer that retrieves the specified interface.
	*/
	IMPORT_C virtual void GetInterface(TUid aInterfaceUid, TAny*& aInterface);
private:
	IMPORT_C virtual void Reserved2();
	};
	
/** Extension interface UID
@publishedAll
@released
*/
const TUid KUidVersitPluginExtensionBase64Termination = {0x10285A92};

/**
Provides an extension to the MVersitPlugIn interface to allow a different ending of 
the base64 multi-lines from the default format.

Implementation of this interface is optional. Parser behaviour is not modified
if the function MVersitPlugIn::GetInterface() has not been implemented or does
not return an instance of a class implementing this interface when passed the
Uid parameter value KUidVersitPluginExtensionBase64Termination.

@publishedAll
@released
*/
class MVersitPlugInExtensionBase64Ending
	{
public:

	/** Allows Base64 encoding multi-lines without leading space and/or a blank line at the end
	
	@return ETrue, if the base64 data doesn't have leading spaces and/or a blank line at the end. The
	versit parser will then search for ":" to determine if the next line is a continuation or a 
	new property
	*/
	virtual TBool BlankLineAndLeadingSpaceNotRequired()=0;
	};

#endif
