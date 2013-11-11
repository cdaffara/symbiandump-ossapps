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
* Description:
*
*/

#include "hsdomainmodeldatastructures.h"
#include "hspagenewwidgetlayout.h"
#include "hsscene.h"
#include "hspage.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsconfiguration.h"
#include "hsgui.h"


/*!
    \class HsPageNewWidgetLayout
    \ingroup group_hsdomainmodel
    \brief Represents a page in the framework.
    HsPage is a parent for a group of widgets. HsPage can have a wallpaper.
*/

/*!
    Constructor.

    \a parent Owner.
*/
HsPageNewWidgetLayout::HsPageNewWidgetLayout(const QPointF &touchPoint,
                                             QGraphicsLayoutItem *parent)
    : QGraphicsLayout(parent),    
    mTouchPoint(touchPoint)
    // add mWidgetRects
{
    mSize = HsGui::instance()->layoutRect().size();
}

/*!
    Destructor.
*/
HsPageNewWidgetLayout::~HsPageNewWidgetLayout()
{
}

/*!
    Returns children count.
*/
int HsPageNewWidgetLayout::count() const
{
    return mNewWidgets.count();
}

/*!
    Returns item index of \a i.
*/
QGraphicsLayoutItem *HsPageNewWidgetLayout::itemAt(int i) const
{
    return mNewWidgets.at(i)->visual();
}

/*!
    Removes item \a index.
*/
void HsPageNewWidgetLayout::removeAt(int index)
{
    mNewWidgets.removeAt(index);
}

/*!
    Size hint.
*/
QSizeF HsPageNewWidgetLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
     switch (which) {
     case Qt::MinimumSize:
     case Qt::PreferredSize:
         return mSize;
     case Qt::MaximumSize:
         return mSize;
     default:
         break;
     }
     return constraint;
}

/*!
    Recalculates children positions.
*/
void HsPageNewWidgetLayout::setGeometry(const QRectF &rect)
{
    QGraphicsLayout::setGeometry(rect);

    // get rects for new widgets
    QList<QRectF> newRects;
    foreach (HsWidgetHost *newWidget, mNewWidgets) {
        newRects << QRectF(QPointF(), newWidget->visual()->preferredSize());
        // temp fix for double setGeometry call
        QRectF defaultRect = QRectF(0,0,58.625, 58.625);
        if (QRectF(QPointF(), newWidget->visual()->preferredSize()) == defaultRect) {
            return;
        }
    }

    /* if there is touch point defined (widget added from context menu)
       then there is only one widget in the list
       -> set widget center point to this touch point
    */
    if (mTouchPoint != QPointF() && newRects.count() == 1) {
        QRectF widgetRect = newRects.at(0);
        widgetRect.moveCenter(mTouchPoint);
        widgetRect.moveTopLeft(HsScene::instance()->activePage()->adjustedWidgetPosition(widgetRect));
        mNewWidgets.at(0)->visual()->setGeometry(widgetRect);
        /* we have to save widget presentation data here after drawing
           to get correct position for later use
        */
        mNewWidgets.at(0)->savePresentation();
    }
    // otherwise calculate widget positions with algorithm
    else {
        // get page rect
        QRectF pageRect = HsScene::instance()->activePage()->contentGeometry();

        // scan existing widgets rects
        QList<HsWidgetHost*> existingWidgets;
        existingWidgets = HsScene::instance()->activePage()->widgets();
        QList<QRectF> existingRects;
        foreach (HsWidgetHost *widget, existingWidgets) {
            if (!mNewWidgets.contains(widget)) {
                existingRects << QRectF(widget->visual()->pos(), widget->visual()->preferredSize());
            }
        }
        
        // calculate new widget positions with "stuck 'em all"-algorithm
        HsWidgetPositioningOnWidgetAdd *algorithm =
            HsWidgetPositioningOnWidgetAdd::instance();
        HsWidgetPositioningOnWidgetAdd::Result result;
        result = algorithm->convert(pageRect, existingRects, newRects, QPointF());
        if (result.rectOrder != QList<int>()) {
            // set new widgets to screen in sorted order and save presentation for each widget
            for (int i = 0; i < mNewWidgets.count(); i++) {
                int id = result.rectOrder.at(i);
                // take visual
                HsWidgetHostVisual *visual(mNewWidgets.at(id)->visual());
                visual->setGeometry(result.calculatedRects.at(id));
                // update z value
                int zValue = visual->zValue();
                visual->setZValue(zValue - id + i);
                mNewWidgets.at(id)->savePresentation();
            }
        }
    }
}

/*!
    Adds item to layout.
*/
void HsPageNewWidgetLayout::addItem(HsWidgetHost *item)
{
    mNewWidgets.append(item);
}
