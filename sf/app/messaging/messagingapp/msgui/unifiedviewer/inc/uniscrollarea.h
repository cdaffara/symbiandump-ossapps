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
 * Description: Customized scroll area which handles dynamic slide loading in
 *              unifiedviewer
 *
 */
#ifndef UNISCROLL_AREA_H
#define UNISCROLL_AREA_H

#include <hbwidget.h>
#include <hbscrollarea.h>
#include <qobject.h>
#include <QTimer>

class QGraphicsItem;
/**
 * Customized scroll area which handles dynamic slide loading in unifiedviewer
 */
class UniScrollArea : public HbScrollArea
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    UniScrollArea(QGraphicsItem * parent = 0);

    /**
     * Destructor
     */
    ~UniScrollArea();

public:

    /**
     * Set the sroll area pos to the start
     */
    void setPosToStart();

public slots:
    /**
     * Handle scrolling started signal
     */
    void handleScrollingStarted();

    /**
     * Handle scrolling started signal
     */
    void handleScrollingEnded();

    /**
     * Set total slides present in the contents widget
     * @param  totalSlides
     */
    void setTotalSlides(int totalSlides);

    /**
     * Resets the current slide to zero
     */
    void resetCurrentSlide();

    signals:

    /**
     * Sinal emitted when next slide is to be loaded
     */
    void scrolledToNextSlide();

    /**
     * Sinal emitted when previous slide is to be loaded
     */
    void scrolledToPreviousSlide();

private:

    /**
     * current slide number
     */
    int mCurrentSlide;

    /**
     * Total slide count
     */
    int mTotalSlides;

    /**
     * Scrolling start pos
     */
    QPointF mScrollStartPoint;

    /**
     * Scrolling end pos
     */
    QPointF mScrollEndPoint;
};

#endif //UNISCROLL_AREA_H
// EOF
