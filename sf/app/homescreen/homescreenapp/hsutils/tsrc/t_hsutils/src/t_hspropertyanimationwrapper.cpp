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

#ifndef ONLY_MENU_TESTCASES

#include "t_hsutils.h"
#include "hspropertyanimationwrapper.h"

class TestObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int number READ number WRITE setNumber)
public:
    TestObject(QObject *parent=0):QObject(parent),mNumber(0){}
    int number()const { return mNumber; }
    void setNumber(int num) {mNumber = num; }
private:
    int mNumber;
};
#include "t_hspropertyanimationwrapper.moc"

void  t_hsUtils::testPropertyAnimationWrapper()
{
    HsPropertyAnimationWrapper propertyAnimation;
    TestObject target;
   
    propertyAnimation.setTargetObject(&target);
    propertyAnimation.setDuration(1);
    propertyAnimation.setEndValue(5);
    propertyAnimation.setPropertyName("number");
    
    QSignalSpy spy(&propertyAnimation,SIGNAL(finished()));
    propertyAnimation.start();
    QTest::qWait(100);
    
    QVERIFY(!propertyAnimation.isRunning());
    QCOMPARE(spy.count(),1);
    QVERIFY(target.number() == 5);
    propertyAnimation.stop();
    
    propertyAnimation.setForward();
    QVERIFY(propertyAnimation.isForward());

    propertyAnimation.setBackward();
    QVERIFY(!propertyAnimation.isForward());
    
    propertyAnimation.setEasingCurve(QEasingCurve::Linear);
}

#endif // ONLY_MENU_TESTCASES
