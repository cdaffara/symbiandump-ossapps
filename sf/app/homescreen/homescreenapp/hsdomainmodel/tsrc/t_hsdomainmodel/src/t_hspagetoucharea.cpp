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

#include <HbTapAndHoldGesture>
#include <HbPanGesture>
#include <HbTapGesture>

#include "t_hsdomainmodel.h"
#include "hspagetoucharea.h"


void TestHsDomainModel::testPageTouchAreaConstruction()
{
    HsPageTouchArea *pageTouchArea = new HsPageTouchArea();
    QVERIFY(pageTouchArea);
    delete pageTouchArea;
}

void TestHsDomainModel::testPageTouchAreaGestureEvent()
{
    HsPageTouchArea *pageTouchArea = new HsPageTouchArea();
    QList<QGesture*> QGestureList;
    // test empty gesture list
    pageTouchArea->gestureEvent(new QGestureEvent(QGestureList));

    // test tapAndHold
    HbTapAndHoldGesture *tapAndHoldGesture = new HbTapAndHoldGesture();
    QGestureList << tapAndHoldGesture;
    QGestureEvent *gestureEvent = new QGestureEvent(QGestureList);
    pageTouchArea->gestureEvent(gestureEvent);
    
    QGestureList.clear();

    // test pan
    HbPanGesture *panGesture = new HbPanGesture();
    QGestureList << panGesture;
    gestureEvent = new QGestureEvent(QGestureList);
    pageTouchArea->gestureEvent(gestureEvent);

    QGestureList.clear();

    // test tap
    HbTapGesture *tapGesture = new HbTapGesture();
    QGestureList << tapGesture;
    gestureEvent = new QGestureEvent(QGestureList);
    pageTouchArea->gestureEvent(gestureEvent);

    delete tapAndHoldGesture;
    delete panGesture;
    delete tapGesture;

    delete pageTouchArea;
}
