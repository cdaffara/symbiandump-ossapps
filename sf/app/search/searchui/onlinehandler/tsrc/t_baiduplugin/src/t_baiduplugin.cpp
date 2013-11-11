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
 * Description:   t_baidplugin
 *
 */

#include "t_baiduplugin.h"

#include <QDebug>
#include <QGraphicsLayout>
#include <QWidget>
#include <QtWebKit/QGraphicsWebView>
#include <QtGui/QApplication>
#include <QGraphicsWidget>
#include <hbdialog.h>
#include <hbaction.h>
#include <hblabel.h>
#include <qdesktopservices.h>
// ---------------------------------------------------------------------------
// baiduplugin::baiduplugin()
// ---------------------------------------------------------------------------
//
baiduplugin::baiduplugin()
    {
    // Set up plugin settings 
    }

// ---------------------------------------------------------------------------
// baiduplugin::~baiduplugin()
// ---------------------------------------------------------------------------
//
baiduplugin::~baiduplugin()
    {
    }

// ---------------------------------------------------------------------------
// baiduplugin::initialize()
// ---------------------------------------------------------------------------
//
void baiduplugin::initializePlugin()
    {
    emit pluginInitialized(true);
    }

// ---------------------------------------------------------------------------
// baiduplugin::activate()
// ---------------------------------------------------------------------------
//
void baiduplugin::activatePlugin()
    {
    }

// ---------------------------------------------------------------------------
// baiduplugin::activate(const QString &)
// ---------------------------------------------------------------------------
//
void baiduplugin::activatePlugin(const QString &searchQuery)
    {
#ifdef __WINSCW__
    HbDialog* popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);
    QString str("Baidu Search  ");
    str.append(searchQuery);
    HbLabel *label = new HbLabel(str);
    popup->setHeadingWidget(label);
    HbAction * action = new HbAction(hbTrId("Back"), popup);
    connect(action, SIGNAL(triggered()), this, SLOT(BackEvent()));
    popup->addAction(action);
    popup->show();
#else
    QString url("http://www.baidu.com/s?wd=");
    url.append(searchQuery);
    QDesktopServices::openUrl(url);
#endif    
    }
void baiduplugin::BackEvent()
    {
    emit handleBackEvent();
    }
// ---------------------------------------------------------------------------
// baiduplugin::deactivate()
// ---------------------------------------------------------------------------
//
void baiduplugin::deactivatePlugin()
    {
    // Destroy everything but the initial view and data that takes a long time
    // to reacquire when the plugin is deactivated
    }

// ---------------------------------------------------------------------------
// baiduplugin::shutdown()
// ---------------------------------------------------------------------------
//
void baiduplugin::shutdownPlugin()
    {
    // Destroy all views
    emit shutdownCompleted(true);

    //return true;
    }

