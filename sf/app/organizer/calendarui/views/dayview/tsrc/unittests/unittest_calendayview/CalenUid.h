/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UIDS used by the S60 Calendar
*
*/


#ifndef CALENUID_H
#define CALENUID_H

// UID of the S60 Calendar application
const TUid KUidCalendar         = { 0x10005901 };

// UIDS of the S60 Calendar native views
const TInt KCalenMonthViewUidValue        = 0x00000000 ;
const TInt KCalenAgendaViewUidValue         = 0x00000002;
const TInt KCalenEventViewUidValue   	  = 0x00000003;
const TInt KCalenLandscapeDayViewUidValue = 0x00000004;
const TInt KCalenTodoEditorViewUidValue         = 0x00000005;
const TInt KCalenSettingsViewUidValue         = 0x00000006;


const TUid KUidCalenMonthView        = { KCalenMonthViewUidValue };
const TUid KUidCalenAgendaView      = { KCalenAgendaViewUidValue };
const TUid KCalenTodoEditorView      = { KCalenTodoEditorViewUidValue };
const TUid KUidCalenEventView        = { KCalenEventViewUidValue };
const TUid KUidCalenLandscapeDayView = { KCalenLandscapeDayViewUidValue };
const TUid KUidCalenSettingsView = { KCalenSettingsViewUidValue };

const TUid KCalenChineseImplUid = {0x2002EA3D};

#endif // CALENUID_H

// End of file
