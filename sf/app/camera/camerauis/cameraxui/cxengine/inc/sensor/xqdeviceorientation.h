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
#ifndef XQDEVICEORIENTATION_H
#define XQDEVICEORIENTATION_H

#include <QObject>
#include "sensor_global.h"

class XQDeviceOrientationPrivate;

class XQDeviceOrientation : public QObject
{
    Q_OBJECT

public:
    enum DisplayOrientation
    {
        OrientationUndefined = 0,
        OrientationDisplayUp,
        OrientationDisplayDown,
        OrientationDisplayLeftUp,
        OrientationDisplayRightUp,
        OrientationDisplayUpwards,
        OrientationDisplayDownwards
    };

public:
    XQDeviceOrientation(QObject* parent = 0);
    virtual ~XQDeviceOrientation();
    
    void open();
    void close();
    
    void setResolution(int resolution);
    int resolution() const;

    int xRotation() const;
    int yRotation() const;
    int zRotation() const;
    XQDeviceOrientation::DisplayOrientation orientation() const;

Q_SIGNALS:
    void rotationChanged(int xRotation, int yRotation, int zRotation);
    void orientationChanged(XQDeviceOrientation::DisplayOrientation orientation);

private:
    friend class XQDeviceOrientationPrivate;
    XQDeviceOrientationPrivate* d;
};

#endif /*XQDEVICEORIENTATION_H*/

// End of file
