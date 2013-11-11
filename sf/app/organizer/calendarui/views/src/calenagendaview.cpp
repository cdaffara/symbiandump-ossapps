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
* Description:  CalenAgendaView implementation.
*
*/

// System includes
#include <QGraphicsSceneEvent>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbpangesture.h>
#include <hbswipegesture.h>
#include <hbapplication.h> // hbapplication


// User includes
#include "calenagendaview.h"
#include "calendocloader.h"
#include "calenagendaviewwidget.h"
#include "calenservices.h"
#include "calencommon.h"
#include "calencontext.h"
#include "calendateutils.h"
#include "calenconstants.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenagendaviewTraces.h"
#endif


// ----------------------------------------------------------------------------
// CalenAgendaView::CalenAgendaView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenAgendaView::CalenAgendaView(MCalenServices &services):
CalenNativeView(services),
mSoftKeyAction(NULL),
mGoToTodayAction(NULL),
mSwitchToDayViewAction(NULL),
mActionTaken(false),
mIsAboutToQuitEventConnected(false),
mDayViewActivated(false)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_CALENAGENDAVIEW_ENTRY );
    
    // No implementation yet
    grabGesture(Qt::SwipeGesture);
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_CALENAGENDAVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::~CalenAgendaView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenAgendaView::~CalenAgendaView()
{
    OstTraceFunctionEntry0( DUP1_CALENAGENDAVIEW_CALENAGENDAVIEW_ENTRY );
    
    // No implementation yet
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEW_CALENAGENDAVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::setupView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::setupView(CalenDocLoader *docLoader)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_SETUPVIEW_ENTRY );
    
    if (!docLoader) {
        // Nothing can be done. Simply return
        OstTraceFunctionExit0( CALENAGENDAVIEW_SETUPVIEW_EXIT );
        return;
    }
    // Store the document loader for reference later
	mDocLoader = docLoader;
	
	// Listen to orientation change events
	connect(&(mServices.MainWindow()), SIGNAL(orientationChanged(Qt::Orientation)),
	        this, SLOT(orientationChanged(Qt::Orientation)));
	
	// Get the pointer to the content widget
	mAgendaViewWidget = qobject_cast<CalenAgendaViewWidget*>(mDocLoader->findWidget(CALEN_AGENDAVIEW_WIDGET));
	if (!mAgendaViewWidget) {
	    qFatal("calenagendaview.cpp : Unable to find the content widget");
	}
	mAgendaViewWidget->setupWidget(this);
	
	// Initialize all the menu and toolbar actions
	setupActions();
	
    //add "show lunar data" action item ,if regional plugin is present
    //regional plugin will add the option itself and handles it accordingly
    //use this api after adding all action item to the menu
    //so that plugin add the "Show lunar data" item as a second last option 
    // in all views
   mServices.OfferMenu(menu());
	
    // clean up any previous versions of this activity, if any, i.e. activityName, 
	// Ignore return value, first boot would always return False. bool declared 
	// only for debugging purpose.
	bool ok = removeActivity();
	OstTraceFunctionExit0( DUP1_CALENAGENDAVIEW_SETUPVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::doPopulation
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::doPopulation()
    {
    OstTraceFunctionEntry0( CALENAGENDAVIEW_DOPOPULATION_ENTRY );
    mDayViewActivated = false;
    // The content widget has not been constructed. Don't do anything
    if (!mAgendaViewWidget) {
        OstTraceFunctionExit0( CALENAGENDAVIEW_DOPOPULATION_EXIT );
        return;
    }
    // Get the day for which this view is being shown from the context
    mDate = mServices.Context().focusDateAndTime();
    
    // Check if the current day being shown is "Today"
    if (mGoToTodayAction) {
        if (mDate.date() == CalenDateUtils::today().date()) {
            // Hide the "Go to today" option
            mGoToTodayAction->setVisible(false);
        } else {
            mGoToTodayAction->setVisible(true);
        }
    }
    
    // Set self as the current view
    // mServices.MainWindow().setCurrentView(this);
    
    // Dont override the soft key behavior if day view or agenda view is the first view
    if (ECalenMonthView == mServices.getFirstView()) {
		mSoftKeyAction = new HbAction(Hb::BackNaviAction);
		setNavigationAction(mSoftKeyAction);
		// Connect to the signal triggered by clicking on back button.
		connect(mSoftKeyAction, SIGNAL(triggered()), this,
		        SLOT(launchMonthView()));
	}
    
    // Initialize the content widget
    mAgendaViewWidget->showWidget();
    
	//set Current Activity as day view
    mActivityId = ECalenAgendaView;

	// connect to receive a call back on Day View exit. Call back would result in saveActivity 
	// to be called in Native View
    if (!mIsAboutToQuitEventConnected) // check if already not connected
        {
        connect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
		mIsAboutToQuitEventConnected = true;
        }


    // Population is complete, issue a notification
    populationComplete();
    captureScreenshot(true);
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEW_DOPOPULATION_EXIT );
    }

/*!
 Funtion to refresh the current view upon selecting a date
 from GoToDate popup
 */
void CalenAgendaView::refreshViewOnGoToDate()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_REFRESHVIEWONGOTODATE_ENTRY );
    
	// Get the day for which this view is being shown from the context
	mDate = mServices.Context().focusDateAndTime();
	
	// Check if the current day being shown is "Today"
	if (mGoToTodayAction) {
		if (mDate.date() == CalenDateUtils::today().date()) {
			// Hide the "Go to today" option
			mGoToTodayAction->setVisible(false);
		} else {
			mGoToTodayAction->setVisible(true);
		}
	}
	
	// Initialize the content widget
	mAgendaViewWidget->showWidget();
	
	OstTraceFunctionExit0( CALENAGENDAVIEW_REFRESHVIEWONGOTODATE_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::docLoader
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenDocLoader* CalenAgendaView::docLoader()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_DOCLOADER_ENTRY );
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_DOCLOADER_EXIT );
    return mDocLoader;
}

/*
	Function to listen for gestures
*/
void CalenAgendaView::gestureEvent(QGestureEvent *event)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_GESTUREEVENT_ENTRY );
    
    if(HbSwipeGesture *gesture = qobject_cast<HbSwipeGesture *>(event->gesture(Qt::SwipeGesture))) {
        if (gesture->state() == Qt::GestureStarted) {
            if(QSwipeGesture::Left == gesture->sceneHorizontalDirection()) {
                // Check if we can swipe
                if (checkIfWeCanSwipe(mDate, false)) {
                    mServices.IssueCommandL(ECalenShowNextDay);
                }
                event->accept(Qt::SwipeGesture);
            } else if(QSwipeGesture::Right == gesture->sceneHorizontalDirection()) {
                // Check if we can swipe
                if (checkIfWeCanSwipe(mDate, true)) {
                    mServices.IssueCommandL(ECalenShowPrevDay);
                }
               event->accept(Qt::SwipeGesture);
            }
        }
    } 
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_GESTUREEVENT_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::createToolBar
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaView::setupActions()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_SETUPACTIONS_ENTRY );
    
	// Get the actions associated with this view
	HbAction *newEventAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_NEW_EVENT));
	if (!newEventAction) {
	    qFatal("calenagendaview.cpp : Unable to find new event action");
	}
	// Connect to the signal triggered by new event action
	connect(newEventAction, SIGNAL(triggered()), this, SLOT(createNewEvent()));
	    
	mGoToTodayAction = qobject_cast<HbAction *>
                        (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_GO_TO_TODAY));
	if (!mGoToTodayAction) {
	    qFatal("calenagendaview.cpp : Unable to find go to today action");
	}
	// Connect to the signal triggered by go to Today action
	connect(mGoToTodayAction, SIGNAL(triggered()), mAgendaViewWidget, SLOT(goToToday()));
	
	HbAction *goToDateAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_GO_TO_DATE));
	if (!goToDateAction) {
	    qFatal("calenagendaview.cpp : Unable to find go to date action");
	}
	// Connect to the signal triggered by go to Date action
	connect(goToDateAction, SIGNAL(triggered()), this, SLOT(goToDate()));
	
	mSwitchToDayViewAction = qobject_cast<HbAction *>
	                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_SWITCH_TO_DAYVIEW));
	if (!mSwitchToDayViewAction) {
		    qFatal("calenagendaview.cpp : Unable to find go to date action");
		}
	// Connect to the signal triggered by switch to dayview action
	connect(mSwitchToDayViewAction, SIGNAL(triggered()), this, SLOT(launchDayView()));
	
	HbAction *settingsAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_SETTINGS));
	if (!settingsAction) {
	    qFatal("calenagendaview.cpp : Unable to find settings action");
	}
	// Connect to the signal triggered by settings action
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(launchSettingsView()));
	
	// Close the menu once closeDialogs() is received
	connect(this, SIGNAL(closeDialogs()), menu(), SLOT(close()));
	OstTraceFunctionExit0( CALENAGENDAVIEW_SETUPACTIONS_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::createNewEvent
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::createNewEvent()
{
 
    mAgendaViewWidget->createNewEvent();
}

// ----------------------------------------------------------------------------
// CalenAgendaView::onLocaleChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::onLocaleChanged(int reason)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_ONLOCALECHANGED_ENTRY );
    
    Q_UNUSED(reason)
    // Notify the content widget about the change
    if(mAgendaViewWidget) {
    mAgendaViewWidget->handleLocaleChange();
    }
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_ONLOCALECHANGED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::orientationChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::orientationChanged(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_ORIENTATIONCHANGED_ENTRY );
    
    // Notify the content widget about the change
    if (mAgendaViewWidget) {
    mAgendaViewWidget->orientationChanged(orientation);
    }
    OstTraceFunctionExit0( CALENAGENDAVIEW_ORIENTATIONCHANGED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::launchMonthView
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::launchMonthView()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_LAUNCHMONTHVIEW_ENTRY );
    
    // Issue the command to launch the month view
    mServices.IssueCommandL(ECalenMonthView);
	// month view launched now, disconnect to get the call backs for saveActivity 
	// on aboutToQuit signal
    disconnectAboutToQuitEvent();
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_LAUNCHMONTHVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaView::launchDayView
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::launchDayView()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEW_LAUNCHDAYVIEW_ENTRY );
    mDayViewActivated = true;
    // Issue the command to launch the day view
    mServices.IssueCommandL(ECalenDayView);
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_LAUNCHDAYVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// clears the list model 
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::clearListModel()
    {
    OstTraceFunctionEntry0( CALENAGENDAVIEW_CLEARLISTMODEL_ENTRY );
    // do not disconnect if day view is open
    // save the agenda view as an activity
    if(!mDayViewActivated){
	// day view is removed from the list disconnect for aboutToQuit events
    disconnectAboutToQuitEvent();
    }
    mAgendaViewWidget->clearListModel();
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_CLEARLISTMODEL_EXIT );
    }

// ----------------------------------------------------------------------------
// disconnectAboutToQuitEvent disconnects for the aboutToQuit events
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::disconnectAboutToQuitEvent()
    {
    OstTraceFunctionEntry0( CALENAGENDAVIEW_DISCONNECTABOUTTOQUITEVENT_ENTRY );
    
    if (mIsAboutToQuitEventConnected)
        {
        disconnect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
        mIsAboutToQuitEventConnected = false;
        }
    
    OstTraceFunctionExit0( CALENAGENDAVIEW_DISCONNECTABOUTTOQUITEVENT_EXIT );
    }

// End of file	--Don't remove this.
