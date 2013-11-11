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
* Description: Homescreen calendar widget's content layout handler.
*
*/

#ifndef CONTENTLAYOUTHANDLER_H
#define CONTENTLAYOUTHANDLER_H

// System includes
#include <QObject>
#include <QGraphicsLinearLayout>

/*!
  \class AgendaUtil
  \brief Interface to the calendar database.
  \details It is used by ContentLayoutHandler to retrieve the list of calendar entries in the next seven days.\n\n
  <b>QList<AgendaEntry> AgendaUtil::fetchEntriesInRange(
            QDateTime rangeStart, QDateTime rangeEnd,
            AgendaUtil::FilterFlags filter = AgendaUtil::IncludeAll)</b>
      
  is the function used to retrieve the list of calendar entries in range. It returns a list of calendar events.
  @param rangeStart is the range date start. In calendar widget is always today.
  @param rangeEnd is the range date end. In calendar widget is always one week after today.
  @param filter is the type of event we want to include in the list. Calendar widget includes only All-Day and timed events.
*/

// Forward declarations
class AgendaEntry;
class AgendaUtil;
class HbDocumentLoader;
class HbFrameDrawer;
class HbFrameItem;
class HbLabel;
class HbWidget;
class QGraphicsLayout;
class QPointF;
class QTimer;
class XQServiceRequest;
class XQSettingsManager;
class XQSettingsKey;
class TestContentLayoutHandler;
class QDateTime;
class QRectF;
//from DateIconLayoutHandler
class DateTimeObserver;
/*!
  \class ContentLayoutHandler
  \brief Decorates the content layout.
*/
// Class declaration
class ContentLayoutHandler: public QObject, public QGraphicsLinearLayout
{
    Q_OBJECT
    Q_PROPERTY(int testID READ testId WRITE setTestId)

public:
    /*! 
        \fn ContentLayoutHandler::ContentLayoutHandler()
        
        Constructor of the ContentLayoutHandler class.
     */
    ContentLayoutHandler();
    /*! 
        \fn ContentLayoutHandler::~ContentLayoutHandler()
        
        Destructor of the ContentLayoutHandler class.
     */
    ~ContentLayoutHandler();

    //Test property functions
    /*! 
        \fn int ContentLayoutHandler::testId()
        
        Used to check which test needs to run.
        @return Test id
     */
    int testId();
    /*! 
        \fn void ContentLayoutHandler::setTestId(int testID)
        
        Used to set which test it needs to run.
     */
    void setTestId(int testID);

signals:
     /*! 
        \fn void ContentLayoutHandler::requestComplete()
        
        Emitted in case the calendar has been launched correctly.
     */
    void requestComplete();
    /*! 
        \fn void ContentLayoutHandler::requestError()
        
        Emitted in case the calendar launch didn't succeed.
     */
    void requestError();
    /*! 
        \fn void ContentLayoutHandler::currentDate(QDateTime& date)
        
        Emits the current date during layout decoration.
        @param date Current date
     */    
    void currentDate(QDateTime& date);
    
    //from DateIconLayoutHandler
    /*! 
        \fn void ContentLayoutHandler::dateChanged()
        
        Emitted when the date has changed.
     */        
    void dateChanged();
    
    //test signals
    void calendarChanged();
    void timerExpired();
    void themeChanged();

public slots:
    /*! 
        \fn void ContentLayoutHandler::eventTimerExpired()
        
        Connected to the timer signal timeout. The timer is needed in order to calculate the time that divides
        the current moment from the expiration of the next upcoming event. This slot is called when the timer expires,
        thus, its purpose is to decorate the layout with the next upcoming event and start the timer again.
    */
    void eventTimerExpired();
    /*! 
        \fn void ContentLayoutHandler::calendarChanged(const XQSettingsKey& key, const QVariant& value)
        
        Calendar P&S listener. Its purpose is to listen to the calendar P&S in order to be notified of any calendar
        event modifications. Whenever it receives any notifications from the calendar, it will decorate the layout.
        @param key The key to listen to
        @param value Key value
    */
    void calendarChanged(const XQSettingsKey& key, const QVariant& value);
    /*! 
        \fn void ContentLayoutHandler::initializeLayout(const HbDocumentLoader &layoutLoader, QObject *owner)
        
        Initializes the layout using the document loader. Connected to CalendarWidget.
        @param layoutLoader Document loader that contains the DOCML needed to load the layout
        @param owner Parent
    */
    void initializeLayout(const HbDocumentLoader &layoutLoader, QObject *owner);
    /*! 
        \fn void ContentLayoutHandler::updateLayout()
        
        Decorates the layout content.
    */
    void updateLayout();
    /*! 
        \fn void ContentLayoutHandler::onThemeChange()
        
        Connected to CalendarWidget. Called when a theme change events occurs. Its purpose is to load the new
        theme graphics into the widget.
    */
    void onThemeChange();
    /*! 
        \fn void ContentLayoutHandler::highlightOn(QPointF& point)
        
        Highlights the layout, if the point is inside the correct area.
        @param point The (scene) position of the highlighting event
    */
    void highlightOn(QPointF &point);
    /*! 
        \fn void ContentLayoutHandler::highlightOff()
        
        Removes highlighting from the layout. 
    */
    void highlightOff();

private:
    /*! 
        \fn void ContentLayoutHandler::hideLabels()
        
        Called when the widget is initialized to hide all the labels in the layout. 
    */
    void hideLabels();
    /*! 
        \fn void ContentLayoutHandler::setLabelsColor()
        
        Called when the widget is initialized and when a theme change occurs to change the labels text color.
    */
    void setLabelsColor();
    /*! 
        \fn void ContentLayoutHandler::removePassedEvents()
        
        Filters out the already passed events from the list of events that comes from the calendar database.
    */
    void removePassedEvents();
    /*! 
        \fn void ContentLayoutHandler::createAgendaUtilsInstance()
        
        Initializes the agenda utils instance.
    */
    void createAgendaUtilsInstance();
    /*! 
        \fn void ContentLayoutHandler::removeTodosFrom()
        
        Filters out the todo events from the list of events that comes from the calendar database.
        Also filters out other type of events that should be ignored by the calendar widget.
    */
    void removeTodosFrom();
    /*! 
        \fn void ContentLayoutHandler::todayHasValidEvents()
        
        Returns true if in the list of events there are upcoming events to be shown today.
        Returns false otherwise.
    */
    bool todayHasValidEvents();
    /*! 
        \fn void ContentLayoutHandler::sortList()
        
        Sorts the list of events according to the starting time.
    */
    void sortList();
    /*! 
        \fn void ContentLayoutHandler::showNoEventsNextWeek()
        
        If there are no events the next seven days, decorates the layout with that information. 
    */
    void showNoEventsNextWeek();
    /*! 
        \fn void ContentLayoutHandler::showNextEvent()
        
        Decorates the layout with the information about today's next upcoming event.
    */
    void showNextEvent();
    /*! 
        \fn void ContentLayoutHandler::showOverlappingEvents(int when, int numOverlappingEvents, int aEndEventIndex)
        
        Decorates the layout with the information about how many upcoming overlapping events there are,
        starting time and end time.
        @param when 1 = today, >1 = upcoming day
        @param numOverlappingEvents Number of overlapping events
        @param aEndEventIndex The index in the list of events of the last event that overlaps the one
        with index 0.
    */
    void showOverlappingEvents(int when, int numOverlappingEvents, int aEndEventIndex);
    /*! 
        \fn void ContentLayoutHandler::showNextDaysEvent()
        
        Decorates the layout with the information about the next days first upcoming event.
    */
    void showNextDaysEvent();
    /*! 
        \fn int ContentLayoutHandler::checkOverlappingEvents(ulong id, int& aEndEventIndex)
        
        Calculates the number of events overlapping event with id \em id.
        @param id The id of the event we want to find overlapping events to
        @param aEndEventIndex The index of the last event that is overlapping the event id
        @return Number of overlapping events. 
    */
    int checkOverlappingEvents(ulong id, int& aEndEventIndex);
    /*! 
        \fn void ContentLayoutHandler::updateList()
        
        Asks calendar database to provide an updated list of events in range now, now + 7 days.
    */
    void updateList();
    /*! 
        \fn void ContentLayoutHandler::decorateContent()
        
        Contains the layout decoration logic. Called whenever an update is needed and upon widget initialization.
    */
    void decorateContent();
    /*! 
        \fn QString ContentLayoutHandler::currentDateFormat()
        
        Checks whether the current date format is english, american or 
        japanese and formats the date string accordingly.
        @return The date string formatted with the current date format
    */
    QString currentDateFormat();
    /*! 
        \fn QString ContentLayoutHandler::currentTimeFormat()
        
        Checks whether the current time format is 12 hours or 24 hours and formats the date string accordingly.
        @return The time string formatted with the current time format
    */
    QString currentTimeFormat();
    /*! 
        \fn int ContentLayoutHandler::timeToNextCheck()
        
        Calculates the time that needs to pass before we need to check whether a layout decoration is needed or not.
        @return Number of milliseconds to the next check
    */
    int timeToNextCheck();
    /*! 
        \fn int ContentLayoutHandler::initializeTimer()
        
        Initializes the timer and calculates the time to the first check.
    */
    void initializeTimer();
    /*! 
        \fn QString ContentLayoutHandler::calculateEndTime(int& aEndEventIndex)
        
        Calculates when the overlapping situation ends by checking the end 
        time of all the involved events and choose the biggest one.
        @param aEndEventIndex The index of the last overlapping event in the list of events
        @return The end time as a string
    */
    QString calculateEndTime(int& aEndEventIndex);
    /*! 
        \fn int ContentLayoutHandler::firstEntryIndex()
        
        Calculates the index of the first timed event in the list of events.
        @return The index of the first timed event
    */
    int firstEntryIndex();
    /*! 
        \fn void ContentLayoutHandler::setTimerForNextUpdate()
        
        Checks whether there are events in the list and timeToNextCheck is not zero and then sets the timer to 
        timeToNextCheck values.
    */
    void setTimerForNextUpdate();
    /*! 
        \fn QDateTime ContentLayoutHandler::currentDateTime()
        
        Get current date, either real device date or fake date during testing.
        @return Date
    */
    QDateTime currentDateTime();
    /*! 
        \fn void ContentLayoutHandler::setLabelText(HbLabel* label, QString text)
        
        Sets the label text. Checks whether the label pointer is valid or not.
        @param label The label to set the text to
        @param text The text to be set
    */
    void setLabelText(HbLabel* label, QString text);
    /*! 
        \fn void ContentLayoutHandler::setLabelVisible(HbLabel* label, bool visible)
        
        Sets visible or invisible a specified label. It checks whether the label pointer is valid or not.
        @param label The label to set the text to
        @param visible Label visible, true or false
    */
    void setLabelVisible(HbLabel* label, bool visible);
    /*! 
        \fn bool ContentLayoutHandler::isAllDayEvent(AgendaEntry entry)
        
        Checks whether a specific entry is an all day event or not.
        @param entry Entry to check
        @return True, if entry is an all day event, otherwise false
    */
    bool isAllDayEvent(AgendaEntry entry);

    /*! 
        \fn bool ContentLayoutHandler::isOneDayAllDayEvent(AgendaEntry entry)
            
        Checks whether the entry is an all day event with the duration of one day.
        @param entry Entry to check
        @return True, if entry is an one day all day event, otherwise false
    */
    bool isOneDayAllDayEvent(AgendaEntry entry);
    
    //from DateIconLayoutHandler
    /*! 
        \fn void ContentLayoutHandler::setCalendarIconContent()
        
        Updates date number label and month label texts.
    */    
    void setCalendarIconContent();
    /*! 
        \fn void ContentLayoutHandler::setLowerLabelOverlapping(int numberOfEvents)
        
        Sets the lower label text to indicate overlapping events.
        @param numberOfEvents Number of overlapping events to insert in the localized text
    */
    void setLowerLabelOverlapping(int numberOfEvents);
    
private: // data
    Q_DISABLE_COPY(ContentLayoutHandler)

    HbWidget* mContainer;

    HbLabel* mReminderLabel;
    HbLabel* mUpperLabelShort;
    HbLabel* mUpperLabelLong;
    HbLabel* mMiddleLabel;
    HbLabel* mLowerLabel;
    
    //From DateIconLayoutHandler
    // Containers
    HbWidget* mIconContainer;
    
    // Labels (not owned)
    HbLabel* mIconLabel;
    HbLabel* mNumberLabel;
    HbLabel* mMonthLabel;
    HbLabel* mHighlightLabel;
        
    // Background drawer (not owned)
    HbFrameDrawer* mDateIconFrameDrawer;

    // Owned
    DateTimeObserver* mDateObserver;
    //From MainLayoutHandler
    HbWidget* mWidget;

    // Background label
    HbLabel* mBgLabel;
    
    // Separator
    HbLabel* mSeparatorLabel;

    //to paint icon and widget background
    HbFrameItem* mIconLayoutItem;
    HbFrameItem* mBackgroundLayoutItem;
    HbFrameItem* mSeparatorLayoutItem;
    //END
    // Frame drawer for reminder icon (not owned)
    HbFrameDrawer* mReminderDrawer;
    //timer used to know when the current shown event disappears
    QTimer* mEventTimer;

    //Agenda Utils
    AgendaUtil* mAgendaUtil;
    QList<AgendaEntry> mAgendaEntryList;
    XQSettingsManager* mCalendarKeyManager;
    
    //label visibility
    bool mReminderVisible;
    bool mUpperLabelLongVisible;
    bool mUpperLabelShortVisible;

    // State variables
    bool mDateHighlightActive;
    bool mContentHighlightActive;
    
    //test variables
    int mTestID;
    int mTestResult;

    friend class TestContentLayoutHandler;
};

#endif // CONTENTLAYOUTHANDLER_H
