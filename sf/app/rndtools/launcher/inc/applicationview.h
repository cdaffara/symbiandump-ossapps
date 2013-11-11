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

#ifndef APPLICATIONVIEW_H
#define APPLICATIONVIEW_H

#include <hbview.h>

class HbAction;
class HbDocumentLoader;
class HbListWidget;
class CommonActions;
class EngineWrapper;

class ApplicationView : public HbView
{
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     */
    ApplicationView();
    
    /**
     * Destructor
     */
    ~ApplicationView();

    /**
     * Created Application View
     */
    static ApplicationView* create(HbDocumentLoader &loader,
                        CommonActions *commonActions, 
                        EngineWrapper *engine);
    
    /**
     * Setter for engine
     */
    void setEngine(EngineWrapper *engine);
    
private slots:

    
    /**
     * Refreshed application list
     */
    void refreshAppList();
    
    /**
     * Launches selected applications
     */
    void launchSelectedApplications();
    
    /**
     * Launches and closes selected applications
     */
    void launchAndCloseSelectedApplications();
    
    /**
     * Selects all list items
     */
    void selectAllItems();
    
    /**
     * unselects all items
     */
    void unselectAllItems();
    
    /**
     * Changes launch options so that hidden applications are skipped
     */
    void launchOptionsSkipHidden();
    
    /**
     * Changes launch options so that hidden applications are not skipped
     */
    void launchOptionsDontSkipHidden();
    
    /**
     * Opens output view
     */
    void openOutputView();


private:
    
    /**
     * Initializes view
     */
    void init(HbDocumentLoader &loader, CommonActions *commonActions);
    
    /**
     * connects signals and slots
     */
    void connectSignalsAndSlots();
    
    /**
     * Loads all UI items from xml file
     */
    void loadItemsFromXml(HbDocumentLoader &loader);
  
private slots:

    
private:
    EngineWrapper *mEngine;
    
    //UI components:
    HbListWidget *mApplicationList;
    
    //Actions:
    HbAction *mActionRefresh;
    HbAction *mActionLaunchApps;
    HbAction *mActionLaunchAndCloseApps;
    HbAction *mActionSkipHidden;
    HbAction *mActionDontSkipHidden;
    HbAction *mActionSelectAll;
    HbAction *mActionUnselectAll;
    HbAction *mActionOpenOutputView;


};

#endif // APPLICATIONVIEW_H
