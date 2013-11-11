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

#ifndef CXUIEVENTLOG_H
#define CXUIEVENTLOG_H

#include <QString>
#include <QTime>
#include <QLinkedList>

/*!
* Class for saving events in run-time to be printed / traced out later.
* Useful when tracing is not possible when the events occur, but can be done later.
*/
class CxuiEventLog
{
public:
    CxuiEventLog(const QString &name, int size = 10);
    ~CxuiEventLog();

    void append(const QString &type, const QString &value);
    void print() const;

private:
    struct CxuiEvent
    {
    public:
        CxuiEvent(const QString &type, const QString &value);

        QTime mTime;
        QString mType;
        QString mValue;
    };

    QLinkedList<CxuiEvent> mEvents;
    int mSize;
    QString mName;
};

#endif // CXUIEVENTLOG_H
