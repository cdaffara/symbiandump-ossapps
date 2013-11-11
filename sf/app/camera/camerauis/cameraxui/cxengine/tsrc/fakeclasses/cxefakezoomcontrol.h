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
#ifndef CXEFAKEZOOMCONTROL_H
#define CXEFAKEZOOMCONTROL_H

#include "cxezoomcontrol.h"
#include "cxestatemachine.h"

class CxeFakeZoomControl : public CxeZoomControl,
                           public CxeStateMachine
{
    Q_OBJECT

public:
    CxeFakeZoomControl();
    ~CxeFakeZoomControl();

    State state() const;
    int min() const;
    int max() const;
    void zoomInOneStep();
    void zoomOutOneStep();
    void zoomTo(int value);

protected:
    void initializeStates();
    void handleStateChanged(int newStateId, CxeError::Id error);
};

#endif // CXEFAKEZOOMCONTROL_H

// End of file
