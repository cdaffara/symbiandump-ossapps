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
#ifndef LOGSEFFECTHANDLER_H
#define LOGSEFFECTHANDLER_H

#include <QObject>
#include <QPointF>

class QPropertyAnimation;
class QEasingCurve;
class QSequentialAnimationGroup;
class QAbstractAnimation;
class LogsEffectContainer;

/**
 * 
 */
class LogsEffectHandler : public QObject
{
    Q_OBJECT
    friend class UT_LogsEffectHandler;
    friend class UT_LogsRecentCallsView;
    
public:
    explicit LogsEffectHandler();
    ~LogsEffectHandler();

    void startDissappearAppearByFadingEffect(QObject& effectTarget);
    void startDissappearAppearByMovingEffect(
        QObject& effectTarget, QObject& secondaryEffectTarget, bool dissappearToLeft, 
        int origX, int secondaryOrigX);
    void startMoveNotPossibleEffect(QObject& effectTarget, bool moveLeft, int origX);
    
signals:

    void dissappearByMovingComplete();
    void dissappearByFadingComplete();
    void appearStarting();
    void appearByMovingComplete();  

private slots:

    void moveAnimationChanged(QAbstractAnimation *currentAnimation);
    void fadeAnimationChanged(QAbstractAnimation *currentAnimation);
    
private:
    
    void doStartDissappearAppearByMovingEffect(
        QSequentialAnimationGroup& group, QObject& effectTarget, 
        bool dissappearToLeft, int origX);
    
    void initAppearByMovingEffect(QPropertyAnimation& animation, QObject* effectTarget,
                                  bool appearFromLeft, int origX);
    void initDissappearByMovingEffect(QPropertyAnimation& animation, QObject* effectTarget, 
                                      bool dissappearToLeft, int origX); 
    void initOpacityEffect(
            QPropertyAnimation& animation, QObject* effectTarget, 
            qreal startVal, qreal endVal, int duration);
    void initMoveHorizontallyEffect(
            QPropertyAnimation& animation, QObject* effectTarget, 
            int startVal, int endVal, int duration, const QEasingCurve& easingCurve);
 
private:
    
    QPropertyAnimation* mItemAppearByFadingAnimation;
    QPropertyAnimation* mItemDissappearByFadingAnimation;
    QPropertyAnimation* mItemMoveNotPossibleAnimationStart;
    QPropertyAnimation* mItemMoveNotPossibleAnimationEnd;
    QSequentialAnimationGroup* mMoveGroup;
    QSequentialAnimationGroup* mMoveGroup2;
    QSequentialAnimationGroup* mFadeGroup;
    QSequentialAnimationGroup* mMoveNotPossibleGroup;
    
};

#endif // LOGSEFFECTHANDLER_H
