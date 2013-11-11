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
#ifndef CXEDUMMYSTATEMACHINE_H
#define CXEDUMMYSTATEMACHINE_H

#include <QObject>
#include <QQueue>
#include "cxestatemachine.h"
#include "cxenamespace.h"

class CxeDummyStateMachine : public QObject, public CxeStateMachine
{
    Q_OBJECT

public:
    enum State {
        A = 0x01,
        B = 0x02,
        C = 0x04
        };

public:
    CxeDummyStateMachine();
    virtual ~CxeDummyStateMachine();

    State state() const;

    void delayedStateChange(State targetState, int ms);

signals:
    void stateChanged(CxeDummyStateMachine::State newState, CxeError::Id error);

public slots:
    void timeout();

protected:
    void handleStateChanged(int newStateId, CxeError::Id error);

private:
    QQueue<State> mTargetStateQueue;
    int mDelay;
};

Q_DECLARE_METATYPE(CxeDummyStateMachine::State)

#endif // CXEDUMMYSTATEMACHINE_H
