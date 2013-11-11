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

#include <QTest>
#include <QSignalSpy>
#include <QString>

#include "unittest_cxeharvestercontrolsymbian.h"
#include "cxeharvestercontrolsymbian.h"
#include "cxutils.h"
#include "cxeerror.h"

UnitTestCxeHarvesterControlSymbian::UnitTestCxeHarvesterControlSymbian()
    : mHarvesterControl(NULL)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
}


UnitTestCxeHarvesterControlSymbian::~UnitTestCxeHarvesterControlSymbian()
{
    cleanup();
}


void UnitTestCxeHarvesterControlSymbian::init()
{
    mHarvesterControl = new CxeHarvesterControlSymbian();
}

void UnitTestCxeHarvesterControlSymbian::cleanup()
{
    delete mHarvesterControl;
    mHarvesterControl = NULL;
}

void UnitTestCxeHarvesterControlSymbian::testHarvestFile()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    QSignalSpy harvestCompleteSpy(mHarvesterControl, SIGNAL(fileHarvested(CxeError::Id, const QString&)));
    QString filename;
    QVariantList initModeArguments;

    QVERIFY(harvestCompleteSpy.isValid()); 

    // case 1: testing with wrong dummy file, we should get an error code with harvestcomplete
    // since filename is invalid
    mHarvesterControl->harvestFile(filename, 0, 0);
    QCOMPARE( harvestCompleteSpy.count(), 1 );
    initModeArguments = harvestCompleteSpy.takeFirst();
    // we are only interested in error code in this case 1
    QCOMPARE(initModeArguments.at(0).value<CxeError::Id>(), CxeError::NotFound);
    
    // case 2: testing with real filename, harvesting should go fine, callback without errors
    filename = QString("test.jpg");
    mHarvesterControl->harvestFile(filename, 0, 0);
 
    QCOMPARE( harvestCompleteSpy.count(), 1 );
    initModeArguments = harvestCompleteSpy.takeFirst();
    // we check both the filename, since in this case harvesting should go fine
    // and error returned is KErrNone.
    QCOMPARE(initModeArguments.at(0).value<CxeError::Id>(), CxeError::None);
    QCOMPARE(initModeArguments.at(1).toString(), filename);

    CX_DEBUG_EXIT_FUNCTION();
}


// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeHarvesterControlSymbian);

// End of file
