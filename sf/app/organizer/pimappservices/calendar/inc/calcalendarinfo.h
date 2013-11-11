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

#ifndef __CALCALENDARINFO_H__
#define __CALCALENDARINFO_H__

#include <e32base.h>
#include <gdi.h>

#include "calcommon.h"

class CCalCalendarInfoImpl;
class CDesC8Array;

/** The calendar info class where metadata of a calendar file is set and retrieved.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalCalendarInfo) : public CBase
	{
public:
	IMPORT_C static CCalCalendarInfo* NewL();
	IMPORT_C ~CCalCalendarInfo();
	
	IMPORT_C TBool IsValid() const;
	
	IMPORT_C const TDesC& FileNameL() const;

	IMPORT_C void SetNameL(const TDesC& aName);
	IMPORT_C const TDesC& NameL() const;

	IMPORT_C void SetDescriptionL(const TDesC& aDescription);
	IMPORT_C const TDesC& DescriptionL() const;
	
	IMPORT_C void SetColor(TRgb aColor);
	IMPORT_C TRgb Color() const;

	IMPORT_C void SetEnabled(TBool aEnabled); 
	IMPORT_C TBool Enabled() const;
	
	IMPORT_C CDesC8Array* PropertyKeysL() const;
	IMPORT_C const TDesC8& PropertyValueL(const TDesC8& aKey) const;
	IMPORT_C void SetPropertyL(const TDesC8& aKey, const TDesC8& aValue);
	IMPORT_C void RemovePropertyL(const TDesC8& aKey);

public:
	static CCalCalendarInfo* NewL(CCalCalendarInfoImpl* aCalendarInfoImpl);
	const CCalCalendarInfoImpl& Impl() const;
	
private:
    CCalCalendarInfo();
    void ConstructL();
    void ConstructL(CCalCalendarInfoImpl* aCalendarInfoImpl);
    
private:
	CCalCalendarInfoImpl* iImpl;
	};

#endif
