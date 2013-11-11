/*!
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

#ifndef BUBBLEEFFECTHANDLER_H_
#define BUBBLEEFFECTHANDLER_H_

#include <QObject>
#include <QMap>
#include <hbeffect.h>

class QGraphicsItem;

enum BubbleEffect
    {
    BubbleMutedAppear = 1,
    BubbleMutedDisappear
    };

class BubbleEffectHandler : public QObject
    {
    Q_OBJECT
    
public:
    explicit BubbleEffectHandler(QObject* parent = 0);
    virtual ~BubbleEffectHandler();

    void addEffect(QGraphicsItem* item, BubbleEffect effect);
    void removeEffect(QGraphicsItem* item, BubbleEffect effect);

    void startEffect(BubbleEffect effect);
    void cancelAllEffects(QGraphicsItem* item);

protected slots:
    void handleEffectFinished(const HbEffect::EffectStatus &status);

protected:
    QMap<int,QGraphicsItem*> mEffectItem;
    };

#endif /* BUBBLEEFFECTHANDLER_H_ */
