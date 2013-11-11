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
 * Description: Mocked classes for testing CalenDayContentScrollArea class
 *
 */

#ifndef HBSWIPEGESTURE_H_
#define HBSWIPEGESTURE_H_

#include <QSwipeGesture>

// Test variables
extern Qt::GestureState gTestGestureState;
extern qreal gTestSceneSwipeAngle;
extern QSwipeGesture::SwipeDirection gTestSceneHorizontalDirection;

/*!
 Mocked class HbSwipeGesture
 */
class HbSwipeGesture : public QSwipeGesture
{
    Q_OBJECT
    
public:
    explicit HbSwipeGesture(QObject *parent = 0) {
        Q_UNUSED(parent)
    }
    ~HbSwipeGesture() {}
    
    qreal sceneSwipeAngle() const {
        return gTestSceneSwipeAngle;
    }
    
    SwipeDirection sceneHorizontalDirection() const {
        return gTestSceneHorizontalDirection;
    }
    
    Qt::GestureState state() const {
        return gTestGestureState;
    }
};

#endif /* HBSWIPEGESTURE_H_ */
