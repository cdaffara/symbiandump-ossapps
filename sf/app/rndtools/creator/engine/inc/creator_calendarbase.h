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




#ifndef CCREATORCALENDARBASE_
#define CCREATORCALENDARBASE_

#include "creator_modulebase.h"

class CCreatorModuleBaseParameters;

class CCreatorCalendarBase : public CCreatorModuleBase{
public:


    //An appointment, which has a start time and end time. EAppt.
    virtual TInt CreateAppointmentEntryL(CCreatorModuleBaseParameters *aParameters) = 0;
    //An event, which has a start time and end time. EEvent.
    virtual TInt CreateEventEntryL(CCreatorModuleBaseParameters *aParameters) = 0;
    //An anniversary, which has a start time and end time. EAnniv.
    virtual TInt CreateAnniversaryEntryL(CCreatorModuleBaseParameters *aParameters) = 0;
    //A to-do, which can have a start time and end time (the end time is the due date), or can be undated. ETodo.
    virtual TInt CreateTodoEntryL(CCreatorModuleBaseParameters *aParameters) = 0;     
    //A reminder, which has a start time only. EReminder.
    virtual TInt CreateReminderEntryL(CCreatorModuleBaseParameters *aParameters) = 0;

};

#endif /*CCREATORCALENDARBASE_*/
