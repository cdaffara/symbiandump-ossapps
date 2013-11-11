/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QTimer>
#include <QGraphicsLinearLayout>

#include "hsclockwidget.h"
#include "hsclockwidgettimer.h"
#include "hsanalogclockwidget.h"
#include "hsdigitalclockwidget.h"

#ifdef Q_OS_SYMBIAN    
#include "hsclocksettingsnotifier_symbian.h"
#include <apgtask.h>
#include <eikenv.h>
#endif //Q_OS_SYMBIAN

namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";
    const char TIME12[] = "TIME12";
    const char TIME24[] = "TIME24";
}

#ifdef Q_OS_SYMBIAN
#define KClockAppUid TUid::Uid(0x10005903)
_LIT (KClockAppExe, "clock.exe");
#endif //Q_OS_SYMBIAN

/*!
    \class HsClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Implementation for the homescreen clock widget.
*/

/*!
    Constructs widget.
*/
HsClockWidget::HsClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
  : HbWidget(parent, flags),
    mWidget(0),
    mLayout(0),
    mClockType(ANALOG),
    mTimeType(TIME12)
{
#ifdef Q_OS_SYMBIAN    
    mClockSettingsNotifier = new HsClockSettingsNotifier(this);
    mClockType = mClockSettingsNotifier->clockFormat();
    mTimeType = mClockSettingsNotifier->timeFormat();
#endif
}

/*!
    Destructor.
*/
HsClockWidget::~HsClockWidget()
{
}

/*!
    Return shape
*/
QPainterPath HsClockWidget::shape() const
{
    return mWidget->shape();
}

/*!
    Initializes this widget.
*/
void HsClockWidget::onInitialize()
{
    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLayout->setContentsMargins(0, 0, 0, 0);
    mWidget = loadClockWidget();
    mLayout->addItem(mWidget);           
    setLayout(mLayout);
#ifdef Q_OS_SYMBIAN  
    connect(mClockSettingsNotifier, SIGNAL(settingsChanged(QString, QString)), this, SLOT(onSettingsChanged(QString, QString)));
#endif    

}

/*!
    Shows this widget.
*/
void HsClockWidget::onShow()
{    
    HsClockWidgetTimer::instance();
    connect(HsClockWidgetTimer::instance(), 
        SIGNAL(tick()), 
        SLOT(updateTime()), 
        Qt::UniqueConnection);
}


/*!
    Hides this widget.
*/
void HsClockWidget::onHide()
{
    HsClockWidgetTimer::instance()->disconnect(this);
}

/*!
    Uninitializes this widget.
*/
void HsClockWidget::onUninitialize()
{
    HsClockWidgetTimer::instance()->disconnect(this);
}

/*!
    Draws the clock with every second.
*/
void HsClockWidget::updateTime()
{
    if (mClockType == DIGITAL) {
        static_cast<HsDigitalClockWidget*>(mWidget)->tick();
    } else {
        static_cast<HsAnalogClockWidget*>(mWidget)->tick();
    }
}

/*!
    \internal
*/
void HsClockWidget::onSettingsChanged(const QString &clockFormat, const QString &timeFormat)
{
    if (mClockType != clockFormat) {        
        mClockType = clockFormat;
        mLayout->removeItem(mWidget);
        delete mWidget;
        mWidget = 0;
        mWidget = loadClockWidget();
        mLayout->addItem(mWidget);        
    }    
    
    if (mTimeType != timeFormat) {
        mTimeType = timeFormat;
        if (mClockType == DIGITAL) {
            if (mTimeType == TIME12) {
                static_cast<HsDigitalClockWidget*>(mWidget)->setAmPm(true);
            } else {
            static_cast<HsDigitalClockWidget*>(mWidget)->setAmPm(false);
            }    
        }
    }
}

/*!
    Clock tapped.
*/
void HsClockWidget::onClockTapped()
{
#ifndef Q_OS_SYMBIAN
    if (mClockType == ANALOG) {
        mClockType = DIGITAL;
        if (mTimeType == TIME12) {
            mTimeType = TIME24;
        } else {
            mTimeType = TIME12;
        }    
    } else {
        mClockType = ANALOG;
    }
    mLayout->removeItem(mWidget);
    delete mWidget;
    mWidget = 0;
    mWidget = loadClockWidget();
    mLayout->addItem(mWidget); 
    updateTime();
#else //Q_OS_SYMBIAN
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());    
    TApaTask task = taskList.FindApp(KClockAppUid);    
    if (task.Exists()){
        task.BringToForeground();
    }    
    else {
        RProcess process;
        TInt error = process.Create(KClockAppExe, KNullDesC, EOwnerThread);
    
        if (error == KErrNone){
            // start the process running.
            process.Resume();
            process.Close();
        }
    }
#endif //Q_OS_SYMBIAN   
}

/*!
    Loads the digital or analog clock widget.
*/
HbWidget *HsClockWidget::loadClockWidget()
{
    HbWidget *clockWidget = 0;

    if (mClockType == DIGITAL) {
        bool useAmPm = true;
        if (mTimeType == TIME24) {
            useAmPm = false;
        }   
        clockWidget = new HsDigitalClockWidget(useAmPm);
    } else {
        clockWidget = new HsAnalogClockWidget();
    }

    connect(clockWidget, SIGNAL(clockTapped()), this, SLOT(onClockTapped()), Qt::QueuedConnection);
    return clockWidget;
}
