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

#ifndef __VPBAPPLUGIN_H__
#define __VPBAPPLUGIN_H__

#ifndef __VOBSERV_H__
#include <vobserv.h>
#endif

#include <e32base.h>

/** Extension interface UID */
const TUid KUidVersitPlugInExtension = {0xA0001B18};


/**
Provides an extension to the MVersitPlugIn interface to allow further
modification of Versit low level formatting so that a parser compliant with the
vCard 3.0 (RFC2426) and iCalendar (RFC2445) specifications can be created.

Implementation of this interface is optional. Parser behaviour is not modified
if the function MVersitPlugIn::GetInterface() has not been implemented or does
not return an instance of a class implementing this interface when passed the
Uid parameter value KUidVersitPlugInExtension.

@internalComponent
@released
*/
class MVersitPlugInExtension
	{
public:
	/** Allows automatic CHARSET parameter export to be disabled
	
	This function is needed because vCard 3.0 doesnot support the CHARSET parameter.
	Character set encodings are defined in an enclosing MIME header and apply to the
	whole vCard.

	If there is no plug-in then the CHARSET parameter will be exported as required for
	vCard 2.1 and vCal 1.0 

	@return ETrue if the CHARSET parameter should not be exported
	*/
	virtual TBool DisableCharsetParam() = 0;

	/** Determines how a binary property value should be wrapped when externalising to a stream.
	
	If there is no plug-in then line wrapping will follow vCal v1.0 and vCard v2.1 wrapping rules.
	In this case, the text is split into lines with a maximum length of KBase64MaxLineLength (64) 
	characters, and two spaces are inserted at the beginning of each new line.

	@param aCurrentLineLength The number of characters already written to the current 
	line, which needs to be taken into account when calculating where the next 
	line break should occur. This value should be updated before returning.
	@param aBuffer The encoded binary property value
	@return ETrue if the property value is wrapped using the method defined in 
	this (overloaded) function. EFalse if the property value is not wrapped 
	by this function (in which case the default wrapping rules are implemented)	
	*/
	virtual TBool WrapBinaryLinesL(CBufBase& aBuffer, TInt& aCurrentLineLength) = 0;

	/** Allows removal of the blank line automatically inserted after binary property values
	
	This function is needed because vCard 3.0 doesnot require a blank line to determine the
	end of encoded binary data

	If there is no plug-in then a blank line will be added
	
	@return ETrue if a blank line should not be added after a binary value
	*/
	virtual TBool DisableBlankLineAfterBinaryValue() = 0;

	/** Performs folding of property parameters when externalising to a stream.

    This function is needed because vCard 3.0 supports folding of property parameters.

    If there is no plug-in then folding will follow vCard v2.1(folding parameters is optional)
	and vCal v1.0 where only folding of property value is supported.

	@return ETrue if property parameters need folding
    */
	virtual TBool FoldParam() = 0;
	};
	
NONSHARABLE_CLASS(CVCard3ParserPlugIn) : public CBase, public MVersitPlugIn, public MVersitPlugInExtension
/**
Enables vCard 3.0 format export as defined by rfc2425 and rfc2426
@internalComponent
@released
*/
	{
public: //from MVersitPlugIn
	virtual TBool AddSpace(); 
	virtual TBool DeleteAllSpaces();
	virtual TBool NeedsBlankLine();
	virtual void RemoveEscaping(TPtr16& aText);
	virtual void AddEscaping(HBufC16*& aText);
	virtual TBool WrapLine(RWriteStream& aStream, TInt& aCurrentLineLength, const TPtr8& aText);	
	virtual TBool EncodingType(Versit::TVersitEncoding& aEncoding, TBool aRequiresEncoding,
								Versit::TVersitEncoding aDefaultEncoding, TUid aPropertyUid, TUint aPropertyCharsetId);
	virtual const TDesC8& EncodingName(Versit::TVersitEncoding aEncoding);
	virtual void GetInterface(TUid aInterfaceUid, TAny*& aInterface);

public:	//from MVersitPlugInExtension
	virtual TBool DisableCharsetParam();
	virtual TBool DisableBlankLineAfterBinaryValue();
	virtual TBool WrapBinaryLinesL(CBufBase& aBuffer, TInt& aCurrentLineLength);
	virtual TBool FoldParam();
	
private:
	void DoWrapLineL(RWriteStream& aStream, TInt& aCurrentLineLength, const TPtr8& aText);
	};

#endif //__VPBAPPLUGIN_H__
