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
// MIUTCONV.H
//

/**
 * @file 
 * @publishedAll
 * @released
 */

#ifndef __MIUTCONV_H__
#define __MIUTCONV_H__

#include <e32base.h>


#if !defined (__CHARCONV_H_)
#include <charconv.h>
#endif

#if !defined (__MIUTHDR_H__)
#include <miuthdr.h>		
#endif

#if !defined (__UTF_H_)
#include <utf.h>
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cimconvertcharconv.h"
#endif

/** SMTP send methods. 
@publishedAll
@released
*/
enum TImSendMethod
	{ 
	/** Simple email. */
	ESendAsSimpleEmail,
	/** MIME email. */
	ESendAsMimeEmail
	};	


// Information to be used when sending an email message.
// Relating to how parts are encoded and converted.
//
class TImEmailTransformingInfo
/** Encapsulates character conversion and encoding settings for sending SMTP email.

The default outcomes for header, body, HTML, and attachment parts respectively 
for each of the listed categories are given below:

Plain Charset: ISO-8859-1,ISO-8859-1,N/A,N/A

Plain Encoding:None,None,N/A,UU

MIME Charset:UTF8,UTF8,UTF8*,N/A

MIME Encoding:B,QP,Base64,Base64

*: the UTF8 value set in the header field, but no conversion is done. The 
HTML converter converts to UTF8. 
@publishedAll
@released
*/
	{
public:
    IMPORT_C TImEmailTransformingInfo& operator=(const TImEmailTransformingInfo& aInfo);
    IMPORT_C TBool operator==(const TImEmailTransformingInfo& aInfo);
	
	// streaming operations
	IMPORT_C void ExternalizeL( RWriteStream& aWriteStream ) const;
	IMPORT_C void InternalizeL( RReadStream& aReadStream );
	IMPORT_C void StoreL( CMsvStore& aStore ) const;
	IMPORT_C void RestoreL( CMsvStore& aStore );

	// Setting functions
	IMPORT_C void SetToDefault(const TImSendMethod aMethod);
	IMPORT_C void SetSendMethod(const TImSendMethod aMethod);
	IMPORT_C TImSendMethod SendMethod() const;

	IMPORT_C void SetHeaderEncodingQ();
	IMPORT_C void SetHeaderEncodingB();
	IMPORT_C void SetBodyTextEncoding(TImEncodingType aType);
	IMPORT_C void SetHTMLEncoding(TImEncodingType aType);
	IMPORT_C void SetAttachmentEncoding(TImEncodingType aType);
	
	IMPORT_C void SetHeaderAndBodyCharset(TUint aCharset);
	IMPORT_C void SetHeaderCharset(TUint aCharset);
	IMPORT_C void SetBodyTextCharset(TUint aCharset);
	IMPORT_C void SetHTMLCharset(TUint aCharset);

	// Accessor functions
	IMPORT_C TImEncodingType HeaderEncoding() const;
	IMPORT_C TImEncodingType BodyTextEncoding() const;
	IMPORT_C TImEncodingType HTMLEncoding() const;
	IMPORT_C TImEncodingType AttachmentEncoding() const;
	
	IMPORT_C TUint HeaderCharset() const;
	IMPORT_C TUint BodyTextCharset() const;
	IMPORT_C TUint HTMLCharset() const;

private:
	void SetHeaderEncoding(TImEncodingType aType);

private:
	TImEncodingType	iHeaderEncoding;	
	TImEncodingType	iBodyEncoding;	
	TImEncodingType	iHTMLEncoding;	
	TImEncodingType	iAttachmentEncoding;	

	TUint	iHeaderCharset;	
	TUint	iBodyCharset;	
	TUint	iHTMLCharset;	
	TUint	iTextAttachmentCharset;	

	TImSendMethod iSendMethod;
	};

#endif //__MIUTCONV_H__
