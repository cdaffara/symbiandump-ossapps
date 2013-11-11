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
#ifndef CXEFAKECAMERADEVICE_H
#define CXEFAKECAMERADEVICE_H

#include <QObject>

#include "cxecameradevice.h"


/*
 * CxeFakeCameraDevice class is derived from CxeCameraDevice, since there is no interface class
 */
class CxeFakeCameraDevice  : public CxeCameraDevice
{
    Q_OBJECT

public:
    CxeFakeCameraDevice();
    virtual ~CxeFakeCameraDevice();

public:
    // only this methdod is overridden, other implementation comes from the base class
    CxeError::Id newCamera(Cxe::CameraIndex cameraIndex, MCameraObserver2 *observer);


friend class CxeCameraDeviceControlSymbian;
};


#endif  // CXEFAKECAMERADEVICE_H

