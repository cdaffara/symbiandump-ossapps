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
 * Description: This widget holds the message details and silde contents.
 *
 */

#include "univiewslidewidget.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>

// USER INCLUDES
#include "univiewerbodywidget.h"
#include "univiewerfeeder.h"
#include "univiewerheadercontainer.h"
#include "nativemessageconsts.h"

#define LOC_SLIDE_COUNTER hbTrId("txt_messaging_formlabel_slide_l1l2")

//---------------------------------------------------------------
// UniViewSlideWidget :: UniViewSlideWidget
// @see header file
//---------------------------------------------------------------
UniViewSlideWidget::UniViewSlideWidget(UniViewerFeeder* feeder, int slideNumber,
    QGraphicsItem * parent) :
    HbWidget(parent), mInsideLayout(false), mSlideNumber(slideNumber), mViewFeeder(feeder),
        mHeaderContainer(0)

{
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);

    mBody = new UniViewerBodyWidget(this);

    connect(mBody, SIGNAL(sendMessage(const QString&)), this, SIGNAL(sendMessage(const QString&)));

    /**
     * If its a first slide create the headers and attach to the main layout
     */
    if (mSlideNumber == 0) {
        mHeaderContainer = new UniViewerHeaderContainer(mViewFeeder, this);

        // Always connect to populate sms content
        connect(mViewFeeder, SIGNAL(msgBody(QString)), mBody, SLOT(setText(QString)));
        
        connect(mHeaderContainer,SIGNAL(sendMessage(const QString&,const QString&)),
                this, SIGNAL(sendMessage(const QString&,const QString&)));
        
        

        mMainLayout->addItem(mHeaderContainer);
    }

    mMainLayout->addItem(mBody);

    setLayout(mMainLayout);
}

//---------------------------------------------------------------
// UniViewSlideWidget::~UniViewSlideWidget
// @see header file
//---------------------------------------------------------------
UniViewSlideWidget::~UniViewSlideWidget()
{

}

//---------------------------------------------------------------
//UniViewSlideWidget::clearContent
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::clearContent()
{
    if (mHeaderContainer) {
        mHeaderContainer->clearContent();
    }
    if (mBody) {
        mBody->clearContent();
    }
}

//---------------------------------------------------------------
// UniViewSlideWidget::populateContent
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::populateContent()
{
    if (mSlideNumber == 0) {
        mViewFeeder->fetchDetails();
        populateFirstSlide();
    }

    if (mViewFeeder->msgType() == KSenduiMtmMmsUidValue) {
        QString slideString;
        if (mViewFeeder->slideCount() > 1) {
            slideString = (LOC_SLIDE_COUNTER).arg(mSlideNumber + 1).arg(mViewFeeder->slideCount());
        }
        mBody->setSlideContents(mViewFeeder->slideContent(mSlideNumber), slideString);
    }
}

//---------------------------------------------------------------
// UniViewSlideWidget::populateFirstSlide
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::populateFirstSlide()
{
    mHeaderContainer->populateContent();
}

//---------------------------------------------------------------
// UniViewSlideWidget::insideLayout
// @see header file
//---------------------------------------------------------------
bool UniViewSlideWidget::insideLayout()
{
    return mInsideLayout;
}

//---------------------------------------------------------------
// UniViewSlideWidget::setInsideLayout
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::setInsideLayout(bool inside)
{
    mInsideLayout = inside;
}

// EOF
