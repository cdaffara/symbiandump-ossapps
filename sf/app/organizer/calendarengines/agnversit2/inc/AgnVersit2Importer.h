#ifndef AGNVERSIT2IMPORTER_H
#define AGNVERSIT2IMPORTER_H/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements the import functionality of CAgnVersit2
*
*/



// System includes.
#include <calentry.h>		// for CCalEntry::TType
#include "ICalRuleSegment.h"

// User includes.
#include "MAgnImpUtil.h"

// Forward class declarations.
class CAgnVersit2StringProvider;
class CICal;
class CICalComponent;
class CICalProperty;
class CTzRules;
class RReadStream;

/**
Implementation class for CAgnVersit2::ImportL.
@internalTechnology
*/
class CAgnVersit2Importer : public CBase, public MAgnImpUtil
	{
private:	// Types
	/**
	Nested class used internally to maintain a set of rules searchable by TZID.
	@internalTechnology
	*/
	class CTzNamedRules : public CBase
		{
	public:	// Construction/destruction
		CTzNamedRules();
		~CTzNamedRules();

	public:	// Members
		CTzRules* iRules;
		HBufC* iName;
		};

public:		// Construction/destruction.
	static CAgnVersit2Importer* NewL(CAgnVersit2StringProvider& aStringProvider);
	static CAgnVersit2Importer* NewLC(CAgnVersit2StringProvider& aStringProvider);
	~CAgnVersit2Importer();

public:		// Methods.
	void ImportL(RPointerArray<CCalEntry>& aEntries, RReadStream& aReadStream, MAgnImportObserver& aObserver);

private:	// Construction.
	CAgnVersit2Importer(CAgnVersit2StringProvider& aStringProvider);
	void ConstructL();

private:	// Methods.
	void ImportICalL(RPointerArray<CCalEntry>& aEntries, CICal& aCal);

	// Import component methods.
	void ImportComponentL(const CICal& aCal, const CICalComponent& aComponent, RPointerArray<CCalEntry>& aEntries);
	void ImportAlarmL(const CICalComponent& aComponent, CCalEntry& aEntry, const HBufC8& aUid, const TCalTime* aStartTime);
	void ImportEntryL(const CICal& aCal, const CICalComponent& aComponent, RPointerArray<CCalEntry>& aEntries, HBufC8* aUid, CCalEntry::TType aType);
	CCalEntry* CreateEntryLC(const CICal& aCal, const CICalComponent& aComponent, HBufC8* aUid, CCalEntry::TType aType);
	void ImportPropertyL(CCalEntry& aEntry, const CICalProperty& aProperty);
	void ImportTimezoneL(const CICalComponent& aTimezone);
	void ImportTimezoneIntervalL(const CICalComponent& aInterval, CTzRules& aRules);
	
	// Import property methods.
	void ImportCategoriesL(const CICalProperty& aProperty, CCalEntry& aEntry) const;
	void ImportClassL(const CICalProperty& aProperty, CCalEntry& aEntry);
	TBool ImportRRuleL(const CICalProperty& aProperty, CCalEntry& aEntry, const TCalTime& aStartTime);
    
    // Helper methods.
	void GetCalTimeL(const CICalProperty& aProperty, TCalTime& aTime, TValueType aValueType, TInt aValue = 0);
	TInt FindRuleSegment(const RPointerArray<CICalRuleSegment>& aRules, CICalRuleSegment::TSegmentType aType) const;
	const CTzRules* FindTzRule(const TDesC& aName) const;
	void ReportErrorL(MAgnImportObserver::TImpError aType, const TDesC8& aUid, const TDesC& aContext, TBool aCanContinue = ETrue);

private:	// Members.
	CAgnVersit2StringProvider& iStringProvider;
	MAgnImportObserver* iImportObserver;
	MAgnImportObserver::TImpResponse iResponse;
	RPointerArray<CTzNamedRules> iTzRules;
	const CTzRules* iCurrentTzRules;	// This is not owned by the class and should not be deleted by the class.
	TBool iNeedsTzRules;
	};
	
#endif // AGNVERSIT2IMPORTER_H

// End of file.
