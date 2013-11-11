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

#ifndef CXETESTUTILS_H
#define CXETESTUTILS_H

#include <QSignalSpy>
#include "cxutils.h"

const int CXE_TEST_POLL_TIME = 10;

class CxeTestUtils
{
public:
    static bool waitForSignal(
        QSignalSpy &spy, int ms, int count = 1, bool allowExtraSignals = true);

    template<typename T> static bool waitForState(
        T& stateMachine, typename T::State state, int ms);
};


/**
* Wait for a state machine to transition to a given state. Returns false if
* target state is not reached withing the given timeout.
*
* This function expects the state machine to emit a
* "stateChanged(T::State...)" signal. Also, the state machine class needs to
* implement an enum called "State". Remember to use the
* Q_DECLARE_METATYPE macro for the State enum.
*
* waitForState() is template based so the implementation needs to be in the
* header file.
*
* @tparam T            State machine type
* @param  stateMachine State machine instance
* @param  state        Target state
* @param  ms           Timeout in milliseconds
* @return              True if target state was reached, false if timed out
*/
template<typename T> bool CxeTestUtils::waitForState(
    T& stateMachine, typename T::State state, int ms)
{
    int waitingTime = 0;
    bool stateReached = false;

    // Find stateChanged() signal
    QByteArray signalSignature;
    const QMetaObject *meta = stateMachine.metaObject();
    for (int i = 0; i < meta->methodCount(); i++) {
        QMetaMethod method = meta->method(i); 
        if (method.methodType() == QMetaMethod::Signal &&
            QByteArray(method.signature()).startsWith("stateChanged(")) {
            signalSignature = method.signature();
            signalSignature.prepend(QSIGNAL_CODE + '0');
        }
    }

    // Listen for stateChanged signals using signal spy
    QSignalSpy spy(&stateMachine, signalSignature.constData());
    if (!spy.isValid()) {
        CX_DEBUG(("waitForState() - unable to set up signal spy - is there a stateChanged() signal?"));
        return false;
    }

    // Loop for waiting for state changes. This terminates when
    // stateMachine.state() returns the target state, a stateChanged signal
    // indicates a state change to the target state, or the timeout is expired.
    while (!stateReached && waitingTime < ms) {
        QTest::qWait(CXE_TEST_POLL_TIME);
        waitingTime += CXE_TEST_POLL_TIME;

        // Check current state
        if (stateMachine.state() == state) {
            CX_DEBUG(("waitForState() - current state matches"));
            stateReached = true;
        }

        // Check state history from signal spy in case
        // there were quick successive state changes
        for (int i = 0; i < spy.count() && !stateReached; i++) {
            if (!spy[i].at(0).canConvert<T::State>()) {
                CX_DEBUG(("waitForState() - can't convert stateChanged params!"));
                CX_DEBUG(("waitForState() - signature is %s!", signalSignature.constData()));
                return false;
            }

            if (spy[i].at(0).value<T::State>() == state) {
                CX_DEBUG(("waitForState() - found signal spy state signal match"));
                stateReached = true;
            }
        }
    }

    if (stateReached) {
        CX_DEBUG(("waitForState() - got  to state %d successfully",
                  static_cast<int>(state)));
    } else {
        CX_DEBUG(("waitForState() - transition to state %d timed out (state is still %d), waitingTime = %d",
                static_cast<int>(state), static_cast<int>(stateMachine.state()), waitingTime));
    }
    return stateReached;
}
#endif // CXETESTUTILS_H
