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
#ifndef XQDEVICEORIENTATIONPRIVATE_H
#define XQDEVICEORIENTATIONPRIVATE_H

#include "xqaccsensor.h"
#include "xqdeviceorientation.h"

class XQDeviceOrientationPrivate : public XQAbstractAccelerationSensorFilter
{
public:
    XQDeviceOrientationPrivate(XQDeviceOrientation& qq);
    virtual ~XQDeviceOrientationPrivate();

    void open();
    void close();

    void setResolution(TInt resolution);
    TInt resolution() const;

    int xRotation() const;
    int yRotation() const;
    int zRotation() const;
    XQDeviceOrientation::DisplayOrientation orientation() const;

protected:  //from XQAbstractAccelerationSensorFilter
    bool filter(int& xAcceleration, int& yAcceleration, int& zAcceleration);

private:
    TInt RoundAngle(TReal aAngle) const;

private:
    XQDeviceOrientation& q;
    XQAccelerationSensor iAccelerationSensor;
    XQAccelerationDataPostFilter iAccelerationPostFilter;
    TInt iResolution;
    TInt iXRotation;
    TInt iYRotation;
    TInt iZRotation;
    XQDeviceOrientation::DisplayOrientation iOrientation;
};

#endif /*XQDEVICEORIENTATIONPRIVATE_H*/

// End of file
