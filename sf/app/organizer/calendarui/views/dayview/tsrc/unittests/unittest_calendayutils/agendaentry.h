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
* Description: Mocked class AgendaEntry
*
*/

#ifndef AGENDAENTRY_H
#define AGENDAENTRY_H

// System includes
#include <QObject>
#include <QDate>
#include <QDateTime>

class AgendaEntry
{
public:
    AgendaEntry() {
        
    }
    virtual ~AgendaEntry() {
        
    }
    
    bool setStartAndEndTime(
            const QDateTime& startTime, const QDateTime& endTime) {
        mStartTime = startTime;
        mEndTime = endTime;
        
        return true;
    }
    
    QDateTime startTime() const {
        return mStartTime;
    }
    
    QDateTime endTime() const {
        return mEndTime;
    }

    QDateTime mStartTime;
    QDateTime mEndTime;
};

#endif // AGENDAENTRY_H

// End of file  --Don't remove this.
