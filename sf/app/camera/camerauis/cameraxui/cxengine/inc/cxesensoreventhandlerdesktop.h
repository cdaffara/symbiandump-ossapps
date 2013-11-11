/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXESENSOREVENTHANDLERDESKTOP_H_
#define CXESENSOREVENTHANDLERDESKTOP_H_

#include <QObject>
#include <QMetaType>

#include "cxesensoreventhandler.h"

class CxeSensorEventHandlerDesktop : public CxeSensorEventHandler
{
    Q_OBJECT

public:

     // ! Sensor types handled and supported in cxengine.
    enum SensorType
        {
        //! Listening to orientation sensor events
        OrientationSensor
        };

    CxeSensorEventHandlerDesktop();
    virtual ~CxeSensorEventHandlerDesktop();

    /**
    * Data read from sensor. The "type" of data embedded in QVariant is specific to sensor type.
    */
    virtual QVariant sensorData(CxeSensorEventHandler::SensorType type);

public slots:
    virtual void init();
    virtual void deinit();

signals:
    /*
    * A signal indicating that there is new sensor event data available.
    */
    void sensorEvent(CxeSensorEventHandler::SensorType type, QVariant data);

};
#endif /* CXESENSOREVENTHANDLERDESKTOP_H_ */
