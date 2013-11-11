#ifndef AGNVERSIT2EXPORTER_H
#define AGNVERSIT2EXPORTER_H/*
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
* Description:   Implements the export functionality of CAgnVersit2
*
*/



// Agenda includes.
#include <calentry.h>	// for CCalEntry::TReplicationStatus
#include <caluser.h>	// for CCalAttendee::TCalRole

// Forward class declarations.
class CAgnVersit2StringProvider;
class CICal;
class CICalComponent;
class CICalProperty;
class CTzRules;
class MAgnExportObserver;
class RWriteStream;


/**
Implementation class for CAgnVersit2::ExportL.
@internalTechnology
*/
class CAgnVersit2Exporter : public CBase
	{
public:		// Construction/destruction.
	static CAgnVersit2Exporter* NewL(CAgnVersit2StringProvider& aStringProvider);
	static CAgnVersit2Exporter* NewLC(CAgnVersit2StringProvider& aStringProvider);

	~CAgnVersit2Exporter();

public:		// Methods.
	void ExportL(const CCalEntry& aEntry,RWriteStream& aWriteStream, TUint aExportFlags,MAgnExportObserver& aObserver);
	void ExportL(RPointerArray<CCalEntry>& aEntries,RWriteStream& aWriteStream, TUint aExportFlags,MAgnExportObserver& aObserver);
	

private:	// Construction.
	CAgnVersit2Exporter(CAgnVersit2StringProvider& aStringProvider);
		
	void ConstructL();
	
private:	// Methods.
	CICal* AddICalLC(const CCalEntry& aEntry) const;
	
	// Add Component methods.
	void AddAnnivL(CICalComponent& aAnniv, const CCalEntry& aEntry) const;
	void AddAlarmL(CICalComponent& aComponent, const CCalEntry& aEntry) const;
	void AddApptL(CICalComponent& aAppt, const CCalEntry& aEntry) const;
	void AddComponentL(CICal& aICal, const CCalEntry& aEntry) const;
	void AddEventL(CICalComponent& aEvent, const CCalEntry& aEntry) const;
	void AddReminderL(CICalComponent& aReminder, const CCalEntry& aEntry) const;
	void AddTimezoneL(CICal& aICal, const CCalEntry& aEntry);
	void AddTodoL(CICalComponent& aTodo, const CCalEntry& aEntry) const;
	
	// Add Property methods.
	void AddAttendeePropertiesL(CICalComponent& aComponent, const CCalEntry& aEntry) const;
	void AddCategoriesPropertyL(CICalComponent& aComponent, const CCalEntry& aEntry)const ;
	void AddClassPropertyL(CICalComponent& aComponent, const CCalEntry& aEntry) const;
	void AddExceptionDatePropertyL(CICalComponent& aComponent, const CCalEntry& aEntry) const;
	void AddIntegerPropertyL(CICalComponent& aComponent, TInt aInt, const TDesC& aProperty) const;
	void AddOrganizerPropertyL(CICalComponent& aComponent, const CCalEntry& aEntry) const;
	void AddTextPropertyL(CICalComponent& aComponent, const TDesC& aText, const TDesC& aProperty) const;
	void AddRDatePropertyL(CICalComponent& aComponent, const CCalEntry& aEntry) const;
	void AddRRulePropertyL(CICalComponent& aComponent, const CCalEntry& aEntry)const;
	CICalProperty& AddDateTimePropertyL(CICalComponent& aComponent, const TCalTime& aTime, const TDesC& aProperty) const;
	CICalProperty& AddFloatingDateTimePropertyL(CICalComponent& aComponent,	const TTime& aFloatingTime, const TDesC& aProperty) const;
	CICalProperty& AddUtcDateTimePropertyL(CICalComponent& aComponent, const TTime& aUtcTime, const TDesC& aProperty) const;
	
	// Helper methods.
	static const TDesC& ClassStringL(const CCalEntry::TReplicationStatus aStatus);
	const TDesC& CategoryStringL(const CCalCategory& aCategory) const;
	static const TDesC& RoleFromEnum(CCalAttendee::TCalRole aRole);
	static const TDesC& StatusFromEnum(CCalAttendee::TCalStatus aStatus);
	static const TDesC& DayFromInt(TUint8 aDayInt);
	static const TDesC&  DayFromTDay(TDay aDay);
	static TInt WeekNumFromDayOfMonth(TInt aDayOfMonth);
	
private:	// Members.
	MAgnExportObserver* iObserver;
	CAgnVersit2StringProvider& iStringProvider;
	RPointerArray<HBufC> iTzNameArray;
	RPointerArray<CTzRules> iTimeZoneArray;
	TUint iExportProperties;
	TInt iTimezoneIndex;
	};
	
#endif // AGNVERSIT2EXPORTER_H

// End of file.
