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

#include "t_hsaddtohomescreenmockstate.h"


AddToHomeScreenMockState::AddToHomeScreenMockState(QState *aParent):
    QState(aParent), mEntered(false) {}
void AddToHomeScreenMockState::onEntry(QEvent *e)
{
    Q_UNUSED(e);
    mEntered = true;
}

bool AddToHomeScreenMockState::enteredValue() const
{
    return mEntered;
}
