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

#ifndef CXEAUTOFOCUSCONTROL_H
#define CXEAUTOFOCUSCONTROL_H

#include <QObject>
#include <QMetaType>
#include "cxeerror.h"

class QImage;


/**
 * CxeAutoFocusControl is responsible for handling Autofocus events from the camera adaptation.
 * It also routes the Autofocus successful and failed events to the clients interested in ´knowing the
 * status of Autofocus.
 */
class CxeAutoFocusControl : public QObject
    {
    Q_OBJECT
public:

    /*
     * Autofocus control states.
     */
    enum State
        {
        //! focusing
        InProgress = 0x01,

        //! Autofocus Failed
        Failed     = 0x02,

        //! Focused
        Ready      = 0x04,

        //! State after successfully canceling AF or during startup
        Unknown    = 0x08,

        //! Starting to cancel any pending AF requests
        Canceling  = 0x10     // Starting to Cancel any pending AF requests
        };

    // TODO: Check if we have to get all the supported modes from adaptation.
    enum Mode
        {
        //! AF mode: Auto
        Auto,

        //! AF mode: Hyperfocal
        Hyperfocal,

        //! AF mode: Macro
        Macro,

        //! AF mode: Infinity
        Infinity,

        //! AF mode: Portrait
        Portrait
        };

    /**
     * Starts Autofocus.
     * \param soundEnabled indicates if the auto focus sound should be played
     */
    virtual CxeError::Id start(bool soundEnabled = true) = 0;

    /**
     * Cancels Autofocus.
     */
    virtual void cancel() = 0;                // cancels active start operation


    /**
     * Setting Autofocus Mode.
     */
    virtual void setMode( Mode newMode ) = 0; // sets default value for current view/mode

    /**
     * Get current Autofocus mode.
     */
    virtual Mode mode() const = 0;

    /**
     * Is the mode a fixed focus mode.
     */
    virtual bool isFixedFocusMode(Mode mode) const = 0;

    /**
     * Get current Autofocus state.
     */
    virtual State state() const = 0;                // returns current AF state, one of AutoFocusState

    /**
     * Check if Autofocus is Supported or Not.
     */
    virtual bool supported() const = 0;             // true when AF is supported

    /**
    * Check if auto focus sound is enabled
    */
    virtual bool isSoundEnabled() const = 0;

signals:
    void stateChanged( CxeAutoFocusControl::State newState, CxeError::Id error );

protected:
    CxeAutoFocusControl() {} // protected empty contructor so that derived class construction works

private:
    Q_DISABLE_COPY( CxeAutoFocusControl )
    };

Q_DECLARE_METATYPE(CxeAutoFocusControl::State)

#endif // CXEAUTOFOCUSCONTROL_H
