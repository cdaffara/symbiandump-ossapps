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
#ifndef CXESENSOREVENTHANDLER_H_
#define CXESENSOREVENTHANDLER_H_

#include <QObject>
#include <QMetaType>

#include "cxenamespace.h"


/**
 * Sensor Event Handler implements basic sensor's events handling. It
 * automatically updates the clients intersted in knowing the sensor events
 * that is currently supported and used by camera app.
 *
 * All the supported sensor types are defined by enum "SensorType"
 * This class provides one signal which basically carries out corresponding sensor data for
 * the given sensor type.
 */
class CxeSensorEventHandler : public QObject
{
    Q_OBJECT

public:

     // ! Sensor types handled and supported in cxengine.
    enum SensorType
        {
        //! Listening to orientation sensor events
        OrientationSensor
        };

    /**
    * Data read from sensor. The "type" of data embedded in QVariant is specific to sensor type.
    */
    virtual QVariant sensorData(CxeSensorEventHandler::SensorType type) = 0;

public slots:
    /*!
        init sensors
    */
    virtual void init() = 0;

    /*!
        de-init sensors
    */
    virtual void deinit() = 0;

signals:
    /*
    * A signal indicating that there is new sensor event data available.
    */
    void sensorEvent(CxeSensorEventHandler::SensorType type,QVariant data);

protected:
    CxeSensorEventHandler() {} // protected empty contructor so that derived class construction works

private:
    Q_DISABLE_COPY( CxeSensorEventHandler )
};

Q_DECLARE_METATYPE(Cxe::DeviceOrientation)

#endif /* CXESENSOREVENTHANDLER_H_ */
