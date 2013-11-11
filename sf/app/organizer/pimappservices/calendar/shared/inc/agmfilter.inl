// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


// ---------------------------- TAgnFilter -------------------------------------
inline TBool TAgnFilter::AreTimedApptsIncluded() const
/** Tests whether the filter includes or excludes timed appointments.

@return True=included, False=excluded. */
	{ return (iFilter & CalCommon::EIncludeAppts); }

inline TBool TAgnFilter::AreRemindersIncluded() const
/** Tests whether the filter includes or excludes reminders

@return True=included, False=excluded. */
	{ return (iFilter & CalCommon::EIncludeReminder); }

inline TBool TAgnFilter::AreEventsIncluded() const
/** Tests whether the filter includes or excludes events.

@return True=included, False=excluded. */
	{ return (iFilter & CalCommon::EIncludeEvents); }

inline TBool TAgnFilter::AreAnnivsIncluded() const
/** Tests whether the filter includes or excludes anniversaries.

@return True=included, False=excluded. */
	{ return (iFilter & CalCommon::EIncludeAnnivs); }

inline TBool TAgnFilter::AreCompletedTodosIncluded() const
	{ return (iFilter & CalCommon::EIncludeCompletedTodos); }

inline TBool TAgnFilter::AreIncompletedTodosIncluded() const
	{ return (iFilter & CalCommon::EIncludeIncompletedTodos); }
inline TBool TAgnFilter::AreNonTodosIncluded() const
/** Tests whether the filter includes any entry type other than to-dos
@return True=included, False=excluded. */
	{
	return ((iFilter & CalCommon::EIncludeAppts)   ||
			(iFilter & CalCommon::EIncludeReminder) ||
			(iFilter & CalCommon::EIncludeEvents)   ||
			(iFilter & CalCommon::EIncludeAnnivs)   ||
			(iFilter & CalCommon::EIncludeNotes ));
	}

inline TBool TAgnFilter::RptNextInstanceOnly() const
/** Tests whether the filter only includes the next instance of a repeating entry.

@return True=only include the next instance of a repeating entry, False=include 
all instances. */
	{ return (iFilter & CalCommon::EIncludeRptsNextInstanceOnly); }

inline TBool TAgnFilter::AreAlarmedOnlyIncluded() const
/** Tests whether the filter only includes alarmed entries.

@return True=Only include alarmed entries, False=include all 
entries. */
	{ return (iFilter & CalCommon::EIncludeAlarmedOnly); }

/*
 * Tests whether to match folded character only when searching for the entry's text.
 *
 * @since 7.0
 * @return True=Match folded character only
 */
inline TBool TAgnFilter::IsFoldedOnlySearch() const
	{ return (iSearchBehaviour & CalCommon::EFoldedTextSearch); }

/*
 * Tests whether to match exact text only when searching for the entry's text.
 *
 * @since 7.0
 * @return True=Match exact text only
 */
inline TBool TAgnFilter::IsExactTextOnlySearch() const
	{ return (iSearchBehaviour & CalCommon::EExactTextSearch); }

inline TBool TAgnFilter::IsEntryLocationSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchEntryLocation); }

inline TBool TAgnFilter::IsEntryDescriptionSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchEntryDescription); }

inline TBool TAgnFilter::IsAttendeeAddressSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchAttendeeAddress); }

inline TBool TAgnFilter::IsAttendeeSentByAddressSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchAttendeeSentByAddress); }
	
inline TBool TAgnFilter::IsAttendeeCommonNameSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchAttendeeCommonName); }
	
inline TBool TAgnFilter::IsOrganizerAddressSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchOrganizerAddress); }
	
inline TBool TAgnFilter::IsOrganizerSentByAddressSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchOrganizerSentByAddress); }

inline TBool TAgnFilter::IsOrganizerCommonNameSearched() const
	{ return (iSearchBehaviour & CalCommon::ESearchOrganizerCommonName); }

inline TBool TAgnFilter::StartTimeOverlapOnly() const
	{ return (iFilter & CalCommon::EInstanceStartTimeWithinRange); }

inline TBool TAgnFilter::EndTimeOverlapOnly() const
	{ return (iFilter & CalCommon::EInstanceEndTimeWithinRange); }

inline TBool TAgnFilter::ExcludeInstanceAdjoiningRange() const
	{ return (iFilter & CalCommon::EExcludeInstanceAdjoiningRange); }

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
inline TBool TAgnFilter::IsAlarmedInstanceSearch() const
	{ return iAlarmedInstanceSearch; }

inline void TAgnFilter::SetAlarmedInstanceSearch(const TBool aAlarmedInstanceSearch)
	{ iAlarmedInstanceSearch = aAlarmedInstanceSearch; }

inline TBool TAgnFilter::FloatingInstanceOnly() const
	{ return iFloatingInstanceOnly; }

inline void TAgnFilter::SetFloatingInstanceOnly(const TBool aFloatingInstanceOnly)
	{ iFloatingInstanceOnly = aFloatingInstanceOnly; }

#endif

/**
 * @brief check for Notes included in the filter
 * 
 * @return ETrue if notes are included otherwise return EFalse 
 */
inline TBool TAgnFilter::AreNotesIncluded() const
	{ return (iFilter & CalCommon::EIncludeNotes); }


