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

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <hbcolorscheme.h>

#include "hsapp_defs.h"
#include "hssnapline.h"

/*!
    Constructor.
    
    \a parent Owner.
*/
HsSnapLine::HsSnapLine(QGraphicsItem *parent)
  : QGraphicsLineItem(parent),
    mFadeInAnimation(0),
    mFadeOutAnimation(0),
    mOpacity(0.0),
    mFadeInAnimationDuration(0),
    mFadeOutAnimationDuration(0)
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.0);
    effect->setEnabled(false);
    setGraphicsEffect(effect);

    mFadeInAnimation = new QPropertyAnimation(graphicsEffect(), "opacity", this);
    connect(mFadeInAnimation, SIGNAL(finished()), SLOT(fadeInAnimationFinished()));

    mFadeOutAnimation = new QPropertyAnimation(graphicsEffect(), "opacity", this);
    connect(mFadeOutAnimation, SIGNAL(finished()), SLOT(fadeOutAnimationFinished()));
}
 
/*!
    Destructor.
*/
HsSnapLine::~HsSnapLine()
{
}

/*!
    Sets the configuration for the showing the snap line, it includes fade-in / fade-out timeout.
*/
void HsSnapLine::setConfiguration(const QVariantHash &configuration)
{
    bool canConvert = false;
    //The following values should be in int, so the status received in canConvert is ignored
    mFadeInAnimationDuration = configuration[Hs::snapLineFadeinDuration].toInt(&canConvert);
    mFadeOutAnimationDuration = configuration[Hs::snapLineFadeoutDuration].toInt(&canConvert);
}

/*!
    Show the snap line. fade-in animation is started on the line if the line is positioned at a different place.
    Before starting the fade-in animation, the fade-out animation is stoped if it is running.
*/
void HsSnapLine::showLine(const QLineF &snapLine)
{
    QLineF displayLine = snapLine;
    qreal angle = displayLine.angle();
    if (qAbs(angle) == 0.0 || qAbs(angle) == 180.0) { //this is a horizontal line
        //adding 1 is required below, as the line is 3 pixels wide and is translated by 1 point before displaying
        if (displayLine.y1() != (line().y1()+1.0) ) { //this horizontal line is at new position horizontally
            if (isFadeOutAnimationRunning()) { //if fade-out animation is running, stop it, animation is running at old position
                stopFadeOutAnimation();
            }
            //start fade-in animation at new position.
            startFadeInAnimation();
        }
        else { //this horizontal line is at the old position horizontally
            if (isFadeOutAnimationRunning()) { //if fade-out animation is running, stop it, animation is running at old position
                stopFadeOutAnimation();
                //start fade-in animation at the old position
                startFadeInAnimation();
            }
        }
        displayLine.translate(0.0, -1.0);
    }
    if (qAbs(angle) == 90.0 || qAbs(angle) == 270.0) { //this is a vertical line
        if (displayLine.x1() != (line().x1()+1)) { //this Vertical line is at different position vertically
            if (isFadeOutAnimationRunning()) {
                stopFadeOutAnimation();
            }
            startFadeInAnimation();
        }
        else {
            if (isFadeOutAnimationRunning()) {
                stopFadeOutAnimation();
                startFadeInAnimation();
            }
        }
        displayLine.translate(-1.0, 0.0);
    }

    QLinearGradient gradient(displayLine.p1(), displayLine.p2());
    gradient.setColorAt(0.0, Qt::white);
    QColor snapLineColor = HbColorScheme::color("qtc_hs_snapguide");
    if (!snapLineColor.isValid()) {
        //if valid color is not loaded from the theme, the darkCyan color is used as a backup.color
        snapLineColor = Qt::darkCyan;
    }
    gradient.setColorAt(0.4, snapLineColor);
    gradient.setColorAt(0.6, snapLineColor);
    gradient.setColorAt(1.0, Qt::white);
    QBrush brush(gradient);
    QPen pen;
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setBrush(brush);
    setPen(pen);

    setLine(displayLine);
    show();
}

/*!
    Hide the snap line. Fade-out animation is started on the line to be hidden.
    If fade-in animation is running, it is stoped before starting the fade-out animation.
*/
void HsSnapLine::hideLine()
{
    if (isFadeInAnimationRunning()) {
        stopFadeInAnimation();
    }
    startFadeOutAnimation();
}

/*!
    Start fade-in animation.
*/
void HsSnapLine::startFadeInAnimation()
{
    mFadeInAnimation->setStartValue(mOpacity);
    mFadeInAnimation->setEndValue(1.0);
    mFadeInAnimation->setDuration(getFadeInDuration());

    graphicsEffect()->setEnabled(true);
    mFadeInAnimation->start();
}

/*!
    Check if fade-in animation is running.
*/
bool HsSnapLine::isFadeInAnimationRunning() const
{
    return mFadeInAnimation->state() == QAbstractAnimation::Running;
}

/*!
    Stop the fade-in animation.
*/
void HsSnapLine::stopFadeInAnimation()
{
    mFadeInAnimation->stop();
    actionOnFadeInAnimationStop();
}

/*!
    SLOT called when fade-in animation is finished / reaches it's end.
*/
void HsSnapLine::fadeInAnimationFinished()
{
    actionOnFadeInAnimationStop();
}

/*!
    Action when fade-in animation stops running.
*/
void HsSnapLine::actionOnFadeInAnimationStop()
{
    mOpacity = mFadeInAnimation->currentValue().toDouble();
    graphicsEffect()->setEnabled(false);
}

/*!
    Start fade-out animation.
*/
void HsSnapLine::startFadeOutAnimation()
{
    mFadeOutAnimation->setStartValue(mOpacity);
    mFadeOutAnimation->setEndValue(0.0);
    mFadeOutAnimation->setDuration(getFadeOutDuration());

    graphicsEffect()->setEnabled(true);
    mFadeOutAnimation->start();
}

/*!
    Check if fade-out animation is running.
*/
bool HsSnapLine::isFadeOutAnimationRunning() const
{
    return mFadeOutAnimation->state() == QAbstractAnimation::Running;
}

/*!
    Stop the fade-out animation.
*/
void HsSnapLine::stopFadeOutAnimation()
{
    mFadeOutAnimation->stop();
    actionOnFadeOutAnimationStop();
}

/*!
    SLOT called when fade-out animation is finished / reaches it's end.
*/
void HsSnapLine::fadeOutAnimationFinished()
{
    actionOnFadeOutAnimationStop();
}

/*!
    Action when fade-out animation stops running.
*/
void HsSnapLine::actionOnFadeOutAnimationStop()
{
    mOpacity = mFadeOutAnimation->currentValue().toDouble();
    graphicsEffect()->setEnabled(false);
    hide();
    setLine(QLineF());
}
