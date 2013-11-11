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
#include <hblistwidget.h>
#include <qstandarditemmodel.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <qlist.h>

#include "applicationview.h"
#include "launchermainwindow.h"
#include "enginewrapper.h"
#include "notifications.h"
#include "commonActions.h"
#include "outputview.h"




// xml definition of view
const char *APPLICATIONVIEW_XML = ":/xml/applicationview.xml";

// name of the application view object in the xml file.
const char *APPLICATIONVIEW = "applicationview";
const char *LISTVIEW = "applicationList";
const char *ACTIONREFRESH = "actionRefresh";
const char *ACTIONLAUNCHAPPS = "actionLaunchApps";
const char *ACTIONLAUNCHANDCLOSEAPPS = "actionLaunchAppsAndClose";
const char *ACTIONSELECT = "actionSelectAll";
const char *ACTIONUNSELECT = "actionUnselectAll";
const char *ACTIONOPENOUTPUTVIEW = "actionOpenOutputView";
const char *ACTIONSKIPHIDDEN = "actionSkipHidden";
const char *ACTIONDONTSKIPHIDDEN = "actionDontSkipHidden";

// ---------------------------------------------------------------------------

ApplicationView* ApplicationView::create(HbDocumentLoader &loader, CommonActions *commonActions, EngineWrapper *engine)
{
    
    // Load xml-file
    bool ok = false;
    qDebug("QTLauncher::load xml");
    loader.load(APPLICATIONVIEW_XML, &ok);
    
    // Load Application View:
    QGraphicsWidget *widget = loader.findWidget(APPLICATIONVIEW);
    Q_ASSERT_X(ok && (widget != 0), "Launcher", "Invalid launcher.xml file");
    ApplicationView *applicationView = qobject_cast<ApplicationView *>(widget);
      
    // Set enginewrappers pointer to applicationview and initialize it
    applicationView->setEngine(engine);
    applicationView->init(loader, commonActions);
    
    return applicationView;    
}

// ---------------------------------------------------------------------------

ApplicationView::ApplicationView()  
    :   mEngine(0),
        mActionRefresh(0),
        mActionLaunchApps(0),
        mActionLaunchAndCloseApps(0),
        mActionSkipHidden(0),
        mActionDontSkipHidden(0),
        mActionSelectAll(0),
        mActionUnselectAll(0),
        mActionOpenOutputView(0)
{
}

// ---------------------------------------------------------------------------

void ApplicationView::setEngine(EngineWrapper *engine)
{
    mEngine = engine;
}

// ---------------------------------------------------------------------------

ApplicationView::~ApplicationView()
{
    if(mActionRefresh)
        mActionRefresh->deleteLater();
    if(mActionLaunchApps)
        mActionLaunchApps->deleteLater();
    if(mActionLaunchAndCloseApps)
        mActionLaunchAndCloseApps->deleteLater();
    if(mActionSkipHidden)
        mActionSkipHidden->deleteLater();
    if(mActionDontSkipHidden)
        mActionDontSkipHidden->deleteLater();
    if(mActionSelectAll)
        mActionSelectAll->deleteLater();
    if(mActionUnselectAll)
        mActionUnselectAll->deleteLater();
    if(mActionOpenOutputView)
        mActionOpenOutputView->deleteLater();
}

// ---------------------------------------------------------------------------

void ApplicationView::init(HbDocumentLoader &loader, CommonActions *commonActions)
{
    // Load items from xml file
    loadItemsFromXml(loader);
    
    // Set actions checkable and select "Skip hidden & embed"
    mActionSkipHidden->setCheckable(true);
    mActionSkipHidden->setChecked(true);
    mActionDontSkipHidden->setCheckable(true);
    
    commonActions->addActionsToMenu(menu());
    
    // Set list widget settings
    mApplicationList->setClampingStyle(HbScrollArea::BounceBackClamping);
    mApplicationList->setSelectionMode(HbAbstractItemView::MultiSelection);
    
    // Refresh application list
    refreshAppList();
    
    // Connect signals and slots
    connectSignalsAndSlots();
   
}

// ---------------------------------------------------------------------------

void ApplicationView::refreshAppList()
{
    // Clear list

    mApplicationList->clear();
    QStringList apps;

    // show wait dialog
    QObject *dialog =  static_cast<QObject*>(Notifications::showWaitDialog("Initializing"));

    // Get application list from engine
    if(!mEngine->listOfAllApps(apps)) {
        Notifications::showErrorNote("Unable to refresh list");
    }
    
    // Format values to list widget
    int i = 0;
    while(i < apps.size()) {
        mApplicationList->addItem(/*HbIcon(":/gfx/check.svg"),*/apps.at(i));
        i++;
    }
    mApplicationList->selectAll();
    
    // remove the wait dialog
    delete dialog;
}

// ---------------------------------------------------------------------------

void ApplicationView::connectSignalsAndSlots()
{
    // Actions
    connect(mActionRefresh, SIGNAL(triggered()), this, SLOT(refreshAppList()));
    connect(mActionLaunchApps, SIGNAL(triggered()), this, SLOT(launchSelectedApplications()));
    connect(mActionLaunchAndCloseApps, SIGNAL(triggered()), this, SLOT(launchAndCloseSelectedApplications()));
    connect(mActionSelectAll, SIGNAL(triggered()), this, SLOT(selectAllItems()));
    connect(mActionUnselectAll, SIGNAL(triggered()), this, SLOT(unselectAllItems()));
    connect(mActionSkipHidden, SIGNAL(triggered()), this, SLOT(launchOptionsSkipHidden()));
    connect(mActionDontSkipHidden, SIGNAL(triggered()), this, SLOT(launchOptionsDontSkipHidden()));
    connect(mActionOpenOutputView, SIGNAL(triggered()), this, SLOT(openOutputView()));
}

// ---------------------------------------------------------------------------


void ApplicationView::loadItemsFromXml(HbDocumentLoader &loader)
{
    
    // List widget:
    QGraphicsWidget *widget = loader.findWidget(LISTVIEW);
    Q_ASSERT_X((widget != 0), "Launcher", "Invalid launcher.xml file");
    mApplicationList = qobject_cast<HbListWidget *>(widget);
    
    /* ACTIONS: */
    //refresh
    QObject *object= loader.findObject(ACTIONREFRESH);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionRefresh = qobject_cast<HbAction *>(object);
    
    // launch applications
    object= loader.findObject(ACTIONLAUNCHAPPS);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionLaunchApps = qobject_cast<HbAction *>(object);

    // launch and close
    object= loader.findObject(ACTIONLAUNCHANDCLOSEAPPS);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionLaunchAndCloseApps = qobject_cast<HbAction *>(object);
    
    // select all applications
    object= loader.findObject(ACTIONSELECT);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionSelectAll = qobject_cast<HbAction *>(object);
    
    // unselect all applications
    object= loader.findObject(ACTIONUNSELECT);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionUnselectAll = qobject_cast<HbAction *>(object);
    
    // open output view
    object= loader.findObject(ACTIONOPENOUTPUTVIEW);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionOpenOutputView = qobject_cast<HbAction *>(object);
    
    // skip hidden
    object= loader.findObject(ACTIONSKIPHIDDEN);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionSkipHidden = qobject_cast<HbAction *>(object);
    
    // do not skip hidden
    object= loader.findObject(ACTIONDONTSKIPHIDDEN);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionDontSkipHidden = qobject_cast<HbAction *>(object);
    
}

// ---------------------------------------------------------------------------

void ApplicationView::launchSelectedApplications()
{
    QModelIndexList list = mApplicationList->selectionModel()->selectedIndexes();
    mEngine->startAppLaunching(list, false);
}

// ---------------------------------------------------------------------------

void ApplicationView::launchAndCloseSelectedApplications()
{
    QModelIndexList list = mApplicationList->selectionModel()->selectedIndexes();
    mEngine->startAppLaunching(list, true);
}

// ---------------------------------------------------------------------------

void ApplicationView::selectAllItems()
{
    mApplicationList->selectAll();
}

// ---------------------------------------------------------------------------

void ApplicationView::unselectAllItems()
{
    mApplicationList->clearSelection();
}
// ---------------------------------------------------------------------------

void ApplicationView::launchOptionsSkipHidden()
{
    mActionSkipHidden->setChecked(true);
    mActionDontSkipHidden->setChecked(false);
    mEngine->setSkipHiddenAndEmbedOnly(true);
}

// ---------------------------------------------------------------------------

void ApplicationView::launchOptionsDontSkipHidden()
{
    mActionSkipHidden->setChecked(false);
    mActionDontSkipHidden->setChecked(true);
    mEngine->setSkipHiddenAndEmbedOnly(false);
}

// ---------------------------------------------------------------------------

void ApplicationView::openOutputView()
{
    qDebug("OpenOutputView");
    LauncherMainWindow *pMainWindow = static_cast< LauncherMainWindow * >( mainWindow() );
    if( pMainWindow )
        {
        pMainWindow->openOutputView();
        }
}

// ---------------------------------------------------------------------------
