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


#include <HbInstantFeedback>
#include <HbTouchArea>

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
        QRectF visualRect = rect();
        path = mWidget->shape();

        QRectF widgetRect(path.boundingRect());
        if (widgetRect.width() > visualRect.width()
            || widgetRect.height() > visualRect.height()) {
            QPainterPath newPath;
            newPath.addRect(visualRect);
            path = path.intersected(newPath);
        } else if ( widgetRect.isEmpty() ) {
            path.addRect(visualRect);
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
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect =
        static_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
    */
    HbInstantFeedback::play(HbFeedback::ItemPick);

    setTransformOriginPoint(rect().center());

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(HSCONFIGURATION_GET(widgetDragEffectDuration));
    animation->setEndValue(1.1);
    animationGroup->addAnimation(animation);

    /* TODO: Uncomment after the Qt bug has been fixed.
    animation = new QPropertyAnimation(effect, "offset");
    animation->setDuration(200);
    animation->setEndValue(QPointF(8 ,8));
    animationGroup->addAnimation(animation);
    */

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
    Starts the drop effect.
*/
void HsWidgetHostVisual::startDropEffect()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect =
        static_cast<QGraphicsDropShadowEffect *>(graphicsEffect());
    */
    HbInstantFeedback::play(HbFeedback::ItemDrop);

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(HSCONFIGURATION_GET(widgetDropEffectDuration));
    animation->setEndValue(1);
    animationGroup->addAnimation(animation);

    /* TODO: Uncomment after the Qt bug has been fixed.
    animation = new QPropertyAnimation(effect, "offset");
    animation->setDuration(200);
    animation->setEndValue(QPointF(3, 3));
    animationGroup->addAnimation(animation);
    */

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
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
    if (event->type() == QEvent::GraphicsSceneResize ) {
        QGraphicsSceneResizeEvent *resizeEvent = 
            static_cast<QGraphicsSceneResizeEvent *>(event);
        setNewSize(resizeEvent->newSize());
        emit resized();
    }
    return HbWidget::eventFilter(watched, event);
}

/*!
    Reimplemented from HbWidget for pan gesture handling.
*/
void HsWidgetHostVisual::gestureEvent(QGestureEvent *event)
{
    HsScene *scene = HsScene::instance();    
    QGesture *gesture = event->gesture(Qt::PanGesture);
    if (gesture) {
        switch (gesture->state()) {
            case Qt::GestureStarted:
                grabMouse();
                emit scene->pagePanStarted(event);
                break;
            case Qt::GestureUpdated:
                emit scene->pagePanUpdated(event);
                break;
            case Qt::GestureFinished:
            case Qt::GestureCanceled:
                ungrabMouse();
                emit scene->pagePanFinished(event);
                break;
            default:
                break;
        }
    }
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
    mTouchArea = new HsWidgetTouchArea(this);
    mTouchArea->setZValue(1);
}

/*!
    Configures the effects for this widget host.
*/
void HsWidgetHostVisual::setupEffects()
{
    /* TODO: Uncomment after the Qt bug has been fixed.
    QGraphicsDropShadowEffect *effect =
        new QGraphicsDropShadowEffect(this);
    effect->setColor(QColor(0, 0, 0, 150));
    effect->setBlurRadius(5);
    effect->setOffset(3);
    setGraphicsEffect(effect);
    */
}


/*!
    Resizes this widget host to the given \a size.
*/
void HsWidgetHostVisual::setNewSize(const QSizeF &size)
{
    mTouchArea->resize(size);
    resize(size);
    setPreferredSize(size);
}

