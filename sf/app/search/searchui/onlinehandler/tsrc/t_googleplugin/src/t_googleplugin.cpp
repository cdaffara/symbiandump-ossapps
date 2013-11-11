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
 * Description: googleplugin.
 *
 */

#include "t_googleplugin.h"

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
// googleplugin::googleplugin()
// ---------------------------------------------------------------------------
//
googleplugin::googleplugin()
    {
    qDebug() << "googleplugin";
    // Set up plugin settings 
    }

// ---------------------------------------------------------------------------
// googleplugin::~googleplugin()
// ---------------------------------------------------------------------------
//
googleplugin::~googleplugin()
    {
    }

// ---------------------------------------------------------------------------
// googleplugin::initialize()
// ---------------------------------------------------------------------------
//
void googleplugin::initializePlugin()
    {
    emit pluginInitialized(true);
    }

// ---------------------------------------------------------------------------
// googleplugin::activate()
// ---------------------------------------------------------------------------
//
void googleplugin::activatePlugin()
    {
    }

// ---------------------------------------------------------------------------
// googleplugin::activate(const QString &)
// ---------------------------------------------------------------------------
//
void googleplugin::activatePlugin(const QString &searchQuery)
    {
    qDebug() << "search:activatePlugin" ;
#ifdef __WINSCW__
    qDebug() << "search:activatePlugin winscw" ;
    HbDialog* popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);
    QString str;
    str.append(QString("google Search  "));
    str.append(searchQuery);
    HbLabel *label = new HbLabel(str);
    popup->setHeadingWidget(label);
    HbAction * action = new HbAction(hbTrId("Back"), popup);
    connect(action, SIGNAL(triggered()), this, SLOT(BackEvent()));
    popup->addAction(action);
    popup->show();
#else
    qDebug() << "search:activatePlugin armvs" ;
    QString url("http://www.google.co.in/search?hl=en&source=hp&q=");
    url.append(searchQuery);
    url.append("&meta=&aq=f&aqi=g10&aql=&oq=&gs_rfai=");
    QDesktopServices::openUrl(url);
#endif
    
    }
void googleplugin::BackEvent()
    {
    emit handleBackEvent();
    }
// ---------------------------------------------------------------------------
// googleplugin::deactivate()
// ---------------------------------------------------------------------------
//
void googleplugin::deactivatePlugin()
    {
    // Destroy everything but the initial view and data that takes a long time
    // to reacquire when the plugin is deactivated
    }

// ---------------------------------------------------------------------------
// googleplugin::shutdown()
// ---------------------------------------------------------------------------
//
void googleplugin::shutdownPlugin()
    {
    // Destroy all views
    emit shutdownCompleted(true);
    //return true;
    }

