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
* Description: Header file for AgendaEntry and utility classes.
*
*/

#ifndef AGENDAENTRY_H
#define AGENDAENTRY_H

// System includes
#include <QObject>

// User includes
#include "agendainterfacedefines.h"

// Forward declarations
class QDate;
class QDateTime;
class AgendaAlarmPrivate;
class AgendaAttendeePrivate;
class AgendaCategoryPrivate;
class AgendaEntryPrivate;
class AgendaRepeatRulePrivate;
class AgendaGeoValuePrivate;

class XQCALENDAR_EXPORT AgendaAttendee
{
public:
	enum ParticipantRole {
		RequiredParticipant = 0,
		OptionalParticipant,
		NonParticipant,
		ChairParticipant,
		ParticipantRoleUnknown = -1
	};

	enum StatusType {
		StatusNeedsAction = 0,
		StatusAccepted,
		StatusTentative,
		StatusConfirmed,
		StatusDeclined,
		StatusCompleted,
		StatusDelegated,
		StatusInProcess,
		StatusUnknown = -1
	};

	AgendaAttendee();
	virtual ~AgendaAttendee();
	AgendaAttendee(const QString& address);
	AgendaAttendee(const AgendaAttendee& other);

	void setAddress(const QString& address);
	void setCommonName(const QString& name);
	void setResponseRequested(bool responseRequested);
	void setRole(ParticipantRole role);
	void setStatus(StatusType status);

	QString address() const;
	QString commonName() const;
	bool responseRequested() const;
	ParticipantRole role() const;
	StatusType status() const;

	bool isNull() const;

	AgendaAttendee& operator=(const AgendaAttendee& other);
	bool operator==(const AgendaAttendee& other) const;
	bool operator!=(const AgendaAttendee& other) const;

private:
	void detach();

private:
	AgendaAttendeePrivate* d;
};

class XQCALENDAR_EXPORT AgendaCategory
{
public:
	enum CategoryType {
		AppointmentCategory,
		BusinessCategory,
		EducationCategory,
		HolidayCategory,
		MeetingCategory,
		MiscellaneousCategory,
		PersonalCategory,
		PhoneCallCategory,
		SickDayCategory,
		SpecialOccasionCategory,
		TravelCategory,
		VacationCategory,
		ExtendedCategory,
		UnknownCategory = -1
	};

	AgendaCategory();
	AgendaCategory(CategoryType category);
	AgendaCategory(const AgendaCategory& other);
	virtual ~AgendaCategory();

	void setCategory(CategoryType categoryType);
	CategoryType category() const;

	void setExtendedCategoryName(const QString& name);
	QString extendedCategoryName() const;

	bool isNull() const;

	AgendaCategory& operator=(const AgendaCategory& other);
	bool operator==(const AgendaCategory& other) const;
	bool operator!=(const AgendaCategory& other) const;

private:
	void detach();

private:
	AgendaCategoryPrivate* d;
};


class XQCALENDAR_EXPORT AgendaAlarm
{
public:
	static const int MaxFileNameLength = 256;

	AgendaAlarm();
	AgendaAlarm(const AgendaAlarm& other);
	~AgendaAlarm();

	void setTimeOffset(int minutes);
	void setAlarmSoundName(const QString& alarmSoundName);
	QString alarmSoundName() const;
	int timeOffset() const;

	bool isNull() const;

	AgendaAlarm& operator=(const AgendaAlarm& other);
	bool operator==(const AgendaAlarm& other) const;
	bool operator!=(const AgendaAlarm& other) const;

private:
	void detach();

private:
	AgendaAlarmPrivate* d;
};

class XQCALENDAR_EXPORT AgendaGeoValue
{
public:

	AgendaGeoValue();
	AgendaGeoValue(const AgendaGeoValue& other);
	~AgendaGeoValue();

	void setLatLong(double latitude, double longitude);
	void getLatLong(double& latitude, double& longitude) const;

	bool isNull() const;

	AgendaGeoValue& operator=(const AgendaGeoValue& other);
	bool operator==(const AgendaGeoValue& other) const;
	bool operator!=(const AgendaGeoValue& other) const;

private:
	void detach();

private:
	AgendaGeoValuePrivate* d;
};

class XQCALENDAR_EXPORT AgendaRepeatRule
{
public:

	enum RuleType {
		InvalidRule = 0,
		DailyRule,
		WeeklyRule,
		MonthlyRule,
		YearlyRule
	};

	enum Day {
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday,
		InvalidDay = -1
	};

	enum Month {
		January,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December,
		InvalidMonth = -1
	};

	AgendaRepeatRule();
	AgendaRepeatRule(RuleType type);
	AgendaRepeatRule(const AgendaRepeatRule& other);
	~AgendaRepeatRule();

	void setType(RuleType type);
	RuleType type() const;

	int count() const;

	void setInterval(int interval);
	int interval() const;

	void setUntil(const QDateTime& date);
	QDateTime until() const;

	void setRepeatRuleStart(const QDateTime& dateTime);
	QDateTime repeatRuleStart() const;

	void setWeekStart(Day day);
	Day weekStart() const;

	void setByDay(const QList<Day>& days);
	QList<Day> byDay() const;

	void setByMonthDay(const QList<int>& monthDays);
	QList<int> byMonthDay() const;

	void setByMonth(const QList<Month>& months);
	QList<Month> byMonth() const;

	bool isNull() const;
	AgendaRepeatRule& operator=(const AgendaRepeatRule& other);
	bool operator==(const AgendaRepeatRule& other) const;
	bool operator!=(const AgendaRepeatRule& other) const;

private:
	void detach();

private:
	AgendaRepeatRulePrivate* d;
};

class XQCALENDAR_EXPORT AgendaEntry
{
public:
	enum Type {
		TypeAppoinment,
		TypeTodo,
		TypeEvent,
		TypeReminder,
		TypeAnniversary,
		TypeNote,
		TypeUnknown = -1
	};

	enum Method {
		MethodNone,
		MethodPublish,
		MethodRequest,
		MethodReply,
		MethodAdd,
		MethodCancel,
		MethodRefresh,
		MethodCounter,
		MethodDeclineCounter,
		MethodUnknown = -1
	};

	enum Status {
		Tentative,
		Confirmed,
		Cancelled,
		TodoNeedsAction,
		TodoCompleted,
		TodoInProcess,
		NullStatus,

		VCalAccepted,
		VCalNeedsAction,
		VCalSent,
		VCalDeclined,
		VCalDelegated,

		StatusUnknown = -1
	};

	AgendaEntry();
	AgendaEntry(Type type);
	AgendaEntry(const AgendaEntry& other);
	virtual ~AgendaEntry();

	void addAttendee(const AgendaAttendee& attendee);
	void addCategory(const AgendaCategory& category);

	bool setStartAndEndTime(
			const QDateTime& startTime, const QDateTime& endTime);
	QDateTime startTime() const;
	QDateTime endTime() const;

	QList<AgendaAttendee>& attendees();
	const QList<AgendaAttendee>& attendees() const;
	QList<AgendaCategory>& categories();
	const QList<AgendaCategory>& categories() const;

	void setDescription(const QString& description);
	QString description() const;

	void setType(Type type);
	Type type() const;

	void setAlarm(const AgendaAlarm& alarm);
	AgendaAlarm alarm() const;

	void setLocation(const QString& location);
	QString location() const;

	void setSummary(const QString& summary);
	QString summary() const;

	void setMethod(Method method);
	Method method() const;

	void setRepeatRule(const AgendaRepeatRule& repeatRule);
	AgendaRepeatRule repeatRule() const;

	void setPriority(int priority);
	int priority() const;

	void setRDates(const QList<QDate>& rdates);
	QList<QDate> rDates() const;

	void setRecurrenceId(QDateTime& recTime);
	QDateTime recurrenceId();

	bool isNull() const;

	ulong id() const;
	bool isTimedEntry();
	bool isRepeating();

	QDateTime lastModifiedDateTime() const;
	void setLastModifiedDateTime(const QDateTime& dateTime);

	Status status() const;
	void setStatus(Status status);

	uint favourite() const;
	void setFavourite(uint favourite);

	void setCompletedDateTime(const QDateTime& dateTime);
	QDateTime completedDateTime() const;

	void setDTStamp(const QDateTime& dateTime);
	QDateTime dtStamp() const;
	
	void setGeoValue(const AgendaGeoValue& geoValue);
	AgendaGeoValue geoValue() const;
	void clearGeoValue() const;
	
	int durationInSecs() const;
	AgendaEntry& operator=(const AgendaEntry& other);
	bool operator==(const AgendaEntry& other) const;
	bool operator!=(const AgendaEntry& other) const;

private:
	void detach();

private:
	AgendaEntryPrivate* d;
	friend class AgendaUtilPrivate;
};

#endif // AGENDAENTRY_H

// End of file	--Don't remove this.
