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

#include "cxefakesensoreventhandler.h"
#include "cxesensoreventhandlersymbian.h"
#include "cxutils.h"

CxeFakeSensorEventHandler::CxeFakeSensorEventHandler()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeSensorEventHandler::~CxeFakeSensorEventHandler()
{
    CX_DEBUG_IN_FUNCTION();}

void CxeFakeSensorEventHandler::init()
{
    CX_DEBUG_IN_FUNCTION();}

void CxeFakeSensorEventHandler::deinit()
{
    CX_DEBUG_IN_FUNCTION();}

QVariant CxeFakeSensorEventHandler::sensorData(CxeSensorEventHandlerSymbian::SensorType /*type*/)
{
    CX_DEBUG_ENTER_FUNCTION();
    
    QVariant data;

    CX_DEBUG_EXIT_FUNCTION();
    
    return data;
}


// end of file
