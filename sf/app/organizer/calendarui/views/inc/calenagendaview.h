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
* Description: Class declaration for the agenda view
*
*/

#ifndef CALENAGENDAVIEW_H
#define CALENAGENDAVIEW_H

// System includes
#include <QDateTime>

// User includes
#include "calennativeview.h"

// Forward declarations
class QGraphicsSceneMouseEvent;
class CalenAgendaViewWidget;
class CalenDocLoader;

#ifdef  CALENVIEWS_DLL
#define CALENAGENDAVIEW_EXPORT Q_DECL_EXPORT
#else
#define CALENAGENDAVIEW_EXPORT Q_DECL_IMPORT
#endif

/**
 * @class CalenAgendaView
 * @brief Class declaration for the agenda view
 * which shows the list of events &/ to-do's (if any)
 * for any given day in the calendar. User can do other
 * basic operations like creating, viewing or deleting
 * an event
 */
class CalenAgendaView : public CalenNativeView
    {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param services A reference to the MCalenServices object
     */
    CALENAGENDAVIEW_EXPORT CalenAgendaView(MCalenServices &services);
    
    /**
     * @brief Default C++ constructor
     */
    CALENAGENDAVIEW_EXPORT virtual ~CalenAgendaView();
    
    /**
     * @brief Initializes the view after its creation
     * Must be called only after the view and its children are constructed
     * @param docLoader Pointer to the document loader object which has
     * loaded this view from the .docml file
     */
    CALENAGENDAVIEW_EXPORT void setupView(CalenDocLoader *docLoader);
    
    /**
     * @brief Activates the current view. Sets this view as the current
     * view of the application's main window. Must be called only after
     * the view has been constructed and initialized
     */
    CALENAGENDAVIEW_EXPORT virtual void doPopulation();
    
    /**
     * @brief Getter function for the document loader. Called from the content
     * widget in order get its children
     */
    CALENAGENDAVIEW_EXPORT CalenDocLoader* docLoader();

    /**
     * @brief clears the list model from the view
     */
    CALENAGENDAVIEW_EXPORT void clearListModel();

    /**
     * disconnectAboutToQuitEvent disconnects for the aboutToQuit events
     * 
     */
    CALENAGENDAVIEW_EXPORT void disconnectAboutToQuitEvent();
    
	/*
	 * @brief Refreshes the view with the date set in the context
	 */
    void refreshViewOnGoToDate();
    
protected:
    void gestureEvent(QGestureEvent *event);

private:
    
    /**
     * @brief Connects to all the toolbar and menu actions associated
     * with this view
     */
	void setupActions();
	
	/**
	 * @brief Callback function for listening to locale changes like
	 * time format, date format etc
	 */
	void onLocaleChanged(int reason);
	
private slots:
	
    /**
	 * @brief Slot which is called whenever the orientation of the device changes
	 * @param orientation The current device orientation
	 */
	void orientationChanged(Qt::Orientation orientation);
	
	/**
	 * @brief Slot which launches back the month view
	 */
	void launchMonthView();
	
	/**
	 * @brief Slot which launches the day view
	 */
	void launchDayView();
	
	/**
	 * @brief Slot which launches the editor
	 */
	void createNewEvent();

private:
	
	/**
	 * @var mDate
	 * @brief The date for which this view is being shown
	 */
	QDateTime mDate;
	
	/**
	 * @var mSoftKeyAction
	 * TODO: This may not be required going ahead
	 */
	HbAction *mSoftKeyAction;
	
	/**
	 * @var mAgendaViewWidget
	 * @brief The pointer to the content widget of this view
	 */
	CalenAgendaViewWidget *mAgendaViewWidget;
	
	/**
	 * @var mDocLoader
	 * @brief Pointer to the document loader which has loaded
	 * this view
	 */
	CalenDocLoader *mDocLoader;
	
	/**
	 * @var mGoToTodayAction
	 * Action which provides "Go to today" functionality
	 */
	HbAction *mGoToTodayAction;
	
	/**
	 * @var mSwitchToDayViewAction
	 * Action which provides "Switch to day view" functionality
	 */
	HbAction *mSwitchToDayViewAction;
	
    // TODO: Remove these after gestures are available
    QPointF mTapPoint;
    bool    mActionTaken;
    bool    mIsAboutToQuitEventConnected; // bool to check if agenda view is registered to get aboutToQuit signals
    bool    mDayViewActivated;
};

#endif /* CALENAGENDAVIEW_H */

// End of file	--Don't remove this.
