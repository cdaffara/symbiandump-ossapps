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
* Description: Header file for utilities and private implementaions
* used by AgendaEntry.
*
*/

#ifndef AGENDAENTRY_P_H
#define AGENDAENTRY_P_H

// System includes
#include <QTime>

// User includes
#include <agendaentry.h>

class AgendaAttendeePrivate
{
public:
	AgendaAttendeePrivate();

public:
	QAtomicInt ref;

private:
	friend class AgendaAttendee;
	QString m_address;
	AgendaAttendee::ParticipantRole m_role;
	AgendaAttendee::StatusType m_status;
	QString m_commonName;

	bool m_responseRequested;
};

class AgendaCategoryPrivate
{
public:
	AgendaCategoryPrivate();

public:
	QAtomicInt ref;

private:
	friend class AgendaCategory;
	AgendaCategory::CategoryType m_categoryType;
	QString m_extendedCategory;
};

class AgendaAlarmPrivate
{
public:
	AgendaAlarmPrivate();

public:
	QAtomicInt ref;

private:
	friend class AgendaAlarm;
	QString m_soundName;
	int m_timeOffset;
};

class AgendaGeoValuePrivate
{
public:
	AgendaGeoValuePrivate();

public:
	QAtomicInt ref;

private:
	friend class AgendaGeoValue;
	double mLatitude;
	double mLongitude;
};

class AgendaRepeatRulePrivate
{
public:
	AgendaRepeatRulePrivate();

public:
	QAtomicInt ref;

private:
	friend class AgendaRepeatRule;
	AgendaRepeatRule::RuleType m_repeatRuleType;
	QList<AgendaRepeatRule::Day> m_days;
	QList<AgendaRepeatRule::Month> m_months;
	QList<int> m_monthDays;
	AgendaRepeatRule::Day m_weekStartDay;
	QDateTime m_startDate;
	QDateTime m_untilDate;
	int m_interval;
};

class AgendaEntryPrivate
{
public:
	AgendaEntryPrivate();
	void addCategory(const AgendaCategory& category);
	bool deleteCategory(const AgendaCategory& category);
	void addAttendee(const AgendaAttendee& attendee);
	bool deleteAttendee(const AgendaAttendee& attendee);
	void setToDoCompleted(bool complete, QDateTime& dateTime);

public:
	QAtomicInt ref;

private:
	friend class AgendaEntry;
	friend class AgendaUtilPrivate;

	QList<AgendaAttendee> m_attendees;
	QList<AgendaCategory> m_categories;

	QDateTime m_startTime;
	QDateTime m_endTime;

	QString m_summaryText;
	QString m_description;
	QString m_location;

	ulong m_id;
	int m_priority;
	int m_favourite;

	AgendaEntry::Type m_entryType;
	AgendaEntry::Method m_method;
	AgendaEntry::Status m_entryStatus;

	AgendaAlarm m_alarm;
	AgendaGeoValue m_geoValue;
	AgendaRepeatRule m_repeatRule;

	QList<QDate> m_rDates;
	QDateTime m_recurrenceId;
	QDateTime m_lastModTime;
	QDateTime m_completedDateTime;
	QDateTime m_dtStamp;
};

#endif // AGENDAENTRY_P_H

// End of file  --Don't remove this.
