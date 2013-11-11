// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGMCALENDARINFO_H__
#define __AGMCALENDARINFO_H__

#include <e32base.h>
#include <e32hashtab.h> 

#include "calcalendarinfo.h"

class RReadStream;
class RWriteStream;
class CAgnCalendarInfoProperty;

class CAgnCalendarInfo : public CBase
	{
public:
	IMPORT_C static CAgnCalendarInfo* NewL();
	IMPORT_C virtual ~CAgnCalendarInfo();
	
	IMPORT_C void SetIsValid(TBool aIsValid);
	IMPORT_C TBool IsValid() const;
	
	IMPORT_C void SetFileNameL(const TDesC& aFileName);
	IMPORT_C const TDesC& FileNameL() const;
	
	IMPORT_C void SetNameL(const TDesC& aName);
	IMPORT_C const TDesC& NameL() const;
	
	IMPORT_C void SetDescriptionL(const TDesC& aDescription);
	IMPORT_C const TDesC& DescriptionL() const;
	
	IMPORT_C void SetColor(TRgb aColor);
	IMPORT_C TRgb Color() const;
	
	IMPORT_C void SetEnabled(TBool aEnabled);
	IMPORT_C TBool Enabled() const;
	
	IMPORT_C void FileInternalizeL(RReadStream& aReadStream);
	IMPORT_C void FileExternalizeL(RWriteStream& aWriteStream) const;
	
	IMPORT_C void IpcInternalizeL(RReadStream& aReadStream);
	IMPORT_C void IpcExternalizeL(RWriteStream& aWriteStream) const;
	
	IMPORT_C void ExternalizePropertiesL(CFileStore& iFileStore, const CAgnCalendarInfo* aOldCalendarInfo) const;
	
	IMPORT_C CDesC8Array* PropertyKeysL() const;
	IMPORT_C virtual const TDesC8& PropertyValueL(const TDesC8& aKey) const;
    IMPORT_C void SetPropertyL(const TDesC8& aKey, const TDesC8& aValue);
    IMPORT_C void RemovePropertyL(const TDesC8& aKey);
	
protected:
    IMPORT_C CAgnCalendarInfo();
    IMPORT_C void ConstructL();
    
protected:
	TBool iIsValid;
    HBufC* iFileName;
    HBufC* iName;
    HBufC* iDescription;
    TRgb iColor;
    TBool iEnabled;
    
    TLinearOrder<CAgnCalendarInfoProperty> iLinearOrder;
    RPointerArray<CAgnCalendarInfoProperty> iProperties;
	};

	
#endif // __AGMCALENDARINFO_H__
