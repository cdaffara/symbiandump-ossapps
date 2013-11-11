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


#ifndef HSMENUEVENTTRANSITION_H
#define HSMENUEVENTTRANSITION_H

#include <QEvent>
#include <QObject>
#include <qeventtransition.h>

#include "hsmenuevent.h"

#include "hstest_global.h"
#include "hsutils_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)



class HSUTILS_EXPORT HsMenuEventTransition: public QEventTransition
{

public:

    HsMenuEventTransition(
        HsMenuEvent::OperationType aOperation, QState *aSourceState, QState *aTargetState);

protected:

    bool eventTest(QEvent *event);

private:

    HsMenuEvent::OperationType mOperation;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)

};


#endif //HSMENUEVENTTRANSITION_H
