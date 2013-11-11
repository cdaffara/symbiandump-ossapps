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
* Description: Calendar widget datetime observer
*
*/

#include "datetimeobserver.h"
#include "datetimeobserver_p.h"
#include "calendarwidgetdebug.h"

DateTimeObserver::DateTimeObserver(QObject *parent) :
    QObject(parent),
    d_ptr(0)
{
    LOGS("DateTimeObserver::DateTimeObserver");
    d_ptr = new DateTimeObserverPrivate(this);
}

DateTimeObserver::~DateTimeObserver()
{
    LOGS("DateTimeObserver::~DateTimeObserver");
    delete d_ptr;
}

void DateTimeObserver::handleCompletion()
{
    LOGS("DateTimeObserver::handleCompletion");
    emit completed();
}
