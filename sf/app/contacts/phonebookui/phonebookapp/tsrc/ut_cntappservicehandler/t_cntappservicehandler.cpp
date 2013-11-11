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


#include <QtTest/QtTest>
#include <QObject>

#include "t_cntappservicehandler.h"
#include "cntappservicehandler.h"

void TestCntAppServiceHandler::initTestCase()
{
}

void TestCntAppServiceHandler::launch()
{
    CntAppServiceHandler handler;
    handler.launch(); // NOP, nothing to verify
}

void TestCntAppServiceHandler::cleanupTestCase()
{

}

QTEST_MAIN(TestCntAppServiceHandler)



// EOF
