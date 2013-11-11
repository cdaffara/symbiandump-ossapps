// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <calalarm.h>
#include <calcontent.h>
#include <asshddefs.h>


/** Creates a new calendar alarm with no alarm sound name and
an alarm time offset of 0 minutes, and no extended alarm action.

@publishedAll
@released
@capability None
@return CCalAlarm pointer
*/
EXPORT_C CCalAlarm* CCalAlarm::NewL()
	{
	return ( new (ELeave) CCalAlarm() );
	}

CCalAlarm::CCalAlarm()
	{
	}

/** A calendar alarm destructor.

@publishedAll
@released
@capability None
 */
EXPORT_C CCalAlarm::~CCalAlarm()
	{
	delete iAlarmName;
	delete iAlarmAction;
	}


/** Sets a time offset for an alarm to occur prior to a scheduled event.

The time offset is calculated as the number of minutes before the instance time of the event.
- For non-todo entries, the instance time is the start date.
- For todo entries, the instance time is the end date (the due date).
Note that it is not possible to set an alarm on a todo without an end date.

It is possible to have a negative offset provided that the alarm is on the same day (in local time) as the instance time; this means the alarm occurs after the event.
Fixed entries cannot have negative alarm offsets, since then the current day will change depending on the time zone.
@publishedAll
@released
@capability None
@param aOffset The time offset in minutes for an alarm prior to an event.
*/
EXPORT_C void CCalAlarm::SetTimeOffset(TTimeIntervalMinutes aOffset)
	{
	iOffset = aOffset;
	}


/** Gets the time offset for the alarm to occur prior to an event.

@publishedAll
@released
@capability None
@return The alarm offset (in minutes) prior to an event.
*/
EXPORT_C TTimeIntervalMinutes CCalAlarm::TimeOffset() const
	{
	return ( iOffset );
	}


/** Sets the name of the alarm sound.

Note: if the length of the new alarm sound name is greater than KMaxAlarmSoundName
characters, it will be ignored.

@publishedAll
@released
@capability None
@param aAlarmSoundName The name of the alarm sound.
*/
EXPORT_C void CCalAlarm::SetAlarmSoundNameL(const TDesC& aAlarmSoundName)
	{
	const TInt KLen = aAlarmSoundName.Length();
	
	if ( KLen > 0 && KLen <= KMaxAlarmSoundNameLength )
		{
		// delete and set always to NULL before AllocL because
		// that call could leave and the pointer be not NULL

		delete iAlarmName;
		iAlarmName = NULL;
	
		iAlarmName = aAlarmSoundName.AllocL();
		}		
	}


/** Gets a descripter of the alarm sound name.

If there is no name set, KNullDesC will be returned.

@publishedAll
@released
@capability None
@return CCalAlarm descriptor reference
*/
EXPORT_C const TDesC& CCalAlarm::AlarmSoundNameL() const
	{
	if ( iAlarmName )
		{
		return ( *iAlarmName );
		}
		
	return ( KNullDesC() );
	}


/** Associates an action with the alarm. The action could be a link to an
Internet radio station, or it could be a small animated icon.
This class takes ownership of the alarm action.
Any existing alarm action is deleted.

@param aAlarmAction The data or link (url).
@see CCalContent
*/
EXPORT_C void CCalAlarm::SetAlarmAction(CCalContent* aAlarmAction)
	{
	delete iAlarmAction;
	iAlarmAction = aAlarmAction;
	}


/** Returns the action associated with the alarm.  For example, the
action could be a link to an Internet radio station, or it could be a
small animated icon.

@return The alarm action or a NULL pointer if there is no alarm action.
@see CCalContent
*/
EXPORT_C CCalContent* CCalAlarm::AlarmAction() const
	{
	return iAlarmAction;
	}


