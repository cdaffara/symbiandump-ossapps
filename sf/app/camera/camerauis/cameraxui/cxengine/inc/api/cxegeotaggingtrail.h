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

#ifndef CXEGEOTAGGINGTRAIL_H
#define CXEGEOTAGGINGTRAIL_H

#include <QObject>
#include <QMetaType>
#include "cxeerror.h"

class CxeSettings;
class CxeStillCaptureControl;
class CxeVideoCaptureControl;

class CxeGeoTaggingTrailPrivate;

class CxeGeoTaggingTrail : public QObject
{
    Q_OBJECT

public:
    
    //! Geotagging trail states
    enum State {
        //! location utility not connected
        NotConnected            = 0x01,

        //! connected to location utility, but location trail NOT STARTED.
        Connected               = 0x02,
        
        //! starting Location trail, GPS data not yet available
        TrailStarted            = 0x04,
        
        //! location trail started and GPS data available.
        DataAvailable           = 0x08
        };

public:

    CxeGeoTaggingTrail(CxeStillCaptureControl &stillControl,
                       CxeVideoCaptureControl &videoControl,
                       CxeSettings &settings);

    ~CxeGeoTaggingTrail();

signals:
    void stateChanged(CxeGeoTaggingTrail::State newState, CxeError::Id error);

public slots:
    void start();
    void stop();

public:
    State state();

protected:
    CxeGeoTaggingTrailPrivate *const d_ptr;

private:
    Q_DECLARE_PRIVATE(CxeGeoTaggingTrail)

};

Q_DECLARE_METATYPE(CxeGeoTaggingTrail::State)

#endif // CXEGEOTAGGINGTRAIL_H
