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

DateTimeObserverPrivate::DateTimeObserverPrivate(DateTimeObserver *q) :
    q_ptr(q)
{
    LOGS("DateTimeObserverPrivate::DateTimeObserverPrivate");
    TCallBack callback(LocaleChangeCallback, this);
    iLocaleNotifier = CEnvironmentChangeNotifier::NewL(CActive::EPriorityStandard, callback);
    iLocaleNotifier->Start();
}

DateTimeObserverPrivate::~DateTimeObserverPrivate()
{
    LOGS("DateTimeObserverPrivate::~DateTimeObserverPrivate");
    delete iLocaleNotifier;
}

TInt DateTimeObserverPrivate::HandleLocaleChange()
{
    LOGS("DateTimeObserverPrivate::HandleLocaleChange");
    TInt handled = 0;
    if (iLocaleNotifier->Change() & (EChangesSystemTime | EChangesLocale | EChangesMidnightCrossover)) {
        q_ptr->handleCompletion();
        handled = 1;
    }

    return handled;
}

TInt DateTimeObserverPrivate::LocaleChangeCallback(TAny* aThisPtr)
{
    LOGS("DateTimeObserverPrivate::LocaleChangeCallback");
    TInt ret = static_cast<DateTimeObserverPrivate*>(aThisPtr)->HandleLocaleChange();
    return ret;
}
