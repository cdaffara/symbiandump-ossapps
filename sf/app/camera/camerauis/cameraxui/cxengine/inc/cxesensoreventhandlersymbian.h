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

#ifndef CXESENSOREVENTHANDLERSYMBIAN_H
#define CXESENSOREVENTHANDLERSYMBIAN_H

#include "cxesensoreventhandler.h"
#include "xqdeviceorientation.h"
#include "cxenamespace.h"

// forward delcaration
class XQDeviceOrientation;

/*!
    \class CxeSensorEventHandlerSymbian.
    Implements CxeSensorEventHandler.
*/
class CxeSensorEventHandlerSymbian : public CxeSensorEventHandler
{
    Q_OBJECT

public:
    CxeSensorEventHandlerSymbian();
    ~CxeSensorEventHandlerSymbian();

    /*!
        Data read from sensor. The type of data embedded in QVariant is
        specific to sensor type.
    */
    QVariant sensorData(CxeSensorEventHandler::SensorType type);

public slots:
    /*!
        init sensors
    */
    void init();

    /*!
        de-init sensors
    */
    void deinit();

private slots:
    void handleOrientationChanged(XQDeviceOrientation::DisplayOrientation sensorData);

private:
    void enableOrientationSensor();
    void disableOrientationSensor();
    Cxe::DeviceOrientation mapOrientationSensorData(
            XQDeviceOrientation::DisplayOrientation sensorData);

private:
    XQDeviceOrientation *mOrientationSensor;
};


#endif // CXESENSOREVENTHANDLERSYMBIAN_H
