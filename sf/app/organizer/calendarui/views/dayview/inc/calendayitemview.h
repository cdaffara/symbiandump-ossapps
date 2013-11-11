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
 * Description:  Day view control of calendar
 *
 */

#ifndef CALENDAYITEMVIEW_H
#define CALENDAYITEMVIEW_H

//System includes
#include <hbabstractitemview.h>
#include <hbmenu.h>

//User includes

//Forward declarations
class CalenDayInfo;
class CalenDayContainer;
class MCalenServices;

class CalenDayItemView : public HbAbstractItemView
{
	Q_OBJECT

public:
    enum CalendarPanDirection {
        CalendarPanNotSet,
        CalendarPanVertical,
        CalendarPanHorizontal
    };

    enum CalendaRecycleDays {
        CalendarNoRecycle,
        CalendarBackwardRecycle,
        CalendarForwardRecycle
    };

    CalenDayItemView(
        MCalenServices &services,
        HbModelIterator* iterator,
        QGraphicsItem *parent);
    ~CalenDayItemView();

    void scrollTo(
        const QModelIndex &index,
        HbAbstractItemView::ScrollHint hint = EnsureVisible);
    
    HbMenu *contextMenu() const;
    void setContextMenu(HbMenu *contextMenu);

public slots:

    virtual void reset();
    void modelAboutToBeReset();
    void modelReset();

    void scrollVertically(const QPointF &newPosition);

private slots:

    void itemPressed(const QModelIndex &index);
    void itemReleased(const QModelIndex &index);
    void itemActivated(const QModelIndex &index);
    void itemLongPressed(HbAbstractViewItem*, QPointF);

    void orientationChanged(Qt::Orientation orientation);

    void openSelectedItem();
    void editSelectedItem();
    void deleteSelectedItem();

private:

    void setupSlots();
    void setupContextMenu();
    bool issueCommandOnSelectedItem(int command);
    void clearSelectionInModel();

private:

    /**
     * Calendar services. Not owned.
     */
    MCalenServices &mServices;

    /**
     * Item container.
     */
    CalenDayContainer* mContainer;

    /**
     * Day event info.
     * Own.
     */
    CalenDayInfo* mInfo;

    /**
     * context menu
     */
    HbMenu* mContextMenu;
};

#endif // CALENDAYITEMVIEW_H

// End of File
