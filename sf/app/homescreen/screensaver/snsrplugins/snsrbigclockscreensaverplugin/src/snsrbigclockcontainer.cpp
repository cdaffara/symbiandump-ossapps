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
* Description:  Base class container.
*
*/

#include "snsrbigclockcontainer.h"

#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <HbEvent>
#include <HbColorScheme>
#include <HbMainWindow>
#include <HbEffect>

#ifdef Q_OS_SYMBIAN
#include <xqsettingsmanager.h>
#include <screensaverdomaincrkeys.h>
#endif //Q_OS_SYMBIAN

#include "snsrbigclockcontainer.h"
#include "snsrindicatorwidget.h"
#include "snsrindicatormodel.h"
#include "snsrcolors.h"

/*!
    \class SnsrBigClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Base class. Container used for drawing background and for preparing layout.
 */



const int gStep(5);


/*!
    Constructs a new SnsrBigClockContainer.
 */
SnsrBigClockContainer::SnsrBigClockContainer() :
    mBackgroundContainerLayout(0),
    mMainView(0),
    mMainContainer(0),
    mIndicatorModel(0),
    mIndicatorWidget(0),
    mCurrentOrientation(-1)
{
    setBackgroundColor();
    mBackgroundContainerLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    setLayout(mBackgroundContainerLayout);
    qsrand(QTime::currentTime().msec());
    connect( mainWindow(), SIGNAL(aboutToChangeOrientation()), SLOT(fadeOutView()) );
}

/*!
    Destructs the class.
 */
SnsrBigClockContainer::~SnsrBigClockContainer()
{
    // e.g. mIndicatorWidget gets deleted during this call
    resetWidgets();

    //mBackgroundContainerLayout - deleted by the parent
    mIndicatorModel = 0; // not owned
}

/*!
    \fn virtual void update() = 0;

    Slot for members update in container e.g. when time or date changed.
 */

/*!
    Changes screensaver layout based on orientation changes.
    \param orientation Current orientation.
 */
void SnsrBigClockContainer::changeLayout(Qt::Orientation orientation)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockContainer::changeLayout")

    if (mCurrentOrientation != orientation) {
        mCurrentOrientation = orientation;
        
        // delete any old widgets
        if ( mBackgroundContainerLayout->count() ) {
            mBackgroundContainerLayout->removeAt(0);
        }
        resetWidgets();

        // reload widgets from docml
        loadWidgets();
        
        // register orientation effects for the newly loaded main view
        HbEffect::add( mMainView, "notificationdialog_appear", "appear" );
        HbEffect::add( mMainView, "notificationdialog_disappear", "disappear" );
    }
    mBackgroundContainerLayout->setGeometry( mainWindow()->layoutRect() );
    update();

    // Run the fade-in effect, except if this container is a low-power one.
    // In low-power mode, animations don't look good because of reduced 
    // color-depth and refresh rate.
    if ( displayPowerMode() == Screensaver::ScreenModeFullPower ) {
        HbEffect::start( mMainView, "appear" );
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockContainer::changeLayout")
}

/*!
    Set used indicator model that is owned by the screensaver class. 
    Model's life cycle must be the same as screensaver's so that indicators'
    status data can be kept in memory and one can receive updates. 
    This method should be called when the current container is set.
 */
void SnsrBigClockContainer::setIndicatorModel(SnsrIndicatorModel &model)
{
    mIndicatorModel = &model;
}

/*!
    @copydoc Screensaver::currentPowerMode()
 */
Screensaver::ScreenPowerMode SnsrBigClockContainer::displayPowerMode()
{
    // The default implementation returns full power mode. Inherited classes
    // must override this if low power or display off mode are required.
    return Screensaver::ScreenModeFullPower;
}

/*!
    @copydoc Screensaver::getActiveScreenRows()
 */
void SnsrBigClockContainer::getActiveScreenRows(int *firstActiveRow, int *lastActiveRow)
{
    // This default implementation return the whole area of the
    // container. Inherited low power mode containers can and should
    // return smaller area which just barely encloses all the content.
    if ( mMainContainer ) {
        QRect mainRect = mMainContainer->rect().toRect();
        if ( mCurrentOrientation == Qt::Vertical ) {
            *firstActiveRow = mainRect.top();
            *lastActiveRow = mainRect.bottom();
        }
        else {
            *firstActiveRow = mainRect.left();
            *lastActiveRow = mainRect.right();
        }
    }
}

/*!
    Tell if this container wants to lock the screen orientation.
    Default implementation in not locked but inherited classes may
    override this.
 */
bool SnsrBigClockContainer::isOrientationLocked()
{
    return false;
}

/*!
    \fn virtual int updateIntervalInMilliseconds() = 0;

    Concrete inherited container classes must implement this to return
    the desired update interval for that clock mode.
 */

/*!
    \fn virtual int loadWidgets() = 0;

    Concrete inherited container classes must implement this to instantiate
    all the widgets shown in the container. The base class calls this
    method when screen layuot is changed. The old widgets are already 
    deleted by the base class before this is called. Also changing the visible
    container is treated as a layout change, and results in call to this method.
    Thus, inherited containers don't have to load their widgets yet in their
    constructors.
 */

/*!
    \reimp
 */
void SnsrBigClockContainer::changeEvent(QEvent *event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        setBackgroundColor();
    }
    return QGraphicsWidget::changeEvent(event);
}

/*!
    Returns random point for given range.
    \param rect Area within which the generated point will be.
 */
QPointF SnsrBigClockContainer::randomPosition(const QRectF &rect)
{
    int width( rect.width() );
    int height( rect.height() );
    if ( width > 0 && height > 0 ) {
        return rect.topLeft() + QPointF( qrand()%width, qrand()%height );
    }
    else {
        return QPointF();
    }

}

/*!
    Returns new position between curRect position and destPos position.
    \param curPos Current position.
    \param destPos Destination position. When current position is near this
                   position or outside of the container, a new value is generated.
    \param containerRect The container within which the destination position will always
                         be after a call.
 */
QPointF SnsrBigClockContainer::nextRandomPosition(const QPointF &curPos, QPointF &destPos, const QRectF &containerRect)
{
    const int delta(gStep+2);
    const int minCntDimension(3*delta);

    // The random movement logic can work only if the container has enough space to move the
    // clock around. If the container is too small, just return the middle point of the container.
    if ( containerRect.width() < minCntDimension && containerRect.height() < minCntDimension ) {
        return containerRect.center();
    }
    
    int xDistance = abs( destPos.x() - curPos.x() );
    int yDistance = abs( destPos.y() - curPos.y() );
    
    // Generate new destination position when current widget position is close to 
    // destination random position or when current destination position is out of bounds.
    // It is possible that the new random position is very close to the current position,
    // in which case the random position is generated again.
    // It is paramount that container is large enough when next loop is entered
    // to prevent infinite looping.
    while ( (xDistance < delta && yDistance < delta)
            || !containerRect.contains(destPos) ) {
        destPos = randomPosition( containerRect );
        xDistance = abs( destPos.x() - curPos.x() );
        yDistance = abs( destPos.y() - curPos.y() );
    }

    // If, for some reason, the current position is out-of-bounds, then there's no
    // point to slowly move towards the destination. In that case, move immediately
    // to destination point.
    if ( !containerRect.contains(curPos) ) {
        return destPos;
    }
    // Otherwise, this is normal case and we will calculate a point which is just
    // a bit closer to the destination.
    
    // Version 1:
    //  o-----o---------------------------o
    //  p1    p2                          p3
    //
    // Version 2:
    //  o---------------------------o-----o
    //  p3                          p2    p1
    //
    // p1 - current widget position
    // p2 - next position to compute
    // p3 - destination random position
    QPointF p1(curPos);
    QPointF p2(0,0);
    QPointF p3(destPos);

    // Computes point p2 - new position between p1 and p3

    // Move the coordinate which is further away from the destination
    // and calculate the other coordinate from that so that the
    // result point p2 lies on the straigth line between p1 and p3.
    if ( yDistance > xDistance ) {
        if (p3.y() > p1.y()) {
            p2.setY(p1.y()+gStep);
        }
        else {
            p2.setY(p1.y()-gStep);
        }
        p2.setX((((p2.y()-p1.y())*(p3.x()-p1.x())) / (p3.y()-p1.y())) + p1.x()); // x2 = (((y2-y1)*(x3-x1)) / (y3-y1)) + x1
    }
    else {
        if (p3.x() > p1.x()) {
            p2.setX(p1.x()+gStep);
        }
        else {
            p2.setX(p1.x()-gStep);
        }
        p2.setY((((p3.y()-p1.y())*(p2.x()-p1.x())) / (p3.x()-p1.x())) + p1.y()); // y2 = (((y3-y1)*(x2-x1)) / (x3-x1)) + y1
    }

    // Return new position between points p1 and p3.
    return p2;
}

/*!
    Do necessary initializations to show currently active indicators.
    Should be called after the indicator widget is created.
 */
void SnsrBigClockContainer::connectIndicatorWidgetToModel()
{
    Q_ASSERT(mIndicatorModel && mIndicatorWidget);
    
    connect(mIndicatorModel, SIGNAL(indicatorsUpdated(QList<SnsrIndicatorInfo>)),
            mIndicatorWidget, SLOT(showIndicators(QList<SnsrIndicatorInfo>)));
                
    connect(mIndicatorModel, SIGNAL(allIndicatorsDeactivated()),
            mIndicatorWidget, SLOT(removeAllIndicators()));
        
    mIndicatorModel->initializeIndicatorWidget();
}

/*!
    Disconnect connections between indicator model and widget.
    Should be called before deleting the indicator widget.
 */
void SnsrBigClockContainer::resetIndicatorConnections()
{   
    if (mIndicatorWidget && mIndicatorModel) {
        disconnect(mIndicatorModel, SIGNAL(indicatorsUpdated(QList<SnsrIndicatorInfo>)),
                   mIndicatorWidget, SLOT(showIndicators(QList<SnsrIndicatorInfo>)));
        
        disconnect(mIndicatorModel, SIGNAL(allIndicatorsDeactivated()),
                   mIndicatorWidget, SLOT(removeAllIndicators()));
    }
}

/*!
    Destroy all the contained widgets
 */
void SnsrBigClockContainer::resetWidgets()
{
    // deregister effects
    if ( mMainView ) {
        HbEffect::remove( mMainView );
    }
    
    mDocumentLoader.reset();
    qDeleteAll(mDocumentObjects);
    mDocumentObjects.clear();
}

/*!
    Set background color.
 */
void SnsrBigClockContainer::setBackgroundColor()
{
    mBackgroundColor = SnsrColors::BackgroundColor;
}


/*
 * Returns true if swipe widget is used,
 * false if swipe is not used.
 */
bool SnsrBigClockContainer::swipeToUnlockSupported()
{
#ifdef Q_OS_SYMBIAN 
    XQSettingsManager::Error error;
    int swipeWidget = 0; 
    XQCentralRepositorySettingsKey settingsKey(
             KCRUidScreensaverSettings.iUid, KScreensaverSwipeToOpen ); 
    XQSettingsManager settingsManager;
    swipeWidget = settingsManager.readItemValue(settingsKey, XQSettingsManager::TypeInt).toInt();
    error = settingsManager.error();
    if (error == XQSettingsManager::NoError && swipeWidget == 1) {
        return true;
    }
    else {
        return false;
    }
#else
    return true;
#endif //Q_OS_SYMBIAN
}

/*!
    Paints the contents of an item in local coordinates.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void SnsrBigClockContainer::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
        )
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->fillRect(rect(), mBackgroundColor);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Run the fade-out transition effect.
 */
void SnsrBigClockContainer::fadeOutView()
{
    // there's no point in animating the fade-out in the low-power mode
    if ( mMainView && displayPowerMode() == Screensaver::ScreenModeFullPower ) {
        HbEffect::start( mMainView, "disappear" );
    }
}

