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
*
*/
#include <agendaentry.h>
#include "agendaentry_p.h"

#include <calalarm.h>
#include <QDateTime>

/****************************************************
 *
 * AgendaAttendee
 *
 ****************************************************/

/*!
    \class AgendaAttendee

    \brief Class representing an attendee of an event.
           Contains the status and the role information of the attendee.
           Contains information about whether or not a response was requested for this attendee.

*/

/*!
    \enum AgendaAttendee::ParticipantRole

    This enum defines the possible roles for a AgendaAttendee object
    in case of entry's type is event.
*/
/*! \var AgendaAttendee::ParticipantRole AgendaAttendee::RequiredParticipant
    A required participant of the event.
*/
/*! \var AgendaAttendee::ParticipantRole AgendaAttendee::OptionalParticipant
    An optional participant of the event
*/
/*! \var AgendaAttendee::ParticipantRole AgendaAttendee::NonParticipant
    A non-participant of the event
*/
/*! \var AgendaAttendee::ParticipantRole AgendaAttendee::ChairParticipant
    This participant will chair the event
*/
/*! \var AgendaAttendee::ParticipantRole AgendaAttendee::ParticipantRoleUnknown
    The role is unknown
*/

/*!
    \enum AgendaAttendee::StatusType

    This enum defines the possible statuses for a AgendaAttendee object.
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusNeedsAction
    Action is required by attendee
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusAccepted
    Attendee has accepted request
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusTentative
    Attendee has tentatively accepted the request
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusConfirmed
    Attendee's presence is confirmed
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusDeclined
    Attendee has declined request
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusCompleted
    The required action has been completed by attendee
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusDelegated
    Attendee has delegated the request to another person
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusInProcess
    A to-do action in the process of being completed
*/
/*! \var AgendaAttendee::StatusType AgendaAttendee::StatusUnknown
    The status is unknown
*/

/*!
    Creates an attendee.
*/
AgendaAttendee::AgendaAttendee()
    : d(NULL)
{
}

/*!
    Constructs a new attendee using given \a address.

    \param address The address of the attendee
*/
AgendaAttendee::AgendaAttendee(const QString& address)
    : d(new AgendaAttendeePrivate)
{
    d->m_address = address;
    d->ref.ref();
}

/*!
    Destroys the attendee.
*/
AgendaAttendee::~AgendaAttendee()
{
    if (d && !d->ref.deref())
    {
        delete d;
    }
}

/*!
    Creates a copy of a given attendee.

    \param other attendee to be copied.
*/
AgendaAttendee::AgendaAttendee(const AgendaAttendee& other)
    : d(other.d)
{
    if (d) {
        d->ref.ref();
    }
}

/*!
    Sets the email address of the attendee.

    \param address The email address of the attendee
    \sa address()
*/
void AgendaAttendee::setAddress(const QString& address)
{
    detach();
    d->m_address = address;
}

/*!
    Gets the email address of the attendee.

    \return The email address of the attendee on success;
            otherwise returns a null QString.
    \sa setAddress()
*/
QString AgendaAttendee::address() const
{
    return d ? d->m_address : QString();
}

/*!
    Sets the common name of the attendee.

    \param name Common name
    \sa commonName()
*/
void AgendaAttendee::setCommonName(const QString& name)
{
    detach();
    d->m_commonName = name;
}

/*!
    Gets the common name of the attendee.

    \return The common name of the attendee on success;
            otherwise returns a null QString.
    \sa setCommonName()
*/
QString AgendaAttendee::commonName() const
{
    return d ? d->m_commonName : QString();
}

/*!
    Sets the role of the attendee.

    \param role The role
    \sa role()
*/
void AgendaAttendee::setRole(AgendaAttendee::ParticipantRole role)
{
    detach();
    d->m_role = role;
}

/*!
    Gets the role of the attendee.

    \return The role
    \sa setRole()
*/
AgendaAttendee::ParticipantRole AgendaAttendee::role() const
{
    return d ? d->m_role : ParticipantRoleUnknown;
}

/*!
    Sets the status of the attendee.

    \param status Attendees's status
    \sa status()
 */
void AgendaAttendee::setStatus(AgendaAttendee::StatusType status)
{
    detach();
    d->m_status = status;
}

/*!
    Gets the status of the attendee.

    \return Attendees's status
    \sa status()
 */
AgendaAttendee::StatusType AgendaAttendee::status() const
{
    return d ? d->m_status : StatusUnknown;
}

/*!
    Sets whether or not a response is/was requested for this attendee.

    \param responseRequested True if a response is/was requested, false if not.
    \sa responseRequested()
 */
void AgendaAttendee::setResponseRequested(bool responseRequested)
{
    detach();
    d->m_responseRequested = responseRequested;
}

/*!
    Sets whether or not a response is/was requested for this attendee.

    \return True if a response is/was requested, false if not.
    \sa setResponseRequested()
 */
bool AgendaAttendee::responseRequested() const
{
    return d ? d->m_responseRequested : false;
}

/*!
    Returns true if the attendee is empty; otherwise returns false.

    \return true if the attendee is empty; otherwise returns false.
*/
bool AgendaAttendee::isNull() const
{
    return !d;
}

void AgendaAttendee::detach()
{
    if (!d) {
        d = new AgendaAttendeePrivate;
    } else {
        qAtomicDetach(d);
    }
}

/*!
    Assigns \a other to this attendee and returns a reference
    to this attendee.

    \param other the other attendee
    \return a reference to this attendee
*/
AgendaAttendee& AgendaAttendee::operator=(const AgendaAttendee& other)
{
    if (d == other.d)
    {
        return *this;
    }
    if (other.d)
    {
        other.d->ref.ref();
    }
    if (d && !d->ref.deref())
    {
        delete d;
    }
    d = other.d;
    return *this;
}


/*!
    Returns true if this AgendaAttendee is equal to the \a other
    info, otherwise returns false.
*/
bool AgendaAttendee::operator==(const AgendaAttendee& other) const
{
    if (d == other.d)
        return true;

    return (d->m_address == other.d->m_address
            && d->m_commonName == other.d->m_commonName
            && d->m_responseRequested == other.d->m_responseRequested
            && d->m_role == other.d->m_role
            && d->m_status == other.d->m_status);
}

/*!
    Returns true if this AgendaAttendee is not equal to the \a other
    info, otherwise returns false.
*/
bool AgendaAttendee::operator!=(const AgendaAttendee& other) const
{
    return !(*this == other);
}

AgendaAttendeePrivate::AgendaAttendeePrivate()
    : ref(1)
{
	m_status = AgendaAttendee::StatusConfirmed;
}

/****************************************************
 *
 * AgendaCategory
 *
 ****************************************************/

/*!
    \class AgendaCategory

    \brief Calendar categories can be associated with a calendar entry.
           There are 12 built in category types defined by CategoryType.
           Additionally, it is possible to define extended category
           types which are held as a string.
           Any number of categories can be specified for an entry.
           Calendar entry provides member functions for adding, deleting
           and fetching categories for an entry.
*/

/*!
    \enum AgendaCategory::CategoryType

    This enum defines the possible category types for a AgendaCategory object.
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::AppointmentCategory
    Appointment
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::BusinessCategory
    Business
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::EducationCategory
    Education
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::HolidayCategory
    Holiday
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::MeetingCategory
    Meeting
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::MiscellaneousCategory
    All other types
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::PersonalCategory
    Personal
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::PhoneCallCategory
    Phone call
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::SickDayCategory
    Sick day
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::SpecialOccasionCategory
    Special occasion
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::TravelCategory
    Travel
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::VacationCategory
    Vacation
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::ExtendedCategory
     Extended
*/
/*! \var AgendaCategory::CategoryType AgendaCategory::UnknownCategory
    Unknown
*/

/*!
    Creates a category.
*/
AgendaCategory::AgendaCategory()
    : d(NULL)
{
}

/*!
    Creates a copy of a given category.

   \param other catecory to be copied.
 */
AgendaCategory::AgendaCategory(const AgendaCategory& other) : d(other.d)
{
    if (d) {
        d->ref.ref();
    }
}

/*!
    Constructs a new category using given \a category.

    \param category The type of the category
*/
AgendaCategory::AgendaCategory(AgendaCategory::CategoryType category)
    : d(new AgendaCategoryPrivate)
{
    d->m_categoryType = category;
}

/*!
    Destroys the category.
*/
AgendaCategory::~AgendaCategory()
{
    if (d && !d->ref.deref())
    {
        delete d;
    }
}

/*!
    Sets the category type.

    \param categoryType The category type
    \sa category()
*/
void AgendaCategory::setCategory(AgendaCategory::CategoryType categoryType)
{
    detach();
    d->m_categoryType = categoryType;
}

/*!
    Sets the extended category name.

    \param name The entended category name
    \sa extendedCategoryName()
*/
void AgendaCategory::setExtendedCategoryName(const QString& name)
{
    detach();
    d->m_categoryType = AgendaCategory::ExtendedCategory;
    d->m_extendedCategory = name;
}

/*!
    Gets the extended category name.

    \return The entended category name
    \sa setExtendedCategoryName()
*/
QString AgendaCategory::extendedCategoryName() const
{
    return d ? d->m_extendedCategory : QString();
}

/*!
    Gets the category type.

    \return The category type
    \sa setCategory()
*/
AgendaCategory::CategoryType AgendaCategory::category() const
{
    return d ? d->m_categoryType : UnknownCategory;
}

void AgendaCategory::detach()
{
    if (!d) {
        d = new AgendaCategoryPrivate();
    } else {
        qAtomicDetach(d);
    }
}

/*!
    Assigns \a other to this category and returns a reference
    to this category.

    \param other the other category
    \return a reference to this category
*/
AgendaCategory& AgendaCategory::operator=(const AgendaCategory& other)
{
    if (d == other.d)
    {
        return *this;
    }
    if (other.d)
    {
        other.d->ref.ref();
    }
    if (d && !d->ref.deref())
    {
        delete d;
    }
    d = other.d;
    return *this;
}

/*!
    Returns true if this AgendaCategory is equal to the \a other
    info, otherwise returns false.
*/
bool AgendaCategory::operator==(const AgendaCategory& other) const
{
    if (d == other.d)
        return true;

    return (d->m_categoryType == other.d->m_categoryType
            && d->m_extendedCategory == other.d->m_extendedCategory);
}

/*!
    Returns true if this AgendaAttendee is not equal to the \a other
    info, otherwise returns false.
*/
bool AgendaCategory::operator!=(const AgendaCategory& other) const
{
    return !(*this == other);
}

/*!
    Returns true if the category is empty; otherwise returns false.

    \return true if the category is empty; otherwise returns false.
*/
bool AgendaCategory::isNull() const
{
    return !d;
}

AgendaCategoryPrivate::AgendaCategoryPrivate()
    : m_categoryType(AgendaCategory::UnknownCategory)
{
}

/****************************************************
 *
 * AgendaAlarm
 *
 ****************************************************/

/*!
    \class AgendaAlarm

    \brief Class representing an alarm.
           This can be assigned to a calendar entry.
*/

/*!
    Creates an alarm.
*/
AgendaAlarm::AgendaAlarm() : d(NULL)
{
}

/*!
    Creates a copy of a given alarm.

   \param other alarm to be copied.
*/
AgendaAlarm::AgendaAlarm(const AgendaAlarm& other) : d(other.d)
{
    if (d) {
        d->ref.ref();
    }
}

/*!
    Destroys the alarm.
*/
AgendaAlarm::~AgendaAlarm()
{
    if (d && !d->ref.deref())
    {
        delete d;
    }
}

/*! \var AgendaAlarm::MaxFileNameLength
    Maximum file lenght for the alarm
*/

/*!
    Sets a time offset for an alarm to occur prior to a scheduled event.
    The time offset is calculated as the number of minutes before the instance time of the event.
    It is possible to have a negative offset provided that the alarm is
    on the same day (in local time) as the instance time; this means the alarm occurs
    after the event.

    \param minutes The time offset in minutes for an alarm prior to an event.
    \sa timeOffset()
*/
void AgendaAlarm::setTimeOffset(int minutes)
{
    detach();
    d->m_timeOffset = minutes;
}

/*!
    Gets the time offset for the alarm to occur prior to an event.

    \return The alarm offset (in minutes) prior to an event on success;
            otherwise return -1.
    \sa setTimeOffset()
*/
int AgendaAlarm::timeOffset() const
{
    return d ? d->m_timeOffset : -1;
}

/*!
    Sets the name of the alarm sound.
    If this isn't set then the default alarm sound will be used.
    Note: if the length of the new alarm sound name is greater than MaxFileNameLength characters,
    it will be ignored.

    \param alarmSoundName Sound name
    \sa alarmSoundName()
*/
void AgendaAlarm::setAlarmSoundName(const QString& alarmSoundName)
{
    detach();
    d->m_soundName = alarmSoundName;
}

/*!
    Gets the name of the alarm sound.

    \return Sound name on success;
            otherwise returns a null QString.

    \sa setAlarmSoundName()
*/
QString AgendaAlarm::alarmSoundName() const
{
    return d ? d->m_soundName : QString();
}

/*!
	Returns true if the alarlm is empty; otherwise returns false.

	\return true if the alarm is empty; otherwise returns false.
 */
bool AgendaAlarm::isNull() const
	{
	return !d;
	}

/*!
	Assigns \a other to this alarm and returns a reference
	to this alarm.

	\param other the other alarm
	\return a reference to this alarm
 */
AgendaAlarm& AgendaAlarm::operator=(const AgendaAlarm& other)
{
	if (d == other.d) {
		return *this;
	}
	if (other.d) {
		other.d->ref.ref();
	}
	if (d && !d->ref.deref()) {
		delete d;
	}
	d = other.d;
	return *this;
}

/*!
	Returns true if this AgendaAlarm is equal to the \a other
	info, otherwise returns false.
 */
bool AgendaAlarm::operator==(const AgendaAlarm& other) const
{
	if (d == other.d) {
		return true;
	}

	if (!d || !other.d) {
		return false;
	}

	return (d->m_soundName == other.d->m_soundName
			&& d->m_timeOffset == other.d->m_timeOffset);
}

/*!
	Returns true if this AgendaAlarm is not equal to the \a other
	info, otherwise returns false.
 */
bool AgendaAlarm::operator!=(const AgendaAlarm& other) const
{
	return !(*this == other);
}

void AgendaAlarm::detach()
{
	if (!d) {
		d = new AgendaAlarmPrivate;
	} else {
		qAtomicDetach(d);
	}
}

AgendaAlarmPrivate::AgendaAlarmPrivate()
:ref(1)
{
	// Nothing to do.
}

/****************************************************
 *
 * AgendaGeoValue
 *
 ****************************************************/

/*!
    \class AgendaGeoValue

    \brief Class representing the geo values.
           This can be assigned to a calendar entry.
*/

/*!
    Creates a geo value.
*/
AgendaGeoValue::AgendaGeoValue() : d(NULL)
{
}

/*!
    Creates a copy of a given alarm.

   \param other alarm to be copied.
*/
AgendaGeoValue::AgendaGeoValue(const AgendaGeoValue& other) : d(other.d)
{
    if (d) {
        d->ref.ref();
    }
}

/*!
    Destroys the alarm.
*/
AgendaGeoValue::~AgendaGeoValue()
{
    if (d && !d->ref.deref())
    {
        delete d;
    }
}

/*!
	Sets the latitude abd longitude values for a location
 */
void AgendaGeoValue::setLatLong(double latitude, double longitude)
{
	detach();
	d->mLatitude = latitude;
	d->mLongitude = longitude;
}

/*!
	Returns the latitude and longitude of the geo value
 */
void AgendaGeoValue::getLatLong(double& latitude, double& longitude) const
{
	if (d) {
		latitude = d->mLatitude;
		longitude = d->mLongitude;
	}
}

/*!
	Returns true if the alarlm is empty; otherwise returns false.

	\return true if the alarm is empty; otherwise returns false.
 */
bool AgendaGeoValue::isNull() const
	{
	return !d;
	}

/*!
	Assigns \a other to this geovalue and returns a reference
	to this alarm.

	\param other the other geovalue
	\return a reference to this geovalue
 */
AgendaGeoValue& AgendaGeoValue::operator=(const AgendaGeoValue& other)
{
	if (d == other.d) {
		return *this;
	}
	if (other.d) {
		other.d->ref.ref();
	}
	if (d && !d->ref.deref()) {
		delete d;
	}
	d = other.d;
	return *this;
}

/*!
	Returns true if this AgendaGeoValue is equal to the \a other
	info, otherwise returns false.
 */
bool AgendaGeoValue::operator==(const AgendaGeoValue& other) const
{
	if (d == other.d) {
		return true;
	}

	if (!d || !other.d) {
		return false;
	}

	return (d->mLatitude == other.d->mLatitude
			&& d->mLongitude == other.d->mLongitude);
}

/*!
	Returns true if this AgendaGeoValue is not equal to the \a other
	info, otherwise returns false.
 */
bool AgendaGeoValue::operator!=(const AgendaGeoValue& other) const
{
	return !(*this == other);
}

void AgendaGeoValue::detach()
{
	if (!d) {
		d = new AgendaGeoValuePrivate;
	} else {
		qAtomicDetach(d);
	}
}

AgendaGeoValuePrivate::AgendaGeoValuePrivate()
:ref(1)
{
	// Nothing to do.
}

/****************************************************
 *
 * AgendaRepeatRule
 *
 ****************************************************/

/*!
    \class AgendaRepeatRule

    \brief Class representing a repeat rule types.

           This supports the following standard iCal properties:

           FREQ (rule type),
           DTSTART (start date),
           UNTIL (end date),
           COUNT (number of instances),
           INTERVAL (interval between instances),
           BYDAY,
           BYMONTHDAY,
           BYYEARDAY,
           WKST (start day of week).

           Note that the repeat rule type (FREQ) must be set before any of the following properties
           can be set, since their behaviour is dependent on the rule type:
           BYDAY, BYMONTHDAY, BYYEARDAY
           The WKST parameter is only significant in weekly repeat rules with an interval of
           greater than 1.

           The repeat rule type may not be changed once it has been set.
*/

/*!
    \enum AgendaRepeatRule::RuleType

    This enum defines the possible rule types for a AgendaRepeatRule object.
*/
/*! \var AgendaRepeatRule::RuleType AgendaRepeatRule::DailyRule
    Rule which repeats based on a number of days.
*/
/*! \var AgendaRepeatRule::RuleType AgendaRepeatRule::WeeklyRule
    Rule which repeats based on a number of weeks.
*/
/*! \var AgendaRepeatRule::RuleType AgendaRepeatRule::MonthlyRule
    Rule which repeats based on a number of months.
*/
/*! \var AgendaRepeatRule::RuleType AgendaRepeatRule::YearlyRule
    Rule which repeats based on a number of years.
*/
/*! \var AgendaRepeatRule::RuleType AgendaRepeatRule::InvalidRule
     The type has not yet been defined.
*/

/*!
    \enum AgendaRepeatRule::Day

    The days are presented as enums
*/
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Monday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Tuesday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Wednesday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Thursday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Friday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Saturday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::Sunday */
/*! \var AgendaRepeatRule::Day AgendaRepeatRule::InvalidDay */

/*!
    \enum AgendaRepeatRule::Day

    The months are presented as enums
*/
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::January */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::February */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::March */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::April */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::May */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::June */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::July */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::August */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::September */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::October */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::November */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::December */
/*! \var AgendaRepeatRule::Month AgendaRepeatRule::InvalidMonth */

/*!
    Creates a repeat rule.
*/
AgendaRepeatRule::AgendaRepeatRule() : d(NULL)
{

}

/*!
    Constructs a new repeat rule using given \a type.

    \param type The type of the rule
*/
AgendaRepeatRule::AgendaRepeatRule(AgendaRepeatRule::RuleType type)
    : d(new AgendaRepeatRulePrivate)
{
    d->m_repeatRuleType = type;
    d->ref.ref();
}

/*!
    Creates a copy of a given repeat rule.

   \param other repeat rule to be copied.
*/
AgendaRepeatRule::AgendaRepeatRule(const AgendaRepeatRule& other)
    : d(other.d)
{
    if (d) {
        d->ref.ref();
    }
}

/*!
    Destroys the repeat rule.
*/
AgendaRepeatRule::~AgendaRepeatRule()
{
    if (d && !d->ref.deref())
    {
        delete d;
    }
}

/*!
    Sets the repeat definition's type.
    This will be one of: daily, weekly, monthly or yearly.
    If the type is weekly, it must have at least one weekday set by calling setByDay().
    The type may only be set once, from either the constructor or from the setType() function.

    \param type The repeat definition's type.
    \sa type()
*/
void AgendaRepeatRule::setType(AgendaRepeatRule::RuleType type)
{
    detach();
    d->m_repeatRuleType = type;
}

/*!
    Gets the repeat definition's type.
    This will be one of: daily, weekly, monthly or yearly.

    \return The repeat definition's type.
    \sa setType()
*/
AgendaRepeatRule::RuleType AgendaRepeatRule::type() const
{
    return d ? d->m_repeatRuleType : InvalidRule;
}

/*!
    Sets the repeat interval.
    This is a number of days for a daily repeat, a number of weeks for a weekly repeat, etc.

    \param interval Any parameter that is greater than 255 or less than 1 will be ignored.
    \sa interval()
*/
void AgendaRepeatRule::setInterval(int interval)
{
    detach();
    if (interval <= 255 && interval >= 1)
    {
        d->m_interval = interval;
    }
}

/*!
    Gets the repeat interval.
    This is a number of days for a daily repeat, a number of weeks for a weekly repeat, etc.

    \return The interval in minutes on success;
            otherwise returns -1.

    \sa interval()
*/
int AgendaRepeatRule::interval() const
{
    return d ? d->m_interval : -1;
}

/*!
    Sets the repeat's start date.

    \param date The start date
    \sa repeatRuleStart()
*/
void AgendaRepeatRule::setRepeatRuleStart(const QDateTime& date)
{
    detach();
    d->m_startDate = date;
}

/*!
    Gets the repeat's start date.

    \return The start date on success;
            otherwise returns a null QDateTime.

    \sa setRepeatRuleStart()
*/
QDateTime AgendaRepeatRule::repeatRuleStart() const
{
    return d ? d->m_startDate : QDateTime();
}

/*!
    Sets the repeat's end date.

    \parm date The end date
    \sa setUntil()
*/
void AgendaRepeatRule::setUntil(const QDateTime& date)
{
    detach();
    d->m_untilDate = date;
}

/*!
    Gets the repeat's end date.

    \return The end date on success;
            otherwise returns a null QDate.
    \sa setUntil()
*/
QDateTime AgendaRepeatRule::until() const
{
    return d ? d->m_untilDate : QDateTime();
}

/*!
    Sets the first day of the week for this repeat rule.

    \param day The weekday to be the first
    \sa weekStart()
*/
void AgendaRepeatRule::setWeekStart(Day day)
{
    detach();
    d->m_weekStartDay = day;
}

/*!
    Gets the first day of the week for this repeat rule.

    \return The first weekday
    \sa setWeekStart()
*/
AgendaRepeatRule::Day AgendaRepeatRule::weekStart() const
{
    return d ? d->m_weekStartDay : InvalidDay;
}

/*!
    Set all weekdays on which this rule is repeated.

    \param days List containing all weekdays that are to be set.
           Any weekdays not included in this array will not be set.
    \sa getByDay()
*/
void AgendaRepeatRule::setByDay(const QList<Day>& days)
{
    detach();
    d->m_days = days;
}

/*!
    Gets all weekdays on which this rule is repeated.

    \return List contains all weekdays that are to be set on success;
            otherwise returns a null list.
    \sa setByDay()
*/
QList<AgendaRepeatRule::Day> AgendaRepeatRule::byDay() const
{
    return d ? d->m_days : QList<AgendaRepeatRule::Day>();
}

/*!
    Sets all dates of the month on which this rule is repeated.

    \param monthDays List containing all month dates that are to be set.
    \sa byMonthDay()
*/
void AgendaRepeatRule::setByMonthDay(const QList<int>& monthDays)
{
    detach();
    d->m_monthDays = monthDays;
}

/*!
    Gets all dates of the month on which this rule is repeated.

    \return List contains all month dates that are to be set on success;
            otherwise returns a null list.

    \sa setByMonthDay()
*/
QList<int> AgendaRepeatRule::byMonthDay() const
{
    return d ? d->m_monthDays : QList<int>();
}

/*!
    Sets the month of the year for this repeat rule.

    \param months List of months on which to repeat.
    Only the first month in the array is used.
    \sa byMonth()
*/
void AgendaRepeatRule::setByMonth(const QList<Month>& months)
{
    detach();
    d->m_months = months;
}

/*!
    Gets the month of the year for this repeat rule.

    \return List of months on which to repeat on success;
            otherwise returns a null list.
    .
    \sa setByMonth()
*/
QList<AgendaRepeatRule::Month> AgendaRepeatRule::byMonth() const
{
    return d ? d->m_months : QList<AgendaRepeatRule::Month>();
}

void AgendaRepeatRule::detach()
{
    if (!d) {
        d = new AgendaRepeatRulePrivate();
    } else {
        qAtomicDetach(d);
    }
}

/*!
    Returns true if the repeatrule is empty; otherwise returns false.

    \return true if the repeatrule is empty; otherwise returns false.
*/
bool AgendaRepeatRule::isNull() const
{
    return !d;
}

/*!
    Assigns \a other to this repeat rule and returns a reference
    to this repeat rule.

    \param other the other repeat rule
    \return a reference to this repeat rule
*/
AgendaRepeatRule& AgendaRepeatRule::operator=(const AgendaRepeatRule& other)
{
    if (d == other.d)
    {
        return *this;
    }
    if (other.d)
    {
        other.d->ref.ref();
    }
    if (d && !d->ref.deref())
    {
        delete d;
    }
    d = other.d;
    return *this;
}


/*!
    Returns true if this AgendaRepeatRule is equal to the \a other
    info, otherwise returns false.
*/
bool AgendaRepeatRule::operator==(const AgendaRepeatRule& other) const
{
    if (d == other.d)
        return true;

    return (d->m_days == other.d->m_days
            && d->m_interval == other.d->m_interval
            && d->m_monthDays == other.d->m_monthDays
            && d->m_months == other.d->m_months
            && d->m_repeatRuleType == other.d->m_repeatRuleType
            && d->m_startDate == other.d->m_startDate
            && d->m_untilDate == other.d->m_untilDate
            && d->m_weekStartDay == other.d->m_weekStartDay);
}

/*!
    Returns true if this AgendaRepeatRule is not equal to the \a other
    info, otherwise returns false.
*/
bool AgendaRepeatRule::operator!=(const AgendaRepeatRule& other) const
{
    return !(*this == other);
}

AgendaRepeatRulePrivate::AgendaRepeatRulePrivate() : ref(1)
{

}

/****************************************************
 *
 * AgendaEntry
 *
 ****************************************************/

/*!
    \class AgendaEntry

    \brief Class representing a calendar entry.
    This can be an appointment, to-do item, reminder, event or anniversary.
    AgendaEntry contains information about a calendar entry.
*/

/*!
    \enum AgendaEntry::Type

    This enum defines the possible entry types for a AgendaEntry object.
*/
/*! \var AgendaEntry::Type AgendaEntry::TypeAppoinment
    An appointment, which has a start time and end time.
*/
/*! \var AgendaEntry::Type AgendaEntry::TypeTodo
    A to-do, which can have a start time and end time
    (the end time is the due date), or can be undated.
*/
/*! \var AgendaEntry::Type AgendaEntry::TypeEvent
    An event, which has a start time and end time.
*/
/*! \var AgendaEntry::Type AgendaEntry::TypeReminder
    A reminder, which has a start time only.
*/
/*! \var AgendaEntry::Type AgendaEntry::TypeAnniversary
    An anniversary, which has a start time and end time.
*/
/*! \var AgendaEntry::Type AgendaEntry::TypeNote
	A Note, which has description and can be a favorite entry.
 */
/*! \var AgendaEntry::Type AgendaEntry::TypeUnknown
    The type is unknown.
*/

/*!
    \enum AgendaEntry::Method

    This enum defines the possible methods for a AgendaEntry object.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodNone
    The RFC2445-defined method value 'None'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodPublish
    The RFC2445-defined method value 'Publish'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodRequest
    The RFC2445-defined method value 'Request'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodReply
    The RFC2445-defined method value 'Reply'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodAdd
    The RFC2445-defined method value 'Add'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodCancel
    The RFC2445-defined method value 'Cancel'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodRefresh
    The RFC2445-defined method value 'Refresh'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodCounter
    The RFC2445-defined method value 'Counter'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodDeclineCounter
    The RFC2445-defined method value 'DeclineCounter'.
*/
/*! \var AgendaEntry::Method AgendaEntry::MethodUnknown
    The method is unknown
*/

/*!
	\enum AgendaEntry::Status

	This enum defines the possible status properties as per RFC-2445 and
	vCalendar 1.0 standard.
 */
/*! \var AgendaEntry::Status AgendaEntry::Tentative
	Used to support iCalendar (RFC 2445) and vCalendar 1.0.
	The event is tentative.
 */
/*! \var AgendaEntry::Status AgendaEntry::Confirmed
	Used to support iCalendar (RFC 2445) and vCalendar 1.0.
	The event has been confirmed.
 */
/*! \var AgendaEntry::Status AgendaEntry::Cancelled
	Used to support iCalendar (RFC 2445). The event has been cancelled.
 */
/*! \var AgendaEntry::Status AgendaEntry::TodoNeedsAction
	Used to support iCalendar (RFC 2445). The to-do entry needs action.
 */
/*! \var AgendaEntry::Status AgendaEntry::TodoCompleted
	Used to support iCalendar (RFC 2445) and vCalendar 1.0.
	The to-do entry has been completed.
 */
/*! \var AgendaEntry::Status AgendaEntry::TodoInProcess
	Used to support iCalendar (RFC 2445). The to-do entry is in progress.
 */
/*! \var AgendaEntry::Status AgendaEntry::NullStatus
	No status is set.
 */
/*! \var AgendaEntry::Status AgendaEntry::VCalAccepted
	Used to support vCalendar 1.0. The entry has been accepted.
 */
/*! \var AgendaEntry::Status AgendaEntry::VCalNeedsAction
	Used to support vCalendar 1.0. The entry needs action.
 */
/*! \var AgendaEntry::Status AgendaEntry::VCalSent
	Used to support vCalendar 1.0. The entry has been sent.
 */
/*! \var AgendaEntry::Status AgendaEntry::VCalDeclined
	Used to support vCalendar 1.0. The entry has been declined.
 */
/*! \var AgendaEntry::Status AgendaEntry::VCalDelegated
	Used to support vCalendar 1.0. The entry has been delegated.
 */
/*! \var AgendaEntry::Status AgendaEntry::StatusUnknown
	Unknown status.
 */

/*!
	Creates a calendar entry.
 */
AgendaEntry::AgendaEntry() : d(NULL)
{
}

/*!
	Constructs a new entry rule using given \a type.

	\param type The type of the entry
 */
AgendaEntry::AgendaEntry(Type type) : d(new AgendaEntryPrivate)
{
	d->m_entryType = type;
	d->ref.ref();
}

/*!
	Creates a copy of a given entry.

	\param other entry to be copied.
 */
AgendaEntry::AgendaEntry(const AgendaEntry& other) : d(other.d)
{
	if (d) {
		d->ref.ref();
	}
}

/*!
	Destroys the entry.
 */
AgendaEntry::~AgendaEntry()
{
	if (d && !d->ref.deref()) {
		delete d;
	}
}

/*!
	Adds an attendee for this entry.

	\param The attendee to be added.
	\sa attendies(), deleteAttendee()
 */
void AgendaEntry::addAttendee(const AgendaAttendee& attendee)
{
	detach();
	d->addAttendee(attendee);
}

/*!
	Returns true if the calendar entry is empty; otherwise returns false.

	\return true if the Calendar entry is empty; otherwise returns false.
 */
bool AgendaEntry::isNull() const
{
	return !d;
}

/*!
	Sets the alarm for the entry. Alarm can be removed by setting
	null AgendaAlarm with this function.

	\param alarm Alarm object to be set
	\sa alarm()
 */
void AgendaEntry::setAlarm(const AgendaAlarm& alarm)
{
	detach();
	d->m_alarm = alarm;
}

/*!
	Gets the alarm for the entry

	\return Alarm object on success;
			otherwise returns a null AgendaAlarm.

	\sa setAlarm()
 */
AgendaAlarm AgendaEntry::alarm() const
{
	return d ? d->m_alarm : AgendaAlarm();
}

/*!
	Sets the start and end date/times for an entry.

	For an appointment, event or anniversary, the start time must be earlier
	than or equal to the end time. For a reminder, the end time will be ignored.
	For a to-do entry, the end time is the 'due date'. The time portion of the start date
	will not be stored for todos, only the number of days until the due date.
	Only Undated To-do's can have both start and end date as NullTime.

	\param startTime The entry's new start date/time.
	\param endTime The entry's new end date/time.
	\return true if the start and end times are valid; otherwise returns false.
	\sa startTime(), endTime()
 */
bool AgendaEntry::setStartAndEndTime(
		const QDateTime& startTime, const QDateTime& endTime)
{
	detach();
	d->m_startTime = startTime;
	d->m_endTime = endTime;

	return true;
}

/*!
	Gets the start date/time for an entry.

	\return The entry's start date/time on success;
			otherwise returns a null QDateTime.
	\sa endTime(), setStartAndEndTime()
 */
QDateTime AgendaEntry::startTime() const
{
	return d ? d->m_startTime : QDateTime();
}

/*!
	Gets the entry's end date/time.

	\return The entry's end date/time on success;
			otherwise returns a null QDateTime.
	\sa startTime(), setStartAndEndTime()
 */
QDateTime AgendaEntry::endTime() const
{
	return d ? d->m_endTime : QDateTime();
}

/*!
	Returns a reference to the attendees list.
	Using a reference makes it possible to manipulate the list directly.

	\return A reference to the attendees list
	\sa startTime(), setStartAndEndTime()
 */
QList<AgendaAttendee>& AgendaEntry::attendees()
{
	return d->m_attendees;
}

/*!
	Returns a const reference to the attendees list.

	\return A const reference to the attendees list
	\sa startTime(), setStartAndEndTime()
 */
const QList<AgendaAttendee>& AgendaEntry::attendees() const
{
	return d->m_attendees;
}

/*!
	Returns a const reference to the attendees list.

	\return A const reference to the attendees list
	\sa addCategory(), deleteCategory()
 */
const QList<AgendaCategory>& AgendaEntry::categories() const
{
	return d->m_categories;
}

/*!
	Sets the description text for this entry.

	\param description The description text
	\sa description()
 */
void AgendaEntry::setDescription(const QString& description)
{
	detach();
	d->m_description = description;
}

/*!
	Gets the description text for the entry.

	\return The description text on success;
	otherwise returns a null QString.
	\sa setDescription()
 */
QString AgendaEntry::description() const
{
	return d ? d->m_description : QString();
}

/*!
	Gets a list of the entry's categories.

	\return A const reference to the attendees list
	\sa addCategory(), deleteCategory()
 */
QList<AgendaCategory>& AgendaEntry::categories()
{
	return d->m_categories;
}

/*!
	Gets the local uid of the entry.

	\return Entry id on success;
			otherwise returns -1.
 */
ulong AgendaEntry::id() const
{
	return d ? d->m_id : 0;
}

/*!
	Gets the type of entry.

	\return The type of entry.
 */
AgendaEntry::Type AgendaEntry::type() const
{
	return d ? d->m_entryType : TypeUnknown;
}

/*!
	Sets the type of entry.

	\param type The type of entry.
	\sa type()
 */
void AgendaEntry::setType(Type type)
{
	detach();
	d->m_entryType = type;
}

/*!
	Appends a category to the entry's list of categories.

	\param category The category to be added
	\sa deleteCategory(), categories()
 */
void AgendaEntry::addCategory(const AgendaCategory& category)
{
	detach();
	d->addCategory(category);
}

/*!
	Sets the contents of the entry's location field.

	\return Descriptor containing a location, e.g. for a meeting.
	\sa location()
 */
void AgendaEntry::setLocation(const QString& location)
{
	detach();
	d->m_location = location;
}

/*!
	Gets the contents of the entry's location field.

	\return The location field on success;
			otherwise returns a null QString.
	\sa setLocation()
 */
QString AgendaEntry::location() const
{
	return d ? d->m_location : QString();
}

/*!
	Sets the summary text for this entry.

	\param summary The summary text.
	\sa summary()
 */
void AgendaEntry::setSummary(const QString& summary)
{
	detach();
	d->m_summaryText = summary;
}

/*!
	Gets the summary for the entry.

	\return The summary text on success;
			otherwise returns a null QString.
	\sa setSummary()
 */
QString AgendaEntry::summary() const
{
	return d ? d->m_summaryText : QString();
}

/*!
	Sets the entry's method property for group scheduling.

	\param method The method property of a Group Scheduling entry
	\sa method()
 */
void AgendaEntry::setMethod(Method method)
{
	detach();
	d->m_method = method;
}

/*!
	Returns the entry's method property for group scheduling.

	\return The method property of a Group Scheduling entry
	\sa setMethod()
 */
AgendaEntry::Method AgendaEntry::method() const
{
	return d ? d->m_method : MethodUnknown;
}

/*!
	Sets the entry's repeat definition.

	\param repetRule Repeat rule object
	\sa repeatRule()
 */
void AgendaEntry::setRepeatRule(const AgendaRepeatRule& repeatRule)
{
	detach();
	d->m_repeatRule = repeatRule;
}

/*!
	Gets the entry's repeat rule. Count and the Until values can be accessed.
	Note! In the case of a repeat rule which repeats forever,
	the Count will always be returned as 0.

	\return AgendaRepeatRule Repeat rule object on success;
			otherwise returns a null AgendaRepeatRule.
	\sa setRepeatRule()
 */
AgendaRepeatRule AgendaEntry::repeatRule() const
{
	return d ? d->m_repeatRule : AgendaRepeatRule();
}

/*!
	Sets the priority for the entry. The default value is zero.

	\param priority Priority value between 0 and 255.
			The behaviour is undefined if the priority is greater than 255.
	\sa priority()
 */
void AgendaEntry::setPriority(int priority)
{
	detach();
	d->m_priority = priority;
}

/*!
	Gets the priority of the entry.

	\return Priority value between 0 and 255 on success;
			otherwise returns -1.
	\sa setPriority()
 */
int AgendaEntry::priority() const
{
	return d ? d->m_priority : -1;
}

/*!
	Sets the rDates for the entry.

	\param rDates Random dates to be set for the entry
	\sa rDates()
 */
void AgendaEntry::setRDates(const QList<QDate>& rdates)
{
	detach();
	d->m_rDates = rdates;
}

/*!
	Gets the rDates of the entry.

	\return RDates Returns the rendom dates if entry has any;
			otherwise returns empty list.
	\sa setRDates()
*/
QList<QDate> AgendaEntry::rDates() const
{
	return d ? d->m_rDates : QList<QDate>();
}

/*!
	Sets the recurrence Id of an entry

	\param recTime recurrence Id
*/
void AgendaEntry::setRecurrenceId(QDateTime& recTime)
{
	detach();
	d->m_recurrenceId = recTime;
}

/*!
	Returns the recurrence Id of an entry

	return bool returns true if the entry is repeated else false
*/
QDateTime AgendaEntry::recurrenceId()
{
	return d ? d->m_recurrenceId : QDateTime();
}

/*!
	Constructs the P-IMPL if not already there.
 */
void AgendaEntry::detach()
{
	if (!d) {
		d = new AgendaEntryPrivate;
	} else {
		qAtomicDetach(d);
	}
}

/*!
	Informs if the entry is a timed entry or not

	return bool returns true if the entry is timed else false
*/
bool AgendaEntry::isTimedEntry()
{
	return type() == TypeAppoinment || type() == TypeReminder;
}

/*!
	Informs if the entry is a repeated entry or not

	return bool returns true if the entry is repeated else false
*/
bool AgendaEntry::isRepeating()
{
	bool repeating = false;
	bool rdate = rDates().count() > 0;

	if (rdate ||
			(this->repeatRule().type() != AgendaRepeatRule::InvalidRule)) {
		repeating = true;
	}

	return repeating;
}

/*!
	Returns the last modification time for the entry.

	\return QDateTime object containing the last modified time.
 */
QDateTime AgendaEntry::lastModifiedDateTime() const
{
	return d ? d->m_lastModTime : QDateTime();
}

/*!
	Sets the last modification time for the entry.

	\param dateTime Of type QDateTime, holds the last modified time.
 */
void AgendaEntry::setLastModifiedDateTime(const QDateTime& dateTime)
{
	detach();
	d->m_lastModTime = dateTime;
}

/*!
	Returns the status of the entry.

	\return AgendaEntry::Status The status of the entry.
 */
AgendaEntry::Status AgendaEntry::status() const
{
	return d ? d->m_entryStatus : StatusUnknown;
}

/*!
	Sets the status of the entry.

	\param newStatus AgendaEntry::Status argument.
 */
void AgendaEntry::setStatus(AgendaEntry::Status newStatus)
{
	detach();
	d->m_entryStatus = newStatus;
}

/*!
	Returns the value of favourite property for the entry.

	\return uint 0 or 1 depending on whether the entry is marked as favourite or
	not.
 */
uint AgendaEntry::favourite() const
{
	return d ? d->m_favourite : 0;
}

/*!
	Sets or resets the favourite property for an entry. The value should be 0
	for un-marking it as favourite and 1 for marking it as favourite.
 */
void AgendaEntry::setFavourite(uint favourite)
{
	detach();
	d->m_favourite = favourite;
}

/*!
	Sets the to-do completion data and time.This method can be used by
	AgendaUtil to set the completed date while converting from CCalEntry to
	Agenda Entry

	\param dateTime of type QDateTime holds the to-do completion date
 */
void AgendaEntry::setCompletedDateTime(const QDateTime& dateTime)
{
	d->m_completedDateTime = dateTime;
}

/*!
	Returns to-do completion date and time.

	\return QDateTime object containing the to-do completion date and time
 */
QDateTime AgendaEntry::completedDateTime() const
{
	return d->m_completedDateTime;
}

/*!
	Sets the dtStamp time for agenda entry. Currently it is used to store the
	creation time of the agenda entry for the type ENote.
 */
void AgendaEntry::setDTStamp(const QDateTime& dateTime)
{
	d->m_dtStamp = dateTime;
}

/*!
	Returns the DT Stamp for the agenda entry.

	\param QDateTime object containing the dt stamp.
 */
QDateTime AgendaEntry::dtStamp() const
{
	return d->m_dtStamp;
}

/*!
	 Sets the geo values for an entry
 */
void AgendaEntry::setGeoValue(const AgendaGeoValue& geoValue)
{
	d->m_geoValue = geoValue;
}

/*!
	 Returns the geo value if it has any
 */
AgendaGeoValue AgendaEntry::geoValue() const
{
	return d->m_geoValue;
}

/*!
     Clears the geo value if it has any
 */
void AgendaEntry::clearGeoValue() const
{
    if (d) {
        d->m_geoValue = AgendaGeoValue();
    }
}

/*!
	Returns
	duration of the meeting in seconds
	\return int duration in seconds
 */
int AgendaEntry::durationInSecs() const
{
	// Calculate the duration
	QDateTime start = d->m_startTime;
	QDateTime end = d->m_endTime;
	TDateTime startDateTime(start.date().year(),
							static_cast<TMonth> (start.date().month() - 1),
							start.date().day() - 1, start.time().hour(),
							start.time().minute(), start.time().second(), 0);
	
	TDateTime endDateTime(end.date().year(),
							static_cast<TMonth> (end.date().month() - 1),
							end.date().day() - 1, end.time().hour(),
							end.time().minute(), end.time().second(), 0);
	
	TTime startTime(startDateTime);
	TTime endTime(endDateTime);
	TTimeIntervalSeconds duration;
	endTime.SecondsFrom(startTime, duration);
	
	return (duration.Int());
}

/*!
	Assigns \a other to this entry and returns a reference
	to this entry.

	\param other the other entry
	\return a reference to this entry
 */
AgendaEntry& AgendaEntry::operator=(const AgendaEntry& other)
{
	if (d == other.d) {
		return *this;
	}
	if (other.d) {
		other.d->ref.ref();
	}
	if (d && !d->ref.deref()) {
		delete d;
	}
	d = other.d;
	return *this;
}

/*!
	Returns true if this AgendaEntry is equal to the \a other
	info, otherwise returns false.
 */
bool AgendaEntry::operator==(const AgendaEntry& other) const
{
	if (d == other.d) {
		return true;
	}

	return (d->m_alarm == other.d->m_alarm
			&& d->m_attendees == other.d->m_attendees
			&& d->m_categories == other.d->m_categories
			&& d->m_description == other.d->m_description
			&& d->m_endTime == other.d->m_endTime
			&& d->m_entryType == other.d->m_entryType
			&& d->m_id == other.d->m_id
			&& d->m_location == other.d->m_location
			&& d->m_method == other.d->m_method
			&& d->m_priority == other.d->m_priority
			&& d->m_repeatRule == other.d->m_repeatRule
			&& d->m_startTime == other.d->m_startTime
			&& d->m_summaryText == other.d->m_summaryText
			&& d->m_lastModTime == other.d->m_lastModTime
			&& d->m_entryStatus == other.d->m_entryStatus
			&& d->m_favourite == other.d->m_favourite
			&& d->m_completedDateTime == other.d->m_completedDateTime
			&& d->m_geoValue == other.d->m_geoValue);
}

/*!
	Returns true if this AgendaEntry is not equal to the \a other
	info, otherwise returns false.
 */
bool AgendaEntry::operator!=(const AgendaEntry& other) const
{
	return !(*this == other);
}

AgendaEntryPrivate::AgendaEntryPrivate() : ref(1)
{
	m_method = AgendaEntry::MethodNone;
	m_entryStatus = AgendaEntry::NullStatus;
	m_repeatRule = AgendaRepeatRule(AgendaRepeatRule::InvalidRule);
	m_id = 0;
	m_priority = -1;
	m_favourite = 0;
	m_alarm = AgendaAlarm();
	m_geoValue = AgendaGeoValue();
}

bool AgendaEntryPrivate::deleteCategory(const AgendaCategory& category)
{
	for (int i = 0; i < m_categories.count(); i++) {
		if (m_categories.at(i) == category) {
			m_categories.removeAt(i);
			return true;
		}
	}
	return false;
}

void AgendaEntryPrivate::addCategory(const AgendaCategory& category)
{
	m_categories.append(category);
}

bool AgendaEntryPrivate::deleteAttendee(const AgendaAttendee& attendee)
{
	for (int i = 0; i < m_attendees.count(); i++) {
		if (m_attendees.at(i).address() == attendee.address()) {
			m_attendees.removeAt(i);
			return true;
		}
	}
	return false;
}

void AgendaEntryPrivate::addAttendee(const AgendaAttendee& attendee)
{
	m_attendees.append(attendee);
}

// End of file
