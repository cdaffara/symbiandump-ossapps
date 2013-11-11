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
* Description:  Clock widget
*
*/

#ifndef HSCLOCKWIDGET_H
#define HSCLOCKWIDGET_H

#include <HbWidget>

#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HbWidget;
class QGraphicsLinearLayout;
#ifdef Q_OS_SYMBIAN
class HsClockSettingsNotifier;
#endif //Q_OS_SYMBIAN

class HsClockWidget : public HbWidget
{
    Q_OBJECT
    
public:
    HsClockWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HsClockWidget();

    QPainterPath shape() const;
public slots:
    void onInitialize();
    void onShow();
    void onHide();
    void onUninitialize();

    void updateTime();
    void onSettingsChanged(const QString &clockFormat, const QString &timeFormat);
    void onClockTapped();

private:
    Q_DISABLE_COPY(HsClockWidget)
    HbWidget *loadClockWidget();

private:
    HbWidget *mWidget;
    QGraphicsLinearLayout *mLayout;    
    QString mClockType;
    QString mTimeType;        
#ifdef Q_OS_SYMBIAN
    HsClockSettingsNotifier *mClockSettingsNotifier;
#endif //Q_OS_SYMBIAN
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)
};

#endif
