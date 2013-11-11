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
* Description:  Menu Event Transition.
*
*/

#include <QState>
#include "hsmenueventtransition.h"


/*!
    \class HsMenuEventTransition
    \ingroup group_hsutils
    \brief Transition that happens when specifc event occurs.

    \sa HsMenuEvent
*/

/*!
    Constructor
    \param aOperation Menu event specific type.
    \param aSourceState Source state.
    \param aTargetState Target state.
*/
HsMenuEventTransition::HsMenuEventTransition(
    HsMenuEvent::OperationType aOperation, QState *aSourceState, QState *aTargetState) :
    QEventTransition(aSourceState),
    mOperation(aOperation)
{
    setTargetState(aTargetState);
}

/*!
    Reimplements method from QEventTransition. Calls the base class method
    passing \a event. Also stores \a event properties in the transition.
    \param event Event to test.
    \return \a true if \a event is going to trigger the transition.
*/
bool HsMenuEventTransition::eventTest(QEvent *event)
{
    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *me = static_cast<HsMenuEvent *>(event);
        bool result = me->operation() == mOperation;

        return result;
    } else {
        return false;
    }
}


