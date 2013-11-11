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


// System include
#include <QDebug>
#include <hbaction.h>
#include <hbdocumentloader.h>
#include <hbapplication.h>
#include <hbicon.h>
#include <hbprogressdialog.h>

// User includes
#include "pimainwindow.h"
//#include "mcsendengine.h"
#include "pimainview.h"
#include "pisettingsview.h"
#include "generalattributes.h"
#include "piprofilerengine.h"
#include "notifications.h"

// xml definition of view
const char *PI_MAINVIEW_XML = ":/xml/pimainview.docml";

// name of the application view object in the xml file.
const char *APPLICATIONVIEW = "PIMainView";

// ViewManager Constructor
PIMainWindow::PIMainWindow(PIProfilerEngine* engine, HbDocumentLoader &loader, HbApplication *app) :
    mEngine(engine), mMainView(0), mSettingsView(0), mBackAction(0)
{

    HbProgressDialog *note = new HbProgressDialog(HbProgressDialog::WaitDialog);
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setText("Loading PI Profiler plug-ins...");
    note->open();

    // Add the views to the main window
    addMainView(loader, app);
    addSettingsView();

    // Set main view to be displayed on startup
    activateMainView();

    connect(mEngine, SIGNAL(pluginListUpdated(QList<PluginAttributes>&)), this,
        SLOT(setPluginList(QList<PluginAttributes>&)));

    QMetaObject::connectSlotsByName(this);

    mEngine->notifyUIReady();

    note->close();

}

PIMainWindow::~PIMainWindow()
{
    if (mMainView != 0) {
        delete mMainView;
        mMainView = 0;
    }
    if (mSettingsView != 0) {
        delete mSettingsView;
        mSettingsView = 0;
    }

}

// Methods to add views to the main window

void PIMainWindow::addMainView(HbDocumentLoader &loader, HbApplication *app)
{
    // Load xml-file
    bool ok = false;
    loader.load(PI_MAINVIEW_XML, &ok);

    // Load Application View:
    QGraphicsWidget *widget = loader.findWidget(APPLICATIONVIEW);
    Q_ASSERT_X(ok && (widget != 0), "PIProfiler", "Invalid docml file");
    mMainView = qobject_cast<PIMainView *> (widget);

    mMainView->init(this, app, loader, mEngine);

    // Add view to main window
    this->addView(mMainView);

}

void PIMainWindow::addSettingsView()
{
    mSettingsView = new PISettingsView(mEngine);
    mBackAction = new HbAction("back", this);

    bool value = connect(mBackAction, SIGNAL(triggered()), this, SLOT(settingsViewClosed()));
    mSettingsView->setNavigationAction(mBackAction);
    mSettingsView->navigationAction()->setIcon(HbIcon(":/gfx/enabled.svg"));
    addView(mSettingsView);
}

// Slots to handle view change
void PIMainWindow::activateMainView()
{
    setCurrentView(mMainView);
}

void PIMainWindow::activateSettingsView(int uid)
{
    if (uid != 0) {
        mSettingsView->expandPluginGroup(uid);
    }
    setCurrentView(mSettingsView);
}

void PIMainWindow::settingsViewClosed()
{
    if (currentView() == mSettingsView) {
        bool error = false;
        if (!mEngine->saveGeneralSettings(mSettingsView->getGeneralAttributes())) {
            error = true;
        }

        mSettingsView->readPluginSettings();

        for (int index = 0; index < mPluginAttributes.size(); index++) {
            if (!mEngine->savePluginSettings(mPluginAttributes.at(index))) {
                error = true;
            }
        }

        if (error) {
            Notifications::showErrorNote("Unable to save some of the settings into engine");
        }
        activateMainView();
        emit returnedFromSettings();
    }
    else {
        qWarning("Back action is not triggered at the correct view!");
    }
}

void PIMainWindow::setPluginList(QList<PluginAttributes> &pluginList)
{
    mPluginAttributes = pluginList;
    mMainView->setPluginList(&mPluginAttributes);
    mSettingsView->setPluginSettings(&mPluginAttributes);
}
