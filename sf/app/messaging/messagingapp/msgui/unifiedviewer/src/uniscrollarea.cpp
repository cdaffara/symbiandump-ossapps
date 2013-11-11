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

#include "uniscrollarea.h"

#include <HbScrollBar>

//---------------------------------------------------------------
//UniScrollArea :: UniScrollArea
// @see header file
//---------------------------------------------------------------
UniScrollArea::UniScrollArea(QGraphicsItem * parent) :
    HbScrollArea(parent), mCurrentSlide(0), mTotalSlides(0)
{
    bool flag = connect(this, SIGNAL(scrollingEnded()),
    this, SLOT(handleScrollingEnded()));
    bool flag1 = connect(this, SIGNAL(scrollingStarted()),
    this, SLOT(handleScrollingStarted()));

}

//---------------------------------------------------------------
//UniScrollArea :: ~UniScrollArea
// @see header file
//---------------------------------------------------------------
UniScrollArea::~UniScrollArea()
{

}

//---------------------------------------------------------------
//UniScrollArea :: setTotalSlides
// @see header file
//---------------------------------------------------------------
void UniScrollArea::setTotalSlides(int totalSlides)
{
    mTotalSlides = totalSlides;
}

//---------------------------------------------------------------
//UniScrollArea :: resetCurrentSlide
// @see header file
//---------------------------------------------------------------
void UniScrollArea::resetCurrentSlide()
{
    mCurrentSlide = 0;
}

//---------------------------------------------------------------
//UniScrollArea :: handleScrollingEnded
// @see header file
//---------------------------------------------------------------
void UniScrollArea::handleScrollingEnded()
{
    // TODO : reduce the complexity , code readability
    // TODO : avoid calculating the heights every time
    int currentSlide = -1;
    QList<qreal> heights;
    heights.clear();
    // contents() is not available so replaced with contentWidget()
    //    mScrollEndPoint = this->contents()->pos();
    mScrollEndPoint = this->contentWidget()->pos();
    // contents() is not available so replaced with contentWidget()
    QList<QGraphicsItem *> list = this->contentWidget()->childItems();
    for (int i = 0; i < list.size(); i++)
    {
        QRectF rect = list[i]->boundingRect();
        heights.append(- (rect.height()));
    }

    qreal h = 0.0;
    for (int i = 0; i < heights.size(); i++)
    {
        h += heights.at(i);
        if (mScrollEndPoint.y() > h)
        {
            currentSlide = i;
            break;
        }
    }

    if (mScrollEndPoint.y() < mScrollStartPoint.y())
    {
        //Identify if we are in last slide / last but one slide as we always
        // assume the slide next to current slide is always loaded
        //(Assumtion is based on slide initial load count of 2)
        if (currentSlide >= mTotalSlides - 1)
        {
            return;
        }
        //(This can be removed and emit signal if we cross 50% of last slide)
        if (currentSlide >= mTotalSlides - 2)
        {
            return;
        }

        //Determine how much we have scrolled & if we have scrolled beyond 50%
        //of current slide load the slide next to next slide
        qreal delta = heights.at(currentSlide) / 2;
        qreal fh = h - delta;
        if (mScrollEndPoint.y() < fh)
        {
            emit scrolledToNextSlide();
        }
    }

}

//---------------------------------------------------------------
//UniScrollArea :: handleScrollingStarted
// @see header file
//---------------------------------------------------------------
void UniScrollArea::handleScrollingStarted()
{
    // contents() is not available so replaced with contentWidget()
    //   mScrollStartPoint = this->contents()->pos();
    mScrollStartPoint = this->contentWidget()->pos();
}

//---------------------------------------------------------------
//UniScrollArea :: setPosToStart
// @see header file
//---------------------------------------------------------------
void UniScrollArea::setPosToStart()
{
    // contents() is not available so replaced with contentWidget()
    QGraphicsItem* widgetItem = this->contentWidget();
    widgetItem->setPos(0, 0);
}


// EOF
