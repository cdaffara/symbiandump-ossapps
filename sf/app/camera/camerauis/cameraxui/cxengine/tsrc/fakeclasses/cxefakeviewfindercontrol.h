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
#ifndef CXEFAKEVIEWFINDERCONTROL_H
#define CXEFAKEVIEWFINDERCONTROL_H

#include <e32cmn.h>

#include "cxeviewfindercontrol.h"
#include "cxestatemachine.h"


class CxeFakeViewfinderControl : public CxeViewfinderControl,
                                 public CxeStateMachine

{
    Q_OBJECT

public:

    CxeFakeViewfinderControl();
    virtual ~CxeFakeViewfinderControl();

public: // from CxeViewfinderControl
    void setWindow(WId windowId);
    CxeError::Id start();
    void stop();
    State state() const;
    QSize deviceDisplayResolution() const;

public: // methods for unit testing
    enum MethodIndex
    {
        SetWindow,
        Start,
        Stop
    };

    void setState(CxeViewfinderControl::State newState);
    QList<MethodIndex> callHistory() const;
    void resetCallHistory();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);
    void initializeStates();

private: // data
    QList<MethodIndex> mCallHistory;
};



#endif // CXEFAKEVIEWFINDERCONTROL_H

