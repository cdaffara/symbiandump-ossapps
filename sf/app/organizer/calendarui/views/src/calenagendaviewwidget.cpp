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
* Description: CalenAgendaViewWidget implementation
*
*/

// System includes
#include <QStandardItemModel>
#include <QGraphicsSceneEvent>
#include <QDir>
#include <QPluginLoader>
#include <hbi18ndef.h>
#include <hbextendedlocale.h>
#include <hbgroupbox.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbstyleloader.h>
#include <hbdatetimepicker.h>
#include <hbdialog.h>
#include <xqsettingsmanager.h>
#include <agendautil.h>
#include <NotesEditorInterface>

// User includes
#include "calenagendaviewwidget.h"
#include "calencommon.h"
#include "calenagendaview.h"
#include "calendocloader.h"
#include "calenservices.h"
#include "calencontext.h"
#include "calendateutils.h"
#include "calenagendautils.h"
#include "CalenUid.h"
#include "caleneventlistviewitem.h"
#include "calenpluginlabel.h"
#include "calendarprivatecrkeys.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenagendaviewwidgetTraces.h"
#endif


// Constants
const QString singleSpace(" ");
const QString doubleSpace("  ");
const QString space("              ");
const QString singleHyphen("-");
const QString customLayout("custom");
const char *stretchLayout("customstretch");
const QString toDoIcon("qtg_small_todo");
const QString reminderIcon("qtg_mono_alarm");
const QString locationIcon("qtg_mono_location");
const QString repeatIcon("qtg_mono_repeat");
// TODO: Replace with actual image name once its available
const QString allDayIcon("qtg_small_day");
const QString exceptionIcon("qtg_mono_repeat_exception");
const int singleColumn(1);

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::CalenAgendaViewWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
EXPORT_C CalenAgendaViewWidget::CalenAgendaViewWidget(MCalenServices &services,
                                       CalenDocLoader *docLoader) :
mServices(services),
mDocLoader(docLoader),
mRegionalInfoGroupBox(NULL),
mLongTapEventFlag(false),
mNotesPluginLoaded(false)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_CALENAGENDAVIEWWIDGET_ENTRY );
    
    // Construct the list view prototype
    mListViewPrototype = new CalenEventListViewItem(this);
    
    // Create the list model
    mListModel = new QStandardItemModel(this);
    
    // Register the custom docml and css to provide our own style to the list items
    HbStyleLoader::registerFilePath(":/");
    
    //Create the setting manager
    mSettingsManager = new XQSettingsManager(this);
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_CALENAGENDAVIEWWIDGET_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::~CalenAgendaViewWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
EXPORT_C CalenAgendaViewWidget::~CalenAgendaViewWidget()
{
    OstTraceFunctionEntry0( DUP1_CALENAGENDAVIEWWIDGET_CALENAGENDAVIEWWIDGET_ENTRY );
    
	// Unload notes editor if loaded.
	if (mNotesEditorPluginLoader) {
		mNotesEditorPluginLoader->unload();
		delete mNotesEditorPluginLoader;
		mNotesEditorPluginLoader = 0;
	}

    if (mSettingsManager) {
    	delete mSettingsManager;
    	mSettingsManager = NULL;
    }
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_CALENAGENDAVIEWWIDGET_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::setupWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::setupWidget(CalenAgendaView *view)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_SETUPWIDGET_ENTRY );
    
    // Store the view for future reference
	mView = view;
	
	if (!mDocLoader) {
	    // Nothing much can be done. Simply return
	    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_SETUPWIDGET_EXIT );
	    return;
	}

	// Initialize the child widgets
	initChildWidgets();
	OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_SETUPWIDGET_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::showWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::showWidget()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_SHOWWIDGET_ENTRY );
    
    // Get the date for which this view has been launched
    mDate = mServices.Context().focusDateAndTime();
        
    // Set the heading text
    setHeadingText();

    // Get the instance list
    getInstanceList();
    
    // Check if regional information needs to be shown
    // and add it or remove it
    showHideRegionalInformation();

    // Load the appropriate section based on the number of events for the day
    if (0 == mInstanceArray.count()) {
        // There are no entries for the day
        // Load and show the empty list section
        bool loadSuccess = false;

        // Hide the list which shows the events
        mEventsList->hide();

        // Show the empty list text
        mEmptyListLabel->show();

        // Get the current device orientation
        int orientation = mServices.MainWindow().orientation();
        if (Qt::Vertical == orientation) {
            mDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, "emptyList", &loadSuccess);
        } else {
            mDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, "emptyLandscape", &loadSuccess);
        }
        if (!loadSuccess) {
            qFatal("calenagendaviewwidget.cpp : Unable to load empty list section");
        }
    } else {
        // Hide the empty view label
        mEmptyListLabel->hide();

        // Show the events list
        mEventsList->show();

        // There are one or more events. Load the non-empty section
        bool loadSuccess = false;
        mDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, "nonEmptyList", &loadSuccess);
        if (!loadSuccess) {
            qFatal("calenagendaviewwidget.cpp : Unable to load non-empty list section");
        }
        // Now populate the list with the events
        populateListWidget();
    }
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_SHOWWIDGET_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::orientationChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::orientationChanged(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_ORIENTATIONCHANGED_ENTRY );
    
    // Load the appropriate section based on the number of events for the day
    if (0 == mInstanceArray.count()) {
        // There are no entries for the day
        // Load and show the empty list section
        bool loadSuccess = false;

        // Hide the list which shows the events
        mEventsList->hide();

        // Show the empty list text
        mEmptyListLabel->show();

        if (Qt::Vertical == orientation) {
            mDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, "emptyList", &loadSuccess);
        } else {
            mDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, "emptyLandscape", &loadSuccess);
        }
        if (!loadSuccess) {
            qFatal("calenagendaviewwidget.cpp : Unable to load empty list section");
        }
    } else {
        // Hide the empty view label
        mEmptyListLabel->hide();

        // Show the events list
        mEventsList->show();

        // There are one or more events. Load the non-empty section
        bool loadSuccess = false;
        mDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, "nonEmptyList", &loadSuccess);
        if (!loadSuccess) {
            qFatal("calenagendaviewwidget.cpp : Unable to load non-empty list section");
        }
        handleListItemStretching(orientation);
    }
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_ORIENTATIONCHANGED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::handleLocaleChange
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::handleLocaleChange()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_HANDLELOCALECHANGE_ENTRY );

    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_HANDLELOCALECHANGE_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::setContextFromHighlight
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::setContextFromHighlight(AgendaEntry entry)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_SETCONTEXTFROMHIGHLIGHT_ENTRY );
    
    if (entry.isTimedEntry()) {
        // Timed entry.
        QDateTime start = entry.startTime();
        // start.setDate(mDate.date());
        mServices.Context().setFocusDateAndTimeAndInstance(start,
                                                            TCalenInstanceId::create(entry));
    } else {
        // Untimed entry.
        if (CalenDateUtils::onSameDay(TCalenInstanceId::create(entry).mInstanceTime,
                                      mDate)) {
            mServices.Context().setInstanceId(TCalenInstanceId::create(entry));
        } else {
            // Multi-day untimed note (either multi-day day note or weird todo).
            // Focus on the instance and set the focus time to the highlighted day.
            mServices.Context().setFocusDateAndTimeAndInstance(mDate,
                                                                TCalenInstanceId::create(entry));
        }
    }
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_SETCONTEXTFROMHIGHLIGHT_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::initChildWidgets
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::initChildWidgets()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_INITCHILDWIDGETS_ENTRY );
    
    // Get the pointer to the events list
    mEventsList = static_cast<HbListView*> (mDocLoader->findWidget(CALEN_AGENDAVIEW_LISTWIDGET));
    if (!mEventsList) {
        qFatal("calenagendaviewwidget.cpp : Unable to find the events list");
    }
    // NOTE: Layout name MUST be same as the name mentioned in the css
    mEventsList->setLayoutName(customLayout);
    
    // TODO : remove this line after gestures are available
    mEventsList->installEventFilter(mView);
    
    // Enable the pixmap cache for better scrolling performance
    mEventsList->setItemPixmapCacheEnabled(true);
        
    // Connect to the long press and activation signals
    connect(mEventsList, SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
            this, SLOT(itemLongPressed(HbAbstractViewItem*, const QPointF&)));
    connect(mEventsList, SIGNAL(activated(const QModelIndex&)), this,
            SLOT(itemActivated(const QModelIndex&)));
    
    // Get the pointer to label from the loader.
    mHeadingLabel = qobject_cast<HbGroupBox*> (mDocLoader->findWidget(CALEN_AGENDAVIEW_HEADING));
    if (!mHeadingLabel) {
        qFatal("calenagendaviewwidget.cpp : Unable to find heading widget");
    }
    
    // Get the pointer to the empty view label
    mEmptyListLabel = qobject_cast<HbLabel*> (mDocLoader->findWidget("noEventsLabel"));
    if (!mEmptyListLabel) {
        qFatal("calenagendaviewwidget.cpp : Unable to find empty list label");
    }
    HbWidget *headingPluginWidget = 
    		qobject_cast<HbWidget*> (mDocLoader->findWidget(CALEN_AGENDAVIEW_HEADING_REGIONALPLUGIN_WIDGET));
    
    mRegionalPluginLayout = static_cast<QGraphicsLinearLayout*>(headingPluginWidget->layout());
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_INITCHILDWIDGETS_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::populateListWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::populateListWidget()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_POPULATELISTWIDGET_ENTRY );
    
    // Recycle the list items so that only needed rows
    // are added or removed
    if (mInstanceArray.count() == 0) {
        // Reset the list
        mEventsList->reset();
        // Clear the model to ensure it does not have any old items
        mListModel->clear();
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_POPULATELISTWIDGET_EXIT );
        return;
    } else if (mInstanceArray.count() > mListModel->rowCount()) {
        // There are more events than the number of items
        // in the list. Add more rows
        mListModel->insertRows(0, mInstanceArray.count() - mListModel->rowCount());
    } else if (mInstanceArray.count() < mListModel->rowCount()) {
        // There are less number of events than the number
        // of items in the list. Remove the extra rows
        mListModel->removeRows(0, mListModel->rowCount() - mInstanceArray.count());
    }
    mListModel->setColumnCount(singleColumn);
    // Add all the events to the list
    for (int index = 0; index < mInstanceArray.count(); index++) {
        // Get each of the entry details
        AgendaEntry entry = mInstanceArray[index];
        // Create a list item for each entry
        createListItem(index, entry);
    }
    // Add the item on to the list widget
    mEventsList->setModel(mListModel, mListViewPrototype);
    
    // Now get the item to scroll to
    int scrollToIndex = getIndexToScrollTo();
    if (mListModel->rowCount() > 0) {
        // Scroll to the index only if index is valid
        mEventsList->scrollTo(mListModel->index(scrollToIndex, 0));
    }
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_POPULATELISTWIDGET_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::getInstanceList
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::getInstanceList()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_GETINSTANCELIST_ENTRY );
    
    // Clear the previous instance list
    mInstanceArray.clear();
    
    // Check if the date is valid
    if (!CalenDateUtils::isValidDay(mDate)) {
        mDate = CalenDateUtils::today();
    }
    
    // Set the filter flags to fetch all relevant entries
    AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries |
                                                             AgendaUtil::IncludeAppointments | 
                                                             AgendaUtil::IncludeEvents |
                                                             AgendaUtil::IncludeReminders |
                                                             AgendaUtil::IncludeIncompletedTodos);
    
    // Fetch the instance list from the agenda interface
    mInstanceArray = mServices.agendaInterface()->createEntryIdListForDay(mDate, filter);
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_GETINSTANCELIST_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::setDateToLabel
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::setHeadingText()
    {
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_SETHEADINGTEXT_ENTRY );
    
    // Format the date as per the device locale settings
	HbExtendedLocale systemLocale = HbExtendedLocale::system();
	
	// Get localised name of the day from locale
	QString dayString = systemLocale.dayName(mDate.date().dayOfWeek());
	// Append a single space
	dayString.append(singleSpace);
	// Set the heading

    // Append the date which is formatted as per the locale   
    mHeadingLabel->setHeading(QString("%1 %2").arg(dayString).arg(
        systemLocale.format(mDate.date(), r_qtn_date_usual_with_zero)));
	
	
	OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_SETHEADINGTEXT_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::createListItem
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::createListItem(int index, AgendaEntry entry)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_CREATELISTITEM_ENTRY );
    
    if (index < 0 || entry.isNull()) {
        // Not a valid index or entry. Dont do anything
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_CREATELISTITEM_EXIT );
        return;
    }
    // Check if the entry is a timed entry and not all day event
    // an all-day event when it is synched from OUTLOOK then entry type will be EAppt
    if (!CalenAgendaUtils::isAlldayEvent(entry) && entry.isTimedEntry()) {
        // Get the text and icon data for the entry
        addTimedEventToList(index, entry);
    } else {
        addNonTimedEventToList(index, entry);
    }
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_CREATELISTITEM_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::addTimedEventToList
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::addTimedEventToList(int index, AgendaEntry entry)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_ADDTIMEDEVENTTOLIST_ENTRY );
    
    // Create text and icon list to set to the model
    QVariantList textData;
    QVariantList iconData;
    bool twoLines = false;
    
    /**
     * IMPORTANT NOTE: All the text and icon items must be
     * inserted into the variant list in a specific order.
     * If not, chaos will set it!!!
     * On a more serious note, the mesh layout depends on the
     * fact that the icons and text data will be added in a
     * particular order. If one of the items go missing, the
     * layout might get messed up. Also in case a particular item
     * is not required to be added in the middle of the list, an
     * empty QVariant must be added to indicate to the list view
     * that the item must be removed from the layout.
     * Do not mess with the order in case you are not sure what
     * you are exactly doing.
     */
    
    // The first icon to show to-do or all day should not be shown
    // Append empty data to the icons list
    iconData << QVariant();

    // Check if alarm is enabled for the entry       
    if (entry.alarm().isNull()) {
        // Insert a blank icon. Else the next icon
        // will get replaced in this icon's position
        iconData << QVariant();
    } else {
        iconData << HbIcon(reminderIcon);
    }

    // Check if the entry is recurring
    if (entry.isRepeating()) {
        iconData << HbIcon(repeatIcon);
    } else if (!entry.recurrenceId().isNull()) {
        // This is an exceptional entry
        iconData << HbIcon(exceptionIcon);
    } else {
        // Insert a blank icon. Else the next icon
        // will get replaced in this icon's position
        iconData << QVariant();
    }

    HbExtendedLocale locale = HbExtendedLocale::system();
	// Get the start time and format as per the locale
	QDateTime startTime = entry.startTime();
	QTime eventStartTime;

	if (CalenDateUtils::beginningOfDay(startTime)
	        < CalenDateUtils::beginningOfDay(mDate)) {
		// event is started previous day, show StarTime as 12:00 am in Agendaview, 
		eventStartTime.setHMS(00, 00, 00);
	} else {
		eventStartTime = entry.startTime().time();
	}
    QString eventTime = locale.format(eventStartTime, r_qtn_time_usual_with_zero);

	// Get the event end time
	QDateTime endTime = entry.endTime();
	QTime eventEndTime;

	if (CalenDateUtils::beginningOfDay(endTime)
	        > CalenDateUtils::beginningOfDay(mDate)) {
		// event has MidNight crossover, show EndTime as 11:59pm in Agendaview, 
		eventEndTime.setHMS(23, 59, 59);
	} else {
		eventEndTime = entry.endTime().time();
	}

    if (eventStartTime < eventEndTime) {
        // Raise the flag to indicate that the list item
        // would wrap to two lines
        twoLines = true;
        // Append '-' to indicate an end time is present
        eventTime.append(singleHyphen);
    } else {
    	// To align the subject properly if the start and endtime are same
    	eventTime.append(doubleSpace);
    }
    
    // Add the event time to the text list
    // This MUST be the first item to be added to the list 
    textData << eventTime;

    // Get the entry subject
    QString subject = entry.summary();
    if (subject.isEmpty() || subject.isNull()) {
        // No subject. Add "(No subject)" text
        subject.append(hbTrId("txt_calendar_dblist_unnamed"));
    }
    // Add the subject to the text list
    // This MUST be the second item in the list
    textData << subject;

    // Check if the entry has location
    // TODO: This must change to check for geo-location
    // This MUST be the third item in the list
    if (entry.location().isEmpty()) {
        // Insert a blank icon. Else the next icon
        // will get replaced in this icon's position
        iconData << QVariant();
        if (twoLines) {
            /**
             * NOTE: If the primary text is wrapping to two
             * lines, then the location must be set as blank
             * since the bottom of the list item is anchored to
             * the bottom of the secondary text. Else the bottom
             * of the list will overlap on the primary text
             */
            textData << singleSpace;
        } else {
            /**
             * Else, set the secondary text to empty to indicate
             * to the list view to remove the item itself and 
             * shrink its size to a single line size
             */
            textData << QVariant();
        }
    } else {
        // TODO: Location icon must be shown
        // only if valid geo-coordinates are present
        if (!entry.geoValue().isNull()){    
            iconData << HbIcon(locationIcon);
        }
        else {
            iconData << QVariant();
        }
        textData << entry.location();
    }
    // Add the end time to the list item	
    if (eventStartTime < eventEndTime) {
    	QString endtime = locale.format(eventEndTime, r_qtn_time_usual_with_zero);
    	endtime.append(singleSpace);
    	textData << endtime;
    }else {
    	textData<<QString(space);
    }

    
    // Disable item stretching by removing the dynamic property
    HbListViewItem *listViewItem = static_cast<HbListViewItem*>
                                        (mEventsList->itemByIndex(mListModel->index(index, 0)));
    if (listViewItem) {
        listViewItem->setProperty(stretchLayout, false);
    }
    // Get the list model index and set the text and icon data
    QModelIndex listIndex = mListModel->index(index, 0);
    mListModel->setData(listIndex, textData, Qt::DisplayRole);
    mListModel->setData(listIndex, iconData, Qt::DecorationRole);    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_ADDTIMEDEVENTTOLIST_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::addNonTimedEventToList
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::addNonTimedEventToList(int index, AgendaEntry entry)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_ADDNONTIMEDEVENTTOLIST_ENTRY );
    
    QVariantList textData;
    QVariantList iconData;
    
    /**
     * IMPORTANT NOTE: All the text and icon items must be
     * inserted into the variant list in a specific order.
     * If not, chaos will set it!!!
     * On a more serious note, the mesh layout depends on the
     * fact that the icons and text data will be added in a
     * particular order. If one of the items go missing, the
     * layout might get messed up. Also in case a particular item
     * is not required to be added in the middle of the list, an
     * empty QVariant must be added to indicate to the list view
     * that the item must be removed from the layout.
     * Do not mess with the order in case you are not sure what
     * you are exactly doing.
     */
    
    // The first column text has to be empty
    textData << QVariant();
    
    // Get the entry subject
    QString subject = entry.summary();
    if (subject.isEmpty() || subject.isNull()) {
        // No subject. Add "(No subject)" text
        subject.append(hbTrId("txt_calendar_dblist_unnamed"));
    }
    // Add the subject to the text list
    // This MUST be the second item in the list
    textData << subject;
    
    // Get the entry type
    AgendaEntry::Type entryType = entry.type();
        
    if (entryType == AgendaEntry::TypeAnniversary) {
        // Nothing to do as of now as anniversary events
        // cannot be created
        // TODO: To be handled in case of sync
    } else if (CalenAgendaUtils::isAlldayEvent(entry)) {
        // This is an all day event
        // Append the all-day icon
        iconData << HbIcon(allDayIcon);
        
        // Check if alarm is enabled for the entry
        if (entry.alarm().isNull()) {
            // Insert a blank icon. Else next text item will get shifted to left
            iconData << QVariant();
        } else {
        	// if entry is not repeating in place of reminder icon put a blank 
        	// icon and move reminder icon to the place of repeating icon 
        	 if (!entry.isRepeating()) {
                 iconData << QVariant();
        	 }
            iconData << HbIcon(reminderIcon);
        }

        // Check if the entry is recurring
        if (entry.isRepeating()) {
            iconData << HbIcon(repeatIcon);
        } else if (!entry.recurrenceId().isNull()) {
            // This is an exceptional entry
            iconData << HbIcon(exceptionIcon);
        } else {
        	// put the blank icon only when both reminder and repeating icons 
        	// are not there
        	if (entry.alarm().isNull()) {
        	iconData << QVariant();
        	}
        }
        
        // Append the location
        if (!entry.location().isEmpty()) {
            textData << entry.location();
            // TODO: Location icon must be shown
            // only if valid geo-coordinates are present
            if (!entry.geoValue().isNull()){ 
                iconData << HbIcon(locationIcon);
            }else {
                iconData << QVariant();
           
            }
        } else {
            textData << QVariant();
        }
        // The fourth text item has to be empty
        textData << QVariant();
        
    } else if (entryType == AgendaEntry::TypeTodo) {
        // Append the to-do icon
        iconData << HbIcon(toDoIcon);
        
        // Get the due date
        QDateTime dueDate = entry.endTime();
        
        // Append the date first
        QString dueDateString(hbTrId("txt_calendar_dblist_val_due_on_1"));
        QString dueText;
        dueText.setNum(dueDate.date().day());
        dueText.append(singleSpace);
        
        // Append the month name next
        HbExtendedLocale locale = HbExtendedLocale::system();
        QString month = locale.monthName(dueDate.date().month());
        dueText.append(month);
        dueText.append(singleSpace);
        
        // Append the year
        QString year;
        year.setNum(dueDate.date().year());
        dueText.append(year);
        
        textData << dueDateString.arg(dueText);
        
        // The fourth text item has to be empty
        textData << QVariant();
       
        // Check if alarm is enabled for the entry
        if (entry.alarm().isNull()) {
        	// Insert a blank icon. Else next text item will get shifted to left
        	iconData << QVariant();
        } else {
        	// if entry is not repeating in place of reminder icon put a blank 
        	// icon and move reminder icon to the place of repeating icon 
        	if (!entry.isRepeating()) {
        		iconData << QVariant();
        	}
        	iconData << HbIcon(reminderIcon);
        }

        // Check if the entry is recurring
        if (entry.isRepeating()) {
        	iconData << HbIcon(repeatIcon);
        } else {
        	// Insert the blank icon only when both reminder and repeating icons 
        	// are not there
        	if (entry.alarm().isNull()) {
        		iconData << QVariant();
        	}
        }
    }
    // Enable item stretching by adding the dynamic property
    HbListViewItem *listViewItem = static_cast<HbListViewItem*>
                                        (mEventsList->itemByIndex(mListModel->index(index, 0)));
    Qt::Orientation orientation = mServices.MainWindow().orientation();
    // Apply stretch only for landscape orientation
    if (listViewItem) {
        if (Qt::Horizontal == orientation) {
            listViewItem->setProperty(stretchLayout, true);
        } else {
            listViewItem->setProperty(stretchLayout, false);
        }
    }
    // Get the list model index and set the text and icon data
    QModelIndex listIndex = mListModel->index(index, 0);
    mListModel->setData(listIndex, textData, Qt::DisplayRole);
    mListModel->setData(listIndex, iconData, Qt::DecorationRole);
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_ADDNONTIMEDEVENTTOLIST_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::handleListItemStretching
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaViewWidget::handleListItemStretching(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_HANDLELISTITEMSTRETCHING_ENTRY );
    
    if (mInstanceArray.count() == 0) {
        // Nothing much to do. Simply return
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_HANDLELISTITEMSTRETCHING_EXIT );
        return;
    }
    for(int index = 0; index < mInstanceArray.count() ; index ++) {
        AgendaEntry entry = mInstanceArray[index];
        if (!entry.isNull()) {
            AgendaEntry::Type eventType = entry.type();
            // Need to check explicitly for Appt that got synched from OUTLOOK
            // that can be an all-day event
            if (AgendaEntry::TypeAppoinment == eventType) {
            	if (CalenAgendaUtils::isAlldayEvent(entry)) {
            		eventType = AgendaEntry::TypeEvent;
            	}
            }
            switch(eventType) {
                // Apply the stretching to only to-do's,
                // anniversary and all-day event types
                case AgendaEntry::TypeTodo:
                case AgendaEntry::TypeEvent:
                case AgendaEntry::TypeAnniversary:
                {
                    // Get the list view item corresponding to the index
                    HbListViewItem *listItem = static_cast<HbListViewItem*>
                                                (mEventsList->itemByIndex(mListModel->index(index, 0)));
                    if (listItem) {
                        if (orientation == Qt::Horizontal) {
                            // Set a dynamic property to indicate that this list item
                            // must be stretched in landscape.
                            // NOTE: Property name MUST match the name specified in
                            // css file, else wierd things might happen
                            listItem->setProperty(stretchLayout, true);
                        }
                        if (orientation == Qt::Vertical) {
                            // Set a dynamic property to indicate that this list item
                            // NOTE: Property name MUST match the name specified in
                            // css file, else wierd things might happen
                            listItem->setProperty(stretchLayout, false);
                        }
                    }
                }
                    break;
                default:
                {
                    HbListViewItem *listItem = static_cast<HbListViewItem*>
                                                (mEventsList->itemByIndex(mListModel->index(index, 0)));
                    if (listItem) {
                        listItem->setProperty(stretchLayout, false);
                    }
                    break;
                }
            }
        }
    }
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_HANDLELISTITEMSTRETCHING_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::getIndexToScrollTo
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//  
int CalenAgendaViewWidget::getIndexToScrollTo()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_GETINDEXTOSCROLLTO_ENTRY );
    
    int scrollIndex = 0;
    TCalenInstanceId instanceId = mServices.Context().instanceId();
    if (instanceId == TCalenInstanceId::nullInstanceId()) {
        // If the instance is not set, then scroll to zero index
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_GETINDEXTOSCROLLTO_EXIT );
        return scrollIndex;
    }
    
    for (int index = 0 ; index < mInstanceArray.count() ; index++) {
        AgendaEntry entry = mInstanceArray[index];
        if (entry.id() == instanceId.mEntryLocalUid) {
            // Match found.
            scrollIndex = index;
            break;
        }
    }
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_GETINDEXTOSCROLLTO_EXIT );
    return scrollIndex;
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::showHideRegionalInformation
// To Show and hide regional plugin label depends upon settings
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::showHideRegionalInformation()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_SHOWHIDEREGIONALINFORMATION_ENTRY );
    
	if (mView->pluginEnabled()) {
		XQSettingsKey regionalInfo(XQSettingsKey::TargetCentralRepository,
		                           KCRUidCalendar, KCalendarShowRegionalInfo);

		int showRegionalInfo = 
						mSettingsManager->readItemValue(regionalInfo).toUInt();
		if (showRegionalInfo) {

			if (!mRegionalInfoGroupBox) {
				mRegionalInfoGroupBox = new HbGroupBox();
				CalenPluginLabel *regionalInfo = new CalenPluginLabel(
						mServices, this);
				regionalInfo->setFontSpec(HbFontSpec(HbFontSpec::Primary));
				mRegionalInfoGroupBox->setContentWidget(regionalInfo);
				mRegionalPluginLayout->insertItem(1, mRegionalInfoGroupBox);
			}
			QString *pluginString = mView->pluginText();
			HbLabel *pluginInfoLabel = qobject_cast <HbLabel *> 
									(mRegionalInfoGroupBox->contentWidget());
			pluginInfoLabel->setPlainText(*pluginString);
		}
    } else {
        if (mRegionalInfoGroupBox) {
        	mRegionalPluginLayout->removeItem(mRegionalInfoGroupBox);
            delete mRegionalInfoGroupBox;
            mRegionalInfoGroupBox = NULL;
        }
    }
	
	OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_SHOWHIDEREGIONALINFORMATION_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::createNewEvent
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::createNewEvent()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_CREATENEWEVENT_ENTRY );
    
    // Issue a command to launch editor to create
    // a new event
	mServices.IssueCommandL(ECalenNewMeeting);
	
	OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_CREATENEWEVENT_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::editEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::editEntry()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_EDITENTRY_ENTRY );
    
    // Check if the selected index is valid
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_EDITENTRY_EXIT );
        return;
    }

	// Get the entry details first
	AgendaEntry entry = mInstanceArray[mSelectedIndex];
	if (AgendaEntry::TypeTodo == entry.type()) {
		// Load the notes editor plugin if not loaded.
		if (!mNotesPluginLoaded) {
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

		interface->edit(entry, mServices.agendaInterface());

		connect(
				interface, SIGNAL(editingCompleted(bool)),
				this, SLOT(noteEditingCompleted(bool)));
	} else {
		// Set the context
		setContextFromHighlight(entry);
		// Issue a command to launch the editor to edit this entry
		mServices.IssueCommandL(ECalenEditCurrentEntry);
	}
	
	OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_EDITENTRY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::viewEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::viewEntry()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_VIEWENTRY_ENTRY );
    
    // Get the entry details first
    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    
    // Set the context
    setContextFromHighlight(entry);

    // Launch the event viewer.
    mServices.IssueCommandL(ECalenEventView);
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_VIEWENTRY_EXIT );
}


// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::deleteEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::deleteEntry()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_DELETEENTRY_ENTRY );
    
    // Check if the selected index is valid
	if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
		OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_DELETEENTRY_EXIT );
		return;
	}
	
	// Get the entry details
	AgendaEntry entry = mInstanceArray[mSelectedIndex];
	// Set the context
	setContextFromHighlight(entry);
	// Issue the command to delete the entry
	mServices.IssueCommandL(ECalenDeleteCurrentEntry);
	
	OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_DELETEENTRY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::markAsDone
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//  
void CalenAgendaViewWidget::markAsDone()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_MARKASDONE_ENTRY );
    
    // Check if the selected index is valid
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_MARKASDONE_EXIT );
        return;
    }
    
    // Get the entry details
    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    
    // Check again if the event is of type to-to
    if (AgendaEntry::TypeTodo == entry.type()) {
        // Set the status of the to-do as completed
        entry.setStatus(AgendaEntry::TodoCompleted);
        // Update the completed date and time
        entry.setCompletedDateTime(mDate);
        
        // Update the entry in the database
        mServices.agendaInterface()->setCompleted(entry, true, mDate);
		mServices.IssueCommandL(ECalenStartActiveStep);
    }
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_MARKASDONE_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::itemLongPressed
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::itemLongPressed(HbAbstractViewItem* listViewItem,
                                         const QPointF& coords)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_ITEMLONGPRESSED_ENTRY );
    
	mLongTapEventFlag = true;
    // Update the selection index first
    mSelectedIndex = listViewItem->modelIndex().row();
    
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        // Invalid index
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_ITEMLONGPRESSED_EXIT );
        return;
    }

    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    
    // Create new menu.
    HbMenu *contextMenu = new HbMenu();
    
    // Add the open option
    HbAction *openAction = contextMenu->addAction(
									hbTrId("txt_common_menu_open"));
    
    // Check the type of event
    if (AgendaEntry::TypeTodo == entry.type()) {
        // Add an option to mark the note as complete
        HbAction *completeAction = contextMenu->addAction(
									hbTrId("txt_calendar_menu_mark_as_done"));
    }
    
    // Add the edit option
    HbAction *editAction = contextMenu->addAction(
									hbTrId("txt_common_menu_edit"));
    
    // Add the delete option
    HbAction *deleteAction = contextMenu->addAction(
									hbTrId("txt_common_menu_delete"));
    
    contextMenu->setDismissPolicy(HbMenu::TapAnywhere);

    // Show context sensitive menu. 
    // Param const QPointF& coordinate - is a longpress position.
    contextMenu->setPreferredPos(coords);
    connect(contextMenu, SIGNAL(aboutToClose()),
								this, 
								SLOT(contextMenuClosed()));
    
    // Close the context menu once closeDialogs() is received
    connect(mView, SIGNAL(closeDialogs()), contextMenu, SLOT(close()));
    
    contextMenu->open(this, SLOT(contextManuTriggered(HbAction *)));
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_ITEMLONGPRESSED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::itemActivated
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaViewWidget::itemActivated(const QModelIndex &index)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_ITEMACTIVATED_ENTRY );
    
    // Update the selection index first
    mSelectedIndex = index.row();

    // Check if the selected index is valid
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_ITEMACTIVATED_EXIT );
        return;
    }
    if( !mLongTapEventFlag ) {
    // Open the event for viewing
    viewEntry();
    }
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_ITEMACTIVATED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::itemActivated
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaViewWidget::noteEditingCompleted(bool status)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_NOTEEDITINGCOMPLETED_ENTRY );
    
	// We need to refresh the list since user
	// might have marked the to-do as complete or
	// edited it or deleted it. So get the instance
	// list again
	if (status) {
		mServices.IssueCommandL(ECalenStartActiveStep);
	}
	
	OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_NOTEEDITINGCOMPLETED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::goToToday
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaViewWidget::goToToday()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_GOTOTODAY_ENTRY );
    
    // First check if we are not already
    // showing today's agenda
    if (mDate == CalenDateUtils::today()) {
        OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_GOTOTODAY_EXIT );
        return;
    }
    
    // Set the context for the current day
    mServices.Context().setFocusDate(CalenDateUtils::today());
    
    mView->refreshViewOnGoToDate();
    
    OstTraceFunctionExit0( DUP1_CALENAGENDAVIEWWIDGET_GOTOTODAY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::contextMenuClosed
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::contextMenuClosed()
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_CONTEXTMENUCLOSED_ENTRY );
    
	mLongTapEventFlag = false;
	
	OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_CONTEXTMENUCLOSED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::contextManuTriggered
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaViewWidget::contextManuTriggered(HbAction *action)
{
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_CONTEXTMANUTRIGGERED_ENTRY );
    
	if (action->text() == hbTrId("txt_common_menu_open")) {
		viewEntry();
	} else if (action->text() == hbTrId("txt_calendar_menu_mark_as_done")) {
		markAsDone();
	} else if (action->text() == hbTrId("txt_common_menu_edit")) {
		editEntry();
	} else {
		if (action->text() == hbTrId("txt_common_menu_delete")) {
			deleteEntry();
		}
	}
	
	OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_CONTEXTMANUTRIGGERED_EXIT );
}
// ----------------------------------------------------------------------------
// CalenAgendaViewWidget::clearListModel
// clears the list model 
// ----------------------------------------------------------------------------
// 
void CalenAgendaViewWidget::clearListModel()
    {
    OstTraceFunctionEntry0( CALENAGENDAVIEWWIDGET_CLEARLISTMODEL_ENTRY );
    
    mListModel->clear();
    
    OstTraceFunctionExit0( CALENAGENDAVIEWWIDGET_CLEARLISTMODEL_EXIT );
    }


// End of file	--Don't remove this.
