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

#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>
#include <hbcolorscheme.h>
#include <hbevent.h>
#include "logspageindicatoritem.h"
#include "logslogger.h"

const char logsNormalIconName[]    = "qtg_mono_tab_passive";
const char logsHighlightIconName[] = "qtg_mono_tab_active";
const char logsIconColor[] = "qtc_viewtitle_normal"; //groupbox text color
  
const int logsEffectDurationInMs = 1000;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsPageIndicatorItem::LogsPageIndicatorItem(bool active, QGraphicsItem *parent)
  : HbIconItem(parent),
    mIsActive(active)
{
    if (active) {
        setIcon(HbIcon(logsHighlightIconName));
    } else {
        setIcon(HbIcon(logsNormalIconName));
    }

    setFlags(HbIcon::Colorized);
    updateColor();
     
    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
    effect->setColor(Qt::white);
    effect->setStrength(0);
    effect->setEnabled(false);
    setGraphicsEffect(effect);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsPageIndicatorItem::~LogsPageIndicatorItem()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicatorItem::setActive(bool active)
{
    if (mIsActive != active) {
        mIsActive = active;
        if (mIsActive) {
            setIcon(HbIcon(logsHighlightIconName));
            startAnimation();
        } else {
            setIcon(HbIcon(logsNormalIconName));
            updateColor();
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsPageIndicatorItem::isActive() const
{
    return mIsActive;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicatorItem::changeEvent(QEvent * event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        LOGS_QDEBUG( "logs [UI] LogsPageIndicatorItem::changeEvent(), themeChanged" );
        updateColor();
    }
    HbIconItem::changeEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicatorItem::updateColor()
{
    QColor col = HbColorScheme::color(logsIconColor);
    if (col.isValid() && (color() != col) ) {
        setColor(col);
        LOGS_QDEBUG_2( "logs [UI] LogsPageIndicatorItem::updateColor(), color: ", col );
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicatorItem::startAnimation()
{
    graphicsEffect()->setEnabled(true);
    setTransformOriginPoint(rect().center());
    QPropertyAnimation *animation = 
        new QPropertyAnimation(graphicsEffect(), "strength");
    animation->setDuration(logsEffectDurationInMs);
    animation->setKeyValueAt(0.2, 1);
    animation->setEndValue(0);
    connect(animation, SIGNAL(finished()), SLOT(animationFinished()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicatorItem::animationFinished()
{
    graphicsEffect()->setEnabled(false);
    updateColor();
}
