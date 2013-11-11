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
* Description:  Implementation of HbPanGesture
*
*/

// Version : %version:   2 %



#ifndef HBPANGESTURE_H_
#define HBPANGESTURE_H_

#include <QPanGesture>

class HbPanGesture : public QPanGesture
{
    Q_OBJECT

    public:
        HbPanGesture();
        virtual ~HbPanGesture();
        Qt::GestureState state(){ return mState; }
        QPointF sceneDelta();
        QPointF sceneVelocity();
        QPointF sceneOffset();

    public:
        Qt::GestureState mState;
        QPointF mSceneDelta;
        QPointF mSceneVelocity;
        QPointF mSceneOffset;
};

#endif /*HBPANGESTURE_H_*/

