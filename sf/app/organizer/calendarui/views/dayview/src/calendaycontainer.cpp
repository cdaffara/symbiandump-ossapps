/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Day view container - parent widget for events (CalenDayItem)
 * and hours area widgets (CalenDayEventsPane)
 * Responsible for positioning and resizing events widgets.
 */

//System includes
#include <QTime>
#include <QGraphicsLinearLayout>
#include <QGesture>

#ifdef _DEBUG
	#include <QPainter>
#endif

#include <HbAbstractItemView>
#include <HbTextItem>
#include <HbModelIterator>
#include <HbInstance>

//User includes
#include "calendaycontainer.h"
#include "calendayutils.h"
#include "calendayeventspane.h"
#include "calendayitem.h"
#include "calendaymodel.h"
#include "calendayinfo.h"
#include "calendayview.h"
#include "calenagendautils.h"
#include "calendaycommonheaders.h"

/*!
 \class CalenDayContainer
 
 \brief CalenDayContainer Item container class associated with model.
 */

/*!
 \brief Constructor
 
 Sets container initial geometry, creates hours area widgets.
 \param parent Parent object
 */
CalenDayContainer::CalenDayContainer(QGraphicsItem *parent) :
    HbAbstractItemContainer(parent), mGeometryUpdated(false), mInfo(0)
{
    getTimedEventLayoutValues(mLayoutValues);

    QGraphicsLinearLayout* timeLinesLayout = new QGraphicsLinearLayout(
        Qt::Vertical, this);
    for (int i = 0; i < KCalenHoursInDay; i++) {
        CalenDayEventsPane* element = new CalenDayEventsPane(this);
        // Draw top line at midnight
        if (i == 0) {
            element->setDrawTopLine(true);
        }
        timeLinesLayout->addItem(element);
    }
    timeLinesLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    timeLinesLayout->setSpacing(0.0);

    setLayout(timeLinesLayout);
}

/*!
 \brief Destructor
 */
CalenDayContainer::~CalenDayContainer()
{
    // Remove absorbers if exist
    if (mAbsorbers.count()) {
        qDeleteAll(mAbsorbers);
        mAbsorbers.clear();
    }
}

/*
 \reimp
 */
void CalenDayContainer::itemAdded(
    int index,
    HbAbstractViewItem *item,
    bool animate)
{
    Q_UNUSED( index )
    Q_UNUSED( item )
    Q_UNUSED( animate )
}


/*
 \brief Resets the state of container.
 
 Removes absorbers, gets layout values and maintains current position.
 */
void CalenDayContainer::reset()
{
    // Remove absorbers if exist
    if (mAbsorbers.count()) {
        qDeleteAll(mAbsorbers);
        mAbsorbers.clear();
    }

    // Shrink event area when all-day events available after reset
    getTimedEventLayoutValues(mLayoutValues);

    // Position need to be maintained while changing model
    QPointF position(pos());
    HbAbstractItemContainer::reset();
    setPos(position);
}

/*
 \reimp
 */
void CalenDayContainer::itemRemoved(HbAbstractViewItem *item, bool animate)
{
    Q_UNUSED( item )
    Q_UNUSED( animate )
}

/*
 \reimp
 */
void CalenDayContainer::viewResized(const QSizeF &size)
{
    resize(size);
    if (!mGeometryUpdated) {
        mGeometryUpdated = true;
        updateGeometry();
    }
}

/*
 \reimp
 */
HbAbstractViewItem * CalenDayContainer::createDefaultPrototype() const
{
    CalenDayItem *calendarViewItem = new CalenDayItem(this);
    return calendarViewItem;
}

/*
 \reimp
 */
void CalenDayContainer::setItemModelIndex(
    HbAbstractViewItem *item,
    const QModelIndex &index)
{
    QVariant variant = index.data(CalenDayEntry);
    AgendaEntry entry = variant.value<AgendaEntry> ();

    // Check for All Day Events and Timed entries
    if (CalenAgendaUtils::isAlldayEvent(entry)) {
        updateAllDayEventGeometry(item, index);
        item->setParentItem(this);
    }
    else
        if (entry.isTimedEntry()) {
            updateTimedEventGeometry(item, index);
            item->setParentItem(this);
        }
        else {
            item->setVisible(false);
        }

    // Create touch event absorbers after last item
    if (index.row() == index.model()->rowCount() - 1) {
        createTouchEventAbsorbers();
    }

    HbAbstractItemContainer::setItemModelIndex(item, index);
}


/*!
 \brief Set size and position of singe timed event widget (bubble)
 \a item bubble widget
 \a index pointing item data in model
 */
void CalenDayContainer::updateTimedEventGeometry(
    HbAbstractViewItem *item,
    const QModelIndex &index)
{
    // Safety check
    if (!mInfo) {
        return;
    }

    QVariant variant = index.data(CalenDayEntry);
    AgendaEntry entry = variant.value<AgendaEntry> ();

    // 1. Get 'virtual' event position from DayInfo
    SCalenApptInfo apptInfo;
    apptInfo.iIndex = index;

    QDateTime start;
    QDateTime end;
    QDateTime currentDate;
    currentDate
        = static_cast<const CalenDayModel*> (index.model())->modelDate();
    CalenDayUtils::instance()->getEventValidStartEndTime(start, end, entry,
        currentDate);
    apptInfo.iStartTime = start;
    apptInfo.iEndTime = end;

    TCalenInstanceId id = TCalenInstanceId::nullInstanceId();
    id.mEntryLocalUid = index.row(); //index.row() - temporary ID
    id.mInstanceTime = apptInfo.iStartTime;
    apptInfo.iId = id;
    apptInfo.iAllDay = 0;
    apptInfo.iColor = 0xffff;

    int startSlot, endSlot, columnIdx, columns;
    mInfo->GetLocation(apptInfo, startSlot, endSlot, columnIdx, columns);

    // 2. Set timed event's geometry
    qreal eventStartX(mLayoutValues.eventAreaX);
    qreal eventStartY(0.0);
    qreal eventWidth(mLayoutValues.eventAreaWidth);
    qreal eventHeight(0.0);

    // Event's startY/height
    eventStartY = startSlot * mLayoutValues.slotHeight;
    eventHeight = (endSlot - startSlot) * mLayoutValues.slotHeight;

    // Event's startX/width
    eventWidth /= columns;
    
    // In case when eventWidth will be smaller then KCalenMinEventWidth [un]
    // spacings between events should be smaller.
    // Check whether it's possible to shrink them so the bubbles width 
    // can stay at KCalenMinEventWidth [un] (time stripe + frame margins).
    qreal minWidth = KCalenMinEventWidth * mLayoutValues.unitInPixels;
    if (eventWidth - mLayoutValues.eventMargin < minWidth) {

        // Calculate new margin value
        // from totalMarginSpace we need to subtract mLayoutValues.eventMargin 
        // because first margin is always KCalenSpaceBeetwenEvents
        qreal totalMarginSpace = mLayoutValues.eventAreaWidth - minWidth
            * columns - mLayoutValues.eventMargin;
        qreal newMarginValue = totalMarginSpace / (columns - 1);

        // Check if we managed to pack all the events into space we have
        if (newMarginValue > 0) {
            eventWidth = minWidth;
        }
        else {
            // There's not enough space
            // New minWidth is KCalenMinTimeStripWidth [un] (time stripe only)
            minWidth = KCalenMinTimeStripWidth * mLayoutValues.unitInPixels;
            totalMarginSpace = mLayoutValues.eventAreaWidth - minWidth * columns 
				- mLayoutValues.eventMargin;
            newMarginValue = totalMarginSpace / (columns - 1);
            eventWidth = minWidth;
        }
        
        // First column margin should be always KCalenSpaceBeetwenEvents 
        // (mLayoutValues.eventMargin)
        eventStartX += columnIdx * (eventWidth + newMarginValue) + mLayoutValues.eventMargin;
    }
    else {
        // Add margins between the event
        eventStartX += columnIdx * eventWidth + mLayoutValues.eventMargin;
        eventWidth -= mLayoutValues.eventMargin;
    }

    // Verify if height of event is greater than minimum (UI spec)
    qreal minHeight = CalenDayUtils::instance()->minEventHeight();
    eventHeight = (eventHeight < minHeight ? minHeight : eventHeight);
    QRectF eventGeometry(eventStartX, eventStartY, eventWidth, eventHeight);
    
    // Workaround to prevent size hint caching inside effectiveSizeHint
    item->setMinimumSize(0, 0);
    item->setMaximumSize(eventWidth, eventHeight);
    item->setGeometry(eventGeometry);
}


/*!
 \brief Set size and position of singe all-day event widget (bubble)
 \a item bubble widget
 \a index pointing item data in model
 */
void CalenDayContainer::updateAllDayEventGeometry(
    HbAbstractViewItem *item,
    const QModelIndex &index)
{
    // Safety check
    if (!mInfo) {
        return;
    }

    QVariant variant = index.data(CalenDayEntry);
    AgendaEntry entry = variant.value<AgendaEntry> ();

    // 1. Get 'virtual' event position from DayInfo 
    SCalenApptInfo apptInfo;
    apptInfo.iIndex = index;

    QDateTime start;
    QDateTime end;
    QDateTime currentDate;
    currentDate
        = static_cast<const CalenDayModel*> (index.model())->modelDate();
    CalenDayUtils::instance()->getEventValidStartEndTime(start, end, entry,
        currentDate);
    apptInfo.iStartTime = start;
    apptInfo.iEndTime = end;

    TCalenInstanceId id = TCalenInstanceId::nullInstanceId();
    id.mEntryLocalUid = index.row(); //index.row() - temporary ID
    id.mInstanceTime = apptInfo.iStartTime;
    apptInfo.iId = id;
    apptInfo.iAllDay = true;
    apptInfo.iColor = 0xffff;

    int startSlot, endSlot, columnIdx, columns;
    mInfo->GetLocation(apptInfo, startSlot, endSlot, columnIdx, columns);

    // 2. Set timed event's geometry
    qreal eventStartX(0.0);
    qreal eventStartY(0.0);
    qreal eventWidth(mLayoutValues.eventAreaX);
    qreal eventHeight = (endSlot - startSlot) * mLayoutValues.slotHeight;

    // Event's startX/width
    if (columns > 1) {
        eventWidth /= columns;
        eventStartX += columnIdx * eventWidth + mLayoutValues.eventMargin;
        // Add margins between the event
        eventWidth -= mLayoutValues.eventMargin;
    }
    else {
        eventStartX += mLayoutValues.eventMargin;
        eventWidth -= mLayoutValues.eventMargin;
    }

    QRectF eventGeometry(eventStartX, eventStartY, eventWidth, eventHeight);
	
    // Workaround to prevent size hint caching inside effectiveSizeHint
    item->setMinimumSize(0, 0);
    item->setMaximumSize(eventWidth, eventHeight);
    item->setGeometry(eventGeometry);
}


/*!
 \brief Gets event layout values
 \a layoutValues structure to be filled with layout data
 */
void CalenDayContainer::getTimedEventLayoutValues(LayoutValues& layoutValues)
{
    // Get the width of entire content area
    qreal contentWidth = CalenDayUtils::instance()->contentWidth();

    HbStyle style;
    HbDeviceProfile deviceProfile;
    layoutValues.unitInPixels = deviceProfile.unitValue();

    // Empty right column's width
    qreal emptyRightColumnWidth = KCalenEmptyRightColumnWidth
        * layoutValues.unitInPixels;

    // Margins between the overlapping events -> eventMargin[out]
    layoutValues.eventMargin = KCalenSpaceBeetwenEvents
        * layoutValues.unitInPixels;

    // Start position (x) for drawing events -> eventAreaX[out]
    if (mInfo && mInfo->AlldayCount()) {
        layoutValues.eventAreaX = KCalenAllDayEventArea * (contentWidth
            - emptyRightColumnWidth);
    }
    else {
        layoutValues.eventAreaX = 0;
    }

    // Event area width (excluding All Day Events area)-> eventAreaWidth[out]
    layoutValues.eventAreaWidth = contentWidth - emptyRightColumnWidth
        - layoutValues.eventAreaX;

    // Half-hour slot's height -> slotHeight[out]
    layoutValues.slotHeight = CalenDayUtils::instance()->hourElementHeight()
        / KCalenSlotsInHour;

    // Check if touch absorbers should be created over some overlapping regions
    layoutValues.maxColumns = layoutValues.eventAreaWidth
        / ((KCalenMinTouchableEventWidth + KCalenSpaceBeetwenEvents)
            * layoutValues.unitInPixels);
}


/*!
 \brief Sets day's info structure to the container.
 \a dayInfo day's info data

 \sa CalenDayInfo, CalenDayContainer::dayInfo
 */
void CalenDayContainer::setDayInfo(CalenDayInfo* dayInfo)
{
    mInfo = dayInfo;
}

/*!
 \brief It return pointer to info structure of container.
 
 \sa CalenDayInfo, CalenDayContainer::setDayInfo
 */
CalenDayInfo* CalenDayContainer::dayInfo()
{
    return mInfo;
}

/*!
 \brief Sets date to the container. 
 Changes according to model which is connected to given view.
 
 \a date Date of container
 */
void CalenDayContainer::setDate(const QDate &date)
{
    mDate = date;
}

// -----------------------------------------------------------------------------
// date()
// Returns date of the container.
// -----------------------------------------------------------------------------
//
/*!
 \brief Returns date of the container.
 
 \sa date Date of container
 */
const QDate &CalenDayContainer::date() const
{
    return mDate;
}

/*!
 \brief Slot handles layout switch.
 \a orientation current device orientation
 */
void CalenDayContainer::orientationChanged(Qt::Orientation orientation)
{
    getTimedEventLayoutValues(mLayoutValues);

    Q_UNUSED( orientation )
    QList<HbAbstractViewItem *> items = this->items();
    int count(items.count());
    for (int i = 0; i < count; i++) {
        QModelIndex modelIndex = items[i]->modelIndex();
        if (modelIndex.isValid()) {
            QVariant variant = modelIndex.data(CalenDayEntry);
            AgendaEntry entry = variant.value<AgendaEntry> ();
            if (entry.isTimedEntry() && !CalenAgendaUtils::isAlldayEvent(entry)) {
                updateTimedEventGeometry(items[i], modelIndex);
            }
            else
                if (CalenAgendaUtils::isAlldayEvent(entry)) {
                    updateAllDayEventGeometry(items[i], modelIndex);
                }
        }
    }

    createTouchEventAbsorbers();
}


/*!
 \brief Creates absorbers which prevent touching to small items
 According to UI spec items smaller than 8.2 un are untouchable
 */
void CalenDayContainer::createTouchEventAbsorbers()
{
    // remove absorbers if exist
    if (mAbsorbers.count()) {
        qDeleteAll(mAbsorbers);
        mAbsorbers.clear();
    }

    // Create absorber for all-day events
    Qt::Orientation orientation = CalenDayUtils::instance()->orientation();
    int allDayCount = mInfo->AlldayCount();

    if ((orientation == Qt::Vertical 
        && allDayCount > KCalenTouchableAllDayEventsCountPortrait) 
        || (orientation == Qt::Horizontal
        && allDayCount > KCalenTouchableAllDayEventsCountLandscape)) {
        TouchEventAbsorber* absorber = crateAbsorberBetweenSlots(0, 0, true);
        mAbsorbers.append(absorber);
    }

    // Create absorbers for timed events
    const QList<CalenTimeRegion>& regionList = mInfo->RegionList();
    for (int i = 0; i < regionList.count(); i++) {
        if (regionList[i].iColumns.count() > mLayoutValues.maxColumns) {
            TouchEventAbsorber* absorber = crateAbsorberBetweenSlots(
                regionList[i].iStartSlot, regionList[i].iEndSlot, false);

            mAbsorbers.append(absorber);
        }
    }
}


/*!
 \brief Creates single absorber in given location
 \a startSlot absorber area starts from there
 \a endSlot absobrber area ends here
 \a forAllDayEvents if true absorber in all-day events area is created
 */
TouchEventAbsorber *CalenDayContainer::crateAbsorberBetweenSlots(
    int startSlot,
    int endSlot,
    bool forAllDayEvents)
{
    TouchEventAbsorber *absorber = new TouchEventAbsorber(this);
    absorber->setZValue(1000);
    absorber->setVisible(true);
    if (!forAllDayEvents) {
        absorber->setGeometry(mLayoutValues.eventAreaX, // x
            startSlot * mLayoutValues.slotHeight, // y
            mLayoutValues.eventAreaWidth, // w
            (endSlot - startSlot) * mLayoutValues.slotHeight); // h
    }
    else {
        absorber->setGeometry(0, 0, mLayoutValues.eventAreaX,
            KCalenHoursInDay * KCalenSlotsInHour * mLayoutValues.slotHeight);
    }

    return absorber;
}


/*!
 \brief Handles tap event on overlapping area
 Currently it leads to Agenda View -  as described in UI spec
 \a event qt gesture event
 */
void TouchEventAbsorber::gestureEvent(QGestureEvent *event)
{
    QTapGesture *tapGesture = qobject_cast<QTapGesture*> (event->gesture(
        Qt::TapGesture));
    
    if (tapGesture && tapGesture->state() == Qt::GestureFinished) {
    	CalenDayView* dayView = static_cast<CalenDayView*>
                (CalenDayUtils::instance()->mainWindow()->currentView());
    	
    	dayView->changeView(ECalenAgendaView);
    }
}

/*!
 \brief Constructor
 */
TouchEventAbsorber::TouchEventAbsorber(QGraphicsItem *parent) :
    HbWidget(parent)
{
#ifdef _DEBUG
    setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif	
    grabGesture(Qt::TapGesture);
}


/*!
 \brief Destructor
 
 Sets container initial geometry, creates hours area widgets.
 */
TouchEventAbsorber::~TouchEventAbsorber()
{

}


/*!
 \brief Used for debugging purposes to see absorbers areas
 Not active in release builds!
 
 */
#ifdef _DEBUG
void TouchEventAbsorber::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawRect(boundingRect());
    painter->restore();
}
#endif
// End of File
