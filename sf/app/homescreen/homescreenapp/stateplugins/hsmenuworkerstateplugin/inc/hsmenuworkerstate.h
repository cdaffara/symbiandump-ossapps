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

#ifndef HSMENUWORKERSTATE_H
#define HSMENUWORKERSTATE_H

#include <QObject>
#include <QState>
#include <QStateMachine>
#include <hsmenuevent.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuWorkerState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsMenuWorkerState(QState *parent = 0);

    virtual ~HsMenuWorkerState();
    
signals:

    void reset();    

private:

    void construct();

    template<class T>
    T *createChildState(HsMenuEvent::OperationType operation, 
            QState* parentState, QState* initialState);

};

#endif //HSMenuWorkerState_H
