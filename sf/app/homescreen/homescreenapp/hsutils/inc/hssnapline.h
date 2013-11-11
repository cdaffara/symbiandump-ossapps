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

#ifndef HSSNAPLINE_H
#define HSSNAPLINE_H

#include <QGraphicsLineItem>
#include <QPen>
#include "hsutils_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)

class QLineF;
class QPropertyAnimation;

class HSUTILS_EXPORT HsSnapLine : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    HsSnapLine(QGraphicsItem *parent = 0);
    ~HsSnapLine();

public:
    void setConfiguration(const QVariantHash &configuration);
    void hideLine();
    void showLine(const QLineF &snapLine);

private:
    Q_DISABLE_COPY(HsSnapLine)
    void startFadeInAnimation();
    bool isFadeInAnimationRunning() const;
    void stopFadeInAnimation();
    void actionOnFadeInAnimationStop();
    void startFadeOutAnimation();
    bool isFadeOutAnimationRunning() const;
    void stopFadeOutAnimation();
    void actionOnFadeOutAnimationStop();
    int getFadeInDuration() { return (mFadeInAnimationDuration - mFadeInAnimationDuration*mOpacity); }
    int getFadeOutDuration() { return (mFadeOutAnimationDuration*mOpacity); }

private slots:
    void fadeInAnimationFinished();
    void fadeOutAnimationFinished();

private:
    QPropertyAnimation *mFadeInAnimation;
    QPropertyAnimation *mFadeOutAnimation;
    qreal mOpacity;
    int mFadeInAnimationDuration;
    int mFadeOutAnimationDuration;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif
