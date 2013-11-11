// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cmsgtestpropertwatcher.h
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __CMSGTESTPROPERTYWATCHER_H__
#define __CMSGTESTPROPERTYWATCHER_H__

#include <e32base.h>
#include <e32property.h>

class MMsgTestPropertyWatcher;

class CMsgTestPropertyWatcher : public CActive
	{
public:
	IMPORT_C static CMsgTestPropertyWatcher* NewL(TUid aCategory, TUint aKey, MMsgTestPropertyWatcher& aCallback);
	IMPORT_C virtual ~CMsgTestPropertyWatcher();
    
	IMPORT_C RProperty& Property();
	
protected:
	void ConstructL(TUid aCategory, TUint aKey,  MMsgTestPropertyWatcher& aCallback);
	CMsgTestPropertyWatcher();
     
private:
	void RunL();
	void DoCancel();
    
public:
	//TBD: these should be private.
	TUid iCategory;
	TUint iKey;

private:
	MMsgTestPropertyWatcher* iCallback;

protected:
	RProperty iProperty;
	};

#endif //__CMSGTESTPROPERTYWATCHER_H__
