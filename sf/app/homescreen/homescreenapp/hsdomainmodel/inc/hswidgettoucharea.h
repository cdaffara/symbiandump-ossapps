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

#ifndef HSWIDGETTOUCHAREA_H
#define HSWIDGETTOUCHAREA_H

#include <HbTouchArea>

#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HsWidgetHostVisual;

class HsWidgetTouchArea : public HbTouchArea
{
    Q_OBJECT

public:
    HsWidgetTouchArea(HsWidgetHostVisual *widgetHost);
    ~HsWidgetTouchArea();

    bool sceneEvent(QEvent *event);
    QPainterPath shape() const;

protected:
    void gestureEvent(QGestureEvent *event);

private:
    Q_DISABLE_COPY(HsWidgetTouchArea)
    
private:
    HsWidgetHostVisual *mWidgetHostVisual;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWIDGETTOUCHAREA_H
