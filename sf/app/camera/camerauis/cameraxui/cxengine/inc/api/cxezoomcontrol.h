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
#ifndef CXEZOOMCONTROL_H
#define CXEZOOMCONTROL_H

#include <QObject>
#include <QMetaType>
#include "cxeerror.h"

class CxeZoomControl : public QObject
    {
    Q_OBJECT
public:

    // State Machine
    enum State {
        Idle       = 0x01,
        Ready      = 0x02,
        ZoomingIn  = 0x04,
        ZoomingOut = 0x08
        };

    /**
     * Returns the state of the control.
     */
    virtual State state() const = 0;

    /**
     * Returns the minimum zoom level/step.
     */
    virtual int min() const = 0;

    /**
     * Returns the maximum zoom levels/steps.
     */
    virtual int max() const = 0;

    /**
     * Zoom to a certain specified zoom level/step.
     */
    virtual void zoomTo( int value ) = 0;

signals:
    /**
     * Signal to notify the observers that zoom level/step has changed.
     */
    void zoomLevelChanged( int zoomPosition );

    /**
     * Signal to notify the observers that state of zoom control has changed.
     */
    void stateChanged( CxeZoomControl::State newState, CxeError::Id error );

protected:
    /**
     * Protected empty contructor so that derived class construction works.
     */
    CxeZoomControl() {}

private:
    Q_DISABLE_COPY( CxeZoomControl )
    };

Q_DECLARE_METATYPE(CxeZoomControl::State)

#endif // CXEZOOMCONTROL_H
