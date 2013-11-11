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
* Description: Graphical effects/animations for nmail ui.
*
*/

#ifndef NMUIEFFECTS_H_
#define NMUIEFFECTS_H_

#include <QObject>
#include <hbeffect.h>

class QGraphicsPixmapItem;
class HbMainWindow;

class NmUiEffects : public QObject
{
    Q_OBJECT

public:
    /*!
     * \enum NmEffectType
     * \brief Defines effect to be launched
     */
    enum NmUiEffectType
    {
        NmNoEffect = 0,
        // Editor 'exiting' effect when message has been sent.
        NmEditorSendMessageAnimation
    };

    NmUiEffects(HbMainWindow &mainWindow);
    ~NmUiEffects();

    void prepareEffect(NmUiEffectType effect);
    void startEffect(NmUiEffectType effect);

private:
    QGraphicsPixmapItem *screenShot();
    void resetSendAnimation();
    
private slots:
    void sendAnimationComplete(HbEffect::EffectStatus status);

private:
    HbMainWindow &mMainWindow;
    QGraphicsPixmapItem *mSendAnimationScreenShot; // Not owned.
    bool mDoSendAnimation;
};

#endif /* NMUIEFFECTS_H_ */
