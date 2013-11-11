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


#include "cxesensoreventhandlerdesktop.h"
#include "cxutils.h"


CxeSensorEventHandlerDesktop::CxeSensorEventHandlerDesktop()
: CxeSensorEventHandler()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

CxeSensorEventHandlerDesktop::~CxeSensorEventHandlerDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Data read from sensor. The "type" of data embedded in QVariant is specific to sensor type.
*/
QVariant CxeSensorEventHandlerDesktop::sensorData(CxeSensorEventHandler::SensorType type)
{
    QVariant sensorData;
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
    return sensorData;
}

/*
* initializes and enables all supported sensor events
*/
void CxeSensorEventHandlerDesktop::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


/*
* closes all supported sensors.
*/
void CxeSensorEventHandlerDesktop::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
