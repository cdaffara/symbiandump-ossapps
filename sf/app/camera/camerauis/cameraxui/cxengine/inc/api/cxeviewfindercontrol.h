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
#ifndef CXEVIEWFINDERCONTROL_H_
#define CXEVIEWFINDERCONTROL_H_

#include <QObject> 
#include <QWidget> // for WId
#include <QMetaType>
#include "cxeerror.h"

class CxeViewfinderControl : public QObject
{
    Q_OBJECT
public:

    enum State
        {
        Uninitialized = 0x01, // VF not yet initialized
        Ready         = 0x02, // VF initialized but not running
        Running       = 0x04, // VF running
        };

    virtual void setWindow(WId windowId) = 0;

    /*!
    * Start viewfinder 
    * @return CxeEngine specific error code
    */
    virtual CxeError::Id start() = 0;
    
    /*!
    * Stop viewfinder 
    */
    virtual void stop() = 0;
    
    /*!
    * Current viewfinder state
    */
    virtual State state() const = 0;

    /*!
    * Returns Device's Display resolution
    */
    virtual QSize deviceDisplayResolution() const = 0;

signals:
    void stateChanged(CxeViewfinderControl::State newState, CxeError::Id error);

protected:
    CxeViewfinderControl() {}

private:
    Q_DISABLE_COPY(CxeViewfinderControl)
};

Q_DECLARE_METATYPE(CxeViewfinderControl::State)

#endif /*CXEVIEWFINDERCONTROL_H_*/
