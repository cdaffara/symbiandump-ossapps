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
 * Description:  bingplugin.
 *
 */

#include "t_bingplugin.h"

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
// bingplugin::bingplugin()
// ---------------------------------------------------------------------------
//
bingplugin::bingplugin()
    {
    // Set up plugin settings 
    }

// ---------------------------------------------------------------------------
// bingplugin::~bingplugin()
// ---------------------------------------------------------------------------
//
bingplugin::~bingplugin()
    {
    }

// ---------------------------------------------------------------------------
// bingplugin::initialize()
// ---------------------------------------------------------------------------
//
void bingplugin::initializePlugin()
    {
    emit pluginInitialized(true);
    }

// ---------------------------------------------------------------------------
// bingplugin::activate()
// ---------------------------------------------------------------------------
//
void bingplugin::activatePlugin()
    {
    }

// ---------------------------------------------------------------------------
// bingplugin::activate(const QString &)
// ---------------------------------------------------------------------------
//
void bingplugin::activatePlugin(const QString &searchQuery)
    {
#ifdef __WINSCW__
    HbDialog* popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);
    QString str;
    str.append("bing Search  ");
    str.append(searchQuery);
    HbLabel *label = new HbLabel(str);
    popup->setHeadingWidget(label);
    HbAction * action = new HbAction(hbTrId("Back"), popup);
    connect(action, SIGNAL(triggered()), this, SLOT(BackEvent()));
    popup->addAction(action);
    popup->show();
#else
    QString url("http://www.bing.com/search?q=");
    url.append(searchQuery);
    url.append("&form=QBLH&qs=n&sk=");
    QDesktopServices::openUrl(url);
#endif
    }
void bingplugin::BackEvent()
    {
    emit handleBackEvent();
    }
// ---------------------------------------------------------------------------
// bingplugin::deactivate()
// ---------------------------------------------------------------------------
//
void bingplugin::deactivatePlugin()
    {
    // Destroy everything but the initial view and data that takes a long time
    // to reacquire when the plugin is deactivated
    }

// ---------------------------------------------------------------------------
// bingplugin::shutdown()
// ---------------------------------------------------------------------------
//
void bingplugin::shutdownPlugin()
    {
    // Destroy all views
    emit shutdownCompleted(true);
    }

