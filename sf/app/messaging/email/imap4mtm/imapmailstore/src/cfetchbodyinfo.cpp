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


#include "cfetchbodyinfo.h"
#include "imappaniccodes.h"

EXPORT_C CFetchBodyInfo*  CFetchBodyInfo::NewLC(TMsvId aPartId) 
	{
	CFetchBodyInfo* self = new (ELeave) CFetchBodyInfo(aPartId);
	CleanupStack::PushL(self);
	return self;
	}
	
EXPORT_C CFetchBodyInfo*  CFetchBodyInfo::NewL(TMsvId aPartId) 
	{
	CFetchBodyInfo* self = CFetchBodyInfo::NewLC(aPartId);
	CleanupStack::Pop(self);
	return self;;
	}
	
CFetchBodyInfo::CFetchBodyInfo(TMsvId aPartId)
 : iPartId(aPartId)
	{
	}	

CFetchBodyInfo::~CFetchBodyInfo()
	{
	delete iRelativePath;
	}

EXPORT_C void CFetchBodyInfo::SetSizeToFetch(TInt32 aSize)
	{
	iSizeToFetch=aSize;
	}

EXPORT_C void CFetchBodyInfo::SetIsText(TBool aIsText)
	{
	iIsText=aIsText;
	}

EXPORT_C void CFetchBodyInfo::SetPartId(const TMsvId aPartId)
	{
	iPartId=aPartId;
	}

EXPORT_C void CFetchBodyInfo::SetRelativePathL(const TDesC8& aRelativePath)
	{
	HBufC8* newRelativePath = aRelativePath.AllocL();
	delete iRelativePath;
	iRelativePath = newRelativePath;
	}

EXPORT_C void CFetchBodyInfo::SetBodyPartRemainingSize(const TInt32 aSize)
	{
	iBodyPartRemainingSize=aSize;
	}
	
EXPORT_C TInt32 CFetchBodyInfo::BodyPartRemainingSize()
	{
	return iBodyPartRemainingSize;
	}

EXPORT_C TInt32 CFetchBodyInfo::SizeToFetch()
	{
	return iSizeToFetch;
	}

EXPORT_C TBool CFetchBodyInfo::IsText()
	{
	return iIsText;
	}

EXPORT_C TMsvId  CFetchBodyInfo::PartId()
	{
	return iPartId;
	}

EXPORT_C HBufC8* CFetchBodyInfo::RelativePath() const
	{
	return iRelativePath;
	}

EXPORT_C void CFetchBodyInfo::SetContentTransferEncoding(TImEncodingType aContentTransferEncoding)
	{
	iContentTransferEncoding=aContentTransferEncoding;
	}
	
EXPORT_C void CFetchBodyInfo::SetCharsetId(const TUint aCharsetId)
	{
	iCharsetId=aCharsetId;
	}

EXPORT_C TImEncodingType CFetchBodyInfo::ContentTransferEncoding() const
	{
	return iContentTransferEncoding;
	}
	
EXPORT_C TUint CFetchBodyInfo::CharsetId() const
	{
	return iCharsetId;
	}
	
EXPORT_C void CFetchBodyInfo::SetCaf(CImCaf* aCaf)
	{
	__ASSERT_DEBUG(aCaf!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EMailStoreFetchBodyBadCAFHandle));
	iCaf=aCaf;
	}

EXPORT_C void CFetchBodyInfo::ResetCaf()
	{
	iCaf=NULL;
	}
	
EXPORT_C CImCaf* CFetchBodyInfo::Caf() const
	{
	return iCaf;
	}

EXPORT_C TInt32 CFetchBodyInfo::BytesFetched()
	{
	return iBytesFetched;	
	}
	
EXPORT_C void CFetchBodyInfo::IncrementBytesFetched(TInt32 aByteCount)
	{
	iBytesFetched+=aByteCount;	
	}
