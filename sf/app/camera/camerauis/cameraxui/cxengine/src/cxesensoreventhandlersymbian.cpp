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

#include "xqdeviceorientation.h"
#include "cxesensoreventhandlersymbian.h"
#include "cxutils.h"


CxeSensorEventHandlerSymbian::CxeSensorEventHandlerSymbian()
: mOrientationSensor(NULL)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

CxeSensorEventHandlerSymbian::~CxeSensorEventHandlerSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}



/*
* initializes and enables all supported sensor events
*/
void CxeSensorEventHandlerSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    enableOrientationSensor();
    CX_DEBUG_EXIT_FUNCTION();
}


/*
* closes all supported sensors.
*/
void CxeSensorEventHandlerSymbian::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();
    disableOrientationSensor();
    CX_DEBUG_EXIT_FUNCTION();
}


/*
 * Initializes and opens connection to recieve orientation sensor events
 */
void CxeSensorEventHandlerSymbian::enableOrientationSensor()
{
    CX_DEBUG_ENTER_FUNCTION();

    if( mOrientationSensor == NULL) {
        mOrientationSensor = new XQDeviceOrientation(this);
        if (mOrientationSensor) {

        // connect signals from orientationsensor, so we recieve events when orientation changes
        QObject::connect( mOrientationSensor,
                          SIGNAL(orientationChanged(XQDeviceOrientation::DisplayOrientation)),
                          this,SLOT(handleOrientationChanged(XQDeviceOrientation::DisplayOrientation)) );

        // open the sensor connection
        mOrientationSensor->open();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Closes and destroys connection to orientation sensor events.
 */
void CxeSensorEventHandlerSymbian::disableOrientationSensor()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mOrientationSensor) {
        mOrientationSensor->close();
        delete mOrientationSensor;
        mOrientationSensor = NULL;
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*
 * Closes and destroys connection to orientation sensor events.
 */
QVariant CxeSensorEventHandlerSymbian::sensorData(CxeSensorEventHandlerSymbian::SensorType type)
{
    CX_DEBUG_ENTER_FUNCTION();

    QVariant data;

    if (mOrientationSensor && type == CxeSensorEventHandler::OrientationSensor) {
        data = qVariantFromValue<Cxe::DeviceOrientation > (mapOrientationSensorData(mOrientationSensor->orientation()));
    }

    CX_DEBUG_EXIT_FUNCTION();

    return data;
}


/*
 * Emits signal whenever we receive data from orientation sensor.
 */
void CxeSensorEventHandlerSymbian::handleOrientationChanged(XQDeviceOrientation::DisplayOrientation sensorData)
{
    CX_DEBUG_ENTER_FUNCTION();

    QVariant data = qVariantFromValue<Cxe::DeviceOrientation > (mapOrientationSensorData(sensorData));
    emit sensorEvent(CxeSensorEventHandler::OrientationSensor, data);

    CX_DEBUG_EXIT_FUNCTION();
}


/*
* Maps sensor data to camera enums
*/
Cxe::DeviceOrientation
CxeSensorEventHandlerSymbian::mapOrientationSensorData(XQDeviceOrientation::DisplayOrientation sensorData)
{
    CX_DEBUG_ENTER_FUNCTION();

    Cxe::DeviceOrientation uiOrientation(Cxe::Orientation0);

    switch (sensorData) {
        case XQDeviceOrientation::OrientationDisplayUpwards:
        case XQDeviceOrientation::OrientationDisplayDownwards:
            uiOrientation = Cxe::OrientationNone;
            break;
        case XQDeviceOrientation::OrientationDisplayRightUp:
        case XQDeviceOrientation::OrientationUndefined:
            uiOrientation = Cxe::Orientation0;
            break;
        case XQDeviceOrientation::OrientationDisplayUp:
            uiOrientation = Cxe::Orientation90;
            break;
        case XQDeviceOrientation::OrientationDisplayLeftUp:
            uiOrientation = Cxe::Orientation180;
            break;
        case XQDeviceOrientation::OrientationDisplayDown:
            uiOrientation = Cxe::Orientation270;
            break;
        default:
            break;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return uiOrientation;
}


// end of file
