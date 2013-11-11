// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MIUTCONV.CPP
//

#include "MIUTCONV.H"
#include "IMCMMAIN.H"
#include "MIUT_ERR.H"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "cimconvertcharconv.h"
#include "miut_errconsts.h"
#include "timrfc822datefield.h"
#endif
#include <msvstore.h>	// CMsvStore

#include <imcm.rsg>
#include <barsc.h>
#include <barsread.h>
#include <centralrepository.h>

const TInt KMaxMIMECharSetLength = 64; 	// Should be long enough for character sets


//****************************************************************************************
//				Class TImEmailTransformingInfo functions
//****************************************************************************************

/** Overloaded assignment operator.

@param aInfo Object from which to copy settings
@return Reference to 'this' object with the copied settings
*/
EXPORT_C TImEmailTransformingInfo& TImEmailTransformingInfo::operator=(const TImEmailTransformingInfo& aInfo)
	{
	SetSendMethod(aInfo.SendMethod());
	SetHeaderEncoding(aInfo.HeaderEncoding());
	SetBodyTextEncoding(aInfo.BodyTextEncoding());
	SetHTMLEncoding(aInfo.HTMLEncoding());
	SetAttachmentEncoding(aInfo.AttachmentEncoding());
	SetHeaderCharset(aInfo.HeaderCharset());
	SetBodyTextCharset(aInfo.BodyTextCharset());
	SetHTMLCharset(aInfo.HTMLCharset());

	return *this;
	}

/** Overloaded equality operator.

@param aInfo Object with which to compare this object
@return ETrue if the objects have the same settings, otherwise EFalse
*/
EXPORT_C TBool TImEmailTransformingInfo::operator==(const TImEmailTransformingInfo& aInfo)
	{
	return (SendMethod()==aInfo.SendMethod() &&
	HeaderEncoding()==aInfo.HeaderEncoding() &&
	BodyTextEncoding()==aInfo.BodyTextEncoding() &&
	HTMLEncoding()==aInfo.HTMLEncoding() &&
	AttachmentEncoding()==aInfo.AttachmentEncoding() &&
	HeaderCharset()==aInfo.HeaderCharset()&&
	BodyTextCharset()==aInfo.BodyTextCharset() &&
	HTMLCharset()==aInfo.HTMLCharset());
	}


/** Writes the object to the specified message store.

The function can leave with the standard stream leave codes.

@param aStore Store to which to write
*/
EXPORT_C void TImEmailTransformingInfo::StoreL( CMsvStore& aStore ) const
	{
	RMsvWriteStream out;
	out.AssignLC( aStore, KUidMsgFileTransformingInfo ); // pushes 'out' to the stack
	ExternalizeL(out);
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

/** Restores the object to the specified message store.

The function can leave with the standard stream leave codes.

@param aStore Store to which to write
*/
EXPORT_C void TImEmailTransformingInfo::RestoreL( CMsvStore& aStore )
	{
	RMsvReadStream in;
	in.OpenLC( aStore, KUidMsgFileTransformingInfo ); // pushes 'in' to the stack
	InternalizeL(in);
	in.Close();// make sure we close the stream
	CleanupStack::PopAndDestroy();
	}


EXPORT_C void TImEmailTransformingInfo::ExternalizeL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteUint8L( SendMethod() );

	aWriteStream.WriteUint8L( HeaderEncoding() );
	aWriteStream.WriteUint8L( BodyTextEncoding());
	aWriteStream.WriteUint8L( HTMLEncoding() );
	aWriteStream.WriteUint8L( AttachmentEncoding() );

	aWriteStream.WriteUint32L( HeaderCharset() );
	aWriteStream.WriteUint32L( BodyTextCharset() );
	aWriteStream.WriteUint32L( HTMLCharset() );
	}


EXPORT_C void TImEmailTransformingInfo::InternalizeL( RReadStream& aReadStream )
	{
	SetSendMethod( static_cast<TImSendMethod> (aReadStream.ReadUint8L()) );

	SetHeaderEncoding( static_cast<TImEncodingType> (aReadStream.ReadUint8L()));
	iBodyEncoding= static_cast<TImEncodingType> (aReadStream.ReadUint8L());
	SetHTMLEncoding(  static_cast<TImEncodingType> (aReadStream.ReadUint8L()) );
	SetAttachmentEncoding(  static_cast<TImEncodingType> (aReadStream.ReadUint8L()) );

	SetHeaderCharset( aReadStream.ReadUint32L());
	SetBodyTextCharset( aReadStream.ReadUint32L());
	SetHTMLCharset( aReadStream.ReadUint32L());
	}


EXPORT_C void TImEmailTransformingInfo::SetSendMethod(const TImSendMethod aMethod)
/** Sets the method by which email is sent.

This is either ESendAsSimpleEmail or ESendAsMimeEmail.

@param aMethod The method by which email is sent. */
	{
	iSendMethod=aMethod;
	}

EXPORT_C void TImEmailTransformingInfo::SetToDefault(TImSendMethod aMethod)
/** Sets the member data to their default values, for the send method specified.

@param aMethod The send method. */
	{
	SetSendMethod(aMethod);

	// Set to default values.
	if (iSendMethod==ESendAsSimpleEmail)
		{
		SetHeaderEncoding(KDefaultPlainHeaderEncoding);
		SetBodyTextEncoding(KDefaultPlainBodyTextEncoding);
		SetAttachmentEncoding(KDefaultPlainAttachmentEncoding);
		}
	else
		{
		SetHeaderEncoding(KDefaultMIMEHeaderEncoding);
		SetBodyTextEncoding(KDefaultMIMEBodyTextEncoding);
		SetHTMLEncoding(KDefaultMIMEHTMLEncoding);
		SetAttachmentEncoding(KDefaultMIMEAttachmentEncoding);	
		}

	SetHeaderCharset(0);
	SetBodyTextCharset(0);
	SetHTMLCharset(0);
	}

EXPORT_C TImSendMethod TImEmailTransformingInfo::SendMethod() const
/** Gets the current send method.

@return The current send method. */
	{
	return iSendMethod;
	}


EXPORT_C void TImEmailTransformingInfo::SetHeaderEncodingQ()
/** Sets the header encoding to type Q. */
	{
	iHeaderEncoding=EEncodingTypeQP;
	}

EXPORT_C void TImEmailTransformingInfo::SetHeaderEncodingB()
/** Sets the header encoding to type B. */
	{
	iHeaderEncoding=EEncodingTypeBASE64;
	}

void TImEmailTransformingInfo::SetHeaderEncoding(TImEncodingType aType)
	{
	iHeaderEncoding=aType;
	}

EXPORT_C void TImEmailTransformingInfo::SetBodyTextEncoding(TImEncodingType aType)
/** Sets the body text encoding.

@param aType The body text encoding. This can be one of four possible values: 
EEncodingTypeNone, EEncodingTypeQP, EEncodingTypeBASE64, or EEncodingTypeUU. */
	{
	iBodyEncoding=aType;
	}


EXPORT_C void TImEmailTransformingInfo::SetHTMLEncoding(TImEncodingType aType)
/** Sets the encoding for HTML email.

@param aType The HTML email encoding. This can be one of four possible values: 
EEncodingTypeNone, EEncodingTypeQP, EEncodingTypeBASE64, or EEncodingTypeUU. */
	{
	iHTMLEncoding=aType;
	}

EXPORT_C void TImEmailTransformingInfo::SetAttachmentEncoding(TImEncodingType aType)
/** Sets the encoding for email attachments.

This can be one of four possible values : EEncodingTypeNone, EEncodingTypeQP, 
EEncodingTypeBASE64, or EEncodingTypeUU.

@param aType The email attachment encoding. */
	{
	iAttachmentEncoding=aType;
	}

EXPORT_C void TImEmailTransformingInfo::SetHeaderAndBodyCharset(TUint aCharset)
/** Sets the character set used for both the header and body text.

@param aCharset The character set. */
	{
	iHeaderCharset=iBodyCharset=aCharset;
	}


EXPORT_C void TImEmailTransformingInfo::SetHeaderCharset(TUint aCharset)
/** Sets the character set used for the header.

@param aCharset The character set. */
	{
	iHeaderCharset=aCharset;
	}
	
EXPORT_C void TImEmailTransformingInfo::SetBodyTextCharset(TUint aCharset)
/** Sets the character set for body text.

@param aCharset The character set. */
	{
	iBodyCharset=aCharset;
	}

EXPORT_C void TImEmailTransformingInfo::SetHTMLCharset(TUint aCharset)
/** Sets the HTML character set.

@param aCharset The character set. */
	{
	iHTMLCharset=aCharset;
	}

	
EXPORT_C TImEncodingType TImEmailTransformingInfo::HeaderEncoding() const
/** Gets the header encoding.

@return The header encoding. */
	{
	return iHeaderEncoding;
	}


EXPORT_C TImEncodingType TImEmailTransformingInfo::BodyTextEncoding() const
/** Gets the body text encoding.

@return The body text encoding. */
	{
	return iBodyEncoding;
	}

EXPORT_C TImEncodingType TImEmailTransformingInfo::HTMLEncoding() const
/** Gets the HTML encoding.

@return The HTML encoding. */
	{
	return iHTMLEncoding;
	}
	
EXPORT_C TImEncodingType TImEmailTransformingInfo::AttachmentEncoding() const
/** Gets the attachment encoding.

@return The attachment encoding. */
	{
	return iAttachmentEncoding;
	}
		
EXPORT_C TUint TImEmailTransformingInfo::HeaderCharset() const
/** Gets the character set for the header.

@return The character set for the header. */
	{
	return iHeaderCharset;
	}

EXPORT_C TUint TImEmailTransformingInfo::BodyTextCharset() const
/** Gets the character set for the body text.

@return The character set for the body text. */
	{
	return iBodyCharset;
	}

EXPORT_C TUint TImEmailTransformingInfo::HTMLCharset() const
/** Gets the HTML character set.

@return The HTML character set. */
	{
	return iHTMLCharset;
	}
	



//****************************************************************************************
//              Class CImConvertCharconv Functions
//****************************************************************************************

EXPORT_C CImConvertCharconv* CImConvertCharconv::NewL(CCnvCharacterSetConverter& aConverter,
													  RFs& anFs)
	{
	CImConvertCharconv* self = new (ELeave) CImConvertCharconv (aConverter, anFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CImConvertCharconv::CImConvertCharconv(CCnvCharacterSetConverter& aConverter, RFs& anFs) : 
									   iConverter (aConverter), iFs(anFs)
	{
	}

CImConvertCharconv::~CImConvertCharconv() 
	{
	delete iCharsetsAvailable;
	}

void CImConvertCharconv::ConstructL() 
	{
	iCharsetsAvailable = iConverter.CreateArrayOfCharacterSetsAvailableL(iFs);

	// Open .ini file to get the system default
	SetSystemDefaultCharsetL();
	iCharconvState = CCnvCharacterSetConverter::KStateDefault;
	}

TInt CImConvertCharconv::StraightCopy( const TDesC8& aBufIn, TDes& rBufOut)
	{
	TInt inLen=aBufIn.Length();
	TInt outMaxLen=rBufOut.MaxLength();

	if (inLen >= outMaxLen)
		{
		TPtrC8 in = aBufIn.Left(outMaxLen);
		rBufOut.Copy(in);
		return (inLen-outMaxLen);
		}
	else
		rBufOut.Copy(aBufIn);
	return 0;
	}

TInt CImConvertCharconv::StraightCopy( const TDesC& aBufIn, TDes8& rBufOut)
	{
	TInt inLen=aBufIn.Length();
	TInt outMaxLen=rBufOut.MaxLength();

	if (inLen >= outMaxLen)
		{
		TPtrC16 in = aBufIn.Left(outMaxLen);
		rBufOut.Copy(in);
		return (inLen-outMaxLen);
		}
	else
		rBufOut.Copy(aBufIn);
	return 0;
	}

EXPORT_C TInt CImConvertCharconv::PrepareToConvertToFromOurCharsetL(const TUint aUid)
	{
	if (!aUid)
		{
		iCharsetUid=DefaultCharset();
		return ETrue;
		}
	else if (CharsetAvailable(aUid))
		{
		iCharconvState = CCnvCharacterSetConverter::KStateDefault;
		iConverter.PrepareToConvertToOrFromL(aUid, *iCharsetsAvailable, iFs);
		iConverter.SetReplacementForUnconvertibleUnicodeCharactersL(KMiutUndisplayableChar);
		iCharsetUid=aUid;
		return ETrue;
		}

	iCharsetUid=KUidMsvCharsetNone;
	return EFalse; 
	}

TBool CImConvertCharconv::CharsetAvailable(const TUint aUid)
	{
	iCharsetUid=aUid ? aUid : DefaultCharset();

	if (!iCharsetUid)
		return EFalse; // No charset

	TInt i=iCharsetsAvailable->Count();
	while (i--)
		{
		if (iCharsetsAvailable->At(i).Identifier()==iCharsetUid)
			return ETrue; // Charset conversion available.
		}

	return EFalse; // Charset id exists, but conversion unavaliable.
	}


// Convert decoded buffer to Unicode
//
EXPORT_C TInt CImConvertCharconv::ConvertToOurCharsetL(const TDesC8& aBufIn, TDes& rBufOut, 
										TInt& rNumUnconvertedChars,
										TInt& rIndexOfFirstUnconvertedChar)
	{
	// Conversion is not needed if iCharsetUid is any of UTF-16(UCS2), UTF-16LE(LittleEndian), UTF-16BE
	// just do a straight copy.
	if (!iCharsetUid || iCharsetUid==KUidMsvCharsetNone  || iCharsetUid == KCharacterSetIdentifierUcs2
	    || iCharsetUid == KCharacterSetIdentifierUnicodeLittle || iCharsetUid == KCharacterSetIdentifierUnicodeBig)
		{
		return StraightCopy(aBufIn, rBufOut);
		}
	else
		{
		rNumUnconvertedChars=0;
		rIndexOfFirstUnconvertedChar=-1;
		TInt err = iConverter.ConvertToUnicode(rBufOut, aBufIn, iCharconvState, 
							rNumUnconvertedChars, rIndexOfFirstUnconvertedChar);
		if (err<0 && rBufOut.Length()>0)
			rBufOut.Zero();
		return err;
		}
	}

// Convert decoded buffer to Unicode
EXPORT_C TInt CImConvertCharconv::ConvertFromOurCharsetL(const TDesC& aBufIn, TDes8& rBufOut, 
										TInt& rNumUnconvertedChars,
										TInt& rIndexOfFirstUnconvertedChar)
	{
	// Conversion is not needed if iCharsetUid is any of UTF-16(UCS2), UTF-16LE(LittleEndian), UTF-16BE
	// just do a straight copy.
	if (iCharsetUid && iCharsetUid != KUidMsvCharsetNone && iCharsetUid != KCharacterSetIdentifierUcs2
	    && iCharsetUid != KCharacterSetIdentifierUnicodeLittle && iCharsetUid != KCharacterSetIdentifierUnicodeBig)		
		{
		return iConverter.ConvertFromUnicode(rBufOut, aBufIn, rNumUnconvertedChars, rIndexOfFirstUnconvertedChar);
		}
	else
		{
		rNumUnconvertedChars=0;
		rIndexOfFirstUnconvertedChar=-1;
		return StraightCopy(aBufIn, rBufOut);
		}
	}


EXPORT_C TUint CImConvertCharconv::GetMimeCharsetUidL(const TDesC8& aBufIn) const
	{
	TUint id=0;
	id=iConverter.ConvertStandardNameOfCharacterSetToIdentifierL(aBufIn, iFs);
	if (!id)
		id = KUidMsvCharsetNone;
	return id;
	}

EXPORT_C HBufC8* CImConvertCharconv::GetMimeCharsetTextStringL(const TUint& aUid) const
    {
	return iConverter.ConvertCharacterSetIdentifierToStandardNameL(aUid, iFs);
    }

EXPORT_C TUint CImConvertCharconv::DefaultCharset() const
	{
	return 	iSystemDefaultCharset? iSystemDefaultCharset : KDefaultPlainCharset;
	}

EXPORT_C TUint CImConvertCharconv::SystemDefaultCharset() const
	{
	return iSystemDefaultCharset;
	}

void CImConvertCharconv::SetSystemDefaultCharsetL()
	{	
	TBuf8<KMaxMIMECharSetLength> charsetName;
	TUint charset = KDefaultPlainCharset;
	
	// Try to get the character set from the Central Repository
	TInt err = GetSystemCharsetFromCenRepL(charsetName);
	if (KErrNone != err)
		{
		// That failed, fallback to reading from the resource file
		// Check the resource file..
		RResourceFile resFile;
		OpenResourceFileL(resFile, iFs);	// NB leaves if file not found

		// make sure the resource file will be closed if anything goes wrong
		TCleanupItem close( CloseResourceFile, &resFile );
		CleanupStack::PushL( close );		
			
		HBufC8* buf = resFile.AllocReadLC( DEFAULT_SYSTEM_CHARSET );

		TResourceReader reader;
		reader.SetBuffer(buf);
		charsetName.Copy(reader.ReadTPtrC8());
		
		CleanupStack::PopAndDestroy(2,&resFile); // buf, resFile
		}

	charset = GetMimeCharsetUidL(charsetName);

	// If the character set in the resource file is not recognised. Leave..
	if (charset==KUidMsvCharsetNone || !CharsetAvailable(charset))
		User::Panic(_L("IMCM"),EImcmSystemDefaultCharsetNotSupported);
	else
		iSystemDefaultCharset=charset;
	}
// Default charset is wrong when language is English on a China phone
TInt CImConvertCharconv::GetSystemCharsetFromCenRepL(TDes8& aMimeCharSet)
	{
	// Open the repository
	CRepository* repository = CRepository::NewL(KUidMsgEmailGeneralCenRep);
	// Read the value
	TInt err = repository->Get(EEmailGeneralCharSetId, aMimeCharSet);
	delete repository;
	return err;
	}

