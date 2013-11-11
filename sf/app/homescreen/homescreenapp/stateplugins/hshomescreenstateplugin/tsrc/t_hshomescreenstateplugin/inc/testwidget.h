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

#ifndef HOMESCREEN_TEST_TESTWIDGET_H
#define HOMESCREEN_TEST_TESTWIDGET_H

#include <HbWidget>

class TestWidget : public HbWidget
{
    Q_OBJECT

public:
    TestWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~TestWidget();

signals:
    void finished();

public slots:
    void onInitialize();
    void onShow();
    void onHide();

public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setSucceeded() { emit finished(); }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    friend class HomeScreenStatePluginTest;
};

#endif
