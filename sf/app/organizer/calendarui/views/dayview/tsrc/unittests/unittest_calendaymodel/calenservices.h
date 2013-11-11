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
* Description: Mocked classes for testing CalenDayModel class
*
*/

#ifndef AGENDAUTIL_H
#define AGENDAUTIL_H

#include <agendaentry.h>

/*!
 Mocked class AgendaUtil
 */
class AgendaUtil
{
public:
    enum FilterFlags {
        IncludeAppointments = 0x01, 
        IncludeReminders = 0x02,
        IncludeEvents = 0x03
    };

public:
    AgendaUtil() {

    }

    ~AgendaUtil() {

    }
    
    QList<AgendaEntry> createEntryIdListForDay(const QDateTime &date,AgendaUtil::FilterFlags filter)
    {
        Q_UNUSED(date);
        Q_UNUSED(filter);
        QList<AgendaEntry> agenda;
        AgendaEntry entry;
        agenda << entry;
        return agenda;
    }
};

#endif // AGENDAUTIL_H

#ifndef CALENSERVICES_H
#define CALENSERVICES_H

/*!
 Mocked class MCalenServices
 */
class MCalenServices
{
public:
    MCalenServices() : mAgendaUtil(NULL) {
        mAgendaUtil = new AgendaUtil();
    }
    
    ~MCalenServices() {
        if (mAgendaUtil) {
            delete mAgendaUtil;
            mAgendaUtil = NULL;
        }
    }
    
    AgendaUtil* agendaInterface() {
        return mAgendaUtil;
    }
    
public:
    AgendaUtil* mAgendaUtil;
};

#endif // CALENSERVICES_H

// End of file

