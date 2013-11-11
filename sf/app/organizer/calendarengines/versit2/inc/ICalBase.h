#ifndef ICALBASE_H
#define ICALBASE_H/*
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
* Description:   Holds the definition of CICalBase.
*
*/



// System includes
#include <e32base.h>	// CBase

// Forward declarations.
class CICalContentLineReader;
class CICalContentLineWriter;
class CICalProperty;
class CICalComponent;
class CICalValue;

/**
Abstract base class of all classes representing iCalendar components.
@publishedPartner
*/
class CICalBase : public CBase
	{
public:	// Enumeration.
	enum TICalComponentType
		{
		/** An invalid or X-custom component. */
		EICalInvalid,
		/** A VCALENDAR component.*/
		EICalCalendar,
		/** A VEVENT component.*/
		EICalEvent,
		/** A VTODO component.*/
		EICalTodo,
		/** A VJOURNAL component.*/
		EICalJournal,
		/** A VALARM component.*/
		EICalAlarm,
		/** A VFREEBUSY component.*/
		EICalFreeBusy,
		/** A VTIMEZONE component.*/
		EICalTimeZone,
		/** A STANDARD component.*/
		EICalStandard,
		/** A DAYLIGHT component.*/
		EICalDaylight
		};
public: // Enumeration.
	enum TICalMethod
		{
		/** No method specified. */
		EMethodNone,
		/** The RFC2445-defined method value 'Publish'. */
		EMethodPublish,
		/** The RFC2445-defined method value 'Request'. */
		EMethodRequest,
		/** The RFC2445-defined method value 'Reply'. */
		EMethodReply, 
		/** The RFC2445-defined method value 'Add'. */
		EMethodAdd,
		/** The RFC2445-defined method value 'Cancel'. */
		EMethodCancel,
		/** The RFC2445-defined method value 'Refresh'. */
		EMethodRefresh,
		/** The RFC2445-defined method value 'Counter'. */
		EMethodCounter,
		/** The RFC2445-defined method value 'DeclineCounter'. */
		EMethodDeclineCounter
		};
protected:
	enum TICalComponentMethodBitMasks
		{
		EMaskEventNone				= 1 << 0,
		EMaskEventPublish			= 1 << 1,
		EMaskEventRequest			= 1 << 2,
		EMaskEventReply				= 1 << 3,
		EMaskEventAdd				= 1 << 4,
		EMaskEventCancel			= 1 << 5,
		EMaskEventRefresh			= 1 << 6,
		EMaskEventCounter			= 1 << 7,
		EMaskEventDeclineCounter	= 1 << 8,
		EMaskTodoNone				= 1 << 9,
		EMaskTodoPublish			= 1 << 10,
		EMaskTodoRequest			= 1 << 11,
		EMaskTodoReply				= 1 << 12,
		EMaskTodoAdd				= 1 << 13,
		EMaskTodoCancel				= 1 << 14,
		EMaskTodoRefresh			= 1 << 15,
		EMaskTodoCounter			= 1 << 16,
		EMaskTodoDeclineCounter		= 1 << 17,
		EMaskJournalNone			= 1 << 18,
		EMaskJournalPublish			= 1 << 19,
		EMaskJournalAdd				= 1 << 20,
		EMaskJournalCancel			= 1 << 21,
		EMaskFreeBusyNone			= 1 << 22,
		EMaskFreeBusyPublish		= 1 << 23,
		EMaskFreeBusyRequest		= 1 << 24,
		EMaskFreeBusyReply			= 1 << 25,
		EMaskAlarmAny				= 1 << 26,
		EMaskTimezoneAny			= 1 << 27,
		EMaskTimezoneIntervalAny	= 1 << 28
		};
	enum TICalComponentMethodFlags
		{
		EICalAttendeeFlags =		EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventRefresh		|
									EMaskEventCounter		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoRefresh		|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalCancel		|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
		
		EICalDtStampFlags =			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventRefresh		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoRefresh		|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalDtStartFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalOrganizerFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventRefresh		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalSummaryFlags =			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCounter		|
									EMaskTodoCounter		|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalUIDFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventRefresh		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoRefresh		|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalRecurrenceIdFlags = 	EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventCancel		|
									EMaskEventRefresh		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoCancel			|
									EMaskTodoRefresh		|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalCancel,
		
		EICalSequenceFlags =		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalAttachFlags =			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalCategoriesFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalClassFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalCommentFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventRefresh		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalContactFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalCreatedFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalDescriptionFlags = 	EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalDtEndFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyRequest	|
									EMaskFreeBusyReply,
									
		EICalFreeBusyFlags = 		EMaskFreeBusyPublish	|
									EMaskFreeBusyReply,
									
		EICalDueFlags =				EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter,
									
		EICalDurationFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter,
									
		EICalExDateFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
		
		EICalExRuleFlags =			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalGeoFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter,
									
		EICalLastModifiedFlags = 	EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalLocationFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter,
		
		EICalPercentCompleteFlags = EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter,
									
		EICalPriorityFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel		    |
									EMaskTodoPublish        |
									EMaskTodoDeclineCounter,
									
		EICalRDateFlags	= 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
						
		EICalRelatedToFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalRequestStatusFlags = 	EMaskEventRequest		|
									EMaskEventCounter		|
									EMaskEventDeclineCounter|
									EMaskTodoReply			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskFreeBusyReply,
		
		EICalResourcesFlags = 		EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter,
									
		EICalRRuleFlags	= 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalStatusFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel,
									
		EICalTranspFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter,
									
		EICalUrlFlags = 			EMaskEventPublish		|
									EMaskEventRequest		|
									EMaskEventReply			|
									EMaskEventAdd			|
									EMaskEventCancel		|
									EMaskEventCounter		|
									EMaskTodoPublish		|
									EMaskTodoRequest		|
									EMaskTodoReply			|
									EMaskTodoAdd			|
									EMaskTodoCancel			|
									EMaskTodoCounter		|
									EMaskTodoDeclineCounter	|
									EMaskJournalPublish		|
									EMaskJournalAdd			|
									EMaskJournalCancel		|
									EMaskFreeBusyPublish	|
									EMaskFreeBusyReply,
		
		
		
		
	//	EICalClassFlags			= 0b00000001111010111111010111111,
		

		
		};
public:	// Destruction.
	~CICalBase();
	
public:	// Methods.
	IMPORT_C CICalProperty& AddPropertyL(const TDesC& aName, const TDesC& aValue);
	IMPORT_C CICalProperty& AddPropertyL(const TDesC& aName, CICalValue* aValue);
	IMPORT_C CICalComponent& AddComponentL(TICalComponentType aType);
	
	IMPORT_C const RPointerArray<CICalComponent>& Components() const;
	IMPORT_C const RPointerArray<CICalProperty>& Properties() const;
	
	IMPORT_C const TDesC& TypeStringL() const;
	IMPORT_C TICalComponentType Type() const;
	
	IMPORT_C TBool ComponentExists(TICalComponentType aType) const;
	IMPORT_C const CICalProperty* FindProperty(const TDesC& aName) const;
		
	void InternalizeL(CICalContentLineReader& aReader);
	void ExternalizeL(CICalContentLineWriter& aWriter) const;
	
	static TICalComponentType TypeFromNameL(const TDesC& aName);

protected:	// Construction.
	CICalBase();
	
protected:	// Methods.
	const TDesC& TypeStringL(TICalComponentType aType) const;
	virtual TBool ValidatePropertyImpl(const TDesC& aName) const = 0;
	TBool ValidateProperty(const TDesC& aName) const;	
	virtual TBool ValidateComponent(TICalComponentType aType) const = 0;
	
private:	// Methods.
	CICalComponent* CreateComponentL(TICalComponentType aType);
	CICalProperty* CreatePropertyL(const TDesC& aName);

protected:	// Attributes.
	RPointerArray<CICalComponent> iComponents;
	RPointerArray<CICalProperty> iProperties;
	
	TICalComponentType iComponentType;
	TICalMethod iMethod;
	TUint32 iComponentMethodBitMask;		
	};

#endif	// ICALBASE_H

// End of File
