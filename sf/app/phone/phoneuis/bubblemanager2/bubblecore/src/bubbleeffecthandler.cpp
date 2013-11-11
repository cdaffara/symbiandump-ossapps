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

#include <QtCore>
#include "bubbleeffecthandler.h"

const QLatin1String BUBBLE_MUTED_APPEAR_FXML(":/bubble_muted_appear.fxml");
const QLatin1String BUBBLE_MUTED_DISAPPEAR_FXML(":/bubble_muted_disappear.fxml");
const QLatin1String BUBBLE_APPEAR_EVENT("appear");
const QLatin1String BUBBLE_DISAPPEAR_EVENT("disappear");


BubbleEffectHandler::BubbleEffectHandler(QObject* parent)
    : QObject(parent)
{
}

BubbleEffectHandler::~BubbleEffectHandler()
{
}

void BubbleEffectHandler::addEffect(QGraphicsItem* item, BubbleEffect effect)
{
    switch (effect) {
    case BubbleMutedAppear:
        HbEffect::add(item, BUBBLE_MUTED_APPEAR_FXML, BUBBLE_APPEAR_EVENT);
        mEffectItem.insert(effect,item);
        break;
    case BubbleMutedDisappear:
        HbEffect::add(item, BUBBLE_MUTED_DISAPPEAR_FXML, BUBBLE_DISAPPEAR_EVENT);
        mEffectItem.insert(effect,item);
        break;
    default:
        break;
    }
}

void BubbleEffectHandler::removeEffect(QGraphicsItem* item, BubbleEffect effect)
{
    switch (effect) {
    case BubbleMutedAppear:
        HbEffect::remove(item, BUBBLE_MUTED_APPEAR_FXML, BUBBLE_APPEAR_EVENT);
        mEffectItem.remove(effect);
        break;
    case BubbleMutedDisappear:
        HbEffect::remove(item, BUBBLE_MUTED_DISAPPEAR_FXML, BUBBLE_DISAPPEAR_EVENT);
        mEffectItem.remove(effect);
        break;
    default:
        break;
    }
}

void BubbleEffectHandler::startEffect(BubbleEffect effect)
{
    QGraphicsItem* item;

    switch (effect) {
    case BubbleMutedAppear:
        item = mEffectItem.value(effect);
        Q_ASSERT(item);
        item->setVisible(true);
        HbEffect::start(item, BUBBLE_APPEAR_EVENT);
        break;
    case BubbleMutedDisappear:
        item = mEffectItem.value(effect);
        Q_ASSERT(item);
        item->setVisible(true);
        HbEffect::start(item, BUBBLE_DISAPPEAR_EVENT,
                        this, "handleEffectFinished", QVariant(effect));
        break;
    default:
        break;
    }
}

void BubbleEffectHandler::cancelAllEffects(QGraphicsItem* item)
{
    Q_ASSERT(item);
    HbEffect::cancel(item);
}

void BubbleEffectHandler::handleEffectFinished(
    const HbEffect::EffectStatus &status)
{
    int effect = status.userData.toInt();

    switch (effect) {
    case BubbleMutedDisappear:
        if (status.item) {
            status.item->setVisible(false);
        }
        break;
    default:
        break;
    }
}
