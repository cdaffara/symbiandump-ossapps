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
* Description:  Swipe widget.
*
*/

#ifndef SNSRSWIPEWIDGET_H
#define SNSRSWIPEWIDGET_H

#include <QGesture>

#include <hbwidget.h>
#include <hblabel.h>
#include <hbeffect.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrSwipeWidget)

class HbAction;
class HbPushButton;
class HbLabel;
class HbIconItem;

class SnsrSwipeWidget : public HbWidget
    {
    Q_OBJECT

public:
    
    explicit SnsrSwipeWidget(QGraphicsItem *parent = 0);
    virtual ~SnsrSwipeWidget();
    
    void setCurrentOrientation(int orientation);

private slots:

    void createPrimitives();

protected: // from base classes

    virtual void gestureEvent(QGestureEvent *event);

signals:
    void swipeDownDetected();

private: // data

    HbLabel *mSlideLabel;
    HbIconItem* mIconItem;
    //for swipe direction we need to know if the device is in landscapemode
    int mCurrentOrientation;

    Q_DISABLE_COPY(SnsrSwipeWidget)

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrSwipeWidget)
    
};

#endif // SNSRSWIPEWIDGET_H

// EOF
