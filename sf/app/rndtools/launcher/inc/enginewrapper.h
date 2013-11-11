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

#ifndef ENGINEWRAPPER_H
#define ENGINEWRAPPER_H

#include <qstringlist.h>
#include <qabstractitemmodel.h>
#include <qlist.h>
#include <coecntrl.h>

#include "launcherengine.h"

class HbProgressDialog;
class HbDeviceProgressDialog;
class HbAction;

class LauncherMainWindow;


class EngineWrapper : public QObject, public MLauncherUI
{
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     */
    EngineWrapper(LauncherMainWindow* mainWindow);
    
    /**
     * Destructor
     */
    ~EngineWrapper();
    
    /**
     * Initializes Engine Wrapper
     * @return true if engine was started successfully
     */
    bool init();
    
public:
    // functions that are called from UI
    
    /**
     * Fetches list of applications from engine
     */
    bool listOfAllApps(QStringList &allAppsQStringList);
    
    /**
     * Starts launching applications that are given as parameter
     */
    bool startAppLaunching(const QModelIndexList &list, bool autoClose);
    
    /**
     * Sets engine to skip hidden applications when launching
     */
    void setSkipHiddenAndEmbedOnly(bool skip);
    
    /**
     * Stops launching applications
     */
    bool stopLaunching();
    
    /**
     * Sends log
     */
    bool sendLog();
    
    /**
     * Deletes log
     */
    bool deleteLog();
    
    /**
     * Compares dll
     */
    void compareDlls();
    
    /**
     * Sends list of dlls
     */
    bool sendListOfDlls();
    
public:
    // From MLauncherUIEngine
    // functions that are called from Engine
    
    /**
     * Shows error message with error text
     */
    virtual void ShowErrorMessage(const TDesC& aText);
    
    /**
     * Shows information message with info text
     */
    virtual void ShowInfoMessage(const TDesC& aText);

    /**
     * Shows progress bar with text
     */
    virtual void ShowProgressBar(const TDesC& aText, TInt aMinimum = 0, TInt aMaximum = 0 );

    /**
     * Sets progress bar value
     */
    virtual void SetProgressBarValue(TInt aValue);

    /**
     * Hides progress bar
     */
    virtual void HideProgressBar();

    /**
     * Shows wait dialog with text
     */
    virtual void ShowWaitDialog(const TDesC& aText);

    /**
     * Hides wait dialog
     */
    virtual void HideWaitDialog();

    /**
     * Changes focus to output view
     */
    virtual void ChangeFocusToOutputView();
    
    /**
     * Prints text to output views log
     */
    virtual void PrintText(const TDesC& aText);
    
public slots:
    void doCompareDlls(HbAction*);
    void CancelDllBCanalysis();
    
private:
    
    /**
     * Allocate CArrayFix and copy QList to it
     */
    CArrayFix<TInt>* QModelIndexListToCArrayFixL(const QModelIndexList& qlist);
    
private:
    CLauncherEngine *mEngine;
    LauncherMainWindow *mMainWindow;
    HbProgressDialog* mProgressDialog;
    HbDeviceProgressDialog* mWaitDialog;
};




#endif //ENGINEWRAPPER_H
