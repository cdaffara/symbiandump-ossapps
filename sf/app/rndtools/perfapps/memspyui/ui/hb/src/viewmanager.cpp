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
 * Description:
 *  
 */

#include <HbMainWindow>
#include <HbAction>
#include <HbMessageBox>
#include <HbLabel>

#include "viewmanager.h"

#include "enginewrapper.h"
#include "memspyview.h"
#include "memspymainview.h"
#include "memspyprocessview.h"
#include "memspythreadview.h"
#include "memspythreaddetailindexview.h"
#include "memspythreaddetailview.h"
#include "memspykernelobjecttypeview.h"
#include "memspykernelobjectview.h"
#include "memspykernelobjectdetailview.h"
#include "memspytrackingview.h"
#include "memspyswmtview.h"
#include "memspyheapdumpsview.h"
#include "memspyserverview.h"
#include "memspyecomcategoryview.h"
#include "memspyecominterfaceview.h"
#include "memspyecomimplementationview.h"
#include "memspyecomimplementationdetailview.h"
#include "memspywindowgroupsview.h"
#include "memspywindowgroupsdetailview.h"

template <typename T>
static MemSpyView* factory(EngineWrapper &engine, ViewManager &viewManager)
{
	return new T(engine, viewManager);
}
// This array needs to be in sync with view enum
MemSpyView* (*sFactories[])(EngineWrapper&, ViewManager&) = { 
	&factory<MemSpyMainView>,
	&factory<MemSpyProcessView>,
	&factory<MemSpyThreadView>,
	&factory<MemSpyThreadDetailIndexView>,
	&factory<MemSpyThreadDetailView>,
	&factory<MemSpyKernelObjectTypeView>,
	&factory<MemSpyKernelObjectView>,
	&factory<MemSpyKernelObjectDetailView>,
	&factory<MemSpyTrackingView>,
	&factory<MemSpySwmtView>,
	&factory<MemSpyHeapDumpsView>,
	&factory<MemSpyServerView>,
	&factory<MemSpyEComCategoryView>,
	&factory<MemSpyEComInterfaceView>,
	&factory<MemSpyEComImplementationView>,
	&factory<MemSpyEComImplementationDetailView>,
	&factory<MemSpyWindowGroupsView>,
	&factory<MemSpyWindowGroupsDetailView>,
};


ViewManager::ViewManager(HbMainWindow &window, EngineWrapper &engine, QObject *parent) :
	QObject(parent),
	mWindow(window),
	mEngine(engine)
{
	connect(&mWindow, SIGNAL(currentViewChanged(HbView *)), this, SLOT(viewChanged(HbView *)));
}

void ViewManager::showView(ViewIndex viewIndex, const QVariantMap &params)
{
    MemSpyView* view;
    try {
        view = sFactories[viewIndex](mEngine, *this);
        view->initialize(params);
    }
    catch (std::exception& ex)
    {
        // show message box with exception
        // TODO: qt_symbian_exception2Error shouldn't probably be here
        QString error = tr("An error occured during the operation. Error code: %1").arg(
                qt_symbian_exception2Error(ex));
        
        HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeWarning);
        messageBox->setText(error);
        HbLabel *header = new HbLabel(tr("Error"), messageBox);
        messageBox->setHeadingWidget(header);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->setTimeout(HbPopup::StandardTimeout);
        messageBox->open();
        
        delete view;
        return;
    }
    
	if (viewIndex != MainView) {
		HbAction* action = new HbAction(Hb::BackNaviAction, this);
		connect(action, SIGNAL(triggered()), this, SLOT(goBack()));
		view->setNavigationAction(action);
	}
		
	mWindow.addView(view);
	mWindow.setCurrentView(view);
}

void ViewManager::showView(ViewIndex viewIndex)
{
	showView(viewIndex, QVariantMap());
}

void ViewManager::goBack()
{
	const QList<HbView*> views = mWindow.views();
	int index = views.indexOf(mWindow.currentView());
	mWindow.setCurrentView(views.at(index-1), true, Hb::ViewSwitchUseBackAnim);
}

void ViewManager::viewChanged(HbView *view)
{
	const QList<HbView*> views = mWindow.views();
	int index = views.indexOf(view);
	for (int i=views.count()-1; i>index; i--)
		mWindow.removeView(views.at(i));
}
