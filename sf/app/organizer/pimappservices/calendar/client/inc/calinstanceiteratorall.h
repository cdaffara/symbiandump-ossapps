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
//

#ifndef __CALINSTANCEITERATORALL_H__
#define __CALINSTANCEITERATORALL_H__

#include "calinstanceiterator.h"
#include "calinstanceviewimpl.h"

class CCalSessionImpl;
class CCalFindInstanceSettings;
class CCalInstance;
class CCalLiteEntry;
class TAgnInstanceId;

/** Instance Iterator for iterating though all the instances in the calendar.

@internalComponent
*/
NONSHARABLE_CLASS(CCalInstanceIteratorAll) : public CCalInstanceIterator
	{
public:
	static CCalInstanceIteratorAll* NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl);
	static CCalInstanceIteratorAll* NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId);
	~CCalInstanceIteratorAll();

	// From CCalInstanceIterator
	CCalInstance* NextL();
	CCalInstance* PreviousL();
	TBool HasMore() const;
	TInt Count() const;

protected:
	CCalInstanceIteratorAll(const CCalInstanceViewImpl& aInstanceViewImpl);
	void ConstructL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId);
	void ConstructL(const CCalFindInstanceSettings& aSettings);
		
	void SetFindInstanceSettingsL(const CCalFindInstanceSettings& aSettings);
	
	TBool HasMoreL() const;

private:
	TInt CountL() const;
	
protected:
	CArrayFixSeg<TAgnInstance>* iInstanceIdCache;
	TInt iCurrentIndex;
	
	// This is mutable so we can change the number of instances that are searched for in different cases
	TFindInstanceParams iFindInstanceParams; 
	};

#endif // __CALINSTANCEITERATORALL_H__
