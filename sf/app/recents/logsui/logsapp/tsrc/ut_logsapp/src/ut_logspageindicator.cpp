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

//USER
#include "ut_logspageindicator.h"
#include "logspageindicator.h"

//SYSTEM
#include <QtTest/QtTest>

void UT_LogsPageIndicator::initTestCase()
{
}

void UT_LogsPageIndicator::cleanupTestCase()
{
}


void UT_LogsPageIndicator::init()
{
    mIndicator = new LogsPageIndicator();
}

void UT_LogsPageIndicator::cleanup()
{
    delete mIndicator;
}

void UT_LogsPageIndicator::testInitialize()
{
    QVERIFY(mIndicator->mItems.count() == 0);
    QVERIFY(mIndicator->mActiveItemIndex == -1);
    
    //normal init
    mIndicator->initialize(2,1);
    QVERIFY(mIndicator->mItems.count() == 2);
    QVERIFY(mIndicator->mActiveItemIndex == 1);

    //items count is less then 1
    mIndicator->initialize(0,1);
    QVERIFY(mIndicator->mItems.count() == 0);
    QVERIFY(mIndicator->mActiveItemIndex == -1);

    //index is out of boundaries
    mIndicator->initialize(2,2);
    QVERIFY(mIndicator->mItems.count() == 0);
    QVERIFY(mIndicator->mActiveItemIndex == -1);
}

void UT_LogsPageIndicator::testSetActiveItemIndex()
{
    mIndicator->initialize(3,1);
    QVERIFY(mIndicator->mItems.count() == 3);
    QVERIFY(mIndicator->mActiveItemIndex == 1);
    
    //index is out of boundaries
    mIndicator->setActiveItemIndex(3);
    QVERIFY(mIndicator->mActiveItemIndex == 1);
    
    //invalid index
    mIndicator->setActiveItemIndex(-1);
    QVERIFY(mIndicator->mActiveItemIndex == 1);
    
    //valid index
    mIndicator->setActiveItemIndex(0);
    QVERIFY(mIndicator->mActiveItemIndex == 0);       
}
