/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header file for AgendaUtil class.
*
*/

#ifndef AGENDAUTIL_H
#define AGENDAUTIL_H

// System includes
#include <QObject>
#include <QList>
#include <QDateTime>

// User includes
#include "agendaentry.h"
#include "agendainterfacedefines.h"

// Forward declarations
class AgendaUtilPrivate;

class XQCALENDAR_EXPORT AgendaUtil : public QObject
{
	Q_OBJECT

public:

	enum Error {
		NoError = 0,
		OutOfMemoryError,
		AlreadyInUse,
		UnknownError = -1
	};

	enum FilterFlags {
		IncludeAppointments = 0x01,
		IncludeReminders = 0x02,
		IncludeEvents = 0x04,
		IncludeAnniversaries = 0x08,
		IncludeCompletedTodos = 0x10,
		IncludeIncompletedTodos = 0x20,
		IncludeAlarmedOnly = 0x40,
		IncludeRptsNextInstanceOnly = 0x80,
		IncludeNotes=0x300,
		IncludeAll=IncludeAppointments
		| IncludeReminders
		| IncludeEvents
		| IncludeAnniversaries
		| IncludeCompletedTodos
		| IncludeIncompletedTodos
		| IncludeNotes
	};

	enum RecurrenceRange {
		ThisOnly,
		ThisAndAll,
		ThisAndFuture,
		ThisAndPrior
	};

	AgendaUtil(QObject* parent = 0);
	~AgendaUtil();

	ulong store(AgendaEntry &entry, AgendaUtil::RecurrenceRange range = ThisAndAll,
			QDateTime instanceOriginalDateTime = QDateTime());
	ulong cloneEntry(const AgendaEntry& entry, AgendaEntry::Type type);
	bool deleteEntry(ulong id);
	void deleteRepeatedEntry(
			AgendaEntry& entry, AgendaUtil::RecurrenceRange range);
	QList<ulong> entryIds(
			AgendaUtil::FilterFlags filter = AgendaUtil::IncludeAll);
	QList<AgendaEntry> fetchAllEntries(
			AgendaUtil::FilterFlags filter = AgendaUtil::IncludeAll);
	QList<AgendaEntry> fetchEntriesInRange(
			QDateTime rangeStart, QDateTime rangeEnd,
			AgendaUtil::FilterFlags filter = AgendaUtil::IncludeAll);
	QList<AgendaEntry> createEntryIdListForDay(
			QDateTime day,
			AgendaUtil::FilterFlags filter = AgendaUtil::IncludeAll);
	AgendaEntry fetchById(ulong id);

	int importvCalendar(const QString& fileName, AgendaEntry& entry);
	bool exportAsvCalendar(const QString& fileName, long int calendarEntryId);

	AgendaUtil::Error error() const;
	void setCompleted(AgendaEntry& entry, bool complete, QDateTime& dateTime);
	void deleteEntries(
			QDateTime& start, QDateTime& end,
			AgendaUtil::FilterFlags filter = AgendaUtil::IncludeAll);
	AgendaEntry parentEntry(AgendaEntry& entry);
	void getPreviousInstanceTimes(AgendaEntry& entry, QDateTime& startTime, 
	                              QDateTime& endTime);
	void getNextInstanceTimes(AgendaEntry& entry, QDateTime& startTime, 
	                          QDateTime& endTime);
	void markDatesWithEvents(QDateTime rangeStart, QDateTime rangeEnd,
						 AgendaUtil::FilterFlags filter, QList<QDate>& dates);
	bool areNoEntriesInCalendar();
	static QDateTime minTime();
	static QDateTime maxTime();

	static bool isWorkdaysRepeatingEntry(const AgendaRepeatRule& repeatRule);
	
Q_SIGNALS:
	void entriesChanged(QList<ulong> ids);
	void entryAdded(ulong id);
	void entryDeleted(ulong id);
	void entryUpdated(ulong id);
	void entriesDeleted(int status);
	void entryViewCreationCompleted(int status);
	void instanceViewCreationCompleted(int status);

private:
	friend class AgendaUtilPrivate;
	AgendaUtilPrivate* d;
};

#endif // AGENDAUTIL_H

// End of file	--Don't remove this.
