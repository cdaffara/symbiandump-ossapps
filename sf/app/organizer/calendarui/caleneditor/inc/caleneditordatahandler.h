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
* The header file for CalenEditorDataHandler class.
*
*/

#ifndef CALENEDITORDATAHANDLER_H
#define CALENEDITORDATAHANDLER_H

// User Includes
#include "caleneditor_p.h"

// Forward Declarations
class AgendaEntry;
class CalenEditorDataHandler
{	
public:
	CalenEditorDataHandler(CalenEditorPrivate* calenEditor,
	                       AgendaEntry* editedEntry, 
	                       AgendaEntry* originalEntry);
	virtual ~CalenEditorDataHandler();
	
public:
	CalenEditorPrivate::Action shouldSaveOrDeleteOrDoNothing(bool launchCalendar) const;
	CalenEditorPrivate::Error checkErrorsForThisAndAll();
	void displayErrorMsg(int error);
	
private:
	AgendaEntry* editedEntry();
	AgendaEntry* originalEntry();
	bool isEdited() const;
	bool isSummaryEdited() const;
	bool isAllDayEdited() const;
	bool isLocationEdited() const;
	bool isStartDateTimeEdited() const;
	bool isEndDateTimeEdited() const;
	bool isAlarmEdited() const;
	bool isRepeatRuleEdited() const;
	bool isDescriptionEdited() const;
	bool nonTextItemsEdited() const;
	bool areTextItemsEmpty() const;
	bool areTextItemsCleared() const;
	bool isSummaryEmptied() const;
	bool isLocationEmptied() const;
	bool isDescriptionEmptied() const;
	bool durationGreaterThanRepeatIntervalError() const;
	CalenEditorPrivate::Error checkAlarmFieldsForErrors(bool series = false) const;
	bool isAlarmInAcceptablePeriod(CalenEditorPrivate::Error &error, const QDateTime &alarmTime,
								   const QDateTime &startTime) const;
	void dispalyErrorMsgByRepeatType();
	
private:
	CalenEditorPrivate* mCalenEditor;
	AgendaEntry* mEditedEntry;
	AgendaEntry* mOriginalEntry;
};

#endif // CALENEDITORDATAHANDLER_H

// End of file	--Don't remove this.
