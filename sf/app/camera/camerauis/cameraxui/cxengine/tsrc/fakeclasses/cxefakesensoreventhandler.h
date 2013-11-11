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

#ifndef CXEFAKESENSOREVENTHANDLER_H
#define CXEFAKESENSOREVENTHANDLER_H

#include "cxesensoreventhandler.h"  //API

/**
 * Sensor Event Handler implements basic sensor's events handling. It
 * automatically updates the clients intersted in knowing the sensor events
 * that is currently supported and used by camera app.
 *
 * All the supported sensor types are defined by enum "SensorType"
 * This class provides one signal which basically carries out corresponding sensor data for
 * the given sensor type.
 */
class CxeFakeSensorEventHandler : public CxeSensorEventHandler
{
public:

    CxeFakeSensorEventHandler();

    virtual ~CxeFakeSensorEventHandler();


    // From CxeSensorEventHandler
    void init();
    void deinit();

    QVariant sensorData(CxeSensorEventHandler::SensorType type);

};

#endif // CXEFAKESENSOREVENTHANDLER_H
