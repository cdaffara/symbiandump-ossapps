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

#include <QMetaType>
#include <QImage>

//#include "cxesettings.h"
#include "cxeautofocuscontroldesktop.h"
#include "cxutils.h"
#include "cxestate.h"

/*
* CxeAutoFocusControlSymbian::CxeAutoFocusControlSymbian
*/
CxeAutoFocusControlDesktop::CxeAutoFocusControlDesktop()
    : mCancelled(false),
      mState(Unknown)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeAutoFocusControl::State>();

    initializeStates();
    initializeResources();

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* CxeAutoFocusControlDesktop::~CxeAutoFocusControlDesktop
*/
CxeAutoFocusControlDesktop::~CxeAutoFocusControlDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*
* Start Autofocus
*/
CxeError::Id CxeAutoFocusControlDesktop::start(bool soundEnabled)
{
    Q_UNUSED(soundEnabled);

    CX_DEBUG( ("CxeAutoFocusControlDesktop::start() <> state: %d", state() ) );

    CxeError::Id err = CxeError::None;

    if (state() != CxeAutoFocusControl::InProgress && state() != CxeAutoFocusControl::Canceling) {
        CX_DEBUG(("CxeAutoFocusControlDesktop::start() calling SetAutoFocusType"));
        mCancelled = false;
        mState = Ready;
        emit stateChanged(mState, CxeError::None);
    } else { // AF was started earlier, can't start until it completes
        err = CxeError::InUse;
    }

    CX_DEBUG( ("CxeAutoFocusControlDesktop::start() <= err : %d", err ) );

    return err;
}

/*
* Cancel Autofocus
*/
void CxeAutoFocusControlDesktop::cancel()
{
    CX_DEBUG( ("CxeAutoFocusControlSymbian::cancel <> state: %d", state() ) );

    if (!mCancelled) {
        if (state() == CxeAutoFocusControl::InProgress) {
            // Need to stop current AF first. Wait for AF event to proceed.
            mState = Canceling;
            emit stateChanged(mState, CxeError::None);
        } else if (state() != CxeAutoFocusControl::Canceling) {
            // Cancel means move to hyperfocal.
            mState = Canceling;
            emit stateChanged(mState, CxeError::None);
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*
* Set Autofocus mode
*/
void CxeAutoFocusControlDesktop::setMode(CxeAutoFocusControl::Mode newMode)
{
    CX_DEBUG_ENTER_FUNCTION();

    mAfMode = newMode;
    mCancelled = false;

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* returns Autofocus mode
*/
CxeAutoFocusControl::Mode CxeAutoFocusControlDesktop::mode() const
{
    return mAfMode;
}

/**
* Is the given mode a fixed focus mode?
*/
bool CxeAutoFocusControlDesktop::isFixedFocusMode(CxeAutoFocusControl::Mode mode) const
{
    return (mode == CxeAutoFocusControl::Hyperfocal
         || mode == CxeAutoFocusControl::Infinity);
}

/*
* To check if Autofocus is supported
*/
bool CxeAutoFocusControlDesktop::supported() const
{
    CX_DEBUG_ENTER_FUNCTION();

    bool supported = true;
    CX_DEBUG_EXIT_FUNCTION();
    return supported;
}

/*
* CxeAutoFocusControlDesktop::state
*/
CxeAutoFocusControl::State CxeAutoFocusControlDesktop::state() const
{
    return mState;
}

/*
* CxeAutoFocusControlDesktop::initializeStates
*/
void CxeAutoFocusControlDesktop::initializeStates()
{
}

/*
* CxeAutoFocusControlDesktop::initializeResources
*/
void CxeAutoFocusControlDesktop::initializeResources()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
  * Public method for checking if auto focus sound is enabled
  * \return true if enabled
  */
bool CxeAutoFocusControlDesktop::isSoundEnabled() const
{
    return false; // should actually return mSoundEnabled
}

// end of file
