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

#ifndef __AGMCONTENT_H__
#define __AGMCONTENT_H__

#include "calcontent.h"

class RReadStream;
class RWriteStream;

NONSHARABLE_CLASS(CAgnContent) : public CBase
/**
@capability None
@internalAll
@released
*/
	{
public:
	IMPORT_C CAgnContent();
	IMPORT_C ~CAgnContent();
	IMPORT_C void SetContentL(TDesC8* aSmallContent, TDesC8* aContentMIMEType, CCalContent::TDisposition aContentDisposition);
	IMPORT_C const TDesC8& Content() const;
	IMPORT_C const TDesC8& MimeType() const;
	IMPORT_C CCalContent::TDisposition Type() const;
	IMPORT_C CAgnContent* CloneL() const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
protected:
	CAgnContent(CCalContent::TDisposition aType);

	void SetContent(TDesC8* aContent);
	void SetMimeType(TDesC8* aMimeType);

private:	
	TDesC8* iContent;
	TDesC8* iMimeType;
	CCalContent::TDisposition iType;
	};


#endif // __AGMCONTENT_H__
 
