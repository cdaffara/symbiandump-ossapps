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

#ifndef __CALALARM_H__
#define __CALALARM_H__

#include <calcontent.h>

/** Alarm category Id for Calendar alarms
@publishedAll
@released
*/
const TUid KUidAgendaModelAlarmCategory = { 0x101F4A70 };

NONSHARABLE_CLASS(CCalAlarm) : public CBase
/** Class representing an alarm. contains associated data extensions

This can be assigned to a calendar entry.

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CCalAlarm* NewL();
	IMPORT_C ~CCalAlarm();
	IMPORT_C void SetTimeOffset(TTimeIntervalMinutes aOffset);
	IMPORT_C TTimeIntervalMinutes TimeOffset() const;
	IMPORT_C void SetAlarmSoundNameL(const TDesC& aAlarmSoundName);
	IMPORT_C const TDesC& AlarmSoundNameL() const;
	IMPORT_C void SetAlarmAction(CCalContent* aAlarmAction);
	IMPORT_C CCalContent* AlarmAction() const;

private:
	CCalAlarm();

private:
	TTimeIntervalMinutes iOffset;
	HBufC* iAlarmName;
	// the associated data for rich alarm action
	CCalContent* iAlarmAction;
	};


#endif // __CALALARM_H__
 
