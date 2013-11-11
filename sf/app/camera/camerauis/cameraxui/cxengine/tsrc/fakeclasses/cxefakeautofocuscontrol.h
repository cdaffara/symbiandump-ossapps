/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXEFAKEAUTOFOCUSCONTROL_H
#define CXEFAKEAUTOFOCUSCONTROL_H

#include "cxeautofocuscontrol.h"
#include "cxestatemachine.h"

class CxeCameraDevice;

class CxeFakeAutoFocusControl : public CxeAutoFocusControl,
                                public CxeStateMachine
{
    Q_OBJECT

public:
    CxeFakeAutoFocusControl();
    virtual ~CxeFakeAutoFocusControl();

    CxeError::Id start(bool soundEnabled = true);
    void cancel();
    void setMode(CxeAutoFocusControl::Mode newMode);
    CxeAutoFocusControl::Mode mode() const;
    bool isFixedFocusMode(Mode mode) const;
    CxeAutoFocusControl::State state() const;
    bool supported() const;
    bool isSoundEnabled() const;

public: // methods for testing
    void setFakeState(CxeAutoFocusControl::State);

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);
    void initializeStates();

private:
    CxeAutoFocusControl::Mode mAfMode;
};

#endif // CXEFAKEAUTOFOCUSCONTROL_H
