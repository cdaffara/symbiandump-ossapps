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
* Description:  Container for digital clock.
*
*/

#include "snsrdigitalclockcontainer.h"

#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <HbExtendedLocale>
#include <HbMainWindow>

#include "snsrswipewidget.h"
#include "snsrindicatorwidget.h"
#include "snsrlabel.h"

/*!
    \class SnsrDigitalClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for digital clock.
 */

const char *gDigitalLayoutDocml = ":/xml/snsrbigclockscreensaverdigital.docml";
const char *gSwipeDigitalLayoutDocml = ":/xml/snsrbigclockscreensaverdigitalswipe.docml";

extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;

const char *gTimeLabelName = "timeLabel";
const char *gAmPmLabelName = "amPmLabel";
extern const char *gDateLabelName;

extern const char *gDateFormatStr;

// This is same as r_qtn_time_usual_with_zero defined in
// hbi18ndef.h expect am/pm place holders are removed.
const char *gTimeFormatStr = "%:0%J%:1%T%:3";

extern const char *gIndicatorWidgetName;
extern const char *gSwipeWidgetName;


/*!
    Constructs a new SnsrDigitalClockContainer.
 */
SnsrDigitalClockContainer::SnsrDigitalClockContainer() :
    SnsrBigClockContainer(),
    mTimeLabel(0),
    mAmPmLabel(0),
    mDateLabel(0),
    mSwipeWidget(0)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrDigitalClockContainer::SnsrDigitalClockContainer")
    SCREENSAVER_TEST_FUNC_EXIT("SnsrDigitalClockContainer::SnsrDigitalClockContainer")
}

/*!
    Destructs the class.
 */
SnsrDigitalClockContainer::~SnsrDigitalClockContainer()
{
    resetIndicatorConnections();
    //mTimeLabel, mAmPmLabel, mDateLabel - deleted by the parent
}

/*!
    Updates displayed time and date.
 */
void SnsrDigitalClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrDigitalClockContainer::update")

    HbExtendedLocale locale = HbExtendedLocale::system();
    
    // time
    // Even though the format string (hh:mm) contain the leading zero, it's
    // removed always when clock type is 12h -> h:mm. 
   
    // TODO: within this method call, you find a comment in code that
    // it's intentional to remove leading zero always with 12h type clock.
    // However it seems to be recommended to use hbExtendedLocale to get
    // proper time formatting for all locales and settings..    
    mTimeLabel->setPlainText(
        locale.format(QTime::currentTime(), gTimeFormatStr)
        );
    
    QString amPmString("");
    if (locale.timeStyle() == HbExtendedLocale::Time12 ) {
        amPmString = (QTime::currentTime().hour()<12) ? locale.amText() : locale.pmText();
    }
    mAmPmLabel->setPlainText( amPmString );
    
    // date
    mDateLabel->setPlainText(
        locale.format(QDate::currentDate(), gDateFormatStr)
        );
    
    if (mSwipeWidget) {
        mSwipeWidget->setCurrentOrientation(mCurrentOrientation);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrDigitalClockContainer::update")
}

/*!
    @copydoc SnsrBigClockContainer::updateIntervalInMilliseconds()
 */
int SnsrDigitalClockContainer::updateIntervalInMilliseconds()
{
    return 1000;
}

/*!
    @copydoc SnsrBigClockContainer::loadWidgets()
 */
void SnsrDigitalClockContainer::loadWidgets()
{
    //if swipe is used we load the docml file containing it
    if ( swipeToUnlockSupported() ) {
        loadWidgets(gSwipeDigitalLayoutDocml);
        Q_ASSERT_X( mSwipeWidget, gSwipeDigitalLayoutDocml, "Swipe widget not found in DocML file.");
        connect( mSwipeWidget, SIGNAL(swipeDownDetected()), SIGNAL(unlockRequested()) );
    }
    else {
        loadWidgets(gDigitalLayoutDocml);
    }
}

/*!
    Instantiate widgets from the given docml file
 */
void SnsrDigitalClockContainer::loadWidgets(const char* docmlName)
{
    bool ok(true);

    // reset widget pointers, any previous widgets are already deleted by now
    mMainView = 0;
    mDateLabel = 0;
    mTimeLabel = 0;
    mAmPmLabel = 0;
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
        mTimeLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gTimeLabelName));
        mAmPmLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gAmPmLabelName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
                mDocumentLoader.findWidget(gIndicatorWidgetName));
        mSwipeWidget = qobject_cast<SnsrSwipeWidget *>(
            mDocumentLoader.findWidget(gSwipeWidgetName));
        
        Q_ASSERT_X(
            mMainView && mDateLabel && mTimeLabel && mAmPmLabel &&
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
        mTimeLabel->setTextColorType(SnsrLabel::ThemedColorForActiveMode);
        mAmPmLabel->setTextColorType(SnsrLabel::ThemedColorForActiveMode);
        
        mBackgroundContainerLayout->addItem(mMainView);
    }
}

