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
#include "ut_logsconfigurationparams.h"
#include "logsconfigurationparams.h"

#include <QtTest/QtTest>

LogsConfigurationParams myparam;

void UT_LogsConfigurationParams::initTestCase()
{
}

void UT_LogsConfigurationParams::cleanupTestCase()
{
}


void UT_LogsConfigurationParams::init()
{
}

void UT_LogsConfigurationParams::cleanup()
{
}

void UT_LogsConfigurationParams::testSetListItemTextWidth()
{
    myparam.setListItemTextWidth(200);
}

void UT_LogsConfigurationParams::testListItemTextWidth()
{
    QVERIFY (myparam.listItemTextWidth() == 200);
}

