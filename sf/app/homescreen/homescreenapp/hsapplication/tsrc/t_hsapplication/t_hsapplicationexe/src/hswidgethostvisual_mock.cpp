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
* Description:
*
*/

#include <QGraphicsLinearLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneResizeEvent>
#include <QGesture>
#include <QGraphicsScene>

#include "hswidgethostvisual.h"
#include "hswidgethost.h"
#include "hsscene.h"
#include "hsconfiguration.h"
#include "hswidgettoucharea.h"


/*!
    \class HsWidgetHostVisual
    \ingroup group_hsdomainmodel
    \brief Each widget is controlled by the home screen framework through a widget host.
*/

/*!
    Constructs a new widget host with given \a databaseId and
    \a parent item.
*/
HsWidgetHostVisual::HsWidgetHostVisual(QGraphicsItem *parent)
  : HbWidget(parent),mWidget(0)
  
{
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    setFlag(QGraphicsItem::ItemHasNoContents);

    grabGesture(Qt::TapGesture);
    grabGesture(Qt::TapAndHoldGesture);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
    grabGesture(Qt::CustomGesture);
    
    setupTouchArea();
    setupEffects();
    
    setMinimumSize(HSCONFIGURATION_GET(minimumWidgetSizeInPixels));
    setMaximumSize(HSCONFIGURATION_GET(maximumWidgetSizeInPixels));
    
}

/*!
    Destructor.
*/
HsWidgetHostVisual::~HsWidgetHostVisual()
{
}


/*!
    Reimplemented from QGraphicsItem. Returns the shape of the
    this widget host. The shape is computed based on the contained
    widget.
*/
QPainterPath HsWidgetHostVisual::shape() const
{
    QPainterPath path;

    if (mWidget) {
        QRectF currRect = rect();
        path = mWidget->shape();

        QRectF pathRect(path.boundingRect());

        if (pathRect.width() > currRect.width()
            || pathRect.height() > currRect.height()) {
            QPainterPath newPath(currRect.topLeft());
            newPath.addRect(currRect);
            path = path.intersected(newPath);
        }
    }
    return path;
}
void HsWidgetHostVisual::setWidget(QObject *widget)
{
    mWidget = qobject_cast<QGraphicsWidget *>(widget);
    if (mWidget) {
        mWidget->installEventFilter(this);
        mWidget->setParentItem(this);
        setNewSize(mWidget->size());
    }
}

void HsWidgetHostVisual::setVisualModel(HsWidgetHost *model)
{
    mVisualModel = model;
}
HsWidgetHost *HsWidgetHostVisual::visualModel() const
{
    return mVisualModel;
}
/*!
    Starts the drag effect.
*/
void HsWidgetHostVisual::startDragEffect()
{
}

/*!
    Starts the drop effect.
*/
void HsWidgetHostVisual::startDropEffect()
{
}
/*!
    \fn HsWidgetHost::resized()
    Notifies the home screen framework that this widget
    host has resized itself.
*/

/*!
    Reimplemented from QObject for monitoring changes in 
    contained widget's size.
*/
bool HsWidgetHostVisual::eventFilter(QObject *watched, QEvent *event)
{
    return HbWidget::eventFilter(watched, event);
}

/*!
    Reimplemented from HbWidget for pan gesture handling.
*/
void HsWidgetHostVisual::gestureEvent(QGestureEvent *event)
{
}

/*!
    \fn HsWidgetHostVisual::mousePressEvent(QGraphicsSceneMouseEvent *)
    Reimplemented from QGraphicsItem for eating all mouse presses.
*/

/*!
    Configures the touch are for this widget host.
*/
void HsWidgetHostVisual::setupTouchArea()
{
}

/*!
    Configures the effects for this widget host.
*/
void HsWidgetHostVisual::setupEffects()
{
}


/*!
    Resizes this widget host to the given \a size.
*/
void HsWidgetHostVisual::setNewSize(const QSizeF &size)
{
}

