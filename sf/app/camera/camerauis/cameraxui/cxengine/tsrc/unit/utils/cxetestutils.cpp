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
* Description:
*
*/

#include <QSignalSpy>
#include <QTest>

#include "cxetestutils.h"

/*!
* Wait for a signal (or multiple instances of the same signal) to be caught
* by signal spy. Returns false if signal is not received within the given
* timeout.
*
* @param spy               Signal spy attached to the signal to wait for.
* @param ms                Timeout in milliseconds
* @param count             How many instances of the signal to wait for
*                          (default 1)
* @param allowExtraSignals True to ignore any extranous signal. Setting
*                          this as false will cause this function to return
*                          false the spy receives more signals than received
* @return       True if signal(s) were received, false if timed out or
*               signal spy was invalid.
*/
bool CxeTestUtils::waitForSignal(
    QSignalSpy &spy, int ms, int count, bool allowExtraSignals)
{
    int waitingTime = 0;

    if (spy.isValid()) {
        while (spy.count() < count && waitingTime < ms) {
            QTest::qWait(CXE_TEST_POLL_TIME);
            waitingTime += CXE_TEST_POLL_TIME;
        }
        if (spy.count() > count && !allowExtraSignals) {
            CX_DEBUG(("waitForSignal() - got too many signals for %s, count = %d",
                      spy.signal().constData(), spy.count()));
            return false;
        } else if (spy.count() >= count) {
            CX_DEBUG(("waitForSignal() - got signal %s successfully, count = %d",
                      spy.signal().constData(), spy.count()));
            return true;
        } else {
            CX_DEBUG(("waitForSignal() - signal %s timed out, waitingTime = %d",
                      spy.signal().constData(), waitingTime));
            return false;
        }
    } else {
        CX_DEBUG(("waitForSignal() - given signalspy is not valid!"));
        return false;
    }
}
