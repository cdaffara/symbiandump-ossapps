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

#ifndef PIPROFILERMAINWINDOW_H_
#define PIPROFILERMAINWINDOW_H_

// System includes
#include <qobject.h>
#include <hbmainwindow.h>

// User includes
#include "pluginattributes.h"

// Forward declarations
class PIMainView;
class PISettingsView;
class PIProfilerEngine;
class HbDocumentLoader;
class HbApplication;

// Class declaration
class PIMainWindow: public HbMainWindow
{
Q_OBJECT
public:

    // Member functions

    PIMainWindow(PIProfilerEngine* engine, HbDocumentLoader &loader, HbApplication *app);
    virtual ~PIMainWindow();

signals:

    /**
     * Signal that is emitted when back button is pressed at settings view
     */
    void returnedFromSettings();

public slots:

    /**
     * Activates main view
     */
    void activateMainView();

    /**
     * Activates settings view
     */
    void activateSettingsView(int uid = 0);

    /**
     * method that is called when back button pressed at settings view
     */
    void settingsViewClosed();

    /**
     * Sets and new plugin list to all views
     */
    void setPluginList(QList<PluginAttributes> &pluginList);

private:

    /**
     * creates and adds main view to window
     */
    void addMainView(HbDocumentLoader &loader, HbApplication *app);

    /**
     * creates and adds settings view to main window
     */
    void addSettingsView();

private slots:

private:
    // data

    // Engine wrapper
    PIProfilerEngine* mEngine;

    // Main view
    PIMainView* mMainView;

    // Settings view
    PISettingsView* mSettingsView;

    // Back Action
    HbAction *mBackAction;

    // plug-in attributes
    QList<PluginAttributes> mPluginAttributes;

};

#endif /* PIPROFILERMAINWINDOW_H_ */
