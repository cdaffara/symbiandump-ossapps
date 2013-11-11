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
* Description:  Menu Application Library state.
 *
*/
#include <QScopedPointer>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbinstance.h>
#include <qstatemachine.h>
#include <hbaction.h>
#include <qpluginloader.h>

#include "hsdomainmodel_global.h"
#include "hsapp_defs.h"
#include "hsmenuevent.h"
#include "hsviewappsettingsstate.h"
#include "hsmenuentryremovedhandler.h"
#include "caentry.h"
#include "canotifier.h"
#include "canotifierfilter.h"

#include "hsmenuservice.h"

/*!
 \class HsViewAppSettingsState
 \ingroup HsViewAppSettingsState
 \brief State for showing Application Settings HbView from provided plugin
 */

/*!
 \var HsViewAppSettingsState::mView
 View.
 Owned.
 */

/*!
 \var HsViewAppSettingsState::mPreviousView
 Previous view.
 Not owned.
 */

/*!
 Constructor
 \param parent Parent state.
 \retval void
 */
HsViewAppSettingsState::HsViewAppSettingsState(QState *parent) :
        QState(parent),
        mView(0), 
        mPreviousView(0), 
        mActionConfirm(0)
{
    construct();
}

/*!
 Constructs contained objects.
 */
void HsViewAppSettingsState::construct()
{
    setObjectName("/ViewAppSettingsState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
}

/*!
 Destructor.
 */
HsViewAppSettingsState::~HsViewAppSettingsState()
{
}

/*!
 onEntry method invoked on entering the state
 \param event: event
 \retval void
 */
void HsViewAppSettingsState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppSettingsState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    
    const int entryId = data.value(Hs::itemIdKey).toInt();   
    QSharedPointer<const CaEntry> entry = CaService::instance()->getEntry(entryId);    
    
    QString pluginPath;
    pluginPath = pluginPath.append("/resource/qt/plugins/appsettings/")
        .append(entry->attribute(Hs::appSettingsPlugin)).append(".qtplugin");
    QPluginLoader loader(pluginPath);
    mView = qobject_cast<HbView *>(loader.instance()); 
    
    mActionConfirm = new HbAction(Hb::ConfirmNaviAction, mView);
    connect(mActionConfirm, SIGNAL(triggered()), SIGNAL(exit()));
    
    if (mView) {    
        QScopedPointer<HsMenuEntryRemovedHandler> entryObserver(
            new HsMenuEntryRemovedHandler(entryId, this, SIGNAL(exit())));
        
        entryObserver.take()->setParent(mView);
        // remove it when JAVA will start to provide only version with two strings
        QObject::connect(this, SIGNAL(initialize(QString, QString)), mView, SLOT(initialize(QString)));        
        QObject::connect(this, SIGNAL(initialize(QString, QString)), mView, SLOT(initialize(QString, QString)));
        mView->setParent(this);
        emit initialize(entry->attribute(Hs::applicationUidEntryKey), entry->text());        
        // Add View to main window
        HbMainWindow *hbMainWindow = mainWindow();
        // add confirm action
        mView->setNavigationAction(mActionConfirm);
    
        hbMainWindow->addView(mView);
        // record the current view in order to activate it once done
        mPreviousView = hbMainWindow->currentView();
        hbMainWindow->setCurrentView(mView);
        hbMainWindow->show();
    }

    
    HSMENUTEST_FUNC_EXIT("HsViewAppSettingsState::onEntry");
}


#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (only returns HbMainWindow)
/*!
 Returns pointer to tha main window.
 \return Pointer to the main window.
 */
HbMainWindow *HsViewAppSettingsState::mainWindow() const
{
    return HbInstance::instance()->allMainWindows().value(0);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


void HsViewAppSettingsState::onExit(QEvent *event)
{
    
    QState::onExit(event);
    // Remove mView from main window and restore previous view.
    HbMainWindow *hbMainWindow = mainWindow();
    hbMainWindow->setCurrentView(mPreviousView);
    hbMainWindow->removeView(mView);

    
    delete mActionConfirm;
    mActionConfirm = NULL;
    delete mView;
    mView = NULL;
}
