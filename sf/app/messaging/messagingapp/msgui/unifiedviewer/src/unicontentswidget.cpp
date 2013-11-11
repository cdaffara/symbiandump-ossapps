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
 * Description: This widget is place holder for message headers and body
 *
 */
#include "unicontentswidget.h"

#include <QGraphicsLinearLayout>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>

#include "univiewerfeeder.h"
#include "univiewslidewidget.h"
#include "convergedmessage.h"
#include "debugtraces.h"
#include "nativemessageconsts.h"
//---------------------------------------------------------------
//UniContentsWidget :: UniContentsWidget
// @see header file
//---------------------------------------------------------------
UniContentsWidget::UniContentsWidget(UniViewerFeeder* feeder,QGraphicsItem * parent) :
    HbWidget(parent), mViewFeeder(feeder), mTotalSlides(1), mCurrentSlide(0), //Not used now
            mSlideWindow(10), // These need not be class members
            mInitialLoadCount(2), // These need not be class members
            mTotalSlidesLoaded(0), mRowCount(0)

{
    QDEBUG_WRITE("UniContentsWidget: Constructor start");

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical,this);
    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    
    if(mViewFeeder->msgType() == KSenduiMtmMmsUidValue)
    {
        mMessageType = ConvergedMessage::Mms; 
    }
    else if (mViewFeeder->msgType() == KSenduiMtmSmsUidValue)
    {
        mMessageType = ConvergedMessage::Sms;
    }
    else // Does this hold good.
    {   
        mMessageType = ConvergedMessage::None;
    }

    // create the mInitailLoadCount number of slides
    for (int i = 0; i < mInitialLoadCount; i++)
    {
        UniViewSlideWidget* slide = new UniViewSlideWidget(feeder, i, this);
        addItemToLayout(slide);
        slide->setInsideLayout(true);
        mSlides.append(slide);
        
        connect(slide,SIGNAL(sendMessage(const QString&,const QString&)),
                this, SIGNAL(sendMessage(const QString&,const QString&)));
    }

    setLayout(mMainLayout);

    QDEBUG_WRITE("UniContentsWidget: Constructor end");
}

//---------------------------------------------------------------
//UniContentsWidget :: ~UniContentsWidget
// @see header file
//---------------------------------------------------------------
UniContentsWidget::~UniContentsWidget()
{
    for (int a = 0; a < mSlides.count(); a++)
    {
        delete mSlides[a];
    }
}

//---------------------------------------------------------------
//UniContentsWidget ::addItemToLayout
// @see header file
//---------------------------------------------------------------
void UniContentsWidget::addItemToLayout(UniViewSlideWidget* item)
{
    mMainLayout->addItem(item);
    ++mRowCount;
}

//---------------------------------------------------------------
//UniContentsWidget ::removeItemFromLayout
// @see header file
//---------------------------------------------------------------
void UniContentsWidget::removeItemFromLayout(UniViewSlideWidget* item)
{
    mMainLayout->removeItem(item);
    mRowCount--;
}

//---------------------------------------------------------------
//UniContentsWidget::clearContent
// @see header file
//---------------------------------------------------------------
void UniContentsWidget::clearContent()
{
    QDEBUG_WRITE("UniContentsWidget::clearContent() start");

    // Clear the first slide content
    int slidesToClear;
    bool returnAfterClear = false;
    if (mSlides.count() < mSlideWindow)
    {
        slidesToClear = mSlides.count();
        returnAfterClear = true;
    }
    else
    {
        slidesToClear = mSlideWindow;
    }
    for (int i = 0; i < slidesToClear; i++)
    {
        mSlides[i]->clearContent();
        if (mSlides[i]->insideLayout())
        {
            this->removeItemFromLayout(mSlides[i]);
            mSlides[i]->setParent(NULL);
            mSlides[i]->setParentItem(NULL);
            mSlides[i]->hide();
            mSlides[i]->setInsideLayout(false);
        }
    }

    mTotalSlidesLoaded = slidesToClear;
    if (returnAfterClear)
        return;

    // Delete the remaining slides
    for (int a = slidesToClear; a < mSlides.count(); a++)
    {
        mSlides[a]->setParent(NULL);
        mSlides[a]->setParentItem(NULL);
        delete mSlides[a];
        mSlides.removeAt(a);
        mRowCount--;
    }

    QDEBUG_WRITE("UniContentsWidget::clearContent() end");
}

//---------------------------------------------------------------
//UniContentsWidget::populateContent
// @see header file
//---------------------------------------------------------------
void UniContentsWidget::populateContent()
{
    QDEBUG_WRITE("UniContentsWidget::populateContent() start");

    if ( (mViewFeeder->msgType() == KSenduiMtmMmsUidValue) &&
         (mViewFeeder->slideCount() > 0) )
    {
        mMessageType = ConvergedMessage::Mms; 
        mTotalSlides = mViewFeeder->slideCount();
        if (mTotalSlides >= mSlides.count())
        {
            mTotalSlidesLoaded = mSlides.count();
        }
        else
        {
            mTotalSlidesLoaded = mTotalSlides;
        }
    }
    else
    {
        mTotalSlidesLoaded = 1;
        mTotalSlides = 1;
    }

    //Populate the mTotalSlidesLoaded
    for (int i = 0; i < mTotalSlidesLoaded; i++)
    {
        if (!mSlides[i]->insideLayout())
        {
            mSlides[i]->show();
            mSlides[i]->setInsideLayout(true);
            this->addItemToLayout(mSlides[i]);
        }
        mSlides[i]->populateContent();
    }

    for (int j = mTotalSlidesLoaded; j < mSlides.count(); j++)
    {
        if (mSlides[j]->insideLayout())
        {
            this->removeItemFromLayout(mSlides[j]);
            mSlides[j]->setInsideLayout(false);
            mSlides[j]->hide();
        }
    }

    //Create remaining slides on demand

    QDEBUG_WRITE("UniContentsWidget::populateContent() end");
}

//---------------------------------------------------------------
//UniContentsWidget::populateNextSlide
// @see header file
//---------------------------------------------------------------
void UniContentsWidget::populateNextSlide()
{
    if (mTotalSlidesLoaded >= mTotalSlides)
    {
        return;
    }

    UniViewSlideWidget* slide = new UniViewSlideWidget(mViewFeeder,
        mTotalSlidesLoaded);
    addItemToLayout(slide);
    slide->setInsideLayout(true);
    mSlides.append(slide);

    slide->populateContent();
    
    connect(slide,SIGNAL(sendMessage(const QString&,const QString&)),
            this, SIGNAL(sendMessage(const QString&,const QString&)));

    mTotalSlidesLoaded++;
    //TODO to remove the previous slide if the mTotalSlidesLoaded
    //exceeds the mSlideWindow
}

// EOF
