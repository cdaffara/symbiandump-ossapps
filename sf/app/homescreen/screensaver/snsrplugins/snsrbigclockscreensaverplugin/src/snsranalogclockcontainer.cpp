/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for analog clock.
*
*/

#include "snsranalogclockcontainer.h"

#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <HbExtendedLocale>
#include <HbMainWindow>

#include "snsranalogclockwidget.h"
#include "snsrindicatorwidget.h"
#include "snsrswipewidget.h"
#include "snsrlabel.h"

/*!
    \class SnsrAnalogClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for analog clock.
 */

const char *gAnalogLayoutDocml = ":/xml/snsrbigclockscreensaveranalog.docml";
const char *gSwipeAnalogLayoutDocml = ":/xml/snsrbigclockscreensaveranalogswipe.docml";


const char *gPortraitSectionName = "portrait";
const char *gLandscapeSectionName = "landscape";

const char *gMainViewName = "view";
const char *gMainContainerName = "mainContainer";
const char *gClockContainerName = "clockContainer";

const char *gDateLabelName = "dateLabel";
const char *gAnalogClockWidgetName = "analogClockWidget";
const char *gIndicatorWidgetName = "indicatorWidget";

const char *gSwipeWidgetName = "swipeWidget";

const char *gDateFormatStr = r_qtn_date_usual_with_zero; //"%E%,% %*D%*N%/0%4%/1%5";


/*!
    Constructs a new SnsrAnalogClockContainer.
 */
SnsrAnalogClockContainer::SnsrAnalogClockContainer() :
    SnsrBigClockContainer(),
    mDateLabel(0),
    mAnalogClockWidget(0),
    mSwipeWidget(0)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrAnalogClockContainer::SnsrAnalogClockContainer")
    SCREENSAVER_TEST_FUNC_EXIT("SnsrAnalogClockContainer::SnsrAnalogClockContainer")
}

/*!
    Destructs the class.
 */
SnsrAnalogClockContainer::~SnsrAnalogClockContainer()
{
    resetIndicatorConnections();
    //mDateLabel, mAnalogClockWidget - deleted by the parent
}

/*!
    Updates displayed time and date.
 */
void SnsrAnalogClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrAnalogClockContainer::update")

    // time
    mAnalogClockWidget->tick();

    // date
    mDateLabel->setPlainText(
        HbExtendedLocale().format(QDate::currentDate(), gDateFormatStr)
        );

    if (mSwipeWidget) {
        mSwipeWidget->setCurrentOrientation(mCurrentOrientation);
    }
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrAnalogClockContainer::update")
}

/*!
    @copydoc SnsrBigClockContainer::updateIntervalInMilliseconds()
 */
int SnsrAnalogClockContainer::updateIntervalInMilliseconds()
{
    return 1000;
}

/*!
    @copydoc SnsrBigClockContainer::loadWidgets()
 */
void SnsrAnalogClockContainer::loadWidgets()
{
    //if swipe is used we load the docml file containing it
    if ( swipeToUnlockSupported() ) {
        loadWidgets(gSwipeAnalogLayoutDocml);
        Q_ASSERT_X( mSwipeWidget, gSwipeAnalogLayoutDocml, "Swipe widget not found in DocML file.");
        connect( mSwipeWidget, SIGNAL(swipeDownDetected()), SIGNAL(unlockRequested()) );
    }
    else {
        loadWidgets(gAnalogLayoutDocml);
    }
}

/*!
    Instantiate widgets from the given docml file
 */
void SnsrAnalogClockContainer::loadWidgets(const char* docmlName)
{
    bool ok(true);

    // reset widget pointers, any previous widgets are already deleted by now
    mMainView = 0;
    mDateLabel = 0;
    mAnalogClockWidget = 0;
    mIndicatorWidget = 0;
    mSwipeWidget = 0;
    // load widgets from docml
    qDebug() << docmlName;
    mDocumentObjects = mDocumentLoader.load(docmlName, &ok);
    Q_ASSERT_X(ok, docmlName, "Invalid DocML file.");
    if (ok) {
        mMainView = mDocumentLoader.findWidget(gMainViewName);
        mDateLabel = qobject_cast<SnsrLabel *>(
            mDocumentLoader.findWidget(gDateLabelName));
        mAnalogClockWidget = qobject_cast<SnsrAnalogClockWidget *>(
            mDocumentLoader.findWidget(gAnalogClockWidgetName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
            mDocumentLoader.findWidget(gIndicatorWidgetName));
        mSwipeWidget = qobject_cast<SnsrSwipeWidget *>(
            mDocumentLoader.findWidget(gSwipeWidgetName));
        
        Q_ASSERT_X(
            mMainView && mDateLabel && mAnalogClockWidget &&
            mIndicatorWidget,
            docmlName, "Objects not found in DocML file."
            );
        
        // In case of landscape layout, read also the landscape delta section
        if ( mCurrentOrientation == Qt::Horizontal ) {
            qDebug() << "loading: " << docmlName << ", section: " << gLandscapeSectionName;
            mDocumentLoader.load(docmlName, gLandscapeSectionName, &ok);
            Q_ASSERT_X(ok, docmlName, "Invalid section in DocML file.");
        }
   
    mIndicatorWidget->setIconColorType(SnsrIndicatorWidget::ThemedColorForActiveMode);
    connectIndicatorWidgetToModel();
        
    mDateLabel->setTextColorType(SnsrLabel::ThemedColorForActiveMode);
    
    mBackgroundContainerLayout->addItem(mMainView);
    }
}

