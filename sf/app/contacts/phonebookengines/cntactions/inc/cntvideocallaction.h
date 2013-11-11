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

#ifndef QMOBCNTVIDEOCALLACTION_H
#define QMOBCNTVIDEOCALLACTION_H

#include "cntaction.h"

class CntVideoCallAction : public CntAction
{
    Q_OBJECT

public:
    CntVideoCallAction();
    ~CntVideoCallAction();

    CntVideoCallAction* clone() const;
    void performAction();
};

#endif
