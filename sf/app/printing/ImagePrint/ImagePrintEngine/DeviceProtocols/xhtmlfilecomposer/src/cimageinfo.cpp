/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains the declarations for miscellanous image information.
*
*/


#include <uri8.h> // curi8
#include <uri16.h> // curi16
#include <uriutils.h> // uriutils
#include <escapeutils.h> //utf8 conversions

#include "cimageinfo.h"
#include "cxfclogger.h"

// log macros for static methods

#ifdef ENABLE_LOGGING
//#define LOCAL_LOGGING
#endif

#ifdef LOCAL_LOGGING

#define I_LOG(a) LOG(a)            
#define I_LOG1(s, v) LOG1(s, v)        
#define I_LOG2(s, v1, v2) LOG2(s, v1, v2)   
                                              
#define I_LOG8(a) LOG8(a)           
#define I_LOG81(s, v) LOG81(s, v)       
#define I_LOG82(s, v1, v2) LOG82(s, v1, v2)  

#else

#ifdef ENABLE_LOGGING
#undef ENABLE_LOGGING
#endif

#define I_LOG(a)
#define I_LOG1(s, v)
#define I_LOG2(s, v1, v2)

#define I_LOG8(a)
#define I_LOG81(s, v)
#define I_LOG82(s, v1, v2)


#endif

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo* CImageInfo::NewL()
{
	CImageInfo *self = NewLC();
	CleanupStack::Pop();	// self
	return self;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo* CImageInfo::NewL(const CImageInfo& a)
{
	CImageInfo *self = NewLC(a);
	CleanupStack::Pop();	// self
	return self;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo* CImageInfo::NewLC()
{
	CImageInfo *self = new (ELeave) CImageInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo* CImageInfo::NewLC(const CImageInfo& a)
{
	CImageInfo *self = new (ELeave) CImageInfo(a);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::ConstructL()
{

}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo::CImageInfo() :
	iUri(KNullDesC8()),
	iEncoded(EFalse),
	iFilePath(KNullDesC8()),
	iId(KErrNotFound),
	iCopies(1),
	iIndex(KErrNotFound)
{

}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo::CImageInfo(const CImageInfo &a) :
	iUri(a.iUri),
	iEncoded(a.iEncoded),
	iFilePath(a.iFilePath),
	iId(a.iId),
	iCopies(a.iCopies),
	iIndex(a.iIndex)
{

}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CImageInfo
//
//--------------------------------------------------------------------------------------------
EXPORT_C CImageInfo::~CImageInfo()
{

}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::UriLog
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::UriLog(const TDesC8& aUri)
{
#ifdef ENABLE_LOGGING
	TFileName8 uri;
    UriLog(aUri, uri);
    LOG81("[CImageInfo::UriLog]\t \"%S\"", &uri);
#else // prevent warning in compiler
	aUri.Length();
#endif
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::UriLog
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::UriLog(const TDesC& aUri)
{
#ifdef ENABLE_LOGGING
	TFileName uri;
    UriLog(aUri, uri);
    LOG1("[CImageInfo::UriLog(const TDesC&)]\t \"%S\"", &uri);
#else // prevent warning in compiler
	aUri.Length();
#endif
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::UriLog
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::UriLog(const TDesC8& aUri, TDes8& aConvert)
{
#ifdef ENABLE_LOGGING

    I_LOG2("[CImageInfo::UriLog(const TDesC8&, TDes8&)]\t %d - %d", aConvert.MaxLength(), aUri.Length());
	if(aUri.Length() >= aConvert.MaxLength())
	{
    	aConvert.Copy(aUri.Left(aConvert.MaxLength()));
	}
	else
	{
		aConvert.Copy(aUri);
	}

	TInt pos = aConvert.Find(KPros8());
	while(pos != KErrNotFound)
	{
		if(KErrNotFound != pos && pos < aConvert.Length())
			aConvert.Replace(pos, KPros8().Length(), KUnderScore8());
	
		pos = aConvert.Find(KPros8());
	}
#else // prevent warning in compiler
	aUri.Length();
	aConvert.Length();
#endif
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::UriLog
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::UriLog(const TDesC& aUri, TDes& aConvert)
{
#ifdef ENABLE_LOGGING
    I_LOG2("[CImageInfo::UriLog(const TDesC&, TDes&)]\t %d - %d", aConvert.MaxLength(), aUri.Length());
	if(aUri.Length() >= aConvert.MaxLength())
	{
    	aConvert.Copy(aUri.Left(aConvert.MaxLength()));
	}
	else
	{
		aConvert.Copy(aUri);
	}

	TInt pos = aConvert.Find(KPros());
	while(pos != KErrNotFound)
	{
		if(KErrNotFound != pos && pos < aConvert.Length())
			aConvert.Replace(pos, KPros().Length(), KUnderScore());
	
		pos = aConvert.Find(KPros());
	}
#else // prevent warning in compiler
	aUri.Length();
	aConvert.Length();
#endif
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::ConvertL
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::ConvertToUTF8L(const TDesC& aSource, TDes8& aResult)
{
	HBufC8* buf = EscapeUtils::ConvertFromUnicodeToUtf8L(aSource);
	if(buf)
	{
	    I_LOG2("[CImageInfo::ConvertToUTF8L]\t %d - %d", aResult.MaxLength(), buf->Des().Length());
		aResult.Copy(buf->Des());
		delete buf;
	}
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::ConvertL
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::ConvertToUnicodeL(const TDesC8& aSource, TDes& aResult)
{
	HBufC16* buf = EscapeUtils::ConvertToUnicodeFromUtf8L(aSource);
	if(buf)
	{
	    I_LOG2("[CImageInfo::ConvertToUnicodeL]\t %d - %d", aResult.MaxLength(), buf->Des().Length());
		aResult.Copy(buf->Des());
		delete buf;
	}
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::EncodeUriL
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::EncodeUriL(const TDesC8& aDecodedUri, TDes8& aEncodedUri)
{
    TFileName unicodeUri;
    ConvertToUnicodeL( aDecodedUri, unicodeUri );
    EncodeUriL(unicodeUri, aEncodedUri);
}
//--------------------------------------------------------------------------------------------
//
// CImageInfo::EncodeUriL
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::EncodeUriL(const TDesC16& aDecodedUri, TDes8& aEncodedUri)
{
    I_LOG("[CImageInfo::EncodeUriL]");
    CUri8* uri8 = UriUtils::CreateUriL( aDecodedUri );
    CleanupStack::PushL( uri8 );
    HBufC16* uriBuf = uri8->Uri().DisplayFormL();
    CleanupStack::PushL( uriBuf );

    //alloc enough memory: encoding replaces one char with three: '%XX'
    HBufC8* uriBuf8 = HBufC8::NewLC(KMaxFileName*3);

   	TPtr8 buf8Ptr(uriBuf8->Des());
    ConvertToUTF8L( uriBuf->Des(), buf8Ptr );
    I_LOG("[CImageInfo::EncodeUriL]\t <-ConvertToUTF8L");

    // encode additionally the following characters
    //";"  | "?"  | "@" | "&" | "=" | "+" | "$" | ","
    // no encoding of "/" or ":" for paths

	TInt pos = buf8Ptr.LocateReverse('/');
    I_LOG1("[CImageInfo::EncodeUriL]\t pos %d", pos);
    if(KErrNotFound != pos)
    {
		TPtr8 namePtr(buf8Ptr.LeftTPtr(pos));
	    I_LOG1("[CImageInfo::EncodeUriL]\t namePtr.Length() %d", namePtr.Length());
	    UriLog(namePtr);

	    I_LOG("[CImageInfo::EncodeUriL]\t ->EncodeFileNameL");
	    EncodeFileNameL(namePtr);
    }

    if(buf8Ptr.Length() > aEncodedUri.MaxLength())
    	User::Leave(KErrOverflow);

    aEncodedUri.Copy(buf8Ptr);
    I_LOG("[CImageInfo::EncodeUriL]\t aEncodedUri:");
    UriLog(aEncodedUri);

    CleanupStack::PopAndDestroy( uriBuf8 );
    CleanupStack::PopAndDestroy( uriBuf );
    CleanupStack::PopAndDestroy( uri8 );
}


//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetUri(const TDesC8& aUri, const TBool aEncoded)
{
	iUri.Copy(aUri);
	iEncoded = aEncoded;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetUriL(const TDesC& aUri)
{
	ConvertToUTF8L(aUri, iUri);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::GetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::GetUri(TDes8& aUri) const
{
	aUri.Copy(iUri);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::GetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::GetUriL(TDes& aUri) const
{
	ConvertToUnicodeL( iUri, aUri );
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::GetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::GetEncodedUriL(TDes8& aUri)
{
    I_LOG1("[CImageInfo::EncodeUriL]\t iEncoded = %d", iEncoded);

	if(iEncoded)
		GetUri(aUri);
	else
		EncodeUriL(iUri, aUri);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CompareUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C TBool CImageInfo::CompareUri(const TDesC& aUri)
{
	if(KNullDesC() == aUri && KNullDesC8() == iUri)
		return ETrue;

	TFileName8 tmpUri8;
	//First try copy (if earlier copied 8->16 the conversion cannot be done anymore):
	tmpUri8.Copy(aUri);
	if(CompareUri(tmpUri8))
		return ETrue;
	
	//Then convert:
	TRAPD(err, ConvertToUTF8L(aUri, tmpUri8));
	if(KErrNone == err)
		return CompareUri(tmpUri8);

	return EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CompareUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C TBool CImageInfo::CompareUri(const TDesC8& aUri)
{
	if(0 == iUri.Compare(aUri))
		return ETrue;

	TFileName8 encodedUri;
	TRAPD(err, EncodeUriL(iUri, encodedUri));
	if(KErrNone == err && 0 == encodedUri.Compare(aUri))
		return ETrue;

	return EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetFilePath(const TDesC8& aFilePath)
{
	iFilePath = aFilePath;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetFilePathL(const TDesC& aFilePath)
{
	ConvertToUTF8L(aFilePath, iFilePath);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::GetFilePath(TDes8& aFilePath) const
{
	aFilePath.Copy(iFilePath);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::GetFilePathL(TDes& aFilePath) const
{
	ConvertToUnicodeL(iFilePath, aFilePath);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CompareFilePath
//
//--------------------------------------------------------------------------------------------
EXPORT_C TBool CImageInfo::CompareFilePath(const TDesC& aFilePath)
{
	if(KNullDesC() == aFilePath && KNullDesC8() == iFilePath)
		return ETrue;

	TFileName8 tmpFile8;
	TRAPD(err, ConvertToUTF8L(aFilePath, tmpFile8));
	if(KErrNone == err)
		return CompareFilePath(tmpFile8);

	return EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::CompareFilePath
//
//--------------------------------------------------------------------------------------------
EXPORT_C TBool CImageInfo::CompareFilePath(const TDesC8& aFilePath)
{
	if(0 == iFilePath.Compare(aFilePath))
		return ETrue;

	return EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetId(const TInt aId)
{
	iId = aId;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C TInt CImageInfo::Id() const
{
	return iId;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetCopies(const TInt aCopies)
{
	iCopies = aCopies;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C TInt CImageInfo::Copies() const
{
	return iCopies;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::SetIndex(const TInt aIndex)
{
	iIndex = aIndex;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::SetUri
//
//--------------------------------------------------------------------------------------------
EXPORT_C TInt CImageInfo::Index() const
{
	return iIndex;
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::EncodeFileNameL
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::EncodeFileNameL(TDes8& aFileName)
{

	HBufC8* result = HBufC8::NewLC(aFileName.MaxLength());

	for(TInt i = 0; i < result->Length(); ++i)
	{
		TUint8 ch = aFileName[i];
		switch(ch)
		{
			case ':':
				AppendL(result, KColonUrlEncoded());
				break;
			case '/':
				AppendL(result, KSlashUrlEncoded());
				break;
			default:
				TBufC8<1> character(&ch);
				AppendL(result, character);
				break;
		}
	}
	aFileName.Copy(result->Des());
	CleanupStack::PopAndDestroy(result);
}

//--------------------------------------------------------------------------------------------
//
// CImageInfo::Append
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CImageInfo::AppendL(HBufC8* aWhere, const TDesC8& aWhat)
{
	if( (aWhere->Length() + aWhat.Length()) >= aWhere->Des().MaxLength())
	{
    	I_LOG2("[CImageInfo::AppendL]\t %d - %d", aWhat.Length(), aWhere->Length());
		aWhere = aWhere->ReAllocL(aWhere->Des().MaxLength() + aWhat.Length());
	}

	aWhere->Des().Append(aWhat);
}

//  End of File
