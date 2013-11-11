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

#include "cxutils.h"
#include "cxuieventlog.h"

/*!
* Event log item constructor.
* @param type Event type description
* @param id Event id description
*/
CxuiEventLog::CxuiEvent::CxuiEvent(const QString &type, const QString &value)
    : mTime(QTime::currentTime()), mType(type), mValue(value)
{
}


/*!
* Event log constructor.
* @param name Event log name.
* @param size Maximum number of events stored to log. After maximum size is encountered,
* old events are discarded as new ones are added.
*/
CxuiEventLog::CxuiEventLog(const QString& name, int size)
    : mSize(size),
      mName(name)
{
}

/*!
* Event log destructor.
*/
CxuiEventLog::~CxuiEventLog()
{
}

/*!
* Append new event to the log.
* @param type Event type description
* @param id Event id description
*/
void CxuiEventLog::append(const QString &type, const QString &value)
{
    if (mSize) {
        if (mEvents.size() >= mSize) {
            mEvents.removeFirst();
        }
        mEvents.append(CxuiEvent(type, value));
    }
}

/*!
* Print out the event log.
*/
void CxuiEventLog::print() const
{
    if (mEvents.size()) {

        CX_DEBUG(("[INFO] >>> CameraX event log[%s]", mName.toAscii().constData()));
        foreach(CxuiEvent e, mEvents) {
            CX_DEBUG(("[INFO] <-> event: time[%s] type[%s] value[%s]",
                      e.mTime.toString("HH:mm:ss.zzz").toAscii().constData(),
                      e.mType.toAscii().constData(),
                      e.mValue.toAscii().constData()));
        }
        CX_DEBUG(("[INFO] <<< CameraX event log[%s]", mName.toAscii().constData()));
    }
}

// end of file
