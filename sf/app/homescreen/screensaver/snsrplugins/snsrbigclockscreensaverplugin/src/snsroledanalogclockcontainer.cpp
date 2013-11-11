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
* Description:  Container for oled analog clock.
*
*/

#include "snsroledanalogclockcontainer.h"

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QTimer>
#include <QTime>
#include <HbExtendedLocale>
#include <HbMainWindow>

#include "snsroledclockwidget.h"
#include "snsrindicatorwidget.h"
#include "snsrlabel.h"

/*!
    \class SnsrOledAnalogClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for oled analog clock.
 */

const char *gOledAnalogLayoutDocml = ":/xml/snsrbigclockscreensaveroledanalog.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;
extern const char *gMainContainerName;
extern const char *gClockContainerName;
extern const char *gIndicatorWidgetName;

extern const char *gDateLabelName;
extern const char *gDateFormatStr;

const char *gOledAnalogClockWidgetName = "oledAnalogClockWidget";


/*!
    Constructs a new SnsrOledAnalogClockContainer.
 */
SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer() :
    SnsrBigClockContainer(), mClockContainer(0), mOledClockWidget(0),
    mDateLabel(0), mDestPosition(QPointF()), mInitialize(false)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer")
    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer")
}

/*!
    Destructs the class.
 */
SnsrOledAnalogClockContainer::~SnsrOledAnalogClockContainer()
{
    resetIndicatorConnections();
    //mOledClockWidget etc - deleted by the parent
}

/*!
    Updates OLED analog clock widget position.
 */
void SnsrOledAnalogClockContainer::updatePosition()
{
    QSizeF containerSize = mMainContainer->size();
    
    // Container must have a valid size to enable calculating the 
    // destination position for the clock.
    if ( containerSize.width() > 0 && containerSize.height() > 0 ) {
        containerSize -= mClockContainer->size();
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
    Updates displayed time.
 */
void SnsrOledAnalogClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::update")

    // time
    mOledClockWidget->tick();
    
    // date
    mDateLabel->setPlainText(
        HbExtendedLocale().format(QDate::currentDate(), gDateFormatStr)
        );
    
    // position
    updatePosition();
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::update")
}

int SnsrOledAnalogClockContainer::updateIntervalInMilliseconds()
{
    return 60*1000;
}

/*!
    @copydoc Screensaver::DisplayPowerMode()
 */
Screensaver::ScreenPowerMode SnsrOledAnalogClockContainer::displayPowerMode()
{
    return Screensaver::ScreenModeLowPower;
}

/*!
    @copydoc SnsrBigClockContainer::getActiveScreenRows()
 */
void SnsrOledAnalogClockContainer::getActiveScreenRows(int *firstActiveRow, int *lastActiveRow)
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
bool SnsrOledAnalogClockContainer::isOrientationLocked()
{
    return true;
}

void SnsrOledAnalogClockContainer::loadWidgets()
{
    bool ok(true);

    // reset widget pointers, any previous widgets are already deleted by now
    mMainView = 0;
    mMainContainer = 0;
    mClockContainer = 0;
    mOledClockWidget = 0;
    mDateLabel = 0;
    mIndicatorWidget = 0;
    mInitialize = false;
    
    // load widgets from docml
    qDebug() << gOledAnalogLayoutDocml;
    mDocumentObjects = mDocumentLoader.load(gOledAnalogLayoutDocml, &ok);
    Q_ASSERT_X(ok, gOledAnalogLayoutDocml, "Invalid DocML file.");
    if (ok) {
        mMainView = mDocumentLoader.findWidget(gMainViewName);
        mMainContainer = mDocumentLoader.findWidget(gMainContainerName);
        mClockContainer = mDocumentLoader.findWidget(gClockContainerName);
        mOledClockWidget = qobject_cast<SnsrOledClockWidget *>(
              mDocumentLoader.findWidget(gOledAnalogClockWidgetName));
        mDateLabel = qobject_cast<SnsrLabel *>(
              mDocumentLoader.findWidget(gDateLabelName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
              mDocumentLoader.findWidget(gIndicatorWidgetName));
        Q_ASSERT_X(
                mMainView && mMainContainer && mClockContainer
                && mOledClockWidget && mDateLabel && mIndicatorWidget,
                gOledAnalogLayoutDocml, "Objects not found in DocML file."
                );
        
        // In case of landscape layout, read also the landscape delta section
        if ( mCurrentOrientation == Qt::Horizontal ) {
            qDebug() << "loading: " << gOledAnalogLayoutDocml << ", section: " << gLandscapeSectionName;
            mDocumentLoader.load(gOledAnalogLayoutDocml, gLandscapeSectionName, &ok);
            Q_ASSERT_X(ok, gOledAnalogLayoutDocml, "Invalid section in DocML file.");
        }

        mIndicatorWidget->setIconColorType(SnsrIndicatorWidget::FixedColorForPowerSaveMode);
        connectIndicatorWidgetToModel();

        mDateLabel->setTextColorType(SnsrLabel::FixedColorForPowerSaveMode);

        mBackgroundContainerLayout->addItem(mMainView);
    }
}

// end of file
