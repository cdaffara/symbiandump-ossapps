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
* Description:  CalenAgendaViewWidget class definition.
*
*/

#ifndef CALENAGENDAVIEWWIDGET_H
#define CALENAGENDAVIEWWIDGET_H

// System includes
#include <QGraphicsLinearLayout>
#include <QDateTime>
#include <QPointer>
#include <hbwidget.h>

// User includes
 
// Forward declarations
class QStandardItemModel;
class QGraphicsSceneMouseEvent; // TODO: Remove this after gestures are supported
class HbAbstractViewItem;
class HbGroupBox;
class HbListView;
class QPluginLoader;
class HbLabel;
class HbAction;
class HbDateTimePicker;
class XQSettingsManager;
class MCalenServices;
class CalenAgendaView;
class AgendaEntry;
class CalenDocLoader;
class CalenEventListViewItem;
class NotesEditor;

/**
 * @class CalenAgendaViewWidget
 * @brief The content widget for the agenda view.
 * It is responsible for showing all the events &/ to-do's
 * (if any) for a particular day selected by the user
 */
class CalenAgendaViewWidget : public HbWidget
    {
    Q_OBJECT

public:
    
    /**
     * @brief Constructor
     * @param services The reference to the MCalenServices object
     * @param docLoader The document loader object
     */
	IMPORT_C CalenAgendaViewWidget(MCalenServices &services,
	                                 CalenDocLoader *docLoader);
	
	/**
	 * @brief Default C++ destructor
	 */
	IMPORT_C ~CalenAgendaViewWidget();
	
	/**
	 * @brief Initializes the widget to show all the events/to-do's
	 * for the selected day
	 * @param view Pointer to the view to which this is the content
	 */
    void setupWidget(CalenAgendaView *view);
    
    /**
     * @brief Prepares the widget to be shown.
     * Initializes the widget with the day's events and other details 
     */
    void showWidget();
    
    /**
     * @brief Called from the view whenever the orientation of the
     * device changes
     */
    void orientationChanged(Qt::Orientation);
    
    /**
     * @brief Callback function to handle system locale change
     */
    void handleLocaleChange();
    
    /**
     * @brief clears the list model from the view
     */
    void clearListModel();
    
    /**
     * @brief Opens the editor to create a new entry
     */
    void createNewEvent();
    
private:
    
    /**
     * @brief Sets the context based on the item selected by the user
     * @param entry The entry using which the context is deduced
     */
    void setContextFromHighlight(AgendaEntry entry);
    
    /**
     * @brief Initializes the child widgets of this widget
     */
    void initChildWidgets();
    
    /**
     * @brief Populates the list with the events for the current day.
     * Does nothing if the day has no events. To be called whenever
     * the view has to be refreshed
     */
    void populateListWidget();
    
    /**
     * @brief Gets the instance list for day
     */
    void getInstanceList();
    
    /**
     * @brief Sets the heading text to show the date for which this
     * widget is being shown
     */
    void setHeadingText();
    
    /**
     * @brief Creates and adds an item to the list at position index
     * @param index The position in the list when the item is added
     * @param entry The entry whose details must be shown in the item
     */
    void createListItem(int index, AgendaEntry entry);
    
    /**
     * @brief Creates and add a list item for the event
     * @param index The position in the list when the item is added
     * @param entry The entry whose details must be shown in the item
     */
    void addTimedEventToList(int index, AgendaEntry entry);
    
    /**
     * @brief Creates and add a list item for the event which is non timed
     * @param index The position in the list when the item is added
     * @param entry The entry whose details must be shown in the item
     */
    void addNonTimedEventToList(int index, AgendaEntry entry);
    
    /**
     * @brief Sets strecthing to list items showing non-timed events
     */
    void handleListItemStretching(Qt::Orientation orientation);
    
    /**
     * @brief Returns the index in the list to which scrolling must
     * happen. For ex: After creating a new event, the list must scroll
     * to the newly created event
     */
    int getIndexToScrollTo();
    
    /**
     * @brief To Show and hide regional plugin label depends upon settings
     */
    
    void showHideRegionalInformation();

    
private slots:

    /**
     * @brief Opens the entry at position index in the list for
     * editing in the editor. Does nothing if the index is invalid
     */
    void editEntry();
    
    /**
     * @brief Opens the entry at position index in the list for
     * viewing in the viewer. Does nothing if the index is invalid
     */
    void viewEntry();
	
    /**
     * @brief Deletes the entry at position index in the list.
     * Does nothing if the list is empty
     */
    void deleteEntry();
    
    /**
     * @brief Marks a to-do as done
     */
    void markAsDone(); 
    
    /**
     * @brief Handles long press on a particular item. Presents a
     * context menu with various operations that the user can
     * perform
     * @param ite, The index item which is being long pressed
     * @param coords The (x, y) co-ordinates where the long tap has
     * occured
     */
    void itemLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    
    /**
     * @brief Handles user selection on a particular item of a
     * particular event in the list. Default action is to view it
     * @param index Index of the item which was selected 
     */
    void itemActivated(const QModelIndex &index);
    
    /**
     * @brief Called whenever a editing a to-do is completed
     * @param status Inidcates if the editing was successful or not
     */
    void noteEditingCompleted(bool status);
    
    /**
     * @brief Callback function for navigating to today's agenda
     * This option must be available only if the view is showing
     * events for some day other than the current day
     */
    void goToToday();
    
    /**
     * @brief This slot connected to aboutToClose()signal of contextmenu
     * To reset the flag of mLongTapEventFlag
     */
    void contextMenuClosed();
    
    /**
     * @brief Called when contextMenu item is triggered
     */
    void contextManuTriggered(HbAction *action);

private:
    /**
     * @var mDate
     * @brief Stores the date for which this widget is being shown
     */
    QDateTime mDate;
    
    /**
     * @var mInstanceArray
     * @brief Holds the list of instances for the day
     */
    QList<AgendaEntry> mInstanceArray;
    
    /**
     * @var mHeadingLabel
     * @brief The heading widget which shows the date
     */
    HbGroupBox *mHeadingLabel;
    
    /**
     * @var mEventsList
     * @brief The list view which shows the list of events
     */
    HbListView *mEventsList;

    /**
     * @var mServices
     * @brief The reference to the MCalenServices object
     */
    MCalenServices &mServices;
    
    /**
     * @var mView
     * @brief The pointer to the view
     */
    CalenAgendaView *mView;
    
    /**
     * @var mDocLoader
     * @brief The pointer to the document loader which has
     * loaded this widget
     */
    CalenDocLoader *mDocLoader;
    
    /**
     * @var mListViewPrototype
     * @brief Acts as the item prototype for list view.
     * Allows providing own layout for list items
     */
    CalenEventListViewItem *mListViewPrototype;
	
    /**
     * @var mEmptyListLabel
     * @brief The pointer to the empty view label
     */
	HbLabel *mEmptyListLabel;
	
	/**
	 * @var mListModel
	 * @brief The event list model
	 */
	QStandardItemModel *mListModel;
	
	/**
	 * @var mSelectedIndex
	 * @brief Stores the index of the item being acted upon
	 * Since there is no focus, it becomes necessary to store the
	 * index of the item on which user is performing some action
	 */
	int mSelectedIndex;
	
	/**
	 * @var NotesEditorPluginLoader
	 * @brief Pointer to the notes editor plugin loader
	 */
	QPointer<QPluginLoader> mNotesEditorPluginLoader;

	/**
	 * @var mDatePicker
	 * @brief The date picker component
	 */
	HbDateTimePicker *mDatePicker;
	
	/**
	 * @var mRegionalPluginLayout
	 * @brief Regional Plugin layout pointer
	 */
	QGraphicsLinearLayout* mRegionalPluginLayout;
	
	/**
	 * @var mRegionalInfoGroupBox
	 * @brief Regional Plugin Info groupbox
	 */
	HbGroupBox *mRegionalInfoGroupBox;

	/**
	 * @var mSettingsManager
	 * @brief Setting Manager pointer
	 */
	
	XQSettingsManager *mSettingsManager;
	
	/**
	 * @var mLongTapeventFlag
	 * @brief Set flag true if contextmenu is opened
	 */
	bool mLongTapEventFlag;

	/**
	 * @var mNotesPluginLoaded
	 * @brief Holds notes editor plugin load info.
	 */
	bool mNotesPluginLoaded;

};

#endif //CALENAGENDAVIEWWIDGET_H

// End of file	--Don't remove this.
