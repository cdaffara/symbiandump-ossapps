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
* Description:  This class is the base class of all Calendar views.
 *
 */

// system includes
#include <QtGui>
#include <hbwidget.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbdialog.h>
#include <hbmainwindow.h>
#include <hbdatetimepicker.h>
#include <vwsdef.h>
#include <hbactivitymanager.h> //Activity Manager
#include <hbapplication.h> //hbapplication
#include <afactivitystorage.h>

//user includes
#include <CalenUid.h>
#include "calennativeview.h"
#include "calenservices.h"
#include "calencontext.h"
#include "calensettingsview.h"
#include "calendateutils.h"
#include "calenconstants.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calennativeviewTraces.h"
#endif


/*!
 \class CalenNativeView
 Base class for all calendar views
 */

/*!
 Default constructor.
 */
CalenNativeView::CalenNativeView(MCalenServices &services) :
	mServices(services),
	mEntriesInDataBase(false),
	mIsCapturedScreenShotValid(false),
	mDatePicker(0)
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_CALENNATIVEVIEW_ENTRY );
    
	setTitle(hbTrId("txt_calendar_title_calendar"));
	 //initialize the activity 
	mActivityStorage = new AfActivityStorage(this);
	// Create services API and register for notifications
	RArray<TCalenNotification> notificationArray;
	CleanupClosePushL(notificationArray);

	notificationArray.Append(ECalenNotifySystemTimeChanged);
	notificationArray.Append(ECalenNotifySystemLocaleChanged);
	notificationArray.Append(ECalenNotifyContextChanged);
	notificationArray.Append(ECalenNotifyCloseDialogs);

	mServices.RegisterForNotificationsL(this, notificationArray);

	CleanupStack::PopAndDestroy(&notificationArray);
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_CALENNATIVEVIEW_EXIT );
}

/*!
 Destructor
 */
CalenNativeView::~CalenNativeView()
{
    OstTraceFunctionEntry0( DUP1_CALENNATIVEVIEW_CALENNATIVEVIEW_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CALENNATIVEVIEW_CALENNATIVEVIEW_EXIT );
}

/*!
 Issues populcaiton complete to the framework
 */
void CalenNativeView::populationComplete()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_POPULATIONCOMPLETE_ENTRY );
    
	// Population is complete, issue the notification
	mServices.IssueNotificationL(ECalenNotifyViewPopulationComplete);
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_POPULATIONCOMPLETE_EXIT );
}

/*!
 Slot for delete before date
 */
void CalenNativeView::deleteBeforeDate()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_DELETEBEFOREDATE_ENTRY );
    
	mServices.IssueCommandL(ECalenDeleteEntriesBeforeDate);
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_DELETEBEFOREDATE_EXIT );
}

/*!
 Slot for delete all entries
 */
void CalenNativeView::deleteAllEntries()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_DELETEALLENTRIES_ENTRY );
    
	mServices.IssueCommandL(ECalenDeleteAllEntries);
	mEntriesInDataBase = false;
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_DELETEALLENTRIES_EXIT );
}

/*!
 Refreshes the already open date picker with proper 
 date format as per current locale settings.
 */
 void CalenNativeView::refreshDatePicker()
{
	if(!(mDatePicker.isNull())) {
		mDatePicker->setDisplayFormat(CalenDateUtils::dateFormatString());
		mDatePicker->setDate(QDate::currentDate());
	}
}

/*!
 Slot to handle gotodate
 */
void CalenNativeView::goToDate()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_GOTODATE_ENTRY );
    
	// Create a popup with datepicker for the user to select date.
	HbDialog *popUp = new HbDialog();
	popUp->setParent(this);
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	popUp->setHeadingWidget(new HbLabel(hbTrId("txt_calendar_opt_go_to_date")));
	
	if(mDatePicker) {
		mDatePicker = NULL;
	}
	mDatePicker = new HbDateTimePicker(QDate::currentDate(), popUp);
	// Set the date range.
	mDatePicker->setMinimumDate(CalenDateUtils::minTime().date());
	mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());
	mDatePicker->setDisplayFormat(CalenDateUtils::dateFormatString());
	mDatePicker->setDate(QDate::currentDate());
	
	popUp->setContentWidget(mDatePicker);
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(goToSelectedDate()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popUp));
	
	// Close the popup once closeDialogs() is received
	connect(this, SIGNAL(closeDialogs()), popUp, SLOT(close()));
	
	popUp->open();
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_GOTODATE_EXIT );
}

/*
 Slot to handle date selected on gotodate popup
 */
void CalenNativeView::goToSelectedDate()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_GOTOSELECTEDDATE_ENTRY );
    
	QDate selectedDate = mDatePicker->date();

	// Check if the selected date is within the range.
	if (selectedDate.isValid() &&
	        selectedDate >= CalenDateUtils::minTime().date() &&
	        selectedDate <= CalenDateUtils::maxTime().date()) {
		MCalenContext& context = mServices.Context();
		QDateTime contextDate = context.focusDateAndTime();

		//Set the selected date to contextDate.
		contextDate.setDate(selectedDate);
		context.setFocusDateAndTime(contextDate);
	}
	refreshViewOnGoToDate();
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_GOTOSELECTEDDATE_EXIT );
}

/*!
 Virtual function to refresh the current view upon selecting a date
 from GoToDate popup
 */
void CalenNativeView::refreshViewOnGoToDate()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_REFRESHVIEWONGOTODATE_ENTRY );
    
	mServices.IssueCommandL(ECalenStartActiveStep);
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_REFRESHVIEWONGOTODATE_EXIT );
}

/*!
 Slot to handle setting item in options menu
 */
void CalenNativeView::launchSettingsView()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_LAUNCHSETTINGSVIEW_ENTRY );
    
	mServices.IssueCommandL(ECalenShowSettings);
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_LAUNCHSETTINGSVIEW_EXIT );
}

/*!
 Slot to handle to orientation change
 */
void CalenNativeView::changeOrientation(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_CHANGEORIENTATION_ENTRY );
    
    Q_UNUSED(orientation);
    // Nothing, derived classes will implement it
    OstTraceFunctionExit0( CALENNATIVEVIEW_CHANGEORIENTATION_EXIT );
}

/*!
 Handles the interested notifications from the calendar framework
 */
void CalenNativeView::HandleNotification(const TCalenNotification notification)
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_HANDLENOTIFICATION_ENTRY );
    
	switch (notification) {
		case ECalenNotifySystemLocaleChanged: {
			refreshDatePicker();
			onLocaleChanged(EChangesLocale);
		}
		break;
		case ECalenNotifySystemTimeChanged: {
			onLocaleChanged(EChangesSystemTime);
		}
		break;
		case ECalenNotifyContextChanged: {
			onContextChanged();
		}
		break;
		case ECalenNotifyCloseDialogs: {
			// Emit the signal to close the dialogs which are already opened
			emit closeDialogs();
		}
		break;
		default:
			break;
	}
	
	OstTraceFunctionExit0( CALENNATIVEVIEW_HANDLENOTIFICATION_EXIT );
}

/*!
 Returns true if plugin is loaded
 */
TBool CalenNativeView::pluginEnabled()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_PLUGINENABLED_ENTRY );
    
	QString *pluginInfo = mServices.InfobarTextL();
	if (!pluginInfo) {
		OstTraceFunctionExit0( CALENNATIVEVIEW_PLUGINENABLED_EXIT );
		return false;
	} else {
		OstTraceFunctionExit0( DUP1_CALENNATIVEVIEW_PLUGINENABLED_EXIT );
		return true;
	}
}

QString *CalenNativeView::pluginText()
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_PLUGINTEXT_ENTRY );
    
    OstTraceFunctionExit0( CALENNATIVEVIEW_PLUGINTEXT_EXIT );
	return mServices.InfobarTextL();
	
}

// ----------------------------------------------------------------------------
// captureScreenshot caltures screen shot for the given viewId
// @param viewId view for which screenshot needs to be captured
// ----------------------------------------------------------------------------
// 
void CalenNativeView::captureScreenshot(bool captureScreenShot)
    {
    OstTraceFunctionEntry0( CALENNATIVEVIEW_CAPTURESCREENSHOT_ENTRY );
    
    // get a screenshot for saving to the activity manager. It's done for once
    // to optimize the performance
    if (captureScreenShot) {
        mScreenShotMetadata.clear(); // remove any screenshot captured earlier
        mScreenShotMetadata.insert("screenshot", QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
        }
    mIsCapturedScreenShotValid = captureScreenShot; // set the validity of the screenshot captured
    
    OstTraceFunctionExit0( CALENNATIVEVIEW_CAPTURESCREENSHOT_EXIT );
    }

// ----------------------------------------------------------------------------
// saveActivity saves the activity for current view
// ----------------------------------------------------------------------------
// 
void CalenNativeView::saveActivity()
 {
   OstTraceFunctionEntry0( CALENNATIVEVIEW_SAVEACTIVITY_ENTRY );
   // check if alerady a valid screen shot is captured
   if (!mIsCapturedScreenShotValid) {
       mScreenShotMetadata.clear(); // remove any screenshot captured earlier
       mScreenShotMetadata.insert("screenshot", QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
       }
   
   // Save any data necessary to save the state
   QByteArray serializedActivity;
   QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
   stream << mActivityId;
 
   bool ok(false);
   // Save activity
   ok = mActivityStorage->saveActivity(activityName,serializedActivity,mScreenShotMetadata);
    // Check is activity saved sucessfully
   if ( !ok )  {
       qFatal("Add failed" ); // Panic is activity is not saved successfully
       }
   OstTraceFunctionExit0( CALENNATIVEVIEW_SAVEACTIVITY_EXIT );
 }

/*!
 Function to tell if we can perform swipe effect on the preview pane.
 This will affect when the next preview pane that is not going to 
 come has valid date or not
 */
bool CalenNativeView::checkIfWeCanSwipe(QDateTime& date, bool rightGesture)
{
    OstTraceFunctionEntry0( CALENNATIVEVIEW_CHECKIFWECANSWIPE_ENTRY );
    
    bool value;
    // For right gesture, see if previous day is valid or not
    if (rightGesture) {
        value = CalenDateUtils::isValidDay(date.addDays(-1));
    } else { // For left gesture, see if next day is valid or not
        value = CalenDateUtils::isValidDay(date.addDays(1));
    }
    
    OstTraceFunctionExit0( CALENNATIVEVIEW_CHECKIFWECANSWIPE_EXIT );
    
    return value;
}

/*!
 Function to remove the activity 
 */
bool CalenNativeView::removeActivity()
    {
    OstTraceFunctionEntry0( CALENNATIVEVIEW_REMOVEACTIVITY_ENTRY );
    OstTraceFunctionEntry0( CALENNATIVEVIEW_REMOVEACTIVITY_EXIT );
    return mActivityStorage->removeActivity(activityName);
    }
//End Of File
