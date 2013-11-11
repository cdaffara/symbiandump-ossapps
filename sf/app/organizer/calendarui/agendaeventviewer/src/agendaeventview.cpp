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

// System includes
#include <QObject>
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <QFile>
#include <QString>
#include <QTimer>
#include <QPluginLoader>
#include <QDir>
#include <HbInstance>
#include <HbMainWindow>
#include <HbView>
#include <HbMenu>
#include <HbGroupBox>
#include <HbScrollArea>
#include <HbIcon>
#include <HbAction>
#include <HbExtendedLocale>
#include <HbToolBar>
#include <hbi18ndef.h>
#include <HbRadioButtonList>
#include <HbMessageBox>
#include <HbDialog>
#include <HbLabel>
#include <HbTranslator>

// User includes
#include <maptileservice.h>//maptile service
#include <agendautil.h>
#include <NotesEditorInterface>
#include <CalenEditor>

#include "calenagendautils.h"
#include "agendaeventview.h"
#include "agendaeventviewerdocloader.h"
#include "agendaeventviewercommon.h"
#include "agendaeventviewer_p.h"
#include "agendaeventvieweritem.h"
#include "calendateutils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "agendaeventviewTraces.h"
#endif

// Constants
#define CHARACTER_SPACE     " "
#define CHARACTER_HYPHEN    " - "
#define DUMMY_ICON           "dummyIcon"

/*!
	\class AgendaEventView.

	Responsible for viewing viewing agenda entries. This is friend of
	AgendaEventViewerPrivate and it handles all the cases
	related to agenda entry viewing.
 */

/*!
	Default constructor.

	\param owner pointer to AgendaEventViewerPrivate.
	\param parent pointer to QGraphicsWidget.
 */
AgendaEventView::AgendaEventView(
		AgendaEventViewerPrivate *owner, QObject *parent):
		QObject(parent),
		mMainWindow(NULL),
		mOwner(owner),
		mTranslator(new HbTranslator("caleneventviewer")),
		mReminderWidgetAdded(true),
		mMaptilePath(NULL),
		mMaptileService(NULL),
		mProgressTimer(NULL),
		mProgressIconCount(0),
		mMaptileStatusReceived(false),
		mCalenEditorClosed(true),
		mMaptileStatus(-1),
		mNotesPluginLoaded(false),
		mEntryIdDeleted(0)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_AGENDAEVENTVIEW_ENTRY );
	mTranslator->loadCommon();
	mDocLoader = new AgendaEventViewerDocLoader;

	// Load to-do viewer's docml.
	bool loadSuccess;
	mDocLoader->load(AGENDA_EVENT_VIEWER_DOCML, &loadSuccess);
	if (!loadSuccess) {
		    qFatal("agendaeventview.cpp : Unable to load XML");
	}
	// Load the to-viewer's view.
	mViewer = qobject_cast<HbView *> (mDocLoader->findWidget(
			AGENDA_EVENT_VIEWER_VIEW_OBJECT));

	// Load all the widgets.
	mSubjectWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_SUBJECT_WIDGET));

	mDateTimeWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_DATE_TIME_WIDGET));

	mLocationWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_LOCATION_WIDGET));
	
	
	mMaptileLabel = qobject_cast<HbLabel *> (
	                mDocLoader->findWidget(AGENDA_EVENT_VIEWER_MAPTILE_WIDGET));
	
	
	mReminderWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_REMINDER_WIDGET));
	
	mRepeatWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_REPEAT_WIDGET));
	
	mDescriptionWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_DESCRIPTION_WIDGET));

    HbWidget *scrollAreaWidget = qobject_cast<HbWidget *>(
               mDocLoader->findWidget(AGENDA_EVENT_VIEWER_SCROLLAREA_CONTENTS));

    mLinearLayout = 
    		static_cast<QGraphicsLinearLayout *> (scrollAreaWidget->layout());
	

    //maptile service object , to retrive maptile path from database.
    mMaptileService= new MapTileService();   
    mLocationFeatureEnabled = mMaptileService->isLocationFeatureEnabled(
        MapTileService::AppTypeCalendar);
    if (mLocationFeatureEnabled) {
        //timer to run progress indicator icon.
        mProgressTimer = new QTimer(this);
        mProgressTimer->setSingleShot(true);
        connect(mProgressTimer, SIGNAL(timeout()), this, SLOT(updateProgressIndicator()));
    }
        
	
    OstTraceFunctionExit0( AGENDAEVENTVIEW_AGENDAEVENTVIEW_EXIT );
}

/*!
	Destructor.
 */
AgendaEventView::~AgendaEventView()
{
    OstTraceFunctionEntry0( DUP1_AGENDAEVENTVIEW_AGENDAEVENTVIEW_ENTRY );

	// Remove the translator
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	
	mDocLoader->reset();
	delete mDocLoader;

	// Unload notes editor if loaded.
	if (mNotesEditorPluginLoader) {
		mNotesEditorPluginLoader->unload();
		delete mNotesEditorPluginLoader;
		mNotesEditorPluginLoader = 0;
	}

	// Delete the mainwindow if we have created any
	if (mMainWindow) {
		delete mMainWindow;
		mMainWindow = NULL;
	}
	
	if (mMaptileService) {
        delete mMaptileService;
        mMaptileService = NULL;
    }
    
	if (mProgressTimer) {
        delete mProgressTimer;
        mProgressTimer = NULL;
    }
	
	OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEW_AGENDAEVENTVIEW_EXIT );
}

/*!
	Displays the to-do viewer and populates the to-do entry attributes.

	\param entry Agenda entry from which attributes have to be read.
 */
void AgendaEventView::execute(AgendaEntry entry,
											AgendaEventViewer::Actions action)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_EXECUTE_ENTRY );

	mOriginalAgendaEntry = entry;
	mAgendaEntry = entry;
	
	// For later reference
	mParentId = mOwner->mAgendaUtil->parentEntry(mAgendaEntry).id();
	
	// Add the viewer data reading from the agenda entry.
	addViewerData();
	
	// Remove unnecessary widget from event viewer.
	removeWidget();
	
	// Add the menu items to event viewer.
	addMenuItem();
	
	// Add the toolbar items to event viewer
	addToolBarItem(action);

	// Connect for the entry updation and addtion signal to refresh the view
	// when the same is edited in editor.
	connect(mOwner->mAgendaUtil, SIGNAL(entryUpdated(ulong)),
				this, SLOT(handleEntryUpdation(ulong)));
	
	connect(mOwner->mAgendaUtil, SIGNAL(entryAdded(ulong)),
				this, SLOT(handleEntryUpdation(ulong)));

	// Connect for entry deletion signal to close the event viewer.
	connect(mOwner->mAgendaUtil, SIGNAL(entryDeleted(ulong)), this,
	        SLOT(handleEntryDeletion(ulong)));

	// Add the view to the main window.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	if (!window) {
		// Might be some non-ui based app called us
		// so create mainwindow now
		mMainWindow = new HbMainWindow();
		mMainWindow->addView(mViewer);
		mMainWindow->setCurrentView(mViewer);
	    connect(mMainWindow,SIGNAL(orientationChanged(Qt::Orientation)),this,SLOT(changedOrientation(Qt::Orientation)));
	} else {
		window->addView(mViewer);
		window->setCurrentView(mViewer);
		connect(window,SIGNAL(orientationChanged(Qt::Orientation)),this,SLOT(changedOrientation(Qt::Orientation)));
	}
	
	// Add softkey after adding view on window
	mBackAction = new HbAction(Hb::BackNaviAction);
	mViewer->setNavigationAction(mBackAction);
		
	connect(mBackAction, SIGNAL(triggered()), this, SLOT(close()));

	OstTraceFunctionExit0( AGENDAEVENTVIEW_EXECUTE_EXIT );
}

/*!
	Refreshes the to-do viewer after the to-do editor is closed.
 */
void AgendaEventView::addViewerData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDVIEWERDATA_ENTRY );
	
	// Add the title to event viewer.
	addGroupBoxData();

	// Set the summary & priority to viewer.
	addSubjectAndPriorityData();
	
	// Set Date & Time to viewer.
	addDateTimeData();
	
	// Add agenda entry specific fields to the viewer
	switch (mAgendaEntry.type()) {
		case AgendaEntry::TypeAppoinment:
		case AgendaEntry::TypeEvent:
			addLocationData();
			addMapTileImage();	
			addReminderData();
			addRepeatData();
			break;
		case AgendaEntry::TypeAnniversary:
			break;
		case AgendaEntry::TypeTodo:
			if (AgendaEntry::TodoCompleted == mAgendaEntry.status()) { 
				addCompletedTodoData();
			} else {
				addReminderData();
			}
			break;
		default:
			break;
	}

	// Set the description.
	addDescriptionData();
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDVIEWERDATA_EXIT );
}

/*!
	Add the menu item depends up on entry type
 */
void AgendaEventView::addMenuItem()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDMENUITEM_ENTRY );

	if (mAgendaEntry.type() == AgendaEntry::TypeTodo) {

		HbMenu *menu = qobject_cast<HbMenu *> (
				mDocLoader->findWidget(AGENDA_EVENT_VIEWER_MENU));
		
		mMarkTodoAction = new HbAction(this);
		if (mAgendaEntry.status() == AgendaEntry::TodoCompleted) {
			mMarkTodoAction->setText(
								hbTrId("txt_calendar_menu_mark_as_not_done"));
		} else {
			mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_done"));
		}
		connect(mMarkTodoAction, SIGNAL(triggered()), this,
		        SLOT(markTodoStatus()));
		menu->addAction(mMarkTodoAction);
	}
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDMENUITEM_EXIT );
}

/*!
	Add the toolbar item
 */
void AgendaEventView::addToolBarItem(AgendaEventViewer::Actions action)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDTOOLBARITEM_ENTRY );

	HbToolBar *toolBar = qobject_cast<HbToolBar *> (
	                       mDocLoader->findWidget(AGENDA_EVENT_VIEWER_TOOLBAR));

	// Load all the actions for event viewer
	
	if ((action == AgendaEventViewer::ActionEditDelete) || (action
	        == AgendaEventViewer::ActionEdit)) {
		HbAction *editAction = new HbAction();
		editAction->setIcon(HbIcon("qtg_mono_edit"));
		connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
		toolBar->addAction(editAction);
	}

	if ((action == AgendaEventViewer::ActionEditDelete) || (action
	        == AgendaEventViewer::ActionDelete)) {
		HbAction *deleteAction = new HbAction();
		deleteAction->setIcon(HbIcon("qtg_mono_delete"));
		connect(deleteAction, SIGNAL(triggered()), this,
		        SLOT(deleteAgendaEntry()));
		toolBar->addAction(deleteAction);
	}

	if (action == AgendaEventViewer::ActionSave) {
		HbAction *saveAction = new HbAction();
		saveAction->setIcon(HbIcon("qtg_mono_add_to_calendar"));
		connect(saveAction, SIGNAL(triggered()), this, SLOT(saveAgendaEntry()));
		toolBar->addAction(saveAction);
	}

	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDTOOLBARITEM_EXIT );
}

/*!
	Add the groupbox data depends up on entry type
 */
void AgendaEventView::addGroupBoxData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDGROUPBOXDATA_ENTRY );
	
	HbGroupBox *groupBox = qobject_cast<HbGroupBox *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_GROUPBOX));

	AgendaEntry::Type entryType = mAgendaEntry.type();
	if (entryType == AgendaEntry::TypeTodo) {
		groupBox->setHeading(hbTrId("txt_calendar_subhead_to_do"));
	} else if (CalenAgendaUtils::isAlldayEvent(mAgendaEntry)) {
        groupBox->setHeading(hbTrId("txt_calendar_subhead_all_day_event"));
	} else if (entryType == AgendaEntry::TypeAppoinment) {
		groupBox->setHeading(hbTrId("txt_calendar_subhead_meeting"));
	} 
		
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDGROUPBOXDATA_EXIT );
}

/*!
	Add subject and priority data to Event viewer
 */
void AgendaEventView::addSubjectAndPriorityData()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDSUBJECTANDPRIORITYDATA_ENTRY );

	QStringList itemList;
	itemList.append(hbTrId("txt_calendar_dblist_subject"));
	if (mAgendaEntry.summary().isEmpty()) {
		itemList.append(hbTrId("txt_calendar_dblist_val_unnamed"));
	} else {
		itemList.append(mAgendaEntry.summary());
	}


	mSubjectWidget->setEventViewerItemData(itemList, Qt::DisplayRole);

    itemList.clear();
    QString priorityIcon(QString::null);
    QString subjectIcon(QString::null);
    getPriorityIcon(mAgendaEntry.priority(), priorityIcon);
    getSubjectIcon(mAgendaEntry.type(),subjectIcon);
    itemList.append(subjectIcon);
    itemList.append(priorityIcon);
    // This "DUMMY_ICON" is required only for the subject layout.
    itemList.append(DUMMY_ICON);
    itemList.append(QString::null);

	mSubjectWidget->setEventViewerItemData(itemList, Qt::DecorationRole);

	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDSUBJECTANDPRIORITYDATA_EXIT );
}

/*!
	Add date & time data to Event viewer
 */
void AgendaEventView::addDateTimeData()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDDATETIMEDATA_ENTRY );
    
    QStringList itemData;
    HbExtendedLocale systemLocale = HbExtendedLocale::system();
    QDateTime startDateTime = mAgendaEntry.startTime();
    QDateTime endDateTime = mAgendaEntry.endTime();
    
    itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append("qtg_small_calendar");
    itemData.append(QString::null);

    mDateTimeWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
    itemData.clear();
    itemData.append(QString::null);
    
    QString startTimeText;
    QString endTimeText;
    QString startDateText;
    QString dateTimeText;
    QString data;
    
    // Add agenda entry specific fields to the viewer
    
    switch (mAgendaEntry.type()) {
    	case AgendaEntry::TypeAppoinment:
    	case AgendaEntry::TypeEvent:
    	    // Check if entry is an all-day
    	    if (CalenAgendaUtils::isAlldayEvent(mAgendaEntry)) {
                dateTimeText.append(systemLocale.format(startDateTime.date(),
    	                                                        r_qtn_date_usual_with_zero));
                if (CalenDateUtils::onSameDay(startDateTime, endDateTime.addSecs(-60))) {
                    data.append(dateTimeText);
                } else {
                    QString endDate;
                    endDate.append(
                            systemLocale.format(endDateTime.addSecs(-60).date(),
                                                r_qtn_date_usual_with_zero));
                    data.append(dateTimeText);
                    data.append(" - ");
                    data.append(endDate);
                }
    	    } else {
                startTimeText.append(systemLocale.format(
                                startDateTime.time(), r_qtn_time_usual_with_zero));
    
                endTimeText.append(systemLocale.format(endDateTime.time(),
                                                    r_qtn_time_usual_with_zero));
                startDateText.append(
                        systemLocale.format(startDateTime.date(),
                                                    r_qtn_date_usual_with_zero));
                if (CalenDateUtils::onSameDay(startDateTime, endDateTime)) {
                    data.append(startTimeText);
                    data.append(CHARACTER_HYPHEN);
                    data.append(endTimeText);
                    data.append(CHARACTER_SPACE);
                    data.append(startDateText);
                } else {
                    // If both start and end time of a meeting are on different dates
                    data.append(startTimeText);
                    data.append(CHARACTER_SPACE);
                    data.append(startDateText);
                    QString endDateText;
                    endDateText.append(
                            systemLocale.format(endDateTime.date(),
                                                r_qtn_date_usual_with_zero));
                    data.append(CHARACTER_HYPHEN);
                    data.append(endTimeText);
                    data.append(CHARACTER_SPACE);
                    data.append(endDateText);
                }
    	    }
    		break;
    	case AgendaEntry::TypeAnniversary:
    	case AgendaEntry::TypeTodo:
    		dateTimeText.append(systemLocale.format(endDateTime.date(),
    											r_qtn_date_usual_with_zero));
    		data.append(dateTimeText);
    		break;
    	default:
    		break;
    }
	itemData.append(data);
    mDateTimeWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
    
    OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDDATETIMEDATA_EXIT );
}

/*!
	Add location data to Event viewer
 */
void AgendaEventView::addLocationData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDLOCATIONDATA_ENTRY );
	QStringList itemData;
	itemData.append(QString::null);
	itemData.append(QString::null);
	itemData.append("qtg_small_location");
	QString progressIcon(QString::null);	
	if ( mLocationFeatureEnabled ) {
	    getProgressIndicatorstatus(progressIcon);
	}
	 if( progressIcon.isNull() ) {
	     itemData.append(QString::null);
	 }
	 else {
	     itemData.append( progressIcon );
	 }
	mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(mAgendaEntry.location());
	mLocationWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDLOCATIONDATA_EXIT );
}

/*!
    Add maptile image to Event viewer
 */
void AgendaEventView::addMapTileImage()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDMAPTILEIMAGE_ENTRY );
    if (mLocationFeatureEnabled && !mAgendaEntry.location().isEmpty() && !mMaptilePath.isEmpty()) {

        HbIcon maptile(mMaptilePath);
        mMaptileLabel->setIcon(maptile);
        //get the margin size
        qreal left, bottom;
        mMaptileLabel->getContentsMargins( &left, 0, 0, &bottom );
        mMaptileLabel->setPreferredSize( 
                 QSizeF( maptile.width() + left, maptile.height() + bottom ) );
    }
    OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDMAPTILEIMAGE_EXIT );
}
/*!
	Add reminder data to Event viewer
 */
void AgendaEventView::addReminderData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDREMINDERDATA_ENTRY );
	QStringList itemData;
	itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append("qtg_small_reminder");
    itemData.append(QString::null);
	mReminderWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(alarmTimeText());
	mReminderWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	mReminderWidgetAdded = true;
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDREMINDERDATA_EXIT );
}

/*!
	Add completed to-do data to Event viewer
 */
void AgendaEventView::addCompletedTodoData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDCOMPLETEDTODODATA_ENTRY );
	QStringList itemData;
	QString completedText;
	HbExtendedLocale systemLocale = HbExtendedLocale::system();;
	itemData.append(QString::null);
	itemData.append(QString::null);
	itemData.append(QString::null);
	itemData.append(QString::null);
	mReminderWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	completedText = systemLocale.format(mAgendaEntry.completedDateTime().date(),
					                    r_qtn_date_usual_with_zero);
	itemData.append(hbTrId("txt_calendar_dblist_completed_date"));
	itemData.append(completedText);
	mReminderWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDCOMPLETEDTODODATA_EXIT );
}

/*!
	Add repeat data to Event viewer
 */
void AgendaEventView::addRepeatData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDREPEATDATA_ENTRY );
	QStringList itemData;
	itemData.append(QString::null);
    itemData.append(QString::null);
    if(!mAgendaEntry.recurrenceId().isNull()) {
        itemData.append("qtg_small_repeat_exception");
    }else {
           itemData.append("qtg_small_repeat");
    }
    itemData.append(QString::null);
	mRepeatWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(repeatRule());
	mRepeatWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDREPEATDATA_EXIT );
}

/*!
	Add description data to Event viewer
 */
void AgendaEventView::addDescriptionData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDDESCRIPTIONDATA_ENTRY );
	QStringList itemData;
	itemData.append(QString::null);
	itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append(QString::null);
	mDescriptionWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(hbTrId("txt_calendar_dblist_description"));
	itemData.append(mAgendaEntry.description());
	mDescriptionWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDDESCRIPTIONDATA_EXIT );
}

/*!
	Returns priority icon
 */
void AgendaEventView::getPriorityIcon(int priority, QString &priorityIcon)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_GETPRIORITYICON_ENTRY );

	switch(priority) {
		case 1:priorityIcon.append("qtg_small_priority_high");
		break;
		case 3:priorityIcon.append("qtg_small_priority_low");
		break;
		default:
		break;
	}

	OstTraceFunctionExit0( AGENDAEVENTVIEW_GETPRIORITYICON_EXIT );
}

/*!
	Returns repeat rule
 */
QString AgendaEventView::repeatRule()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_REPEATRULE_ENTRY );
	
	QString repeatRule;
	AgendaRepeatRule agendaRepeatRule;
	if (mAgendaEntry.isRepeating()) {
	    agendaRepeatRule = mAgendaEntry.repeatRule();
	} else {
            if (!mAgendaEntry.recurrenceId().isNull()) {
                AgendaEntry parentEntry= mOwner->mAgendaUtil->parentEntry(mAgendaEntry);
                agendaRepeatRule = parentEntry.repeatRule();
                }
	}
	
	if (agendaRepeatRule.type() != AgendaRepeatRule::InvalidRule)
	{
		switch (agendaRepeatRule.type()) {
			case AgendaRepeatRule::DailyRule:
				repeatRule.append(hbTrId("txt_calendar_dblist_repeats_daily"));
			break;
			case AgendaRepeatRule::WeeklyRule:
				if (AgendaUtil::isWorkdaysRepeatingEntry(agendaRepeatRule)) {
					repeatRule.append(
								hbTrId("txt_calendar_dblist_repeats_workdays"));
				} else {
					if (agendaRepeatRule.interval() == 2) {
						repeatRule.append(
							hbTrId("txt_calendar_dblist_repeats_fortnightly"));
					} else {
						repeatRule.append(
							hbTrId("txt_calendar_dblist_repeats_weekly"));
					}
				}
			break;
			case AgendaRepeatRule::MonthlyRule:
				repeatRule.append(
						hbTrId("txt_calendar_dblist_repeats_monthly"));
			break;
			case AgendaRepeatRule::YearlyRule:
				repeatRule.append(
						hbTrId("txt_calendar_dblist_repeats_yearly"));
			break;
			default:
			break;
		}
		repeatRule.append(CHARACTER_SPACE);
		HbExtendedLocale systemLocale = HbExtendedLocale::system();
		QString untilDateString = systemLocale.format(
		                agendaRepeatRule.until().date(), 
		                r_qtn_date_usual_with_zero);
		repeatRule.append(
			hbTrId("txt_calendar_dblist_repeats_daily_val_until_1").
			arg(untilDateString));
	}
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_REPEATRULE_EXIT );
	return repeatRule;
}

/*!
	Alarm time text to display in the viewer.

	\return QString	Holds the alarm time text.
 */
QString AgendaEventView::alarmTimeText() const
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_ALARMTIMETEXT_ENTRY );

	QString alarmDateTimeText;
	QDateTime startTime;
	QDateTime alarmDateTime;
	
	if (mAgendaEntry.type() == AgendaEntry::TypeTodo) { 
		startTime = mAgendaEntry.endTime();
	} else { 
		startTime = mAgendaEntry.startTime();
	}
	if (!mAgendaEntry.alarm().isNull()) { 
		
		int alarmTimeOffsetInMinutes = mAgendaEntry.alarm().timeOffset();
		alarmDateTime = startTime.addSecs(-alarmTimeOffsetInMinutes * 60);

		HbExtendedLocale systemLocale = HbExtendedLocale::system();
		alarmDateTimeText.append(systemLocale.format(alarmDateTime.time(),
										r_qtn_time_usual_with_zero));
		// Show the alarm date only if its not on the same day of the entry
		if (!CalenDateUtils::onSameDay(alarmDateTime, startTime)) {
			alarmDateTimeText.append(CHARACTER_SPACE);
			alarmDateTimeText.append(systemLocale.format(alarmDateTime.date(),
											r_qtn_date_usual_with_zero));	
		}
	}
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ALARMTIMETEXT_EXIT );
	return alarmDateTimeText;
}

/*!
	Remove unnecessary widget from layout.
 */
void AgendaEventView::removeWidget()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_REMOVEWIDGET_ENTRY );
	
	if (mAgendaEntry.location().isEmpty()) { 
		mLocationWidget->hide();
		mLinearLayout->removeItem(mLocationWidget);
	}
	
	QFile file(mMaptilePath);
    if (!mLocationFeatureEnabled || !file.exists()
            || mAgendaEntry.location().isEmpty()) {
        //code added to hide and remove maptile image
        mMaptileLabel->hide();
        mLinearLayout->removeItem(mMaptileLabel);
    }
    file.close();
        
	if (mAgendaEntry.alarm().isNull()) { 
		if (mAgendaEntry.type() == AgendaEntry::TypeTodo ) {
				if (AgendaEntry::TodoCompleted != mAgendaEntry.status()) { 
					mReminderWidget->hide();
					mLinearLayout->removeItem(mReminderWidget);
					mReminderWidgetAdded = false;
				}
		} else { 
			mReminderWidget->hide();
			mLinearLayout->removeItem(mReminderWidget);
			mReminderWidgetAdded = false;
		}
	}
	
	// Check whether the entry is a completed To-do. if so then remove the 
	// time and date widget.
	if (mAgendaEntry.type() == AgendaEntry::TypeTodo &&
			AgendaEntry::TodoCompleted == mAgendaEntry.status()) {
		mDateTimeWidget->hide();
		mLinearLayout->removeItem(mDateTimeWidget);
	}
	
	if ((mAgendaEntry.repeatRule().type() == AgendaRepeatRule::InvalidRule) &&
			(mAgendaEntry.recurrenceId().isNull())) { 
		mRepeatWidget->hide();
		mLinearLayout->removeItem(mRepeatWidget);
	}
	
	if (mAgendaEntry.description().isEmpty()) { 
		mDescriptionWidget->hide();
		mLinearLayout->removeItem(mDescriptionWidget);
	}
	
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_REMOVEWIDGET_EXIT );
}

/*!
	Update all the fields according to to-do status change..
 */
void AgendaEventView::updateFieldsforTodoStatus()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_UPDATEFIELDSFORTODOSTATUS_ENTRY );

	if (AgendaEntry::TodoCompleted == mAgendaEntry.status()) {
		
		// For completed to-do due date and time should be removed.
		mDateTimeWidget->hide();
		mLinearLayout->removeItem(mDateTimeWidget);
		
		// Fill the completed data information.
		addCompletedTodoData();
		
		// Add the completed date to thte viewer.
		if (!mReminderWidgetAdded) {
			mReminderWidget->show();
			mLinearLayout->insertItem(2, mReminderWidget);
			mReminderWidgetAdded = true;
		}

	} else {
			// Add the date and time widget to the viewer.
			mDateTimeWidget->show();
			mLinearLayout->insertItem(1, mDateTimeWidget);
			
		if (!mAgendaEntry.alarm().isNull()) {
			addReminderData();
			if (!mReminderWidgetAdded) {
				mReminderWidget->show();
				mLinearLayout->insertItem(2, mReminderWidget);
				mReminderWidgetAdded = true;
			}
		} else {
			// Remove the completed date from the viewer.
			if (mReminderWidgetAdded) {
				mReminderWidget->hide();
				mLinearLayout->removeItem(mReminderWidget);
				mReminderWidgetAdded = false;
			}
		}

	}
	// The To-do icon changes according to the To-do status change.
	updateSubjectandPriorityData();
	
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	OstTraceFunctionExit0( AGENDAEVENTVIEW_UPDATEFIELDSFORTODOSTATUS_EXIT );
}

/*!
	Remove all widgets from layout.
 */
void AgendaEventView::removeAllWidgets()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_REMOVEALLWIDGETS_ENTRY );

	for (int i = 2; i < mLinearLayout->count(); i++) {
		mLinearLayout->removeAt(i);
	}
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_REMOVEALLWIDGETS_EXIT );
}

/*!
	Add all widgets to layout.
 */
void AgendaEventView::addAllWidgets()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_ADDALLWIDGETS_ENTRY );
	
	mLinearLayout->addItem(mLocationWidget);
	mLocationWidget->show();
	mLinearLayout->addItem(mMaptileLabel);
	mMaptileLabel->show();		
	mLinearLayout->addItem(mReminderWidget);
	mReminderWidget->show();
	mLinearLayout->addItem(mRepeatWidget);
	mRepeatWidget->show();
	mLinearLayout->addItem(mDescriptionWidget);
	mDescriptionWidget->show();
	
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_ADDALLWIDGETS_EXIT );
}

/*!
	Queries user whether to delete whole series or just this single occurence
 */
void AgendaEventView::showDeleteOccurencePopup()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_SHOWDELETEOCCURENCEPOPUP_ENTRY );
	HbDialog *popUp = new HbDialog();
	// Set the parent for the dialog
	// Once the parent object is deleted the dialog will also be deleted
	popUp->setParent(this);
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );

	QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
	HbWidget *deleteWidget = new HbWidget(mViewer);
	deleteWidget->setLayout(layout);

	HbRadioButtonList *deleteButtonList = new HbRadioButtonList(mViewer);

	QStringList list;
	list << hbTrId("txt_calendar_info_this_occurrence_only") 
				<< hbTrId("txt_calendar_info_all_occurences");

	deleteButtonList->setItems(list);

	layout->addItem(deleteButtonList);

	popUp->setContentWidget(deleteWidget);
	popUp->setHeadingWidget(new HbLabel(
			hbTrId("txt_calendar_title_delete_repeated_entry")));

	connect(deleteButtonList, SIGNAL(itemSelected(int)), this,
											SLOT(handleDeleteOccurence(int)));
	connect(deleteButtonList, SIGNAL(itemSelected(int)), popUp, SLOT(close()));

	popUp->addAction(new HbAction(
			hbTrId("txt_common_button_cancel_singledialog")));

	// Show the popup
	popUp->open();

	OstTraceFunctionExit0( AGENDAEVENTVIEW_SHOWDELETEOCCURENCEPOPUP_EXIT );
}

/*!
	Show delete confirmation query
 */
void AgendaEventView::showDeleteConfirmationQuery()
    {
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_SHOWDELETECONFIRMATIONQUERY_ENTRY );
    
    HbMessageBox *popup = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
	// Set the parent for the dialog
	// Once the parent object is deleted the dialog will also be deleted
    popup->setParent(this);
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);
    popup->setAttribute( Qt::WA_DeleteOnClose, true );

    QString text = 0;

    switch (mAgendaEntry.type()) {
        case AgendaEntry::TypeAppoinment:
        case AgendaEntry::TypeEvent: {
			// Check for all-day
        	if (CalenAgendaUtils::isAlldayEvent(mAgendaEntry)) {
				text.append(hbTrId("txt_calendar_info_delete_allday_event"));
        	} else {
				text.append(hbTrId("txt_calendar_info_delete_meeting"));
			}

			break;
		}
        case AgendaEntry::TypeAnniversary: {
        text.append(hbTrId("txt_calendar_info_delete_anniversary"));
        break;
        }
        case AgendaEntry::TypeTodo: {
        text.append(hbTrId("txt_calendar_info_delete_todo_note"));
        break;
        }
        case AgendaEntry::TypeNote: {
        text.append(hbTrId("txt_calendar_info_delete_anniversary"));
        break;
        }
    }
    popup->setText(text);
    
    QList<QAction*> list = popup->actions();
    for(int i=0; i < list.count(); i++)
        {
        popup->removeAction(list[i]);
        }
    HbAction *deleteAction = 
					new HbAction(hbTrId("txt_common_button_delete"), popup);
    popup->addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this ,
												SLOT(handleDeleteAction()));
    popup->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popup));
    popup->open();
    OstTraceFunctionExit0( AGENDAEVENTVIEW_SHOWDELETECONFIRMATIONQUERY_EXIT );
}

/*!
	Handles the delete action
 */
void AgendaEventView::handleDeleteAction()
    {
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_HANDLEDELETEACTION_ENTRY );
    // If delete button is pressed delete the entry
    // To notify client that deleting Started
    // Calendar Application changing state from viewing to deleting.
    mOwner->deletingStarted();

    // Delete the entry.
    mOwner->mAgendaUtil->deleteEntry(mAgendaEntry.id());
    OstTraceFunctionExit0( AGENDAEVENTVIEW_HANDLEDELETEACTION_EXIT );
}

/*!
	Marks to-do entry as done or undone based on the completed value.
 */
void AgendaEventView::markTodoStatus()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_MARKTODOSTATUS_ENTRY );

	QDateTime currentDateTime = QDateTime::currentDateTime();

	// Set the to-do status using the agenda util.
	if (AgendaEntry::TodoCompleted != mAgendaEntry.status()) {
		// Update the menu text to mark to-do as undone.
		mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_not_done"));
		mAgendaEntry.setStatus(AgendaEntry::TodoCompleted);
		mAgendaEntry.setCompletedDateTime(currentDateTime);
		mOwner->mAgendaUtil->setCompleted(mAgendaEntry, true, currentDateTime);
	} else {
		
		// Update the menu text to mark to-do as done.
		mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_done"));
		mAgendaEntry.setStatus(AgendaEntry::TodoNeedsAction);
		mOwner->mAgendaUtil->setCompleted(mAgendaEntry, false, currentDateTime);
	}

	updateFieldsforTodoStatus();
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_MARKTODOSTATUS_EXIT );
}

/*!
	Edits the agenda entry by lanching the to-do viewer.
 */
void AgendaEventView::edit()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_EDIT_ENTRY );

	mOwner->editingStarted();
	
	if (AgendaEntry::TypeTodo == mAgendaEntry.type()) {
		// Load notes editor plugin if not loaded.
		if(!mNotesPluginLoaded) {
			// Launch the to-do editor using notes editor plugin api
			QDir dir(NOTES_EDITOR_PLUGIN_PATH);
			QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

			// Create NotesEditor plugin loader object.
			mNotesEditorPluginLoader = new QPluginLoader(pluginName);

			// Load the plugin
			mNotesPluginLoaded = mNotesEditorPluginLoader->load();
		}
		QObject *plugin = qobject_cast<QObject*> (
				mNotesEditorPluginLoader->instance());

		NotesEditorInterface* interface =
				qobject_cast<NotesEditorInterface*>(plugin);

		interface->edit(mAgendaEntry, mOwner->mAgendaUtil);

		connect(
				interface, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleNoteEditorClosed(bool)));
		

	} else {
		// Launch the calendar entry editor using calendar editor api
		mCalenEditor = new CalenEditor(mOwner->mAgendaUtil, this);
		connect(mCalenEditor, SIGNAL(dialogClosed()),
		                        this, SLOT(handleCalendarEditorClosed()));
		mCalenEditor->edit(mAgendaEntry, false);
		mCalenEditorClosed = false;
		
	}
	OstTraceFunctionExit0( AGENDAEVENTVIEW_EDIT_EXIT );
}

/*!
	Deletes the agenda entry.
 */
void AgendaEventView::deleteAgendaEntry()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_DELETEAGENDAENTRY_ENTRY );

	// Before we do anything, check in the entry is repeating
	// OR its a child item
	bool isChild = !(mAgendaEntry.recurrenceId().isNull());
	bool isRepeating = mAgendaEntry.isRepeating();
	if ((isChild || isRepeating) 
			&& (mAgendaEntry.type() != AgendaEntry::TypeAnniversary) 
			&& (mAgendaEntry.type() != AgendaEntry::TypeTodo)) {
		// Query user if he wants to delete whole series or just this occurence
		showDeleteOccurencePopup();
	} else {
        showDeleteConfirmationQuery();
	}

	OstTraceFunctionExit0( AGENDAEVENTVIEW_DELETEAGENDAENTRY_EXIT );
}

/*!
	Save the agenda entry to calendar db.
 */
void AgendaEventView::saveAgendaEntry()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_SAVEAGENDAENTRY_ENTRY );
	
	// Save entry to calendar.
	mOwner->mAgendaUtil->store(mAgendaEntry);
	
	// Close the agenda entry viewer
	close();
	OstTraceFunctionExit0( AGENDAEVENTVIEW_SAVEAGENDAENTRY_EXIT );
}
/*!
	Closes the event viewer
 */
void AgendaEventView::close()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_CLOSE_ENTRY );

	// Remove the view from main window.
	HbMainWindow *window = hbInstance->allMainWindows().first();

	// Cleanup.
	disconnect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(close()));

	window->removeView(mViewer);
	// For To-do, if due date is in future, we retain it there. 
	// All the past incompleted to-dos will come on today.
	if ((AgendaEntry::TypeTodo == mAgendaEntry.type()) && (mAgendaEntry.startTime().date() <= QDate::currentDate())) {
	    mAgendaEntry.setStartAndEndTime(
	            CalenDateUtils::today(), CalenDateUtils::today());
	}
	mOwner->viewingCompleted(mAgendaEntry.startTime().date());

	OstTraceFunctionExit0( AGENDAEVENTVIEW_CLOSE_EXIT );
}

/*!
	Handles entry updation of the event entry.
 */
void AgendaEventView::handleEntryUpdation(ulong id)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_HANDLEENTRYUPDATION_ENTRY );

	AgendaEntry updatedEntry = mOwner->mAgendaUtil->fetchById(id);

	// Agenda entry is not null then refresh the view else close event viewer
	if (!updatedEntry.isNull()) {

		mAgendaEntry = updatedEntry;
		
		// If the updated entry's & original entry's repeat rule mismatches than
		// show the parent entry.
		if (updatedEntry.isRepeating() &&
		        mAgendaEntry.type() != AgendaEntry::TypeTodo &&
		        mOriginalAgendaEntry.repeatRule() ==
		                updatedEntry.repeatRule()) {
			// if start date of original entry is between start date of updated 
			// entry and until date of updated entry then only update time.
			if (mOriginalAgendaEntry.startTime().date()
			        >= updatedEntry.startTime().date()
			        && mOriginalAgendaEntry.startTime().date()
			                <= updatedEntry.repeatRule().until().date()) {
				QDateTime
				        startDateTime(mOriginalAgendaEntry.startTime().date(),
				                      updatedEntry.startTime().time());
				QDateTime endDateTime(mOriginalAgendaEntry.endTime().date(),
				                      updatedEntry.endTime().time());
				mAgendaEntry.setStartAndEndTime(startDateTime, endDateTime);
			}
		}

		if (mOriginalAgendaEntry.status() == updatedEntry.status()) {
			
			// Remove all widgets from layout.
			removeAllWidgets();
			
			// Add all widgets to layout.
			addAllWidgets();
			
			// Update the viewer's data using the modified entry from parent.
			addViewerData();
			
			// Remove unnecessary widget from event viewer.
			removeWidget();
		}
		mOriginalAgendaEntry = mAgendaEntry;
	} else {
		// Close the agenda entry viewer
		close();
	}

	OstTraceFunctionExit0( AGENDAEVENTVIEW_HANDLEENTRYUPDATION_EXIT );
}

/*!
	Handles the entry deletion of the agenda entry
*/
void AgendaEventView::handleEntryDeletion(ulong id)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_HANDLEENTRYDELETION_ENTRY );
	// Check if the entry which is viewed has been deleted or not
	// The first check id == mAgendaEntry.id() will fail for the following case
    // Exceptional entry is opened in the viewer. Edit this occurence in editor.
    // From editor delete all occurences of the entry. In this case the viewer 
    // has the exceptional entry id which is not the same id which got deleted.
    // Its the parent entry id. So checking if its a child and the entry deleted
    // is the parent of this child.
    if (id == mAgendaEntry.id() || 
    		(!mAgendaEntry.recurrenceId().isNull() && id == mParentId)) {
    	// If the editor is opened and the deletion is happening from editor
    	// then there is no need to emit the signal deletingCompleted() [as the 
    	// deletingStarted() is not been emitted already]
    	// In this case closing of viewer will happen only once the editor 
    	// is closed properly
    	mEntryIdDeleted = id;
    	if(!mCalenEditor) {
    		// If the editor is not opened then emit deletingCompleted() 
    		// and close the viewer
    		mOwner->deletingCompleted();
    		close();
    	}
    }

	OstTraceFunctionExit0( AGENDAEVENTVIEW_HANDLEENTRYDELETION_EXIT );
}

/*!
	Handles the Note Editor editing completion of the agenda entry
 */
void AgendaEventView::handleNoteEditorClosed(bool status)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_HANDLENOTEEDITORCLOSED_ENTRY );
	Q_UNUSED(status);

	// To avoid loading the plugin again for editing,
	// Unload the plug-in while destruction.

	mOwner->editingCompleted();

	OstTraceFunctionExit0( AGENDAEVENTVIEW_HANDLENOTEEDITORCLOSED_EXIT );
}

/*!
	Handles the Note Editor editing completion of the agenda entry
 */
void AgendaEventView::handleCalendarEditorClosed()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_HANDLECALENDAREDITORCLOSED_ENTRY );

	// Cleanup.
    mCalenEditorClosed = true;
	mOwner->editingCompleted();
	if(mCalenEditor) {
		mCalenEditor->deleteLater();
		// Check if the entry is deleted
		if(mEntryIdDeleted) {
			ulong currentId = mAgendaEntry.id();
			// If the deleted entry is the same as current id then close viewer
			// The second check is for exceptional entries which will be true,
			// when all the occurences of the entry is deleted from the editor
			// In this case the entry id deleted won't the same as current id
			if(currentId == mEntryIdDeleted || mEntryIdDeleted == mParentId) {
				// Close the viewer as the entry no longer exists
				close();
			}
			// Reset the value.
			mEntryIdDeleted = 0;
		}
	}
	
	
	OstTraceFunctionExit0( AGENDAEVENTVIEW_HANDLECALENDAREDITORCLOSED_EXIT );
}

/*!
	Slot to handle User selection for series deleting or single occurence popup
 */
void AgendaEventView::handleDeleteOccurence(int index)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_HANDLEDELETEOCCURENCE_ENTRY );
	
	// To notify client that deleting Started
	// Calendar Application changing state from viewing to deleting.
	mOwner->deletingStarted();
	
	switch (index) {
		case 0:
			// User wants to delete only this occurence
			mOwner->mAgendaUtil->deleteRepeatedEntry(mAgendaEntry,
			                                         AgendaUtil::ThisOnly);

			break;
		case 1:
			// User wants to delete all the occurences
			mOwner->mAgendaUtil->deleteRepeatedEntry(mAgendaEntry,
			                                         AgendaUtil::ThisAndAll);
			break;
	}

	OstTraceFunctionExit0( AGENDAEVENTVIEW_HANDLEDELETEOCCURENCE_EXIT );
}

/*!
    Returns subject icon
 */
void AgendaEventView::getSubjectIcon(AgendaEntry::Type type, QString &subjectIcon)
    {
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_GETSUBJECTICON_ENTRY );
    switch(type) {
        case AgendaEntry::TypeAppoinment:
        case AgendaEntry::TypeEvent:
            {
            // Check for all-day
            if (CalenAgendaUtils::isAlldayEvent(mAgendaEntry)) {
                subjectIcon.append("qtg_small_day");
            } else {
                subjectIcon.append("qtg_small_meeting");
            }
            }
            break;
        case AgendaEntry::TypeTodo:
            {
            if (AgendaEntry::TodoCompleted == mAgendaEntry.status()) {
                subjectIcon.append("qtg_small_todo_done");
            } else {
                subjectIcon.append("qtg_small_todo");
            }
            }
            break;
        case AgendaEntry::TypeAnniversary:
            {
            subjectIcon.append("qtg_small_anniversary");
            }
            break;
        default:
            break;
    }

    OstTraceFunctionExit0( AGENDAEVENTVIEW_GETSUBJECTICON_EXIT );
    }

/*!
    According to maptile fetching status , update the viewer screen.
 */
void AgendaEventView::updateProgressIndicator()
{   
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_UPDATEPROGRESSINDICATOR_ENTRY );
    if (!mMaptileStatusReceived) {
        QString iconName("qtg_anim_small_loading_");
        mProgressIconCount = mProgressIconCount % 10 + 1;
        iconName.append(QVariant(mProgressIconCount).toString());
        QStringList itemData;
        itemData.append(QString::null);
        itemData.append(QString::null);
        itemData.append("qtg_small_location");
        itemData.append(iconName);
        mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
        mProgressTimer->start(100);
    }
    else {
        if (mProgressTimer->isActive()) {
            mProgressTimer->stop();
        }
        if (mMaptileStatus == MapTileService::MapTileFetchingCompleted) {
            QStringList itemData;
            itemData.append(QString::null);
            itemData.append(QString::null);
            itemData.append("qtg_small_location");
            itemData.append(QString::null);
            mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
            Qt::Orientations orientation=hbInstance->allMainWindows().first()->orientation();
            mMaptilePath.clear();
            mMaptileStatus = mMaptileService->getMapTileImage(mAgendaEntry.id(), MapTileService::AddressPlain, mMaptilePath ,orientation);
            addMapTileImage();
            QFile file(mMaptilePath);
            if (file.exists()) {
                //add to linear layout  
                int indexMaptileLabel = 3;// index of maptile widget position
                mLinearLayout->insertItem(indexMaptileLabel, mMaptileLabel);
                mMaptileLabel->show();
            }
            file.close();

        }
        else {
            QStringList itemData;
            itemData.append(QString::null);
            itemData.append(QString::null);
            itemData.append("qtg_small_location");
            QString stopIcon;
            stopIcon.append(QString("qtg_mono_search_stop"));
            itemData.append(stopIcon);
            mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);

        }
    }
    OstTraceFunctionExit0( AGENDAEVENTVIEW_UPDATEPROGRESSINDICATOR_EXIT );
}

/*!
    Maptile status received from maptile service 
 */
void AgendaEventView::receiveMapTileStatus(int entryid,int addressType, int status)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_RECEIVEMAPTILESTATUS_ENTRY );
    if (mAgendaEntry.id() == entryid && addressType == MapTileService::AddressPlain) {
        mMaptileStatusReceived = true;
        mMaptileStatus = status;
        updateProgressIndicator();
    }
    OstTraceFunctionExit0( AGENDAEVENTVIEW_RECEIVEMAPTILESTATUS_EXIT );
}

/*!
    Returns progress indication icon as per status of entry in database.
 */
void AgendaEventView::getProgressIndicatorstatus(QString &progressIcon)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_GETPROGRESSINDICATORSTATUS_ENTRY );
    MapTileService::AddressType addressType;
    addressType = MapTileService::AddressPlain;
    int eventId = mAgendaEntry.id();
    mMaptilePath.clear();
    mMaptileStatus = -1;
    connect(mMaptileService, SIGNAL(maptileFetchingStatusUpdate(int,
            int ,int)), this, SLOT(receiveMapTileStatus(int,int,int)));
    Qt::Orientations orientation=hbInstance->allMainWindows().first()->orientation();
    mMaptileStatus = mMaptileService->getMapTileImage(eventId, addressType, mMaptilePath ,orientation);    
    if (mMaptileStatus == MapTileService::MapTileFetchingNetworkError || mMaptileStatus
        == MapTileService::MapTileFetchingInProgress) {
        mMaptilePath.clear();
        mMaptileStatusReceived = false; //reseting receiving status value       
        progressIcon.append(QString("qtg_anim_small_loading_1"));
        mProgressTimer->start(100);
    }
    else if (mMaptileStatus == MapTileService::MapTileFetchingInvalidAddress || mMaptileStatus
        == MapTileService::MapTileFetchingUnknownError) {
        mMaptilePath.clear();
        //no further need of this coonnection 
        disconnect(mMaptileService, SIGNAL(maptileFetchingStatusUpdate(int,
                int ,int)), this, SLOT(receiveMapTileStatus(int,int,int)));
        progressIcon.append(QString("qtg_mono_search_stop"));
    }
    else {
        //no further need of this coonnection
        disconnect(mMaptileService, SIGNAL(maptileFetchingStatusUpdate(int,
                int ,int)), this, SLOT(receiveMapTileStatus(int,int,int)));
        progressIcon.append(QString::null);
    }
    OstTraceFunctionExit0( AGENDAEVENTVIEW_GETPROGRESSINDICATORSTATUS_EXIT );
}

/*!
	Updates the To-do icon for the changes in to-do status.
 */
void AgendaEventView::updateSubjectandPriorityData()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_UPDATESUBJECTANDPRIORITYDATA_ENTRY );
	QStringList itemList;
    QString priorityIcon(QString::null);
    QString subjectIcon(QString::null);
    getPriorityIcon(mAgendaEntry.priority(), priorityIcon);
    getSubjectIcon(mAgendaEntry.type(),subjectIcon);
    itemList.append(subjectIcon);
    itemList.append(priorityIcon);
    // This "DUMMY_ICON" is required only for the subject layout.
    itemList.append(DUMMY_ICON);
    itemList.append(QString::null);

	mSubjectWidget->setEventViewerItemData(itemList, Qt::DecorationRole);
	OstTraceFunctionExit0( AGENDAEVENTVIEW_UPDATESUBJECTANDPRIORITYDATA_EXIT );
}

/*!
    Reload the maptile image on system orientation change.
 */
void AgendaEventView::changedOrientation(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_CHANGEDORIENTATION_ENTRY );
    if (mMaptileStatus == MapTileService::MapTileFetchingCompleted) {
        mMaptilePath.clear();
        mMaptileService->getMapTileImage(mAgendaEntry.id(), MapTileService::AddressPlain, mMaptilePath,orientation);
        addMapTileImage();
    }
    OstTraceFunctionExit0( AGENDAEVENTVIEW_CHANGEDORIENTATION_EXIT );
}

/*!
    Close the editor and save the entry.
    should be call if editor is open
 */
void AgendaEventView::saveAndCloseEditor()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_SAVEANDCLOSEEDITOR_ENTRY );
    if(mCalenEditorClosed){
        QObject *plugin = qobject_cast<QObject*> (
                mNotesEditorPluginLoader->instance());

        NotesEditorInterface* interface =
            qobject_cast<NotesEditorInterface*>(plugin);

        
        interface->close(NotesEditorInterface::CloseWithSave ,mOwner->mAgendaUtil);

    }
    else{
        if(mCalenEditor){
            mCalenEditor->saveAndCloseEditor();
        }
    }
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_SAVEANDCLOSEEDITOR_EXIT );
}

/*!
    Close the view .
    should be call if view is open
 */
void AgendaEventView::closeAgendaEventView()
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEW_CLOSEAGENDAEVENTVIEW_ENTRY );
    close();
    OstTraceFunctionEntry0( AGENDAEVENTVIEW_CLOSEAGENDAEVENTVIEW_EXIT );
}
// End of file
