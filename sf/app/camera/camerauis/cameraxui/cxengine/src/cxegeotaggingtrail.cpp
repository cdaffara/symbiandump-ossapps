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

#include "cxutils.h"
#include "cxegeotaggingtrail.h"
#ifdef Q_OS_SYMBIAN
#include "cxegeotaggingtrail_symbian_p.h"
#else
#include "cxegeotaggingtrail_desktop_p.h"
#endif // ifdef Q_OS_SYMBIAN

/*!
* Constructor
*/
CxeGeoTaggingTrail::CxeGeoTaggingTrail(CxeStillCaptureControl &stillControl,
                                       CxeVideoCaptureControl &videoControl,
                                       CxeSettings &settings)
    : d_ptr(new CxeGeoTaggingTrailPrivate(stillControl, videoControl, settings))
{
    // connect signal state changed signal.
    connect(d_ptr, SIGNAL(stateChanged(CxeGeoTaggingTrail::State, CxeError::Id)), 
            this, SIGNAL(stateChanged(CxeGeoTaggingTrail::State, CxeError::Id)),
            Qt::UniqueConnection);
}

/*!
* Destructor
*/
CxeGeoTaggingTrail::~CxeGeoTaggingTrail()
{
    delete d_ptr;
}

/*!
* Slot to start location trail.
*/
void CxeGeoTaggingTrail::start()
{
    Q_D(CxeGeoTaggingTrail);
    d->start();
}

/*!
* Slot to stop location trail.
*/
void CxeGeoTaggingTrail::stop()
{
    Q_D(CxeGeoTaggingTrail);
    d->stop();
}

/*!
* Returns current state of Geo-tagging trail.
*/
CxeGeoTaggingTrail::State CxeGeoTaggingTrail::state()
{
    Q_D(CxeGeoTaggingTrail);
    return d->state();
}

// end of file
