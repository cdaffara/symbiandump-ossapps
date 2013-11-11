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


#ifndef PIMAINVIEW_H
#define PIMAINVIEW_H

#include <hbview.h>
#include <hbicon.h>

#include "pluginattributes.h"
#include "piprofilerengine.h"

class HbAction;
class HbMainWindow;
class HbDocumentLoader;
class HbApplication;
class HbLabel;
class HbWidget;
class HbListWidget;
class HbListWidgetItem;
class HbMenu;
class PISettingsView;
class PIMainWindow;
class HbDialog;

class PIMainView: public HbView
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    PIMainView();

    /**
     * Destructor
     */
    ~PIMainView();

    /**
     * Initializes view
     */
    void init(PIMainWindow *mainWindow, HbApplication *app, HbDocumentLoader &loader,
        PIProfilerEngine *engine);

    /**
     * Sets and updates new plug-in list into main view
     * @param pluginList List of plug-ins
     */
    void setPluginList(QList<PluginAttributes> *pluginList);

private:

    /**
     * connects signals and slots
     */
    void connectSignalsAndSlots(HbApplication* app);

    /**
     * Loads all UI items from xml file
     */
    void loadItemsFromResources(HbDocumentLoader &loader);

    /**
     * Updates plug-in list according to member variable mPluginList
     */
    void updatePlugInLists();

    /**
     * Gets plug-in attributes for plug-in which UID number is defined in parameter
     * If plugin for UID if not found, empty pluginattributes variable is returned
     */
    PluginAttributes getPluginAttributes(int uid);

    /**
     * Shows profiling note
     */
    void showProfilingNote();

private slots:

    /**
     * Slot that is called when profiling status is changed.
     * Method updated text in popup that is shown in the screen
     */
    void profilingStatusChanged(ProfilerEngineStatus status, const QString &text = 0,
        ProfilingMode profilingMode = PI_PROFILINGMODENORMAL, ProfilingOutput output =
            PI_FILE_OUTPUT);

    /**
     * shows about popup
     */
    void showAboutPopup();

    /**
     * opens settings view
     */
    void openSettingsView();

    /**
     * Enables or disables plug-in from plug-in list. If parameter is zero, selected 
     * plug-in from plug-in list is enabled/disabled.
     */
    void enableOrDisablePlugin(HbListWidgetItem *widget = 0);

    /**
     * Shows context menu for list widget
     */
    void showContexMenu(HbListWidgetItem *widget, const QPointF &point);

    /**
     * Opens sampler info for plug-in that is selected.
     */
    void openSampleInfo();

    /**
     * sets context menu pointer to zero
     */

    void contextMenuClosed();

    /**
     * Shows profiling-popup and starts profiling
     */
    void startProfiling();

    /**
     * Shows profiling popup and starts profiling
     */
    void startTimedProfiling();

    /**
     * Stops profiling
     */
    void stopProfiling();

    /**
     * Updates "Start timed profiling for xx seconds" text according to settings
     */
    void updateTimedProfilingText();

    /**
     * Exits and leaves profiling on
     */
    void exitAndLeaveProfilingOn();

    /**
     * Opens plug-in specific settings for selected plug-in
     */
    void openPluginSpecificSettings();
private:

    // main application
    HbApplication *application;

    // main window
    PIMainWindow *mMainWindow;

    // engine pointer
    PIProfilerEngine *mEngine;

    // list of plug-ins
    QList<PluginAttributes> *mPluginList;

    // context menu
    HbMenu *mContextMenu;

    // Actions
    HbAction *mActionExit;
    HbAction *mActionExitAndLeaveProfilingOn;
    HbAction *mActionAbout;
    HbAction *mActionStartProfiling;
    HbAction *mActionStartTimedProfiling;
    HbAction *mActionSettings;
    HbAction *mActionStopProfiling;

    // plug-in list widget
    HbListWidget *mListPlugins;

    // Profiling note
    HbDialog *mProfilingNote;

    // Icons for enabled and disabled plug-ins
    HbIcon mIconEnabled;
    HbIcon mIconDisabled;

};

#endif // PIMAINVIEW_H
