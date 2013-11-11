/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Definition of CalenEditorDataHandler class.
*
*/

// System Includes
#include <hbmessagebox.h>
#include <hbdataformmodelitem.h>

// User Includes
#include "caleneditordatahandler.h"
#include "calendateutils.h"
#include "calenagendautils.h"
#include <agendaentry.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "caleneditordatahandlerTraces.h"
#endif


/*!
	\class CalenEditorDataHandler
 */
/*!
	Constructor.
 */

CalenEditorDataHandler::CalenEditorDataHandler(CalenEditorPrivate* calenEditor,
                                               AgendaEntry* editedEntry, 
                    	                       AgendaEntry* originalEntry)
: mCalenEditor(calenEditor),mEditedEntry(editedEntry), mOriginalEntry(originalEntry)
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_CALENEDITORDATAHANDLER_ENTRY );
	
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_CALENEDITORDATAHANDLER_EXIT );
}

/*!
	 Destructor
 */
CalenEditorDataHandler::~CalenEditorDataHandler()
{
	OstTraceFunctionEntry0( DUP1_CALENEDITORDATAHANDLER_CALENEDITORDATAHANDLER_ENTRY );
	// Nothing Yet
	OstTraceFunctionExit0( DUP1_CALENEDITORDATAHANDLER_CALENEDITORDATAHANDLER_EXIT );
}

/*!
	 Returns pointer for edited entry
	 \return pointer for edited entry
 */
AgendaEntry* CalenEditorDataHandler::editedEntry()
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_EDITEDENTRY_ENTRY );
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_EDITEDENTRY_EXIT );
	return mEditedEntry;
}

/*!
	 Returns pointer for original entry
	 \return pointer for original entry
 */
AgendaEntry* CalenEditorDataHandler::originalEntry()
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ORIGINALENTRY_ENTRY );
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_ORIGINALENTRY_EXIT );
	return mOriginalEntry;
}

/*!
	 Checks if user entered data violates the permitted attributes
	 \return Error Error indicating the violated parameter
 */
CalenEditorPrivate::Error CalenEditorDataHandler::checkErrorsForThisAndAll()
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_CHECKERRORSFORTHISANDALL_ENTRY );
	//TODO :  Remove implementation once handle all repeating errors
	const QDateTime startTime = mEditedEntry->startTime();
	const QDateTime endTime = mEditedEntry->endTime();

	// Repeating entry checks:
	if (mEditedEntry->isRepeating()) {
		// Check that repeat until date is a) later than start date
		//										(for new notes)
		//									b) not before start date
		//										(for existing notes)
		QDateTime repeatUntilDay = mEditedEntry->repeatRule().until();

		QDateTime repeatStartDay;

		// if new note or old note isnt repeating
		// edited.repeatUntil date must be greater than edited.start date
		// else
		// if IsRepeatRuleEdited or IsStartDateTimeEdited 
		// (either one above will make a new rule in which edited.startdate
		// is the start date)
		// edited.repeatUntil must be greater than edited.start date
		// else
		// edited.repeatUntil must be greater than start date on disk

		if (mCalenEditor->isNewEntry() || mOriginalEntry->repeatRule().isNull()
				|| isRepeatRuleEdited() || isStartDateTimeEdited()) {
			// We don't have an rrule so we can't get the rrule start date,
			// or user has edited a field that will cause new start date to be
			// used in the new rule.
			// Use the edited entry's start date.
			repeatStartDay = startTime;
		} else {
			// original rule is valid and new rule will not be created
			repeatStartDay = mOriginalEntry->repeatRule().repeatRuleStart();
		}

		if (durationGreaterThanRepeatIntervalError()) {
			OstTraceFunctionExit0( CALENEDITORDATAHANDLER_CHECKERRORSFORTHISANDALL_EXIT );
			return CalenEditorPrivate::
							CalenEditorErrorDurationGreaterThanRepeatInterval;
		}
		OstTraceFunctionExit0( DUP1_CALENEDITORDATAHANDLER_CHECKERRORSFORTHISANDALL_EXIT );
		return CalenEditorPrivate::CalenEditorErrorNone;
	}
	OstTraceFunctionExit0( DUP2_CALENEDITORDATAHANDLER_CHECKERRORSFORTHISANDALL_EXIT );
	return CalenEditorPrivate::CalenEditorErrorNone;
}

/*!
	Returns true if the entry has been modified, false otherwise.
	\return true if the entry has been modified, false otherwise.
 */
bool CalenEditorDataHandler::isEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISEDITED_ENTRY );
	return (isSummaryEdited() ||
			isAllDayEdited() ||
			isLocationEdited() ||
			isStartDateTimeEdited() ||
			isEndDateTimeEdited() ||
			isAlarmEdited() ||
			isRepeatRuleEdited() ||
			isDescriptionEdited());
}

/*!
	Returns true if the summary has been edited, false otherwise.
	\return true if the summary has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isSummaryEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISSUMMARYEDITED_ENTRY );
	return (mOriginalEntry->summary() != mEditedEntry->summary());
}

/*!
	Returns true if the all day has been edited, false otherwise.
	\return true if the all day has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isAllDayEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISALLDAYEDITED_ENTRY );
	HbDataFormModelItem* alldayItem = mCalenEditor->allDayCheckBoxItem();
	if (alldayItem) {
		if (CalenAgendaUtils::isAlldayEvent(*mOriginalEntry)) {
			if (alldayItem->contentWidgetData("checkState")
			        == Qt::Checked) {
				OstTraceFunctionExit0( CALENEDITORDATAHANDLER_ISALLDAYEDITED_EXIT );
				return false;
			} else {
				OstTraceFunctionExit0( DUP1_CALENEDITORDATAHANDLER_ISALLDAYEDITED_EXIT );
				return true;
			}
		} else if (mOriginalEntry->type() == AgendaEntry::TypeAppoinment) {
			if (alldayItem->contentWidgetData("checkState")
			        == Qt::Checked) {
				OstTraceFunctionExit0( DUP2_CALENEDITORDATAHANDLER_ISALLDAYEDITED_EXIT );
				return true;
			} else {
				OstTraceFunctionExit0( DUP3_CALENEDITORDATAHANDLER_ISALLDAYEDITED_EXIT );
				return false;
			}
		}
	}
	OstTraceFunctionExit0( DUP4_CALENEDITORDATAHANDLER_ISALLDAYEDITED_EXIT );
	return false;
}

/*!
	Returns true if the location has been edited, false otherwise.
	\return true if the location has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isLocationEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISLOCATIONEDITED_ENTRY );
	return (mOriginalEntry->location() != mEditedEntry->location());
}

/*!
	Returns true if the start date/time has been edited, false otherwise.
	\return true if the start date/time has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isStartDateTimeEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISSTARTDATETIMEEDITED_ENTRY );
	return (mOriginalEntry->startTime() != mEditedEntry->startTime());
}

/*!
	Returns true if the end date/time has been edited, false otherwise.
	\return true if the end date/time has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isEndDateTimeEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISENDDATETIMEEDITED_ENTRY );
	return (mOriginalEntry->endTime() != mEditedEntry->endTime());
}

/*!
	Returns true if the alarm has been edited, false otherwise.
	\return true if the alarm has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isAlarmEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISALARMEDITED_ENTRY );
	return (mOriginalEntry->alarm() != mEditedEntry->alarm());
}

/*!
	Returns true if the repeat rule has been edited, false otherwise.
	\return true if the repeat rule has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isRepeatRuleEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISREPEATRULEEDITED_ENTRY );
	if ((mOriginalEntry->repeatRule().type() == AgendaRepeatRule::InvalidRule)
	        && (mEditedEntry->repeatRule().type()
	                == AgendaRepeatRule::InvalidRule)) {
		OstTraceFunctionExit0( CALENEDITORDATAHANDLER_ISREPEATRULEEDITED_EXIT );
		return false;
	} else {
		return (mOriginalEntry->repeatRule() != mEditedEntry->repeatRule());
	}
}

/*!
	Returns true if the Description field has been edited, false otherwise.
	\return true if the Description field has been edited, false otherwise.
 */
bool CalenEditorDataHandler::isDescriptionEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISDESCRIPTIONEDITED_ENTRY );
	return (mOriginalEntry->description() != mEditedEntry->description());
}

/*!
	Returns true if any of the non-text items (e.g. time fields) of the entry
	have been edited, false otherwise.
	\return true if any of the non text items edited,false otherwise.
 */
bool CalenEditorDataHandler::nonTextItemsEdited() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_NONTEXTITEMSEDITED_ENTRY );
	return (isAllDayEdited() ||
			isStartDateTimeEdited() ||
			isEndDateTimeEdited() ||
			isAlarmEdited() ||
			isRepeatRuleEdited());
}

/*!
	Returns true if summary && location && description text items are all empty,
	false otherwise.
	\return true if text items are all empty,false otherwise.
 */
bool CalenEditorDataHandler::areTextItemsEmpty() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ARETEXTITEMSEMPTY_ENTRY );
	return (mEditedEntry->summary().isEmpty()
	        && mEditedEntry->location().isEmpty()
	        && mEditedEntry->description().isEmpty());
}

/*!
	Returns true if the user cleared the text in the location and summary items,
	false otherwise.
	\return true if summary & location items are cleared,false otherwise.
 */
bool CalenEditorDataHandler::areTextItemsCleared() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ARETEXTITEMSCLEARED_ENTRY );
	if (mEditedEntry->summary().isEmpty() &&
			mEditedEntry->location().isEmpty() &&
			mEditedEntry->description().isEmpty()) {
		if (isSummaryEmptied() 
				|| isLocationEmptied() 
				|| isDescriptionEmptied()) {
			OstTraceFunctionExit0( CALENEDITORDATAHANDLER_ARETEXTITEMSCLEARED_EXIT );
			return true;
		}
	}
	OstTraceFunctionExit0( DUP1_CALENEDITORDATAHANDLER_ARETEXTITEMSCLEARED_EXIT );
	return false;
}

/*!
	Returns true the summary was not empty in original && is empty
	in the edited note,false otherwise
	\return true if summary is cleared in edited note,false otherwise
 */
bool CalenEditorDataHandler::isSummaryEmptied() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISSUMMARYEMPTIED_ENTRY );
	return (!mOriginalEntry->summary().isEmpty()
	        && mEditedEntry->summary().isEmpty());
}

/*!
	Returns true the location was not empty in original && is empty
	in the edited note,false otherwise
	\return true if location is cleared in edited note,false otherwise
 */
bool CalenEditorDataHandler::isLocationEmptied() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISLOCATIONEMPTIED_ENTRY );
	return (!mOriginalEntry->location().isEmpty()
	        && mEditedEntry->location().isEmpty());
}

/*!
	Returns true the description was not empty in original && is empty
	in the edited note,false otherwise
	\return true if description is cleared in edited note,false otherwise
 */
bool CalenEditorDataHandler::isDescriptionEmptied() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISDESCRIPTIONEMPTIED_ENTRY );
	return (!mOriginalEntry->description().isEmpty()
	        && mEditedEntry->description().isEmpty());
}

/*!
	Works out whether the entry should be deleted, saved, 
	or whether no action should be taken.
	\return enum Action
 */
CalenEditorPrivate::Action CalenEditorDataHandler::shouldSaveOrDeleteOrDoNothing(bool launchCalendar) 
																		const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_SHOULDSAVEORDELETEORDONOTHING_ENTRY );
	// Need to save the entry if third party calls editor to launch the
	// calendar after that. So, that entry will be new entry adn we assume
	// that client launches editor with some prefilled text items
	if (!isEdited() && !launchCalendar) {
		// Not edited at all OR
		// Only added space characters to text fields but not
		// edited the non-text items
		// no need to save the entry
		OstTraceFunctionExit0( CALENEDITORDATAHANDLER_SHOULDSAVEORDELETEORDONOTHING_EXIT );
		return CalenEditorPrivate::ActionNothing;
	}
	// new entry is edited
	if (mCalenEditor->isNewEntry()) {
		// Subject && Location && Description are text items.
		// If text items as a whole is not empty, we can save the note
		// If text items as a whole is empty, we can still save the note
		// since we edited "non-text" fields
		if (!nonTextItemsEdited() && areTextItemsEmpty()) {
			OstTraceFunctionExit0( DUP1_CALENEDITORDATAHANDLER_SHOULDSAVEORDELETEORDONOTHING_EXIT );
			return CalenEditorPrivate::ActionNothing;
		} else {
			OstTraceFunctionExit0( DUP2_CALENEDITORDATAHANDLER_SHOULDSAVEORDELETEORDONOTHING_EXIT );
			return CalenEditorPrivate::ActionSave;
		}
	}
	// Save the note, since the text fields contain something
	OstTraceFunctionExit0( DUP4_CALENEDITORDATAHANDLER_SHOULDSAVEORDELETEORDONOTHING_EXIT );
	return CalenEditorPrivate::ActionSave;
}

/*!
	Returns true if the duration of instances of the meeting is greater than
	the repeat period of the series, false otherwise.
	\return true if duration of meeting is greater than repeat period, false
	otherwise
 */
bool CalenEditorDataHandler::durationGreaterThanRepeatIntervalError() const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_DURATIONGREATERTHANREPEATINTERVALERROR_ENTRY );
	bool isError = false;
	switch (mEditedEntry->repeatRule().type()) {
		case AgendaRepeatRule::DailyRule: {
			int durationDays =
			        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
			isError = durationDays >= 1;
		}
		break;
		case AgendaRepeatRule::WeeklyRule: {
			int durationDays =
			        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
			if (mEditedEntry->repeatRule().interval() == 1) {
				isError = durationDays >= 7;
			} else {
				isError = durationDays >= 14;
			}
		}
		break;
		case AgendaRepeatRule::MonthlyRule: {
			if (mEditedEntry->endTime()
			        >= (mEditedEntry->startTime().addMonths(1))) {
				isError = true;
			}
		}
		break;
		case AgendaRepeatRule::YearlyRule: {
			if (mEditedEntry->endTime()
			        >= (mEditedEntry->startTime().addYears(1))) {
				isError = true;
			}
		}
		break;
		default:
			// Not repeating, no error
			isError = false;
			break;
	}
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_DURATIONGREATERTHANREPEATINTERVALERROR_EXIT );
	return isError;
}

/*!
	Check the alarm fields for errors.
	\return the error if found, or CalenEditorErrorNone if no error found.
 */
CalenEditorPrivate::Error CalenEditorDataHandler::checkAlarmFieldsForErrors(
															bool series) const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_CHECKALARMFIELDSFORERRORS_ENTRY );
	CalenEditorPrivate::Error error = CalenEditorPrivate::CalenEditorErrorNone;
	// If alarm not active, no check
	if (!mEditedEntry->alarm().isNull()) {
		int alarm = mEditedEntry->alarm().timeOffset();
		QDateTime startTime = mEditedEntry->startTime();
		QDateTime alarmTime;
		if (alarm > 0) {
			alarmTime = startTime.addSecs(-alarm * 60);
		} else {
			alarmTime = startTime.addSecs(alarm * 60);
		}
		QDateTime currentTime = CalenDateUtils::now();
		if (isAlarmInAcceptablePeriod(error, alarmTime, startTime)) {
			if (!series && (alarmTime < currentTime)) {
				// dont let non-repeating future entries have alarms in past
				error = CalenEditorPrivate::CalenEditorErrorAlarmTimePast;
			}
		}
	}
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_CHECKALARMFIELDSFORERRORS_EXIT );
	return error;
}

/*!
	Checks if AlarmTime is 31 days from StartTime, 
	then sets Error to CalenEditorErrorAlarmDateTooManyDaysBeforeNote and
	returns false
	Checks if AlarmTime is later StartTime, 
	then sets Error to CalenEditorErrorAlarmTimeLaterThanNote and returns false
	\return true if error untouched, false otherwise 
 */
bool CalenEditorDataHandler::isAlarmInAcceptablePeriod(CalenEditorPrivate::Error &error,
										const QDateTime &alarmTime,
										const QDateTime &startTime) const
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_ISALARMINACCEPTABLEPERIOD_ENTRY );
	QDateTime upperLimit = startTime;

	QDateTime lowerLimit = startTime.addDays(-31);
	bool acceptable = true;
	if (alarmTime < lowerLimit) {
		acceptable = false;
		error = CalenEditorPrivate::CalenEditorErrorAlarmDateTooManyDaysBeforeNote;
	} else {
		if (alarmTime > upperLimit) {
			acceptable = false;
			error = CalenEditorPrivate::CalenEditorErrorAlarmTimeLaterThanNote;
		}
	}
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_ISALARMINACCEPTABLEPERIOD_EXIT );
	return acceptable;
}

/*!
	Display the given error msg
	\param error Error value for which message has to be displayed
 */
void CalenEditorDataHandler::displayErrorMsg(int error)
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_DISPLAYERRORMSG_ENTRY );
	QString errorMsg = QString::Null();

	switch (error) {
		case CalenEditorPrivate::CalenEditorErrorAlarmTimeLaterThanNote:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_alarm_later_than_note"));
			break;
		case CalenEditorPrivate::CalenEditorErrorAlarmTimePast:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_the_time_for_the_note_alarm"));
			break;
		case CalenEditorPrivate::CalenEditorErrorAlarmDateTooManyDaysBeforeNote:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_alarm_date_is_too_past"));
			break;
		case CalenEditorPrivate::CalenEditorErrorRepeatUntilEarlierThanNote:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_repeat_until_has_to_be_later"));
			break;
		case 
		CalenEditorPrivate::CalenEditorErrorDurationGreaterThanRepeatInterval:
			dispalyErrorMsgByRepeatType();
			break;
		case CalenEditorPrivate::CalenEditorErrorStopTimeEarlierThanStartTime:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_note_ends_before_than_starts"));
			break;
		default:
			break;
	}
	if (!errorMsg.isNull()) {
		HbMessageBox::information(errorMsg);
	}
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_DISPLAYERRORMSG_EXIT );
}

/*!
	 Displays error message related to repeat fields
 */
void CalenEditorDataHandler::dispalyErrorMsgByRepeatType()
{
	OstTraceFunctionEntry0( CALENEDITORDATAHANDLER_DISPALYERRORMSGBYREPEATTYPE_ENTRY );
	QString errorMsg = QString::Null();

	int durationDays =
	        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
	int numDaysEntrySpan = durationDays + 1;
	// Add the text proper text ids
	switch (mEditedEntry->repeatRule().type()) {
		case AgendaRepeatRule::DailyRule:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_daily"));
			break;
		case AgendaRepeatRule::WeeklyRule:
			if (mEditedEntry->repeatRule().interval() == 1) {
				errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_weekly"));
			} else {
				errorMsg.append("meeting duration is more than 2 weeks");
			}
			break;
		case AgendaRepeatRule::MonthlyRule:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_monthly"));
			break;
		case AgendaRepeatRule::YearlyRule:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_yearly"));
			break;
		default:
			break;
	}
	if (!errorMsg.isNull()) {
		HbMessageBox::information(errorMsg.arg(numDaysEntrySpan));
	}
	OstTraceFunctionExit0( CALENEDITORDATAHANDLER_DISPALYERRORMSGBYREPEATTYPE_EXIT );
}

// End of file	--Don't remove this.
