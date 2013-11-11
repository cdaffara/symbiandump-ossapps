// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGMTZRULES_H_
#define __AGMTZRULES_H_

#include <s32std.h>

class CAgnTzRules : public CBase
	{
public:
	static CAgnTzRules* NewL();
	static CAgnTzRules* NewL(const CTzRules& aTzRules);
	static CAgnTzRules* NewL(RReadStream& aStream, TBool aFromBuffer);
	~CAgnTzRules();
	
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	
	//Externalize to buffer used by client to server requests.
	void WriteToBufferL(RWriteStream& aStream);
	
	//Internalize from buffer used by client to server requests.
	void ReadBufferL(RReadStream& aStream);
	
	IMPORT_C const CTzRules* TzRules() const;
	IMPORT_C void SetTzRules(CTzRules* aTzRules);

	CAgnTzRules* CloneL() const;
	CTzRules* CloneTzRulesL() const;
	
	void CopyL(const CAgnTzRules& aAgnTzRules);
	
	IMPORT_C TStreamId TzZoneStreamId() const;
	IMPORT_C TBool HasValidTzZoneStreamId() const;
	IMPORT_C void SetTzZoneStreamId(TStreamId aStreamId);
	
	IMPORT_C TBool SystemTzRule() const;
	IMPORT_C void SetSystemTzRule(TBool aIsSystemRule);
	
	inline TBool SharedWithTzIndex() const;
	inline void SetSharedWithTzIndex(TBool aSharedWithTzIndex);

	IMPORT_C TBool UseCurrentSystemTzRules() const;
	IMPORT_C void SetUseCurrentSystemTzRules(TBool aUseCurrentSystemTzRules);

private:
	CAgnTzRules();
	void ConstructL();
	void ConstructL(const CTzRules& aTzRules);
	
	void Reset();
	void DoInternalizeL(RReadStream& aStream);
	
private:
	
	CTzRules*   iTzRules;
	
	//Stream id for iTimeZone.
	TStreamId   iTzZoneStreamId;
	
	//If iTimeZone is one of system time zones in tz database
	TBool       iTzZoneIsSystemRule;
	
	//If the tz rule is shared and own by tz rule index.
	TBool 		iSharedInTzIndex;
	
	//If the tz rule is using current system tz rule.
	TBool 		iUseCurSysTzZone;	
	};

#endif /*__AGMTZRULES_H_*/
