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
#include "at_logsservices.h"
#include <logsservices.h>

#include <QtTest/QtTest>

void AT_LogsServices::initTestCase()
{
}

void AT_LogsServices::cleanupTestCase()
{
}


void AT_LogsServices::init()
{
}

void AT_LogsServices::cleanup()
{  
}

void AT_LogsServices::testStart()
{
    LogsServices::start( LogsServices::ViewAll );
    LogsServices::start( LogsServices::ViewReceived );
    LogsServices::start( LogsServices::ViewCalled );
    LogsServices::start( LogsServices::ViewMissed );
}

