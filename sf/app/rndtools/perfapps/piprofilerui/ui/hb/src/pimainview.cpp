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


#include <hbdocumentloader.h>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbaction.h>
#include <qgraphicslayout.h>
#include <qvariant.h>
#include <qobject.h>
#include <qlist.h>
#include <hbmessagebox.h>
#include <hblabel.h>
#include <hbwidget.h>
#include <qstringlist.h>
#include <qsignalmapper.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbdialog.h>
#include <hbprogressdialog.h>
#include <hbtextitem.h>
#include <hbindicator.h>
#include <qdebug.h>

#include "pimainview.h"
#include "piprofilerengine.h"
#include "notifications.h"
#include "pimainwindow.h"

const char *INVALIDCONFIGURATIONFILE = "Invalid configuration file";
const char *APPLICATIONNAME = "PI Profiler";

// actions that are found from xml
const char *ACTIONEXIT = "actionExit";
const char *ACTIONSTARTPROFILING = "actionStartProfiling";
const char *ACTIONSTARTTIMEDPROFILING = "actionStartTimedProfiling";
const char *ACTIONSETTINGS = "actionSettings";
const char *ACTIONABOUT = "actionAbout";

// UI components that are found from xml

const char *LISTWIDGETPLUGINS = "listPlugins";
const char *TEXTMANDATORYPLUGINS = "Mandatory plug-ins:";
const char *TEXTOPTIONALPLUGINS = "Optional plug-ins:";

// ---------------------------------------------------------------------------

PIMainView::PIMainView() :
    application(0), mMainWindow(0), mEngine(0), mPluginList(0), mContextMenu(0), mActionExit(0),
        mActionExitAndLeaveProfilingOn(0), mActionAbout(0), mActionStartTimedProfiling(0),
        mActionSettings(0), mActionStopProfiling(0), mListPlugins(0), mProfilingNote(0)
{

}

// ---------------------------------------------------------------------------

PIMainView::~PIMainView()
{
}

// ---------------------------------------------------------------------------

void PIMainView::init(PIMainWindow *mainWindow, HbApplication* app, HbDocumentLoader &loader,
    PIProfilerEngine *engine)
{

    application = app;
    mMainWindow = mainWindow;

    // Load items from xml file
    loadItemsFromResources(loader);

    // Set list widget items
    mListPlugins->setClampingStyle(HbScrollArea::BounceBackClamping);
    mListPlugins->setFrictionEnabled(true);

    // Create stop profiling action
    mActionStopProfiling = new HbAction("Stop Profiling", this);

    // Create exit and leave profiling on action
    mActionExitAndLeaveProfilingOn = new HbAction("Close UI\n(Continue profiling)", this);

    mEngine = engine;

    // Connect signals and slots
    this->connectSignalsAndSlots(app);

    // Create icons
    mIconDisabled = HbIcon(":/gfx/disabled.svg");
    mIconEnabled = HbIcon(":/gfx/enabled.svg");

    // update timed profiling text according to settings
    updateTimedProfilingText();
}

// ---------------------------------------------------------------------------


void PIMainView::connectSignalsAndSlots(HbApplication* app)
{

    // Menu items:
    connect(mActionExit, SIGNAL(triggered()), app, SLOT( quit() ));
    connect(mActionSettings, SIGNAL(triggered()), this, SLOT( openSettingsView() ));
    connect(mActionAbout, SIGNAL(triggered()), this, SLOT( showAboutPopup() ));
    connect(mActionStartProfiling, SIGNAL(triggered()), this, SLOT( startProfiling() ));
    connect(mActionStartTimedProfiling, SIGNAL(triggered()), this, SLOT( startTimedProfiling() ));

    // List widget actions
    connect(mListPlugins, SIGNAL(activated(HbListWidgetItem*)), this,
        SLOT(enableOrDisablePlugin(HbListWidgetItem*)));
    connect(mListPlugins, SIGNAL(longPressed(HbListWidgetItem*, QPointF)), this,
        SLOT(showContexMenu(HbListWidgetItem*, QPointF)));

    connect(
        mEngine,
        SIGNAL(profilingStatusChanged(ProfilerEngineStatus, const QString&, ProfilingMode, ProfilingOutput)),
        this,
        SLOT(profilingStatusChanged(ProfilerEngineStatus, const QString&, ProfilingMode, ProfilingOutput)));

    connect(mMainWindow, SIGNAL(returnedFromSettings()), this, SLOT( updateTimedProfilingText() ));
    connect(mActionExitAndLeaveProfilingOn, SIGNAL(triggered()), this,
        SLOT( exitAndLeaveProfilingOn() ));
    connect(mActionStopProfiling, SIGNAL(triggered()), this, SLOT( stopProfiling() ));

}

// ---------------------------------------------------------------------------


void PIMainView::loadItemsFromResources(HbDocumentLoader &loader)
{

    // ACTIONS:

    // Exit
    QObject *object = loader.findObject(ACTIONEXIT);
    Q_ASSERT_X((object != 0), APPLICATIONNAME, INVALIDCONFIGURATIONFILE);
    this->mActionExit = qobject_cast<HbAction *> (object);

    // About
    object = loader.findObject(ACTIONABOUT);
    Q_ASSERT_X((object != 0), APPLICATIONNAME, INVALIDCONFIGURATIONFILE);
    this->mActionAbout = qobject_cast<HbAction *> (object);

    // Settings
    object = loader.findObject(ACTIONSETTINGS);
    Q_ASSERT_X((object != 0), APPLICATIONNAME, INVALIDCONFIGURATIONFILE);
    this->mActionSettings = qobject_cast<HbAction *> (object);

    // Start profiling
    object = loader.findObject(ACTIONSTARTPROFILING);
    Q_ASSERT_X((object != 0), APPLICATIONNAME, INVALIDCONFIGURATIONFILE);
    this->mActionStartProfiling = qobject_cast<HbAction *> (object);

    // Start timed profiling
    object = loader.findObject(ACTIONSTARTTIMEDPROFILING);
    Q_ASSERT_X((object != 0), APPLICATIONNAME, INVALIDCONFIGURATIONFILE);
    this->mActionStartTimedProfiling = qobject_cast<HbAction *> (object);

    QGraphicsWidget *widget = loader.findWidget(LISTWIDGETPLUGINS);
    Q_ASSERT_X((widget != 0), "Launcher", "Invalid launcher.xml file");
    this->mListPlugins = qobject_cast<HbListWidget *> (widget);
}

// ---------------------------------------------------------------------------

void PIMainView::showAboutPopup()
{

    Notifications::showMessageBox("Version 2.2.1 - 10th August 2010. \n"
            "Copyright © 2010 Nokia Corporation "
            "and/or its subsidiary(-ies). "
            "All rights reserved.");

}
// ---------------------------------------------------------------------------

void PIMainView::setPluginList(QList<PluginAttributes> *pluginList)
{

    this->mPluginList = pluginList;
    this->updatePlugInLists();

}

// ---------------------------------------------------------------------------

void PIMainView::openSettingsView()
{
    mMainWindow->activateSettingsView();
}

// ---------------------------------------------------------------------------

void PIMainView::updatePlugInLists()
{

    mListPlugins->clear();

    // Create Compulsory and Optional labels into list widget:
    HbListWidgetItem *compulsory = new HbListWidgetItem();
    compulsory->setText(TEXTMANDATORYPLUGINS);
    mListPlugins->addItem(compulsory);

    HbListWidgetItem *optional = new HbListWidgetItem();
    optional->setText(TEXTOPTIONALPLUGINS);
    mListPlugins->addItem(optional);

    HbIcon *icon;

    // Go thru pluginlist and add list item for each plug-in into list widget 
    for (int index = 0; index < mPluginList->size(); index++) {
        QString plugInName = mPluginList->at(index).mName;

        // get icon for item 
        if (mPluginList->at(index).mEnabled) {
            icon = &mIconEnabled;
        }
        else {
            icon = &mIconDisabled;
        }

        // Create list widget item
        // TODO ensure that this does not leak memory
        HbListWidgetItem *newItem = new HbListWidgetItem();
        newItem->setIcon(*icon);
        newItem->setText(plugInName);
        newItem->setData(mPluginList->at(index).mUid);

        // check if plug-in is compulsory or optional
        if (mPluginList->at(index).mIsHidden) {
            mListPlugins->insertItem(mListPlugins->row(optional), newItem);
        }
        else {
            mListPlugins->addItem(newItem);

        }
    }

}
// ---------------------------------------------------------------------------

void PIMainView::enableOrDisablePlugin(HbListWidgetItem *widget)
{

    // if widget is not found, set currently highlighted item as widget
    // (in that case this method is called from context menu)
    if (widget == 0) {
        widget = mListPlugins->currentItem();
    }

    // if this method is called from list widget's activated signal, do nothing
    // if context menu is open.
    else {
        if (mContextMenu != 0) {
            return;
        }
    }
    bool ok = false;
    int widgetUid = widget->data().toInt(&ok);
    if (!ok) {
        qWarning("PIProfiler: Unable to find plug-in uid");
        return;
    }

    // go thru plug-in array and find checked plug-in
    for (int index = 0; index < mPluginList->size(); index++) {

        if (mPluginList->at(index).mUid == widgetUid) {
            PluginAttributes pluginAttributes = mPluginList->at(index);

            // if plug-in is hidden it cannot be disabled
            if (pluginAttributes.mIsHidden) {
                return;
            }

            // change enable status and set icon correct
            if (pluginAttributes.mEnabled) {
                pluginAttributes.mEnabled = false;
                widget->setIcon(mIconDisabled);
            }
            else {
                pluginAttributes.mEnabled = true;
                widget->setIcon(mIconEnabled);
            }

            // save plug-in settings
            if (mEngine->savePluginSettings(pluginAttributes)) {
                mPluginList->replace(index, pluginAttributes);
            }
            else {
                // if saving operation failed, revert icon back
                if (pluginAttributes.mEnabled) {
                    widget->setIcon(mIconEnabled);
                }
                else {
                    pluginAttributes.mEnabled = true;
                    widget->setIcon(mIconDisabled);
                }
                Notifications::showErrorNote(
                    "Error occured while saving settings to Profiler engine.");
            }
            break;
        }

    }

}

// ---------------------------------------------------------------------------

void PIMainView::showContexMenu(HbListWidgetItem *widget, const QPointF &point)
{

    bool ok = false;
    int uid = widget->data().toInt(&ok);

    if (!ok) {
        return;
    }

    for (int index = 0; index < mPluginList->size(); index++) {

        if (mPluginList->at(index).mUid == uid) {

            // Create Context menu:
            mContextMenu = new HbMenu();
            mContextMenu->setPreferredPos(point, HbPopup::TopLeftCorner);
            mContextMenu->setDismissPolicy(HbPopup::TapOutside);
            mContextMenu->setAttribute(Qt::WA_DeleteOnClose);

            // Enable/disable plug-in
            if (!mPluginList->at(index).mIsHidden) {
                HbAction *enableAction;
                if (mPluginList->at(index).mEnabled) {
                    enableAction = mContextMenu->addAction(QString("Disable"));
                }
                else {
                    enableAction = mContextMenu->addAction(QString("Enable"));
                }
                connect(enableAction, SIGNAL(triggered()), this, SLOT( enableOrDisablePlugin() ));
            }

            //FIXME take these into use once expanding and collapsing data form groups works. 
            // Plug-in specific settings 
            /*if (mPluginList->at(index).mItemCount != 0 || mPluginList->at(index).mSampleRate != -1) {
             HbAction *pluginSettingsAction = mContextMenu->addAction(QString(
             "Plug-in settings"));
             connect(pluginSettingsAction, SIGNAL(triggered()), this,
             SLOT(openPluginSpecificSettings()));
             }*/

            // Plug in info
            HbAction *infoAction = mContextMenu->addAction(QString("Plug-in info"));
            connect(infoAction, SIGNAL(triggered()), this, SLOT( openSampleInfo() ));

            connect(mContextMenu, SIGNAL(aboutToClose()), this, SLOT( contextMenuClosed() ));
            mContextMenu->open(this, SLOT(contextMenuClosed()));
            break;
        }
    }

}

// ---------------------------------------------------------------------------

void PIMainView::openSampleInfo()
{

    // Show plug-in specific info
    bool ok = false;
    int uid = mListPlugins->currentItem()->data().toInt(&ok);
    if (!ok) {
        return;
    }
    Notifications::showInformationNote(this->getPluginAttributes(uid).mDescription);
}
// ---------------------------------------------------------------------------

PluginAttributes PIMainView::getPluginAttributes(int uid)
{
    // Get plug-in attributes from the plug-in list based on uid number
    for (int index = 0; index < mPluginList->size(); index++) {
        if (mPluginList->at(index).mUid == uid) {
            return mPluginList->at(index);
        }
    }
    return PluginAttributes();
}

// ---------------------------------------------------------------------------

void PIMainView::contextMenuClosed()
{
    mContextMenu = 0;
}

// ---------------------------------------------------------------------------

void PIMainView::startProfiling()
{

    showProfilingNote();
    if (!mEngine->startProfiling()) {
        Notifications::showErrorNote("Unable to start profiling");
        mProfilingNote->close();
        mProfilingNote = 0;
    }
}

// ---------------------------------------------------------------------------
void PIMainView::startTimedProfiling()
{
    showProfilingNote();
    if (!mEngine->startTimedProfiling()) {
        Notifications::showErrorNote("Unable to start profiling");
        mProfilingNote->close();
        mProfilingNote = 0;
    }
}

// ---------------------------------------------------------------------------


void PIMainView::stopProfiling()
{
    mProfilingNote = 0;
    mEngine->stopProfiling();
}

// ---------------------------------------------------------------------------

void PIMainView::profilingStatusChanged(ProfilerEngineStatus status, const QString &text,
    ProfilingMode profilingMode, ProfilingOutput outputMode)
{

    if (status == PI_FINISHED_SUCCEFULLY) {
        // Profiling finished successfully

        // close profiling note
        if (mProfilingNote != 0) {
            mProfilingNote->close();
            mProfilingNote = 0;
        }

        // Show info text
        Notifications::showInformationNote(text);
        return;
    }

    if (mProfilingNote == 0) {
        // if profiling note is not yet shown, create it
        showProfilingNote();
    }

    if (status == PI_ERROR) {
        // if errors occured, close profiling note and show error message
        if (mProfilingNote != 0) {
            mProfilingNote->close();
            mProfilingNote = 0;
        }
        Notifications::showErrorNote(text);
    }

    else if (status == PI_PROFILING) {
        // if profiling is on-going, update info text according to output:

        HbLabel *titleLabel = new HbLabel("Profiling...", mProfilingNote);
        titleLabel->setAlignment(Qt::AlignCenter);

        HbTextItem *textItem = new HbTextItem(text, mProfilingNote);
        textItem->setAlignment(Qt::AlignCenter);

        mProfilingNote->setHeadingWidget(titleLabel);
        mProfilingNote->setContentWidget(textItem);
    }

}

// ---------------------------------------------------------------------------

void PIMainView::showProfilingNote()
{
    // Create new dialog 
    mProfilingNote = new HbDialog();
    mProfilingNote->setAttribute(Qt::WA_DeleteOnClose);

    // Make it modal.
    mProfilingNote->setModal(true);

    // set dismiss policy
    mProfilingNote->setDismissPolicy(HbPopup::NoDismiss);
    mProfilingNote->setTimeout(0); //
    mProfilingNote->setBackgroundFaded(true);

    // create label and text item for popup
    HbTextItem *titleLabel = new HbTextItem("Profiling...", mProfilingNote);
    HbTextItem *label = new HbTextItem("Starting to profile", mProfilingNote);

    label->setAlignment(Qt::AlignCenter);
    titleLabel->setAlignment(Qt::AlignCenter);
    mProfilingNote->setHeadingWidget(titleLabel);
    mProfilingNote->setContentWidget(label);

    // set actions
    mProfilingNote->setPrimaryAction(mActionStopProfiling);
    mProfilingNote->setSecondaryAction(mActionExitAndLeaveProfilingOn);

    mProfilingNote->open();
}

// ---------------------------------------------------------------------------

void PIMainView::updateTimedProfilingText()
{
    QString text = "Start timed profiling(";
    QString number;
    number.setNum(mEngine->getTimeLimit(), 10);
    text.append(number);
    text.append("s)");
    mActionStartTimedProfiling->setText(text);

}

// ---------------------------------------------------------------------------

void PIMainView::exitAndLeaveProfilingOn()
{
    mEngine->leaveProfilingOnAfterClosing();
    application->quit();
}
// ---------------------------------------------------------------------------

void PIMainView::openPluginSpecificSettings()
{
    HbListWidgetItem *listItem = mListPlugins->currentItem();
    bool ok = false;
    int uid = listItem->data().toInt(&ok);
    if (ok) {
        mMainWindow->activateSettingsView(uid);
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
