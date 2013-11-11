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

#ifndef T_HS_ADD_TO_HOME_SCREEN_MOCK_STATE_H_
#define T_HS_ADD_TO_HOME_SCREEN_MOCK_STATE_H_

#include <qstandarditemmodel.h>
#include <qstate.h>
#include "caentry.h"
#include "caitemmodel.h"

class AddToHomeScreenMockState: public QState
{
    Q_OBJECT
    bool mEntered;
public:
    AddToHomeScreenMockState(QState *aParent = 0);
    bool enteredValue() const;
    void setEnteredValue(bool entered) {
        mEntered = entered;
    };

protected:
    void onEntry(QEvent *e);
};

#endif
