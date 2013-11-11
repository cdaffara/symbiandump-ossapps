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

#ifndef HSWIDGETHOSTVISUAL_H
#define HSWIDGETHOSTVISUAL_H

#include <HbWidget>
#include <QMetaMethod>
#include <QMetaProperty>

#include "hsdomainmodel_global.h"
#include "hsdomainmodeldatastructures.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class QStateMachine;

class HsWidgetTouchArea;
class HsWidgetComponent;
class HsPage;
class HsWidgetHost;

class HSDOMAINMODEL_EXPORT HsWidgetHostVisual : public HbWidget
{
    Q_OBJECT

public:
    HsWidgetHostVisual(QGraphicsItem *parent = 0);
    ~HsWidgetHostVisual();
    
    QPainterPath shape() const;
    void setWidget(QObject *widget);
    QObject *widget() { return mWidget; }
    void setVisualModel(HsWidgetHost *model);
    HsWidgetHost *visualModel() const;
signals:
    void resized();

public slots:
    void startDragEffect();
    void startDropEffect();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void gestureEvent(QGestureEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *) {}

private:
    Q_DISABLE_COPY(HsWidgetHostVisual)
    void setupTouchArea();
    void setupEffects();

    void setNewSize(const QSizeF &size);

private:
    HsWidgetTouchArea *mTouchArea;
    HsWidgetHost *mVisualModel;
    QGraphicsWidget *mWidget;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWIDGETHOST_H
