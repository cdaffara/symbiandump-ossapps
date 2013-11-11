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
#include "ut_logseffecthandler.h"
#include "logseffecthandler.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hblabel.h>
#include <hbmainwindow.h>


void UT_LogsEffectHandler::initTestCase()
{
}

void UT_LogsEffectHandler::cleanupTestCase()
{
}


void UT_LogsEffectHandler::init()
{
    mEffect = new LogsEffectHandler();
    mLabel = new HbLabel();
    mLabel2 = new HbLabel();
}

void UT_LogsEffectHandler::cleanup()
{
    delete mEffect;
    mEffect = 0;
    delete mLabel;
    mLabel = 0;
    delete mLabel2;
    mLabel2 = 0;
}

void UT_LogsEffectHandler::testConstructor()
{
    QVERIFY( mEffect );
}

void UT_LogsEffectHandler::testStartDissappearAppearByFadingEffect()
{
    QVERIFY( mEffect->mFadeGroup->state() == QAbstractAnimation::Stopped );
    mEffect->startDissappearAppearByFadingEffect(*mLabel);
    QVERIFY( mEffect->mFadeGroup->state() == QAbstractAnimation::Running );
    
    // Simulate animation progess, when dissappearing has completed effecthandler
    // sends signal, other progress in anim does not cause signal to be sent
    QSignalSpy spy(mEffect, SIGNAL(dissappearByFadingComplete()));
    mEffect->fadeAnimationChanged(mEffect->mFadeGroup->animationAt(1));
    QVERIFY(spy.count() == 1 );
    mEffect->fadeAnimationChanged(0);
    QVERIFY(spy.count() == 1 );
    
    // When effect is running and new effect is requested, previous effect is stopped
    mEffect->startDissappearAppearByFadingEffect(*mLabel); 
}

void UT_LogsEffectHandler::testStartDissappearAppearByMovingEffect()
{
    QVERIFY( mEffect->mMoveGroup->state() == QAbstractAnimation::Stopped );
    mEffect->startDissappearAppearByMovingEffect(*mLabel, *mLabel2, false, 0, 0);
    QVERIFY( mEffect->mMoveGroup->state() == QAbstractAnimation::Running );
    QVERIFY( mEffect->mMoveGroup2->state() == QAbstractAnimation::Running );
    
    // Simulate animation progess, when dissappearing has completed effecthandler
    // sends signal, other progress in anim does not cause signal to be sent
    QSignalSpy spyDissappearCompletion(mEffect, SIGNAL(dissappearByMovingComplete()));
    QSignalSpy spyAppearStarting(mEffect, SIGNAL(appearStarting()));
    mEffect->moveAnimationChanged(mEffect->mMoveGroup->animationAt(1));
    QVERIFY(spyDissappearCompletion.count() == 1 );
    QVERIFY(spyAppearStarting.count() == 0 );
    mEffect->moveAnimationChanged(mEffect->mMoveGroup->animationAt(2));
    QVERIFY(spyDissappearCompletion.count() == 1 );
    QVERIFY(spyAppearStarting.count() == 1 );
    mEffect->moveAnimationChanged(0);
    QVERIFY(spyDissappearCompletion.count() == 1 );
    QVERIFY(spyAppearStarting.count() == 1 );
    
    // When effect is running and new effect is requested, previous effect is stopped
    mEffect->startDissappearAppearByMovingEffect(*mLabel, *mLabel2, true, 0, 0); 
}

void UT_LogsEffectHandler::testStartMoveNotPossibleEffect()
{
    QVERIFY( mEffect->mMoveNotPossibleGroup->state() == QAbstractAnimation::Stopped );
    mEffect->startMoveNotPossibleEffect(*mLabel, false, 0);
    QVERIFY( mEffect->mMoveNotPossibleGroup->state() == QAbstractAnimation::Running );
}
