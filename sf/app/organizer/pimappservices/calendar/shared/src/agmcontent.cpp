// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agmcontent.h"
#include <s32stor.h>


EXPORT_C CAgnContent::CAgnContent() : iType(CCalContent::EDispositionUnknown)
	{
	}

CAgnContent::CAgnContent(CCalContent::TDisposition aType) : iType(aType)
	{
	}

EXPORT_C CAgnContent::~CAgnContent()
	{
	delete iContent;
	delete iMimeType;
	}
	
/** Takes ownership of aSmallContent and aContentMimeType */
EXPORT_C void CAgnContent::SetContentL(TDesC8* aSmallContent, TDesC8* aContentMimeType, CCalContent::TDisposition aType)
	{
	delete iContent;
	iContent = aSmallContent;

	delete iMimeType;
	iMimeType = aContentMimeType;

	iType = aType;
	}


EXPORT_C const TDesC8& CAgnContent::Content() const
	{
	if (iContent == NULL)
		{
		return KNullDesC8();
		}
	
	return *iContent;
	}


EXPORT_C const TDesC8& CAgnContent::MimeType() const
	{
	if (iMimeType == NULL)
		{
		return KNullDesC8();
		}
	
	return *iMimeType;
	}


EXPORT_C CCalContent::TDisposition CAgnContent::Type() const
	{
	return iType;
	}


EXPORT_C CAgnContent* CAgnContent::CloneL() const
	{
	CAgnContent* clone = new (ELeave) CAgnContent();
	CleanupStack::PushL(clone);
	
	HBufC8* content = NULL;
	if (iContent)
		{
		content = iContent->AllocL();
		CleanupStack::PushL(content);
		}
		
	HBufC8* mimeType = NULL;
	if (iMimeType)
		{
		mimeType = iMimeType->AllocL();
		CleanupStack::PushL(mimeType);
		}
		
	clone->SetContentL(content, mimeType, iType); // takes ownership of content & mimeType.
	
	if (mimeType != NULL)
		{
		CleanupStack::Pop(mimeType);
		}
	if (content != NULL)
		{
		CleanupStack::Pop(content);
		}
		
	CleanupStack::Pop(clone);
	return clone;
	}


EXPORT_C void CAgnContent::InternalizeL(RReadStream& aStream)
	{
	delete iContent;
	iContent = NULL;
	iContent = HBufC8::NewL(aStream, KMaxTInt);
	
	delete iMimeType;
	iMimeType = NULL;
	iMimeType = HBufC8::NewL(aStream, KMaxTInt);
	
	TInt32 disposition;
	aStream >> disposition;
	iType = static_cast<CCalContent::TDisposition>(disposition);
	}
	

EXPORT_C void CAgnContent::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << Content(); // Use accessor so a KNullDesC8 is stored if content not set.
	aStream << MimeType(); // Use accessor so a KNullDesC8 is stored if mime type not set.
	aStream.WriteInt32L(iType);
	}

void CAgnContent::SetContent(TDesC8* aContent)
	{
	delete iContent;
	iContent = aContent;
	}

void CAgnContent::SetMimeType(TDesC8* aMimeType)
	{
	delete iMimeType;
	iMimeType = aMimeType;
	}
