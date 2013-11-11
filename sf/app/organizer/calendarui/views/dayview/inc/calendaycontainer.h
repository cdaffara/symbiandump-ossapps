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
 * Description:  Day view container - parent widget for events (CalenDayItem) and
 * hours area widgets (CalenDayEventsPane)
 * Responsible for positioning and resizing events widgets.
 *
 */

#ifndef CALENDAYCONTAINER_H
#define CALENDAYCONTAINER_H

// System includes
#include <QDateTime>
#include "../../../../../mw/hb/src/hbwidgets/itemviews/hbabstractitemcontainer_p.h"

// Forward declarations
class CalenDayInfo;
class TouchEventAbsorber;

// Class declaration
class CalenDayContainer : public HbAbstractItemContainer
{
    Q_OBJECT

public:
    CalenDayContainer(QGraphicsItem *parent = 0);
    ~CalenDayContainer();

    void itemAdded(int index, HbAbstractViewItem *item, bool animate);
    void itemRemoved(HbAbstractViewItem *item, bool animate);
    void reset();
    void viewResized(const QSizeF &size);

    void setDayInfo(CalenDayInfo* dayInfo);
    CalenDayInfo* dayInfo();
    void setDate(const QDate &date);
    const QDate &date() const;

public slots:
    void orientationChanged(Qt::Orientation orientation);

protected:
    HbAbstractViewItem * createDefaultPrototype() const;
    void setItemModelIndex(HbAbstractViewItem *item, const QModelIndex &index);

    void updateTimedEventGeometry(
        HbAbstractViewItem *item,
        const QModelIndex &index);
    void updateAllDayEventGeometry(
        HbAbstractViewItem *item,
        const QModelIndex &index);

    /*!
     \class LayoutValues
     \brief Structure with event layout values.
     
     eventAreaX X value for event area start.
     eventAreaWidth The width of event area.
     eventMargin Margins between the multiple events.
     slotHeight Half hour slot's height.
     unitInPixels No. of pixels in 1un
     maxColumns Max. no. of touchable overlapping timed events
     */
    class LayoutValues
    {
    public:
        LayoutValues() :
            eventAreaX(0), eventAreaWidth(0), eventMargin(0), slotHeight(0),
                unitInPixels(0), maxColumns(0)
        {
        }

        qreal eventAreaX;
        qreal eventAreaWidth;
        qreal eventMargin;
        qreal slotHeight;
        qreal unitInPixels;
        int maxColumns;
    };

    void getTimedEventLayoutValues(LayoutValues& layoutValues);
    void createTouchEventAbsorbers();
    TouchEventAbsorber* crateAbsorberBetweenSlots(
        int startSlot,
        int endSlot,
        bool forAllDayEvents);

private:
    bool mGeometryUpdated;  //!< Indicates if geometry needs to be updated
    CalenDayInfo* mInfo; //!< Day event info. Not owned.
    LayoutValues mLayoutValues; //!< Keeps layout values, see LayoutValues class
    QList<TouchEventAbsorber*> mAbsorbers; //!< Keeps needed touch absorbers
    QDate mDate; //!< Keeps date currently associated with this container
};



/*!
 \class TouchEventAbsorber
 \brief TouchEventAbsorber objects are used to catch touch events.
 \brief Used when event widgets are to small (see UI spec.) 
 */
class TouchEventAbsorber : public HbWidget
{
    Q_OBJECT
    
public:
    TouchEventAbsorber(QGraphicsItem *parent = 0);
    ~TouchEventAbsorber();
    
protected:
    void gestureEvent(QGestureEvent *event);

#ifdef _DEBUG
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget);
#endif       
};

#endif // CALENDAYCONTAINER_H
// End of File
