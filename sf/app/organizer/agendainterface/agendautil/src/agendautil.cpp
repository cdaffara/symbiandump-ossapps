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
#include <agendautil.h>
#include "agendautil_p.h"

#include <QList>
#include <QDateTime>

/*!
    \class AgendaUtil

    \brief The AgendaUtil class can be used for managing calendar entries.
*/

/*!
    Constructs a AgendaUtil object with the given parent.
*/
AgendaUtil::AgendaUtil(QObject* parent)
    : QObject(parent), d(new AgendaUtilPrivate(this))
{
}

/*!
    Destroys the XQContacts object.
*/
AgendaUtil::~AgendaUtil()
{
    delete d;
}

/*!
    \enum AgendaUtil::Error

    This enum defines the possible errors for a AgendaUtil object.
*/
/*! \var AgendaUtil::Error AgendaUtil::NoError
    No error occured.
*/
/*! \var AgendaUtil::Error AgendaUtil::OutOfMemoryError
    Not enough memory.
*/
/*! \var AgendaUtil::Error AgendaUtil::AlreadyInUse
    Already in use
*/
/*! \var AgendaUtil::Error AgendaUtil::UnknownError
    Unknown error.
*/

/*!
    \enum AgendaUtil::FilterFlags

    This enum defines the possible filters for a AgendaUtil object.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeAppointments
    Include timed appointments.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeReminders
    Include appointments which only have a start time.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeEvents
    Include all-day events.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeAnniversaries
    Include anniversaries.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeCompletedTodos
    Include completed todos.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeIncompletedTodos
    Include incompleted todos.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeAlarmedOnly
    Remove non-alarmed entries. This flag cannot be used by itself.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeRptsNextInstanceOnly
    Include next only instance of a repeat entry.
*/
/*! \var AgendaUtil::FilterFlags AgendaUtil::IncludeAll
    Include all entries (appointments, day events, anniversaries and todos).
*/

/*!
	To store the new entry or update the entry in the Calendar db.

	\param entry The entry to be added/updated
	\param range The recurrence range of entry
	\param instanceOriginalDateTime The start time of the original instance.
	\return ulong The local uid of the entry added/updated in the db.
*/
ulong AgendaUtil::store(
		AgendaEntry &entry, AgendaUtil::RecurrenceRange range,
		QDateTime instanceOriginalDateTime)
{
	return d->store(entry, range, instanceOriginalDateTime);
}

/*!
	Clones the `entry' passed in the argument and saves it as type `type'.

	This API clones the entry, by retaining the GUID of the entry and all the
	other data from the entry provided and saves it as the given type. Note that
	only relavant data for the passed type will be saved.

	Note that since the GUID is retained, there will be two CCalEntry'ies with
	the same GUID. This would cause problems with Sync and other such use-cases.
	Hence the client should ensure that the original entry (the one from which
	cloning was done) is deleted after calling this API.

	\param entry Entry which should be used for cloning.
	\param type The new type of the entry.
	\return ulong The local UID of the new entry.

	\sa deleteEntry()
 */
ulong AgendaUtil::cloneEntry(const AgendaEntry& entry, AgendaEntry::Type type)
{
	return d->cloneEntry(entry, type);
}

/*!
    Deletes the entry

    \param id The id of the entry to be deleted
    \return If false is returned, an error has occurred. Call error() to get a value of
    AgendaUtil::Error that indicates which error occurred
    \sa store()
*/
bool AgendaUtil::deleteEntry(ulong id)
{
    return d->deleteEntry(id);
}

/*!
	Deletes the entry for the mentioned range. This applies only for repeating entry

	\param id The id of the entry to be deleted
	\param range Specifies if all the instances of the repeating entry has to be deleted or
				only specific instances
	\return If false is returned, an error has occurred. Call error() to get a value of
	AgendaUtil::Error that indicates which error occurred
	\sa store()
 */
void AgendaUtil::deleteRepeatedEntry(
		AgendaEntry& entry,
		AgendaUtil::RecurrenceRange range)
{
	d->deleteRepeatedEntry(entry, range);
}

/*!
	Fetches the entry which of id is equal to given \a id.

	\param id id of the entry to be returned
	\return the entry on success; null entry on failure
	\sa count()
*/
AgendaEntry AgendaUtil::fetchById(ulong id)
{
	return d->fetchById(id);
}

/*!
    Tries to import calendar entries from the specified file.

    \param fileName The fully specified path and filename of the file from which vCalendars (calendar entries) will be imported
    \return Number of imported vCalendars (calendar entries)
    \sa exportAsvCalendar()
*/
int AgendaUtil::importvCalendar(const QString& fileName, AgendaEntry& entry)
{
    return d->importvCalendar(fileName, entry);
}

/*!
    Exports the specified calendar entry to a file.

    \param fileName Fully specified path and filename of the file to export the vCalendar (calendar entry)  items into
    \param calendarEntryId Id of the calendar entry to be exported
    \return True if the calendar entry was successfully exported; otherwise returns false.
    \sa importvCalendar()
*/
bool AgendaUtil::exportAsvCalendar(const QString& fileName, long int calendarEntryId)
{
    return d->exportAsvCalendar(fileName, calendarEntryId);
}

/*!
    Fetches all entry ids

    \param filters Filter flags specifying the types of entries to be returned
    \return the entry on success; null entry on failure
    \sa count()
*/
QList<ulong> AgendaUtil::entryIds(AgendaUtil::FilterFlags filter)
{
    return d->entryIds(filter);
}

/*!
    Fetches all entries

    \param filters Filter flags specifying the types of entries to be returned
    \return the entry on success; null entry on failure
    \sa count()
*/
QList<AgendaEntry> AgendaUtil::fetchAllEntries(AgendaUtil::FilterFlags filter)
{
    return d->fetchAllEntries(filter);
}

/*!
    Fetches all entries for a given time range

    \param filters Filter flags specifying the types of entries to be returned
    \param rangeStart Time form which entries have to be fetched
    \param rangeEnd Time upto which entries have to be fetched
    \return the entry on success; null entry on failure
    \sa count()
*/
QList<AgendaEntry> AgendaUtil::fetchEntriesInRange(QDateTime rangeStart, QDateTime rangeEnd,
                                AgendaUtil::FilterFlags filter)
{
    return d->fetchEntriesInRange(rangeStart, rangeEnd, filter);
}

/*!
    Fetches all entries for a given day

    \param filters Filter flags specifying the types of entries to be returned
    \param day Day form which entries have to be fetched
    \return the entry on success; null entry on failure
    \sa count()
*/
QList<AgendaEntry> AgendaUtil::createEntryIdListForDay(QDateTime day,
                                            AgendaUtil::FilterFlags filter)
{
    return d->createEntryIdListForDay(day, filter);
}

/*!
    Returns the type of error that occurred if the latest function call failed; otherwise returns NoError.

    \return Error code
*/
AgendaUtil::Error AgendaUtil::error() const
{
    return d->error();
}

/*!
    Sets the to-do entry as completed.

    \param complete True to set to-do as complete , false for Not completed
    \param dateTime Date on which this action is being performed

    return None
*/
void AgendaUtil::setCompleted(AgendaEntry& entry, bool complete, QDateTime& dateTime)
{
    if(d) {
    d->setCompleted(entry, complete, dateTime);
    }
}

/*!
    Deletes the entries specified by the filter for a given range

    \param start Start time from which entries have to be deleted
    \param end End time till which entries have to be deleted
    \param filter Filter flahs telling what type of events have to be deleted
    return None
*/
void AgendaUtil::deleteEntries(QDateTime& start, QDateTime& end,
                                AgendaUtil::FilterFlags filter)
{
    if(d) {
    d->deleteEntries(start, end, filter);
    }
}

/*!
    Returns the parent entry of the entry

    \param entry Entry for which parent entry is requested
    return AgendaEntry Parent entry
*/
AgendaEntry AgendaUtil::parentEntry(AgendaEntry& entry)
{
    if(d) {
    return d->parentEntry(entry);
    }
    return AgendaEntry();
}

/*!
	Returns the start and end times of previous occurence of a particular
	instance

	\param entry The instance with which previous instance details are obtained
	\return None
 */
void AgendaUtil::getPreviousInstanceTimes(AgendaEntry& entry, 
                                          QDateTime& startTime, 
                                          QDateTime& endTime)
{
	if(d) {
		return d->getPreviousInstanceTimes(entry, startTime, endTime);
	 }
}

/*!
	Returns the start and end times of next occurence of a particular
	instance

	\param entry The instance with which next instance details are obtained
	\return None
 */
void AgendaUtil::getNextInstanceTimes(AgendaEntry& entry, 
                                          QDateTime& startTime, 
                                          QDateTime& endTime)
{
	if(d) {
		return d->getNextInstanceTimes(entry,startTime,endTime);
	 }
}

void AgendaUtil::markDatesWithEvents(QDateTime rangeStart, QDateTime rangeEnd,
						 AgendaUtil::FilterFlags filter, QList<QDate>& dates)
{
	if(d) {
		return d->markDatesWithEvents(rangeStart,rangeEnd,filter,dates);
	 }
}

/*!
	 Returns true if there are no entries in the database else returns false
	 
	 \return bool 
 */
bool AgendaUtil::areNoEntriesInCalendar()
{
	if(d) {
		return d->areNoEntriesInCalendar();
	 }
	return false;
}

/*!
    Returns the lower range limit of the time supported by the 
    underlying platform for calendar entries

    \return QDateTime   Lower range time
*/
QDateTime AgendaUtil::minTime()
{
    return AgendaUtilPrivate::minTime();
}

/*!
    Returns the upper range limit of the time supported by the 
    underlying platform for calendar entries

    \return QDateTime   Upper range time
*/
QDateTime AgendaUtil::maxTime()
 {
     return AgendaUtilPrivate::maxTime();
 }

/*!
    Returns true if entry repeats on workdays else false

    \return true if entry repeats on workdays else false
 */
bool AgendaUtil::isWorkdaysRepeatingEntry(const AgendaRepeatRule& repeatRule)
{
	return AgendaUtilPrivate::isWorkdaysRepeatingEntry(repeatRule);
}
/*!
    \fn void XQMedia::entriesChanged(QList<ulong> ids)

    This signal is emitted when an entry or entries have changed.

    \param ids Ids of the entries
*/

// End of file
