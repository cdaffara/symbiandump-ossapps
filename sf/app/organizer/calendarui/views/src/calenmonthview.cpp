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
 * Description:  CalenMonthView implementation.
 *
 */

//system includes
#include <QtGui>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbframeitem.h>
#include <hbcolorscheme.h>
#include <hbtheme.h>
#include <agendautil.h>
#include <agendaentry.h>
#include <hbapplication.h> // hbapplication


//user includes
#include "calenmonthview.h"
#include "calenservices.h"
#include "calennativeview.h"
#include "calenview.h"
#include "calenmonthdata.h"
#include "calencontext.h"
#include "calenagendautils.h"
#include "CalenUid.h"
#include "hb_calencommands.hrh" 
#include "calendateutils.h"
#include "calenmonthgrid.h"
#include "calenpreviewpane.h"
#include "calendocloader.h"
#include "calenthicklinesdrawer.h"
#include "calencommon.h"
#include "calendarprivatecrkeys.h"
#include "calenpluginlabel.h"
#include "calenconstants.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenmonthviewTraces.h"
#endif

// CONSTANTS
#define WEEKNUMWIDTH 6.41604 // in units, need to update this value if it 
								// is changed in the docml
/*!
 \class CalenMonthView

 Class implementing calendar month view
 */

/*!
 Default constructor.
 */
CalenMonthView::CalenMonthView(MCalenServices &services) :
	CalenNativeView(services), mAgendaUtil(services.agendaInterface()), 
	mGoToTodayAction(0),
	mPrevRegionalInfo(0),
	mCurrRegionalInfo(0),
	mNextRegionalInfo(0),
	mIsAboutToQuitEventConnected(false)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_CALENMONTHVIEW_ENTRY );
    
	mIsWeekNumbersShown = 0;
	mOrientation = mServices.MainWindow().orientation();
	// Read the date from the context
	mDate = mServices.Context().focusDateAndTime();

	// Create the settings manager instance and settings key for week number
	mSettingsManager = new XQSettingsManager(this);
	mWeekNumberCenrepKey
	        = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
	                            KCRUidCalendar, KCalendarShowWeekNum);

	mLocale = HbExtendedLocale::system();
	mFirstWeekLabel = NULL;
	mIsPrevPaneGesture = false;
	// Get the week day and preview heading color from the theme
	mWeekDaysColor = HbColorScheme::color("qtc_cal_week_day");
	
	mPreviewHeadingColor = HbColorScheme::color("qtc_cal_day_preview_heading");
	
    connect(HbTheme::instance(), SIGNAL(changed()),
                this, SLOT(handleThemeChange()));
    
	OstTraceFunctionExit0( CALENMONTHVIEW_CALENMONTHVIEW_EXIT );
}

/*!
 Destructor.
 */
CalenMonthView::~CalenMonthView()
{
    OstTraceFunctionEntry0( DUP1_CALENMONTHVIEW_CALENMONTHVIEW_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CALENMONTHVIEW_CALENMONTHVIEW_EXIT );
}

/*!
 Called by the CalenViewManager after loading the view from the docml.
 The initializaion/setup of the view is done here. 
 */
void CalenMonthView::setupView(CalenDocLoader *docLoader)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SETUPVIEW_ENTRY );
    
	mDocLoader = docLoader;
	mTitleLabel
	        = qobject_cast<HbLabel *> (
								   mDocLoader->findWidget(CALEN_MONTH_TITLE));
	// Set the title text color
	QColor monthTitleColor = HbColorScheme::color("qtc_cal_monthgrid_title");
	if (mTitleLabel && monthTitleColor.isValid()) {
		mTitleLabel->setTextColor(monthTitleColor);
	}
		
	mDayNameWidget
	        = qobject_cast<CalenThickLinesDrawer *> (
													mDocLoader->findWidget(
													CALEN_DAYNAMES_WIDGET));

	// Get all the seven day labels
	mFirstDayLabel
	        = qobject_cast<HbLabel *> (
						mDocLoader->findWidget(CALEN_MONTVIEW_FIRST_DAY_LABEL));
	mSecondDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_SECOND_DAY_LABEL));
	mThirdDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_THIRD_DAY_LABEL));
	mFourthDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_FOURTH_DAY_LABEL));
	mFifthDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_FIFTH_DAY_LABEL));
	mSixthDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_SIXTH_DAY_LABEL));
	mSeventhDayLabel
	        = qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_SEVENTH_DAY_LABEL));
	
	// Get the weeknumber widget
	mWeekNumberWidget
			= qobject_cast<CalenThickLinesDrawer *> (
							 mDocLoader->findWidget(CALEN_WEEKNUMBERS_WIDGET));
	// Disable the item so that line seperator doesnt gets drawn by default
	mWeekNumberWidget->setEnabled(false);

	// Get the month grid
	mMonthGrid
	        = qobject_cast<CalenMonthGrid *> (
									mDocLoader->findWidget(CALEN_MONTHGRID));
	mCurrPaneParent = qobject_cast<HbWidget *> (
			mDocLoader->findWidget(CALEN_CURRPREVIEWPARENT));
	// Get current day preview pane widget
	mCurrPreviewPane
			= static_cast<CalenPreviewPane *> (
					docLoader->findWidget(CALEN_CURRPREVIEWPANE));
	//mCurrPreviewPane->setParent(mCurrPaneParent);
	mCurrPaneParent->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
	HbLabel* currPaneNoEntriesLabel = static_cast<HbLabel *> (
			docLoader->findWidget(CALEN_NOENTRIES_LABEL_CURR));
	mCurrPreviewPane->setView(this);
	mCurrPreviewPane->setNoEntriesLabel(currPaneNoEntriesLabel);
	
    connect(
            HbTheme::instance(), SIGNAL(changed()),
            mCurrPreviewPane, SLOT(handleThemeChange()));
	
	mCurrPaneLayoutWidget = qobject_cast<HbWidget*>(docLoader->findWidget(CALEN_CURRPANELAYOUT));
	mCurrPaneLayout = static_cast<QGraphicsLinearLayout*>(mCurrPaneLayoutWidget->layout());
	
	mPrevPaneParent = qobject_cast<HbWidget *> (
			mDocLoader->findWidget(CALEN_PREVPREVIEWPARENT));
	// Get previous day preview pane widget
	mPrevPreviewPane
			= static_cast<CalenPreviewPane *> (
					docLoader->findWidget(CALEN_PREVPREVIEWPANE));
	//mPrevPreviewPane->setParent(mPrevPaneParent);
	mPrevPaneParent->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
	HbLabel* prevPaneNoEntriesLabel = static_cast<HbLabel *> (
			docLoader->findWidget(CALEN_NOENTRIES_LABEL_PREV));
	mPrevPreviewPane->setView(this);
	mPrevPreviewPane->setNoEntriesLabel(prevPaneNoEntriesLabel);
    connect(
            HbTheme::instance(), SIGNAL(changed()),
            mPrevPreviewPane, SLOT(handleThemeChange()));
	mPrevPaneParent->setVisible(false);
	
	mPrevPaneLayoutWidget = qobject_cast<HbWidget*>(docLoader->findWidget(CALEN_PREVPANELAYOUT));
	mPrevPaneLayout = static_cast<QGraphicsLinearLayout*>(mPrevPaneLayoutWidget->layout());
	mNextPaneParent = qobject_cast<HbWidget *> (
			mDocLoader->findWidget(CALEN_NEXTPREVIEWPARENT));
	// Get next day preview pane widget
	mNextPreviewPane
			= static_cast<CalenPreviewPane *> (
					docLoader->findWidget(CALEN_NEXTPREVIEWPANE));
	//mNextPreviewPane->setParent(mNextPaneParent);
	mNextPaneParent->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
	HbLabel* nextPaneNoEntriesLabel = static_cast<HbLabel *> (
			docLoader->findWidget(CALEN_NOENTRIES_LABEL_NEXT));
	mNextPreviewPane->setView(this);
	mNextPreviewPane->setNoEntriesLabel(nextPaneNoEntriesLabel);
    connect(
            HbTheme::instance(), SIGNAL(changed()),
            mNextPreviewPane, SLOT(handleThemeChange()));
    
	mNextPaneParent->setVisible(false);
	mNextPaneLayoutWidget = qobject_cast<HbWidget*>(docLoader->findWidget(CALEN_NEXTPANELAYOUT));
	mNextPaneLayout = static_cast<QGraphicsLinearLayout*>(mNextPaneLayoutWidget->layout());
	
	mMonthGridPlusWeekNumWidget
	        = qobject_cast<HbWidget *> (
					mDocLoader->findWidget(CALEN_MONTHGRID_WEEKNUMBERS_WIDGET));

	mMonthGrid->setView(this);

	
	
	mIsFirstTimeLoad = true;

	// clean up any previous versions of this activity, if any, i.e. activityName
	// Ignore return value, first boot would always return False. bool declared 
	// only for debugging purpose.
	bool ok = removeActivity();

	OstTraceFunctionExit0( CALENMONTHVIEW_SETUPVIEW_EXIT );
}

/*!
 Constructs the remaining part of the month view that was kept as 
 part if lazy loading
 */
void CalenMonthView::doLazyLoading()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_DOLAZYLOADING_ENTRY );
	// Add background items to all the widgets
	addBackgroundFrame();
	
	// Construct and add the previous month and next month items to the view
	mMonthGrid->updateMonthGridWithInActiveMonths(mMonthDataArray);
	
	// Check if regional information needs to be shown
	// and add it or remove it
	showHideRegionalInformation();
	
	// Connect to the menu actions
	HbAction
			*newEventAction =
					qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_NEWEVENT));

	connect(newEventAction, SIGNAL(triggered()), this, SLOT(createEditor()));

	mGoToTodayAction =
					qobject_cast<HbAction *> (
							mDocLoader->findObject(CALEN_MONTVIEW_MENU_GOTOTODAY));
	connect(mGoToTodayAction, SIGNAL(triggered()), this, SLOT(goToToday()));
	
	HbAction
			*goToDateAction =
					qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_GOTODATE));

	connect(goToDateAction, SIGNAL(triggered()), this, SLOT(goToDate()));

	// TODO: Need to move this to docml
	// Set the title to the submenu
	mDeleteSubMenu =
	                qobject_cast<HbMenu *> (
						mDocLoader->findObject(CALEN_MONTVIEW_DELETE_SUBMENU));
	mDeleteSubMenu->setTitle(hbTrId("txt_calendar_opt_delete_entries"));

	HbAction
	        *beforeDateAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_BEFOREDATE));

	connect(beforeDateAction, SIGNAL(triggered()), this,
	        SLOT(deleteBeforeDate()));

	HbAction
	        *allEntriesAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_ALLENTRIES));

	connect(allEntriesAction, SIGNAL(triggered()), this,
	        SLOT(deleteAllEntries()));

	HbAction
	        *settingsAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_SETTINGS));

	connect(settingsAction, SIGNAL(triggered()), this,
	        SLOT(launchSettingsView()));


	// Connect to the signal of change orientation.
	connect(&(mServices.MainWindow()),
	        SIGNAL(orientationChanged(Qt::Orientation)), this,
	        SLOT(changeOrientation(Qt::Orientation)));
	
	// Connect to the signal when options menu is shown
	// This is required to add/remove dynamically some options
	connect(menu(), SIGNAL(aboutToShow ()), this,
										SLOT(addRemoveActionsInMenu()));
	// Close the menu once closeDialogs() is received
	connect(this, SIGNAL(closeDialogs()), menu(), SLOT(close()));
		
	//add "show lunar data" action item ,if regional plugin is present
	//regional plugin will add the option itself and handles it accordingly
	//use this api after adding all action item to the menu
	//so that plugin add the "Show lunar data" item as a second last option 
	// in all views
	mServices.OfferMenu(menu());
	
	OstTraceFunctionExit0( CALENMONTHVIEW_DOLAZYLOADING_EXIT );
}

/*!
 Adds the week numbers to the weeknumber widget
 */
void CalenMonthView::addWeekNumbers()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ADDWEEKNUMBERS_ENTRY );
    
	if (!mFirstWeekLabel) {
	// Get all the six week labels
	mFirstWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_FIRST_WEEK_LABEL));
	mSecondWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_SECOND_WEEK_LABEL));
	mThirdWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_THIRD_WEEK_LABEL));
	mFourthWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_FOURTH_WEEK_LABEL));
	mFifthWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_FIFTH_WEEK_LABEL));
	mSixthWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_SIXTH_WEEK_LABEL));
	}
	
	// Set the text colors well before instead of setting it again and again
	mFirstWeekLabel->setTextColor(mWeekDaysColor);
	mSecondWeekLabel->setTextColor(mWeekDaysColor);
	mThirdWeekLabel->setTextColor(mWeekDaysColor);
	mFourthWeekLabel->setTextColor(mWeekDaysColor);
	mFifthWeekLabel->setTextColor(mWeekDaysColor);
	mSixthWeekLabel->setTextColor(mWeekDaysColor);
	
	// Calculate the week numbers and set them to the week labels
	updateWeekNumGridModel();

	// Get the day names layout
	QGraphicsLinearLayout *dayNamesLayout =
	        static_cast<QGraphicsLinearLayout *> (mDayNameWidget->layout());

	if (KCalenDaysInWeek == dayNamesLayout->count()) {
		// Add one empty label in the day name grid
		HbLabel *label = new HbLabel(mDayNameWidget);

		// Set the attributes same as that of other day names to make it
		// look similar to the day names
		label->setPlainText("");
		label->setFont(mFirstDayLabel->font());
		label->setFontSpec(mFirstDayLabel->fontSpec());
		label->setAlignment(mFirstDayLabel->alignment());
		label->setElideMode(mFirstDayLabel->elideMode());
		label->setZValue(mFirstDayLabel->zValue());
		// Set the proper width to this empty label so that
		// day names are dislayed against the correct columns
		HbDeviceProfile deviceProf;
        qreal unitValue = deviceProf.unitValue();
        qreal widthInPixels = WEEKNUMWIDTH * unitValue;
        label->setPreferredWidth(widthInPixels);
        label->setMinimumWidth(widthInPixels);
        label->setMaximumWidth(widthInPixels);
        label->setContentsMargins(0,0,0,0);
		
		// Add this label into layout
		dayNamesLayout->insertItem(0, label);
		dayNamesLayout->invalidate();
		dayNamesLayout->activate();
	}

	// Now insert the weekNumber Grid
	QGraphicsLinearLayout *layout = static_cast<QGraphicsLinearLayout *> 
										(mMonthGridPlusWeekNumWidget->layout());

	if (1 == layout->count()) {
		layout->insertItem(0, mWeekNumberWidget);
		// Enable the item so that line seperator gets drawn
		mWeekNumberWidget->setEnabled(true);
		layout->invalidate();
		layout->activate();
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_ADDWEEKNUMBERS_EXIT );
}

/*!
 Removes the week numbers from the view
 */
void CalenMonthView::removeWeekNumbers()

{
    OstTraceFunctionEntry0( CALENMONTHVIEW_REMOVEWEEKNUMBERS_ENTRY );
    
	// Remove the week number grid if setting is off
	QGraphicsLinearLayout *layout = static_cast<QGraphicsLinearLayout *> 
									(mMonthGridPlusWeekNumWidget->layout());
	if (2 == layout->count()) {
		layout->removeItem(mWeekNumberWidget);

		// Disable the item as we are removing it from the layout
		mWeekNumberWidget->setEnabled(false);
		layout->invalidate();
		layout->activate();

		// Clear the week labels
		mFirstWeekLabel->clear();
		mSecondWeekLabel->clear();
		mThirdWeekLabel->clear();
		mFourthWeekLabel->clear();
		mFifthWeekLabel->clear();
		mSixthWeekLabel->clear();

		// Get the day names layout
		QGraphicsLinearLayout *dayNamesLayout =
		        static_cast<QGraphicsLinearLayout *> (mDayNameWidget->layout());
		if (dayNamesLayout->count() == KCalenDaysInWeek + 1) {
			// Remove the empty item in the day name grid
			QGraphicsLayoutItem *item = dayNamesLayout->itemAt(0);
			dayNamesLayout->removeAt(0);
			delete item;
			dayNamesLayout->invalidate();
			dayNamesLayout->activate();
		}
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_REMOVEWEEKNUMBERS_EXIT );
}

/*!
 Sets the background frame for the month view
 */
void CalenMonthView::addBackgroundFrame()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ADDBACKGROUNDFRAME_ENTRY );
    
    // Set the background items for all the widgets
    HbFrameItem* frame = NULL;
    HbFrameDrawer *drawer = NULL;
    HbWidget* monthViewExceptPreviewPane = qobject_cast<HbWidget *> (
						 mDocLoader->findWidget(CALEN_MONTHVIEW_EXCEPT_PANE));
    if (monthViewExceptPreviewPane) {
        drawer = new HbFrameDrawer("qtg_fr_cal_monthgrid_bg", HbFrameDrawer::NinePieces);
        // The grid background
        frame = new HbFrameItem(drawer, this);
        monthViewExceptPreviewPane->setBackgroundItem(frame->graphicsItem(), -2);
    }
    if (mTitleLabel) {
        // The month title
        drawer = new HbFrameDrawer("qtg_fr_cal_monthgrid_title_bg", HbFrameDrawer::ThreePiecesHorizontal);
		if (drawer)
        	frame = new HbFrameItem(drawer, this);
		if(frame)
			mTitleLabel->setBackgroundItem(frame->graphicsItem(), -2);
    }
    
    // Set the frame to the preview pane
    drawer = new HbFrameDrawer("qtg_fr_cal_preview_bg", HbFrameDrawer::NinePieces);
	if (drawer)
	    frame = new HbFrameItem(drawer, this);
	if(frame)
	    mPrevPaneLayoutWidget->setBackgroundItem(frame->graphicsItem(), -5);
    
    // Set the frame to the preview pane
    drawer = new HbFrameDrawer("qtg_fr_cal_preview_bg", HbFrameDrawer::NinePieces);
    if(drawer)
	   frame = new HbFrameItem(drawer, this);
	if(frame)
    mCurrPaneLayoutWidget->setBackgroundItem(frame->graphicsItem(), -5);
    
    // Set the frame to the preview pane
    drawer = new HbFrameDrawer("qtg_fr_cal_preview_bg", HbFrameDrawer::NinePieces);
	if(drawer)
           frame = new HbFrameItem(drawer, this);
	if(frame)
	    mNextPaneLayoutWidget->setBackgroundItem(frame->graphicsItem(), -5);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_ADDBACKGROUNDFRAME_EXIT );
}

void CalenMonthView::showHideRegionalInformation()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SHOWHIDEREGIONALINFORMATION_ENTRY );
    
	if (pluginEnabled()) {
		XQSettingsKey regionalInfo(XQSettingsKey::TargetCentralRepository,
									KCRUidCalendar, KCalendarShowRegionalInfo);

		int showRegionalInfo = 
						mSettingsManager->readItemValue(regionalInfo).toUInt();
		if (showRegionalInfo) {

			// Add the regional information to the preview panes
			if (!mPrevRegionalInfo) {
				mPrevRegionalInfo = qobject_cast<CalenPluginLabel *> 
				(mDocLoader->findWidget(CALEN_PREVREGIONALINFO));
				mPrevRegionalInfo->show();
				mPrevPaneLayout->insertItem(0, mPrevRegionalInfo);

				HbFrameItem *frameCurr = new HbFrameItem(this);
				frameCurr->frameDrawer().setFrameType(
										HbFrameDrawer::ThreePiecesHorizontal);
				frameCurr->frameDrawer().setFrameGraphicsName(
										"qtg_fr_cal_preview_title_bg");
				mPrevRegionalInfo->setBackgroundItem(
										frameCurr->graphicsItem(), -2);
			}
			if (!mCurrRegionalInfo) {
				mCurrRegionalInfo = qobject_cast<CalenPluginLabel *> 
				(mDocLoader->findWidget(CALEN_CURRREGIONALINFO));
				mCurrRegionalInfo->show();
				mCurrPaneLayout->insertItem(0, mCurrRegionalInfo);

				HbFrameItem *frameCurr = new HbFrameItem(this);
				frameCurr->frameDrawer().setFrameType(
										HbFrameDrawer::ThreePiecesHorizontal);
				frameCurr->frameDrawer().setFrameGraphicsName(
										"qtg_fr_cal_preview_title_bg");
				mCurrRegionalInfo->setBackgroundItem(
										frameCurr->graphicsItem(), -2);
			}
			if (!mNextRegionalInfo) {
				mNextRegionalInfo = qobject_cast<CalenPluginLabel *> 
				(mDocLoader->findWidget(CALEN_NEXTREGIONALINFO));
				mNextRegionalInfo->show();
				mNextPaneLayout->insertItem(0, mNextRegionalInfo);

				HbFrameItem *frameCurr = new HbFrameItem(this);
				frameCurr->frameDrawer().setFrameType(
										HbFrameDrawer::ThreePiecesHorizontal);
				frameCurr->frameDrawer().setFrameGraphicsName(
										"qtg_fr_cal_preview_title_bg");
				mNextRegionalInfo->setBackgroundItem(
										frameCurr->graphicsItem(), -2);
			}
			QString *pluginString = pluginText();
			mPrevRegionalInfo->setPlainText(*pluginString);
			mPrevRegionalInfo->setTextColor(mPreviewHeadingColor);
			mCurrRegionalInfo->setPlainText(*pluginString);
			mCurrRegionalInfo->setTextColor(mPreviewHeadingColor);
			mNextRegionalInfo->setPlainText(*pluginString);
			mNextRegionalInfo->setTextColor(mPreviewHeadingColor);
		} else {
			if (mPrevRegionalInfo) {
				mPrevPaneLayout->removeItem(mPrevRegionalInfo);
				mPrevRegionalInfo->hide();
				mPrevRegionalInfo = NULL;
			}
			if (mCurrRegionalInfo) {
				mCurrPaneLayout->removeItem(mCurrRegionalInfo);
				mCurrRegionalInfo->hide();
				mCurrRegionalInfo = NULL;
			}
			if (mNextRegionalInfo) {
				mNextPaneLayout->removeItem(mNextRegionalInfo);
				mNextRegionalInfo->hide();
				mNextRegionalInfo = NULL;
			}
		}
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_SHOWHIDEREGIONALINFORMATION_EXIT );
}

/*!
 Handles the context changed notification
 */
void CalenMonthView::onContextChanged()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ONCONTEXTCHANGED_ENTRY );
	//Update plugin label after setting context
	if (mCurrRegionalInfo && mPrevRegionalInfo && mNextPaneLayout
	        && pluginEnabled()) {
		QString *pluginString = pluginText();
		mPrevRegionalInfo->setPlainText(*pluginString);
		mCurrRegionalInfo->setPlainText(*pluginString);
		mNextRegionalInfo->setPlainText(*pluginString);
	}
	OstTraceFunctionExit0( CALENMONTHVIEW_ONCONTEXTCHANGED_EXIT );
}

/*!
 Calculates the week numbers and sets them to the week labels
 */
void CalenMonthView::updateWeekNumGridModel()

{
    OstTraceFunctionEntry0( CALENMONTHVIEW_UPDATEWEEKNUMGRIDMODEL_ENTRY );
    
	// Get the visible date
	QDateTime firstVisibleDate = mFirstDayOfGrid.addDays(KCalenDaysInWeek
	        * mNumOfRowsInPrevMonth);
	// Go to the last day so that we get the proper week number for the first
	// week of January
	mWeekNumbers.clear();
	// get the proper week numbers
	for (int i = 0; i < KNumOfVisibleRows; i++) {
		// Get the week number
		QDateTime day = firstVisibleDate.addDays((KCalenDaysInWeek * i));
		long weekNumber(day.date().weekNumber());
		mWeekNumbers.append(weekNumber);
	}

	// Update the week labels text
	QString text = QString::number(mWeekNumbers.at(0));
	mFirstWeekLabel->setPlainText(text);
	text = QString::number(mWeekNumbers.at(1));
	mSecondWeekLabel->setPlainText(text);
	text = QString::number(mWeekNumbers.at(2));
	mThirdWeekLabel->setPlainText(text);
	text = QString::number(mWeekNumbers.at(3));
	mFourthWeekLabel->setPlainText(text);
	text = QString::number(mWeekNumbers.at(4));
	mFifthWeekLabel->setPlainText(text);
	text = QString::number(mWeekNumbers.at(5));
	mSixthWeekLabel->setPlainText(text);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_UPDATEWEEKNUMGRIDMODEL_EXIT );
}

/*!
 Populates the month view with today as focussed item
 */
void CalenMonthView::goToToday()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_GOTOTODAY_ENTRY );
    
	QDateTime today = CalenDateUtils::today();
	// Set the context and repopulate the view
    MCalenContext &context = mServices.Context();
    context.setFocusDateAndTime(today);
	    
	// First check if we are not alread
	// showing today's month view
	if (mDate == today) {
		OstTraceFunctionExit0( CALENMONTHVIEW_GOTOTODAY_EXIT );
		return;
	} else if (mActiveMonth.date().year() == today.date().year() && 
				mActiveMonth.date().month() == today.date().month()) {
        mDate = today;
		// User is in current month only, so just set the focus to current
		// date grid item and refresh the preview pane
		int currIndex = mFirstDayOfGrid.daysTo(today);
		setCurrGridIndex(currIndex);
		// Populate the preview panes
		populatePreviewPane(mDate);
	} else {	
        refreshViewOnGoToDate();
	}
	
	OstTraceFunctionExit0( DUP1_CALENMONTHVIEW_GOTOTODAY_EXIT );
}

/*
 Slot to handle adding / removing actions frm the menu when menu is about to
 shown to the user
 */
void CalenMonthView::addRemoveActionsInMenu()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ADDREMOVEACTIONSINMENU_ENTRY );
	HbAction* menuAction = mDeleteSubMenu->menuAction();
	if (menuAction) {
        if (!mEntriesInDataBase && mAgendaUtil->areNoEntriesInCalendar()) {
            // hide the delete entries option
            menuAction->setVisible(false);
        } else {
            mEntriesInDataBase = true;
            // Show the option to delete
            menuAction->setVisible(true);
        }
	}
	
	// Check if we are population for current day, if yes then disable the
	// gototoday action
	if ((CalenDateUtils::today().date() == mDate.date()) && mGoToTodayAction) {
		mGoToTodayAction->setVisible(false);
	} else if (mGoToTodayAction){
		mGoToTodayAction->setVisible(true);
	}
	OstTraceFunctionExit0( CALENMONTHVIEW_ADDREMOVEACTIONSINMENU_EXIT );
}

/*!
 Populates the view
 */
void CalenMonthView::doPopulation()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_DOPOPULATION_ENTRY );
    
 	// Get the layout and add the preview pane layout.
	QGraphicsLinearLayout* viewLayout = static_cast<QGraphicsLinearLayout *>
														(widget()->layout());
	if (viewLayout->count() == 1) {
		// Count is 1 implies view has only month grid.
		// Add the preview pane at corresponding position.
		viewLayout->addItem(mCurrPaneParent);
	}
	
	// prepare for the population like reading the date frm the context 
	// calculating the start of the grid etc.,
	prepareForPopulation();
	
	// Populate the view and preview panes only if we are not opening the calendar
	if (!mIsFirstTimeLoad) {
		// fetch list of required calendar instances
		populateWithInstanceView();
		// Populate the preview panes
		populatePreviewPane(mDate);
	}
	
	
    //update the day label 
    //if changes in locale setting
	updateDayLabel();
	
	  // Create the grid items with proper dates
	createGrid();
	

	// Complete the population
	completePopulation();

	//set Currect Activity to month view
	mActivityId = ECalenMonthView;
	
	// connect to receive a call back on Month View exit. Call back would result in saveActivity 
	// to be called in Native View.
	if (!mIsAboutToQuitEventConnected) // check if not already connected
	    {
        connect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
        mIsAboutToQuitEventConnected = true;
	    }
	
		
	// Population is complete, inform it
	populationComplete();
	
	// Handle regional data here if we are not populating the month view for
	// the first time
	if (!mIsFirstTimeLoad) {
		showHideRegionalInformation();
	}
	// Reset the first time load flag
	mIsFirstTimeLoad = false;
	
	OstTraceFunctionExit0( CALENMONTHVIEW_DOPOPULATION_EXIT );
}

/*!
 Reads the date from the context and calculates the grid item dates
 */
void CalenMonthView::prepareForPopulation()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_PREPAREFORPOPULATION_ENTRY );
    
	setActiveDay(dateFromContext(mServices.Context()));
	setDate();
	updateMonthDataArrayWithActiveDates();
	
	OstTraceFunctionExit0( CALENMONTHVIEW_PREPAREFORPOPULATION_EXIT );
}

/*!
 Slot to handle gotodate action
 */
void CalenMonthView::refreshViewOnGoToDate()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_REFRESHVIEWONGOTODATE_ENTRY );
    
	prepareForPopulation();
	setDateToLabel();
	// fetch list of required calendar instances
	populateWithInstanceView();

	populatePreviewPane(mDate);
	
	mMonthGrid->updateMonthGridModel(mMonthDataArray, mIndexToBeScrolled, 
	                                 mIsFirstTimeLoad);
	// Update the week Numbers model
	if (mIsWeekNumbersShown) {
		updateWeekNumGridModel();
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_REFRESHVIEWONGOTODATE_EXIT );
}

/*!
 Reads the date from the context and stores into mDate
 */
QDateTime CalenMonthView::dateFromContext(const MCalenContext &context)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_DATEFROMCONTEXT_ENTRY );
	QDateTime ret;
	if (AgendaEntry::TypeTodo == context.instanceId().mType) {
		QDateTime today = CalenDateUtils::today();
		if (context.focusDateAndTime() < today) {
			ret = today;
		} else {
			ret = context.focusDateAndTime();
		}
	} else {
		ret = context.focusDateAndTime();
	}
	OstTraceFunctionExit0( CALENMONTHVIEW_DATEFROMCONTEXT_EXIT );
	return ret;
}

/*!
 Calculates the necesary parameters of the month view w.r.t mDate
 */
void CalenMonthView::setActiveDay(QDateTime day)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SETACTIVEDAY_ENTRY );
    
	mDate = day;
    
	mActiveMonth = mDate;
	// Get the first day of the previous month
	QDateTime prevMonthDate = mDate.addMonths(-1);
	int prevMonth = prevMonthDate.date().month();

	// Get the first day of the month
	QDate date = prevMonthDate.date();
	date.setDate(date.year(), date.month(), 1);
	QDateTime firstDayOfPrevMonth(date, day.time());

	int offset(firstDayOfPrevMonth.date().dayOfWeek() - (mLocale.startOfWeek()
	        + 1));
	if (offset < 0) {
		offset += KCalenDaysInWeek;
	}

	// Substract the offset days
	mFirstDayOfGrid = firstDayOfPrevMonth.addDays(-offset);

	// Calculate the number of weeks in the previous month that will be hidden
	mNumOfRowsInPrevMonth = offset + prevMonthDate.date().daysInMonth();
	mNumOfRowsInPrevMonth = (mNumOfRowsInPrevMonth / KCalenDaysInWeek);

	// Index to be scrolled is the last day of the visible grid item
	mIndexToBeScrolled = ((mNumOfRowsInPrevMonth + KNumOfVisibleRows)
	        * KCalenDaysInWeek) - 1;

	// Now add the future month days to the count. While doing that, we need to
	// calcualte the count till the end of the week in which the month ends
	QDateTime futureMonthDate = mDate.addMonths(1);
	// Get the first day of the future month
	date = futureMonthDate.date();
	date.setDate(date.year(), date.month(), 1);
	QDateTime firstDayOfFutMonth(date, day.time());
	offset = firstDayOfFutMonth.date().dayOfWeek() - (mLocale.startOfWeek() + 1);
	if (offset < 0) {
		offset += KCalenDaysInWeek;
	}
	// Substract the firstDayNoInWeek days
	QDateTime dateTime = firstDayOfFutMonth.addDays(-offset);

	// Need to add 42 days for dateTime so that it fits into the visible grid
	mLastDayOfGrid = dateTime.addDays(KNumOfVisibleRows * KCalenDaysInWeek - 1);

	// calculate the total numbers of dates that needs to be populated from 
	// previous month to future month
	mTotalNumOfGridItems = mFirstDayOfGrid.daysTo(mLastDayOfGrid) + 1;

	// Calculate the last visible day
	int index = (mNumOfRowsInPrevMonth + KNumOfVisibleRows) * KCalenDaysInWeek;
	QDateTime lastVisibleDate = mFirstDayOfGrid.addDays(index - 1);
	QDateTime dateTimeToCalc = mLastDayOfGrid;
	mNumOfRowsInFutureMonth = 0;
	while (dateTimeToCalc > lastVisibleDate) {
		mNumOfRowsInFutureMonth++;
		dateTimeToCalc = dateTimeToCalc.addDays(-KCalenDaysInWeek);
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_SETACTIVEDAY_EXIT );
}

/*!
 Calculates the dates that needs to be set to mMonthDataArray
 */
void CalenMonthView::setDate()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SETDATE_ENTRY );
    
	mMonthDataArray.clear();
	// Calculate the actual number of dates to be populated from previous month
	// to future month
	for (int i(0); i < mTotalNumOfGridItems; ++i) {
		QDateTime currentDay = CalenDateUtils::futureOf(mFirstDayOfGrid, i);
		CalenMonthData element(currentDay);
		mMonthDataArray.append(element);
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_SETDATE_EXIT );
}

/*!
 Returns the active day(currently focussed day)
 */
QDateTime CalenMonthView::getActiveDay()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_GETACTIVEDAY_ENTRY );
    
	OstTraceFunctionExit0( CALENMONTHVIEW_GETACTIVEDAY_EXIT );
	return mDate;
}

/*!
 Returns the array of CalenMonthData items
 */
QList<CalenMonthData>& CalenMonthView::monthDataList()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_MONTHDATALIST_ENTRY );
    
	OstTraceFunctionExit0( CALENMONTHVIEW_MONTHDATALIST_EXIT );
	return mMonthDataArray;
}

/*!
 Creates the grid and adds the week numbers depending on the week number setting
 */
void CalenMonthView::createGrid()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_CREATEGRID_ENTRY );
	// Update the month grid
	mMonthGrid->updateMonthGridModel(mMonthDataArray, mIndexToBeScrolled, 
	                                 mIsFirstTimeLoad);
    // Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    int startOfWeek = locale.startOfWeek();
    if(startOfWeek != HbExtendedLocale::Monday)
        {
        //if the start of week is other than Monday, don't show the week number
        mIsWeekNumbersShown = 0;
        }
    else
        {
        // Read the week number setting from cenrep
        QVariant value = mSettingsManager->readItemValue(*mWeekNumberCenrepKey);
        mIsWeekNumbersShown = value.toUInt();
        }
	if (mIsWeekNumbersShown) {
		// Add week numbers to week grid
		addWeekNumbers();
	} else {
		// remove the weeknumbergrid from the layout
		removeWeekNumbers();
	}
	OstTraceFunctionExit0( CALENMONTHVIEW_CREATEGRID_EXIT );
}

/*!
 Called when down gesture is performed
 Caluclates the necessary dates for the new previous month
 */
void CalenMonthView::updateModelWithPrevMonth()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_UPDATEMODELWITHPREVMONTH_ENTRY );
    
	// Get the new previous month
	QDateTime prevMonthDateTime = mActiveMonth.addMonths(-1);
	QDateTime dateTime = mFirstDayOfGrid;
	if (prevMonthDateTime.date().month() == mFirstDayOfGrid.date().month()) {
		//  Month starts right on Monday(Start of the week), 
		//hence go back to previous month
		dateTime = mFirstDayOfGrid.addMonths(-1);
	}

	int prevMonth = dateTime.date().month();
	QDateTime prevFirstDayOfGrid = mFirstDayOfGrid;
	for (int i = 0; i < KNumOfVisibleRows; i++) {
		mFirstDayOfGrid = mFirstDayOfGrid.addDays(-KCalenDaysInWeek);
		if (mFirstDayOfGrid.date().month() == prevMonth) {
			continue;
		} else {
			// Entered to another month, stop here
			break;
		}
	}
	// Readjust the mFirstDayOfGrid
	if (mFirstDayOfGrid.addDays(KCalenDaysInWeek - 1).date().month()
	        != prevMonth) {
		mFirstDayOfGrid = mFirstDayOfGrid.addDays(KCalenDaysInWeek);
	}

	// Calculate the new days to be added
	int countToBeAdded = mFirstDayOfGrid.daysTo(prevFirstDayOfGrid);

	// Remove the bottom month entries in mMonthDataArray
	int countToBeDeleted = mNumOfRowsInFutureMonth * KCalenDaysInWeek;
	int count = mMonthDataArray.count() - 1;
	for (int i = 0; i < countToBeDeleted; i++) {
		mMonthDataArray.removeAt(count--);
	}

	// Update the total number of grid items as we will be removing the month
	// at the below
	mTotalNumOfGridItems -= countToBeDeleted;

	// Prepend countToBeAdded new items to mMonthDataArray
	for (int i = 0; i < countToBeAdded; i++) {
		QDateTime currentDay = mFirstDayOfGrid.addDays(i);
		CalenMonthData element(currentDay);
		mMonthDataArray.insert(i, element);
	}
	mTotalNumOfGridItems += countToBeAdded;

	// Update the mMonthDataArray with instances if any
	populatePrevMonth();

	// Update the necessary globals
	mActiveMonth = mActiveMonth.addMonths(-1);
	mNumOfRowsInPrevMonth = (countToBeAdded / KCalenDaysInWeek);
	mLastDayOfGrid = mFirstDayOfGrid.addDays(mTotalNumOfGridItems - 1);
	// Calculate the last visible day
	int index = (mNumOfRowsInPrevMonth + KNumOfVisibleRows) * KCalenDaysInWeek;
	QDateTime lastVisibleDate = mFirstDayOfGrid.addDays(index - 1);
	QDateTime date = mLastDayOfGrid;
	mNumOfRowsInFutureMonth = 0;
	while (date > lastVisibleDate) {
		mNumOfRowsInFutureMonth++;
		date = date.addDays(-KCalenDaysInWeek);
	}
	// Update the week Numbers model
	if (mIsWeekNumbersShown) {
		updateWeekNumGridModel();
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_UPDATEMODELWITHPREVMONTH_EXIT );
}
/*!
 Called when up gesture is performed
 */
void CalenMonthView::updateModelWithFutureMonth()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_UPDATEMODELWITHFUTUREMONTH_ENTRY );
    
	// Get the new future month
	QDateTime previousLastDayOfGrid = mLastDayOfGrid;
	QDateTime futureMonthDateTime = mActiveMonth.addMonths(2);
	QDate date = futureMonthDateTime.date();
	date.setDate(date.year(), date.month(), 1);
	// Get the first day of the future month
	QDateTime firstDayOfFutMonth(date, futureMonthDateTime.time());

	int offset = firstDayOfFutMonth.date().dayOfWeek() - (mLocale.startOfWeek()
	        + 1);
	if (offset < 0) {
		offset += KCalenDaysInWeek;
	}
	// Substract the firstDayNoInWeek days
	QDateTime dateTime = firstDayOfFutMonth.addDays(-offset);
	// Need to add 42 days for dateTime
	mLastDayOfGrid = dateTime.addDays(KNumOfVisibleRows * KCalenDaysInWeek - 1);

	// Remove the first mNumOfRowsInPrevMonth rows in mMonthDataArray
	int countToBeDeleted = mNumOfRowsInPrevMonth * KCalenDaysInWeek;
	for (int i = 0; i < countToBeDeleted; i++) {
		mMonthDataArray.removeAt(0);
	}

	// Update the total number of grid items as we will be removing the month
	// at the top
	mTotalNumOfGridItems -= countToBeDeleted;

	// Calcualate the num fo days to be added
	int countToBeAdded = previousLastDayOfGrid.daysTo(mLastDayOfGrid);
	// Append countToBeAdded new items to mMonthDataArray
	for (int i = 1; i <= countToBeAdded; i++) {
		QDateTime currentDay = previousLastDayOfGrid.addDays(i);
		CalenMonthData element(currentDay);
		mMonthDataArray.append(element);
	}

	

	// Update the necessary globals
	mTotalNumOfGridItems += countToBeAdded;
	mActiveMonth = mActiveMonth.addMonths(1);
	mNumOfRowsInFutureMonth = (countToBeAdded / KCalenDaysInWeek);
	mFirstDayOfGrid = mLastDayOfGrid.addDays(-(mTotalNumOfGridItems - 1));
	// Calculate the first visible date
	int index = mTotalNumOfGridItems - ((mNumOfRowsInFutureMonth
	        + KNumOfVisibleRows) * KCalenDaysInWeek);
	QDateTime firstVisibleDate = mFirstDayOfGrid.addDays(index - 1);
	QDateTime dateTimeToCalc = mFirstDayOfGrid;
	mNumOfRowsInPrevMonth = 0;
	while (dateTimeToCalc < firstVisibleDate) {
		mNumOfRowsInPrevMonth++;
		dateTimeToCalc = dateTimeToCalc.addDays(KCalenDaysInWeek);
	}

	// Update the week Numbers model
	if (mIsWeekNumbersShown) {
		updateWeekNumGridModel();
	}
	
	// Update the mMonthDataArray with instances if any
	populateNextMonth();
		
	OstTraceFunctionExit0( CALENMONTHVIEW_UPDATEMODELWITHFUTUREMONTH_EXIT );
}

/*!
 Returns the numner of rows in previous month
 */
int CalenMonthView::rowsInPrevMonth()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ROWSINPREVMONTH_ENTRY );
    
	OstTraceFunctionExit0( CALENMONTHVIEW_ROWSINPREVMONTH_EXIT );
	return mNumOfRowsInPrevMonth;
}

/*!
 Returns the number of rows in future month
 */
int CalenMonthView::rowsInFutMonth()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ROWSINFUTMONTH_ENTRY );
    
	OstTraceFunctionExit0( CALENMONTHVIEW_ROWSINFUTMONTH_EXIT );
	return mNumOfRowsInFutureMonth;
}

/*!
 Returns current Grid index
 */
int CalenMonthView::getCurrGridIndex()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_GETCURRGRIDINDEX_ENTRY );
    
    OstTraceFunctionExit0( CALENMONTHVIEW_GETCURRGRIDINDEX_EXIT );
	return mMonthGrid->getCurrentIndex();
}

/*!
 Set the currend Index of the Grid
 */
void CalenMonthView::setCurrGridIndex(int index)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SETCURRGRIDINDEX_ENTRY );
	mIsPrevPaneGesture = true;
	mMonthGrid->setCurrentIdex(index);
	OstTraceFunctionExit0( CALENMONTHVIEW_SETCURRGRIDINDEX_EXIT );
}

/*!
 Sets the active flag to the required dates in the array
 */
void CalenMonthView::updateMonthDataArrayWithActiveDates()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_UPDATEMONTHDATAARRAYWITHACTIVEDATES_ENTRY );
    
	int activeMonth = mActiveMonth.date().month();
	int monthDataCount = mMonthDataArray.count();
	for (int i = 0; i < monthDataCount; i++) {
		if (mMonthDataArray[i].Day().date().month() == activeMonth) {
			// Set the active flag
			mMonthDataArray[i].setActive(true);
		} else {
			// Disable the active flag
			mMonthDataArray[i].setActive(false);
		}
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_UPDATEMONTHDATAARRAYWITHACTIVEDATES_EXIT );
}

/*!
 Fetches the calenda entries for a given range
 */
void CalenMonthView::getInstanceList(QList<QDate> &list,
                                     QDateTime rangeStart, QDateTime rangeEnd)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_GETINSTANCELIST_ENTRY );
    
	AgendaUtil::FilterFlags filter =
	        AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries
	                | AgendaUtil::IncludeAppointments
	                | AgendaUtil::IncludeEvents
	                | AgendaUtil::IncludeReminders
	                | AgendaUtil::IncludeIncompletedTodos);
	mAgendaUtil->markDatesWithEvents(rangeStart, rangeEnd, filter, list);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_GETINSTANCELIST_EXIT );
}

/*!
 Parses the fetched entries and sets hasEvent attribute for necessary days
 */
void CalenMonthView::populateWithInstanceView()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_POPULATEWITHINSTANCEVIEW_ENTRY );
    
	const QDateTime today(CalenDateUtils::today());
	const QDateTime gridStart(CalenDateUtils::beginningOfDay(mFirstDayOfGrid));

	// grid index for "today"
	const int todayIndex(gridStart.daysTo(today)); 

	QDateTime gridEnd(mLastDayOfGrid.date(), QTime(23, 59, 59, 0));
	
	// Get the list of dates which have events
	QList<QDate> datesWithEvents;
	getInstanceList(datesWithEvents,gridStart,gridEnd);
	
	// Parse thru the list of dates and set the required flags
	int datesEventsCount = datesWithEvents.count();
	for(int i(0); i < datesEventsCount; i++) {
		int offset = mFirstDayOfGrid.date().daysTo(datesWithEvents.at(i));
		mMonthDataArray[offset].SetHasEvents(true);
	}
	datesWithEvents.clear();
	
	if (datesEventsCount) {
        mEntriesInDataBase = true;
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_POPULATEWITHINSTANCEVIEW_EXIT );
}

/*!
 Parses the fetched entries for previous month and 
 sets hasEvent attribute for necessary days
 */
void CalenMonthView::populatePrevMonth()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_POPULATEPREVMONTH_ENTRY );
    
	const QDateTime gridStart(CalenDateUtils::beginningOfDay(mFirstDayOfGrid));
	const QDateTime today(CalenDateUtils::today());

	const int todayIndex(gridStart.daysTo(today)); // grid index for "today"
	QDateTime end = gridStart.addDays(mNumOfRowsInPrevMonth * KCalenDaysInWeek);
	QDateTime gridEnd(end.date(), QTime(23, 59, 59, 0));


	// Get the list of dates which have events
	QList<QDate> datesWithEvents;
	getInstanceList(datesWithEvents,gridStart,gridEnd);
	
	// Parse thru the list of dates and set the required flags
	int datesEventsCount = datesWithEvents.count();
	for(int i(0); i < datesEventsCount; i++) {
		int offset = mFirstDayOfGrid.date().daysTo(datesWithEvents.at(i));
		mMonthDataArray[offset].SetHasEvents(true);
	}
	datesWithEvents.clear();
	
	if (datesEventsCount) {
        mEntriesInDataBase = true;
    }
	
	OstTraceFunctionExit0( CALENMONTHVIEW_POPULATEPREVMONTH_EXIT );
}

/*!
 Parses the fetched entries for future month and 
 sets hasEvent attribute for necessary days
 */
void CalenMonthView::populateNextMonth()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_POPULATENEXTMONTH_ENTRY );
	QList<AgendaEntry> list;
	int actualIndex = mNumOfRowsInFutureMonth * KCalenDaysInWeek;
	const QDateTime gridStart(mLastDayOfGrid.addDays(-actualIndex));
	actualIndex = mMonthDataArray.count() - actualIndex -1;
	
	const QDateTime today(CalenDateUtils::today());
	const int todayIndex(gridStart.daysTo(today)); // grid index for "today"
	QDateTime gridEnd(mLastDayOfGrid.date(), QTime(23, 59, 59, 0));

	// Get the list of dates which have events
	QList<QDate> datesWithEvents;
	getInstanceList(datesWithEvents,gridStart,gridEnd);
	
	// Parse thru the list of dates and set the required flags
	int datesEventsCount = datesWithEvents.count();
	for(int i(0); i < datesEventsCount; i++) {
		int offset = mFirstDayOfGrid.date().daysTo(datesWithEvents.at(i));
		mMonthDataArray[offset].SetHasEvents(true);
	}
	datesWithEvents.clear();
	
	if (datesEventsCount) {
        mEntriesInDataBase = true;
    }
	OstTraceFunctionExit0( CALENMONTHVIEW_POPULATENEXTMONTH_EXIT );
}

/*!
	Function that gets called when instacne view is created, so that it can
	query agenda server for any entries
 */
void CalenMonthView::fetchEntriesAndUpdateModel()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_FETCHENTRIESANDUPDATEMODEL_ENTRY );
    
	// Get to know if entries are there from the agenda server
	populateWithInstanceView();
	// Update the month grid model
	mMonthGrid->updateMonthGridWithEventIndicators(mMonthDataArray);
	// Populate the preview panes
	populatePreviewPane(mDate);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_FETCHENTRIESANDUPDATEMODEL_EXIT );
}

/*!
 Populates the preview pane
 */
void CalenMonthView::populatePreviewPane(QDateTime &dateTime)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_POPULATEPREVIEWPANE_ENTRY );
    
	mPrevPreviewPane->populateLabel(dateTime.addDays(-1));
	mCurrPreviewPane->populateLabel(dateTime);
	mNextPreviewPane->populateLabel(dateTime.addDays(1));
	
	// Start the auto scroll on current preview pane
    mCurrPreviewPane->startAutoScroll();
    
    OstTraceFunctionExit0( CALENMONTHVIEW_POPULATEPREVIEWPANE_EXIT );
}

/*!
 Completes the population after setting the view as 
 current view to the main window
 */
void CalenMonthView::completePopulation()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_COMPLETEPOPULATION_ENTRY );
    
	setDateToLabel();
	
	OstTraceFunctionExit0( CALENMONTHVIEW_COMPLETEPOPULATION_EXIT );
}

/*!
 Launches the day view on tapping of the grid item
 */
void CalenMonthView::handleGridItemActivated()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_HANDLEGRIDITEMACTIVATED_ENTRY );
    
	mServices.IssueCommandL(ECalenDayView);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_HANDLEGRIDITEMACTIVATED_EXIT );
}

/*!
 Sets the context w.r.t to the active day
 */
void CalenMonthView::setContextForActiveDay(int index)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SETCONTEXTFORACTIVEDAY_ENTRY );
	QDateTime newActiveDay = mFirstDayOfGrid.addDays(index);
	// Set the context
	mServices.Context().setFocusDate(newActiveDay);
	mDate = newActiveDay;
	setDateToLabel();
	
	if(!mIsPrevPaneGesture) {
		populatePreviewPane(mDate);
	} else {
		// reset flag
		mIsPrevPaneGesture = false;
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_SETCONTEXTFORACTIVEDAY_EXIT );
}

/*!
 Slot to launch the event editor
 */
void CalenMonthView::createEditor()
{
	OstTraceFunctionEntry0( CALENMONTHVIEW_CREATEEDITOR_ENTRY );
	
    captureScreenshot(true);
	mServices.IssueCommandL(ECalenNewMeeting);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_CREATEEDITOR_EXIT );
}

/*!
 Slot to launch the Day view
 */
void CalenMonthView::launchDayView()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_LAUNCHDAYVIEW_ENTRY );
    
	mServices.IssueCommandL(ECalenDayView);
	// day view launched now, captre the screen shot of month view 
    captureScreenshot(true);
  
    OstTraceFunctionExit0( CALENMONTHVIEW_LAUNCHDAYVIEW_EXIT );
}

/*!
disconnects for the aboutToQuit events
 */
void CalenMonthView::disconnectAboutToQuitEvent()
{
	OstTraceFunctionEntry0( CALENMONTHVIEW_DISCONNECTABOUTTOQUITEVENT_ENTRY );
	
	if (mIsAboutToQuitEventConnected) {
		disconnect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
		mIsAboutToQuitEventConnected = false;
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_DISCONNECTABOUTTOQUITEVENT_EXIT );
}

/*!
 Slot to handle the change in orientation
 */
void CalenMonthView::changeOrientation(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_CHANGEORIENTATION_ENTRY );
    
		if (mOrientation != orientation) {
			// change the orientation here
			mOrientation = orientation;
			handleChangeOrientation();
		}
		
	OstTraceFunctionExit0( CALENMONTHVIEW_CHANGEORIENTATION_EXIT );
}

/*!
 Slot to handle the change in theme
 */
void CalenMonthView::handleThemeChange()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_HANDLETHEMECHANGE_ENTRY );
    
    // Refresh the month view when the theme change happens
    mWeekDaysColor = HbColorScheme::color("qtc_cal_week_day");
    QColor monthTitleColor = HbColorScheme::color("qtc_cal_monthgrid_title");
    
    if (mTitleLabel && monthTitleColor.isValid()) {
		mTitleLabel->setTextColor(monthTitleColor);
	}
    
    mPreviewHeadingColor = HbColorScheme::color("qtc_cal_day_preview_heading");
    
    mServices.IssueCommandL(ECalenStartActiveStep);
    
    OstTraceFunctionExit0( CALENMONTHVIEW_HANDLETHEMECHANGE_EXIT );
}

/*!
 Loads appropriate section from the docml dsepending on the current orientation
 of the device
 */
void CalenMonthView::handleChangeOrientation()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_HANDLECHANGEORIENTATION_ENTRY );
    
	bool loadSuccess = false;
	if (mOrientation == Qt::Horizontal) {
		mDocLoader->load(CALEN_MONTHVIEW_XML_FILE, CALEN_LANDSCAPE,
		                 &loadSuccess);

	} else if (mOrientation == Qt::Vertical) {
		mDocLoader->load(CALEN_MONTHVIEW_XML_FILE, CALEN_PORTRAIT, 
													&loadSuccess);
	}
	
	Q_ASSERT_X(loadSuccess, "calenmonthview.cpp", "Unable to load XML");
	
	// Remove the current pane read from the docml  from the layout 
	// and add actual current pane to the layout
	QGraphicsLinearLayout* viewLayout = static_cast<QGraphicsLinearLayout *>
													(widget()->layout());
	viewLayout->removeAt(1);
	// Add this item only when orientaion is changed on month view
	// if it is changed in other views, adding the preview pane here was
	// overlapping with the month grid, hence, its been added in dopopulation()
	if (this == mServices.MainWindow().currentView()) {
		viewLayout->addItem(mCurrPaneParent);
	}
	
	OstTraceFunctionExit0( CALENMONTHVIEW_HANDLECHANGEORIENTATION_EXIT );
}

/*!
 Sets the appropriate date to the month title
 */
void CalenMonthView::setDateToLabel()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_SETDATETOLABEL_ENTRY );
    
	// Get the localised string for month name from system locale
	QString dateString = mLocale.monthName(mDate.date().month(), HbExtendedLocale::LongFormat);
	// Append a single space
	dateString.append(" ");
	mLocale.setNumberOptions(QLocale::OmitGroupSeparator);
	// Append the year string also
	dateString.append(mLocale.toString(mDate.date().year()));
	mTitleLabel->setPlainText(dateString);
	
	OstTraceFunctionExit0( CALENMONTHVIEW_SETDATETOLABEL_EXIT );
}

/*!
 To handle previewpane swipe 
 */
void CalenMonthView::handlePreviewPaneGesture(bool rightGesture)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_HANDLEPREVIEWPANEGESTURE_ENTRY );
    
    // Check if we can swipe, if yes, then proceed, else, do nothing
    if (checkIfWeCanSwipe(mDate, rightGesture)) {
    
        QGraphicsLinearLayout* viewLayout = static_cast<QGraphicsLinearLayout *>
                                                    (widget()->layout());
        
        // Set the effect in progress flags for next and previous panes
        // For current pane, we would have set it in gestureEvent() function
        mPrevPreviewPane->effectStarted();
        mNextPreviewPane->effectStarted();
        
        if(rightGesture) {
            // Need to bring up the previous day preview pane
            // Create the effect on mCurrPreviewPane to slide to right side
            mPrevPaneParent->setVisible(true);
            if (mOrientation == Qt::Vertical) {
                HbEffect::add(mCurrPaneLayoutWidget,
                              ":/fxml/portrait_preview_pane_hide_on_right_gesture",
                              "hide");
                HbEffect::add(mPrevPaneLayoutWidget,
                              ":/fxml/portrait_preview_pane_show_on_right_gesture",
                              "show");
            } else {
                HbEffect::add(mCurrPaneLayoutWidget,
                              ":/fxml/landscape_preview_pane_hide_on_right_gesture",
                              "hide");
                HbEffect::add(mPrevPaneLayoutWidget,
                              ":/fxml/landscape_preview_pane_show_on_right_gesture",
                              "show");
            }
            // Start the effects
            HbEffect::start(mCurrPaneLayoutWidget, "hide");
            HbEffect::start(mPrevPaneLayoutWidget, "show", 
                                            this, "handleRightEffectCompleted");
            
            //Remove the mCurrPreviewPane from the layout and add mPrevPreviewPane
            // to the layout
            viewLayout->removeAt(1);
            viewLayout->addItem(mPrevPaneParent);
        } else {
            // Need to bring up the previous day preview pane
            // Create the effect on mCurrPreviewPane to slide to left side
            mNextPaneParent->setVisible(true);
            if (mOrientation == Qt::Vertical) {
                HbEffect::add(mCurrPaneLayoutWidget,
                              ":/fxml/portrait_preview_pane_hide_on_left_gesture",
                              "hide");
                HbEffect::add(mNextPaneLayoutWidget,
                              ":/fxml/portrait_preview_pane_show_on_left_gesture",
                              "show");
            } else {
                HbEffect::add(mCurrPaneLayoutWidget,
                              ":/fxml/landscape_preview_pane_hide_on_left_gesture",
                              "hide");
                HbEffect::add(mNextPaneLayoutWidget,
                              ":/fxml/landscape_preview_pane_show_on_left_gesture",
                              "show");
            }
            
            // Start the effects
            HbEffect::start(mCurrPaneLayoutWidget, "hide");
            HbEffect::start(mNextPaneLayoutWidget, "show", 
                                            this, "handleLeftEffectCompleted");
            
            //Remove the mCurrPreviewPane from the layout and add mNextPreviewPane
            // to the layout
            viewLayout->removeAt(1);
            viewLayout->addItem(mNextPaneParent);
        }
    }
	
	OstTraceFunctionExit0( CALENMONTHVIEW_HANDLEPREVIEWPANEGESTURE_EXIT );
}

/*!
 Returns the first date in the month grid
 */
QDateTime CalenMonthView::firstDayOfGrid()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_FIRSTDAYOFGRID_ENTRY );
    
	OstTraceFunctionExit0( CALENMONTHVIEW_FIRSTDAYOFGRID_EXIT );
	return mFirstDayOfGrid;
}

/*!
 Function to handle any locale related changes
 */
void CalenMonthView::onLocaleChanged(int reason)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_ONLOCALECHANGED_ENTRY );
	Q_UNUSED(reason);
	return;
    OstTraceFunctionExit0( CALENMONTHVIEW_ONLOCALECHANGED_EXIT );
}

/*!
 Slot to handle left gesture effects completion
 */
void CalenMonthView::handleLeftEffectCompleted(
										const HbEffect::EffectStatus &status)
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_HANDLELEFTEFFECTCOMPLETED_ENTRY );
	Q_UNUSED(status);
	
	int index = getCurrGridIndex();
	index++;
	
	// Stop the scrolling on current preview pane
	mCurrPreviewPane->stopScrolling();
	
	// Swap the preview panes properly
	CalenPreviewPane* pane = mCurrPreviewPane;
	HbWidget* paneParent = mCurrPaneParent;
	HbWidget* paneLayoutWidget = mCurrPaneLayoutWidget;
	mCurrPreviewPane = mNextPreviewPane;
	mCurrPaneParent = mNextPaneParent;
	mCurrPaneLayoutWidget = mNextPaneLayoutWidget;
	mNextPreviewPane = mPrevPreviewPane;
	mNextPaneParent = mPrevPaneParent;
	mNextPaneLayoutWidget = mPrevPaneLayoutWidget;
	mPrevPreviewPane = pane;
	mPrevPaneParent = paneParent;
	mPrevPaneLayoutWidget = paneLayoutWidget;
	
	// Reset the effect in progress flag
	mCurrPreviewPane->effectFinished();
    mPrevPreviewPane->effectFinished();
    mNextPreviewPane->effectFinished();
    
	// Set the focus to proper date
	setCurrGridIndex(index);
	// Start the auto scroll on current preview pane
	mCurrPreviewPane->startAutoScroll();
	mNextPreviewPane->populateLabel(mDate.addDays(1));
	OstTraceFunctionExit0( CALENMONTHVIEW_HANDLELEFTEFFECTCOMPLETED_EXIT );
}

/*!
 Slot to handle right gesture effects completion
 */
void CalenMonthView::handleRightEffectCompleted(
										const HbEffect::EffectStatus &status) 	
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_HANDLERIGHTEFFECTCOMPLETED_ENTRY );
    
	Q_UNUSED(status);
	
	int index = getCurrGridIndex();
	index--;
	
	// Stop the scrolling on current preview pane
	mCurrPreviewPane->stopScrolling();
		
	// Swap the preview panes properly
	CalenPreviewPane* pane = mCurrPreviewPane;
	HbWidget* paneParent = mCurrPaneParent;
	HbWidget* paneLayoutWidget = mCurrPaneLayoutWidget;
	mCurrPreviewPane = mPrevPreviewPane;
	mCurrPaneParent = mPrevPaneParent;
	mCurrPaneLayoutWidget = mPrevPaneLayoutWidget;
	mPrevPreviewPane = mNextPreviewPane;
	mPrevPaneParent = mNextPaneParent;
	mPrevPaneLayoutWidget = mNextPaneLayoutWidget;
	mNextPreviewPane = pane;
	mNextPaneParent = paneParent;
	mNextPaneLayoutWidget = paneLayoutWidget;
	
	// Reset the effect in progress flag
	mCurrPreviewPane->effectFinished();
    mNextPreviewPane->effectFinished();
    mPrevPreviewPane->effectFinished();
    
	// Set the focus to proper date
	setCurrGridIndex(index);
	// Start the auto scroll on current preview pane
	mCurrPreviewPane->startAutoScroll();
	mPrevPreviewPane->populateLabel(mDate.addDays(-1));
	
	OstTraceFunctionExit0( CALENMONTHVIEW_HANDLERIGHTEFFECTCOMPLETED_EXIT );
}
/*!
 update the Day labels 
 */
void CalenMonthView::updateDayLabel()
{
    OstTraceFunctionEntry0( CALENMONTHVIEW_UPDATEDAYLABEL_ENTRY );
    
    // Set the short day names to these labels  
    int startOfWeek = mLocale.startOfWeek();
    int weekDayIndex = startOfWeek;
    QStringList  weekDayArray ;
    weekDayArray <<hbTrId("txt_calendar_grid_day_mo")
                           <<hbTrId("txt_calendar_grid_day_tu")
                           <<hbTrId("txt_calendar_grid_day_we")
                           <<hbTrId("txt_calendar_grid_day_th")
                           <<hbTrId("txt_calendar_grid_day_fr")
                           <<hbTrId("txt_calendar_grid_day_sa")
                           <<hbTrId("txt_calendar_grid_day_su");
            
    QList<HbLabel*> labels;
    // append seven day labels 
    labels.append(mFirstDayLabel);
    labels.append(mSecondDayLabel);
    labels.append(mThirdDayLabel);
    labels.append(mFourthDayLabel);
    labels.append(mFifthDayLabel);
    labels.append(mSixthDayLabel);
    labels.append(mSeventhDayLabel);
    QGraphicsLinearLayout* layout = static_cast<QGraphicsLinearLayout*> (mDayNameWidget->layout());
    for(int i=0;i < KCalenDaysInWeek; i++ )
        {
        labels.at(i)->setPlainText(weekDayArray[weekDayIndex]);
        // Set the stretch factor as 1 so that each label occupies equal space
        layout->setStretchFactor(labels.at(i), 1);
        if(weekDayIndex == KCalenDaysInWeek - 1 )//Sunday
            {
            weekDayIndex = 0;//reset to monday
            continue;
            }
        weekDayIndex++;//increase the index for next value
        }
    labels.clear();
    weekDayArray.clear();
    
    OstTraceFunctionExit0( CALENMONTHVIEW_UPDATEDAYLABEL_EXIT );
}

// End of file  --Don't remove this.
