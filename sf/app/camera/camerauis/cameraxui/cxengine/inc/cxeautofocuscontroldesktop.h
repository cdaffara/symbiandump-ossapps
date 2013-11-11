/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXEAUTOFOCUSCONTROLDESKTOP_H
#define CXEAUTOFOCUSCONTROLDESKTOP_H

#include <QVariant>
#include "cxenamespace.h"
#include "cxeautofocuscontrol.h"

/**
 * CxeAutoFocusControl is responsible for handling Autofocus events from the camera adaptation.
 * It also routes the Autofocus successful and failed events to the clients interested in ´knowing the
 * status of Autofocus.
 */
class CxeAutoFocusControlDesktop: public CxeAutoFocusControl
{
    Q_OBJECT
public:

    CxeAutoFocusControlDesktop();
    virtual ~CxeAutoFocusControlDesktop();

    CxeError::Id  start(bool soundEnabled = true);
    void cancel();
    void setMode(CxeAutoFocusControl::Mode newMode);
    CxeAutoFocusControl::Mode mode() const;
    virtual bool isFixedFocusMode(Mode mode) const;


    CxeAutoFocusControl::State state() const;
    bool supported() const;
    bool isSoundEnabled() const;

protected:
    void initializeStates();
    // intializes all the resources we need for AF functionality
    void initializeResources();

protected:
    CxeAutoFocusControl::Mode mAfMode;
    CxeAutoFocusControl::State mState;
    bool mCancelled;
};

#endif // CXEAUTOFOCUSCONTROLDESKTOP_H
