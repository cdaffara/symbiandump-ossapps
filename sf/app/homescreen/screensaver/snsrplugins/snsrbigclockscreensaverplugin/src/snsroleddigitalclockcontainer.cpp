/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for oled digital clock.
*
*/

#include "snsroleddigitalclockcontainer.h"

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QTime>
#include <QTimer>
#include <HbExtendedLocale>
#include <HbMainWindow>

#include "snsrlabel.h"
#include "snsrindicatorwidget.h"
#include "snsroledtimelabel.h"


/*!
    \class SnsrOledDigitalClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for oled digital clock.
 */

const char *gOledDigitalLayoutDocml = ":/xml/snsrbigclockscreensaveroleddigital.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;
extern const char *gMainContainerName;
extern const char *gClockContainerName;

extern const char *gOledTimeLabelName = "oledTimeLabel";
extern const char *gAmPmLabelName;
extern const char *gDateLabelName;

extern const char *gDateFormatStr;

extern const char *gTimeFormatStr;

extern const char *gIndicatorWidgetName;



/*!
    Constructs a new SnsrOledDigitalClockContainer.
 */
SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer() :
    SnsrBigClockContainer(), mClockContainer(0), mAmPmLabel(0),
    mTimeLabel(0), mDateLabel(0), mDestPosition(QPointF()),
    mInitialize(false)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer")
    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer")
}

/*!
    Destructs the class.
 */
SnsrOledDigitalClockContainer::~SnsrOledDigitalClockContainer()
{
    resetIndicatorConnections();
    //mTimeLabel, mAmPmLabel, mDateLabel, mMoveTimer - deleted by the parent
}

/*!
    Updates OLED digital clock widget position.
 */
void SnsrOledDigitalClockContainer::updatePosition()
{
    QSizeF containerSize = mMainContainer->size();
    
    // Container must have a valid size to enable calculating the 
    // destination position for the clock.
    if ( containerSize.width() > 0 && containerSize.height() > 0 ) {
        containerSize -= mClockContainer->boundingRect().size();
        QRectF containerRect( mMainContainer->pos(), containerSize );
        if ( mInitialize ) {
            // disconnect container from parent layout,
            // connected container resets its position to the one defined in docml
            // after label text updates
            mClockContainer->setParentLayoutItem(0);

            QPointF clockPos = nextRandomPosition( mClockContainer->pos(), mDestPosition, containerRect );
            mClockContainer->setPos( clockPos );
        }
        else {
            mDestPosition = randomPosition( containerRect );
            mInitialize = true;
        }
    }
}

/*!
    Updates displayed time and date.
 */
void SnsrOledDigitalClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledDigitalClockContainer::update")

    HbExtendedLocale locale = HbExtendedLocale::system();
    
    // time
    mTimeLabel->setText(
        locale.format(QTime::currentTime(), gTimeFormatStr)
        );
    
    // don't use %B here as some extra space might be added if it's used as an
    // isolated format string
    QString amPmString("");
    if (locale.timeStyle() == HbExtendedLocale::Time12 ) {
        amPmString = (QTime::currentTime().hour()<12) ? locale.amText() : locale.pmText();
    }
    mAmPmLabel->setPlainText( amPmString );
    
    // date
    mDateLabel->setPlainText(
        locale.format(QDate::currentDate(), gDateFormatStr)
        );

    updatePosition();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::update")
}

int SnsrOledDigitalClockContainer::updateIntervalInMilliseconds()
{
    return 60*1000;
}

/*!
    @copydoc Screensaver::DisplayPowerMode()
 */
Screensaver::ScreenPowerMode SnsrOledDigitalClockContainer::displayPowerMode()
{
    return Screensaver::ScreenModeLowPower;
}

/*!
    @copydoc SnsrBigClockContainer::getActiveScreenRows()
 */
void SnsrOledDigitalClockContainer::getActiveScreenRows(int *firstActiveRow, int *lastActiveRow)
{
    if ( mClockContainer ) {
        QRect clockRect( mClockContainer->pos().toPoint(), 
                         mClockContainer->size().toSize() );
        if ( mCurrentOrientation == Qt::Vertical ) {
            *firstActiveRow = clockRect.top();
            *lastActiveRow = clockRect.bottom() + 1;
        }
        else {
            *firstActiveRow = clockRect.left();
            *lastActiveRow = clockRect.right() + 1;
        }
    }
}

/*!
    Orientation is locked in power save mode as sensors are off anyway,
    at least after some timeout.
 */
bool SnsrOledDigitalClockContainer::isOrientationLocked()
{
    return true;
}

void SnsrOledDigitalClockContainer::loadWidgets()
{
    // reset widget pointers, any previous widgets are already deleted by now
    mMainView = 0;
    mMainContainer = 0;
    mClockContainer = 0;
    mDateLabel = 0;
    mTimeLabel = 0;
    mAmPmLabel = 0;
    mIndicatorWidget = 0;
    mInitialize = false;
    
    // load widgets from docml
    bool ok(false);
    qDebug() << gOledDigitalLayoutDocml;
    mDocumentObjects = mDocumentLoader.load(gOledDigitalLayoutDocml, &ok);
    Q_ASSERT_X(ok, gOledDigitalLayoutDocml, "Invalid DocML file.");

    if (ok) {
        mMainView = mDocumentLoader.findWidget(gMainViewName);
        mMainContainer = mDocumentLoader.findWidget(gMainContainerName);
        mClockContainer = mDocumentLoader.findWidget(gClockContainerName);
        mDateLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gDateLabelName));
        mTimeLabel = qobject_cast<SnsrOledTimeLabel *>(
                mDocumentLoader.findWidget(gOledTimeLabelName));
        mAmPmLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gAmPmLabelName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
                mDocumentLoader.findWidget(gIndicatorWidgetName));
        Q_ASSERT_X(
                mMainView && mMainContainer && mClockContainer 
                && mDateLabel && mTimeLabel && mAmPmLabel
                && mIndicatorWidget,
                gOledDigitalLayoutDocml, "Objects not found in DocML file."
                );

        // In case of landscape layout, read also the landscape delta section
        if ( mCurrentOrientation == Qt::Horizontal ) {
            qDebug() << "loading: " << gOledDigitalLayoutDocml << ", section: " << gLandscapeSectionName;
            mDocumentLoader.load(gOledDigitalLayoutDocml, gLandscapeSectionName, &ok);
            Q_ASSERT_X(ok, gOledDigitalLayoutDocml, "Invalid section in DocML file.");
        }

        mIndicatorWidget->setIconColorType(SnsrIndicatorWidget::FixedColorForPowerSaveMode);
        connectIndicatorWidgetToModel();

        mDateLabel->setTextColorType(SnsrLabel::FixedColorForPowerSaveMode);
        mAmPmLabel->setTextColorType(SnsrLabel::FixedColorForPowerSaveMode);

        mBackgroundContainerLayout->addItem(mMainView);
    }
}

// end of file
