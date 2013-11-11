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

// System includes
#include <HbColorScheme>
#include <QDateTime>
#include <HbLabel>
#include <HbDocumentLoader>
#include <HbCore>
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <QTimer>
#ifndef TEST
#include <agendautil.h>
#else
#include "mockagendautil.h"
#endif
#include <agendaentry.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <calnotification.h>
#include <QGraphicsLayout>
#include <QPointF>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <QCoreApplication>

// User includes
#include "contentlayouthandler.h"
#include "calendarwidgetdebug.h"
#include "datetimeobserver.h"

// Local constants
namespace {
    const HbFrameDrawer::FrameType REMINDER_FRAME_TYPE = HbFrameDrawer::OnePiece;
    const char *CONTENT_CONTAINER = "contentContainer";
    const char *UPPER_LABEL_SHORT = "upperLabelShort";
    const char *UPPER_LABEL_LONG = "upperLabelLong";
    const char *MIDDLE_LABEL = "middleLabel";
    const char *LOWER_LABEL = "lowerLabel";
    const char *REMINDER_ICON_LABEL = "reminderLabel";
    const char *REMINDER_IMAGE_NAME = "qtg_small_reminder";
    const char *HIGHLIGHT_IMAGE_NAME = "qtg_fr_hsitems_pressed";
    //same color applies to upper and middle labels
    const char *UPPER_LABEL_COLOR = "qtc_hs_list_item_title_normal";
    const char *LOWER_LABEL_COLOR = "qtc_hs_list_item_content_normal";
    const char *HIGHLIGHT_LABEL_COLOR = "qtc_hs_list_item_pressed";
    // time constants
    const int KHalfHour = 30;
    const int KSecondInMillisec = 1000;
    const int KSecondsInMinute = 60;
    const int KToday = 1;
    const int KComingDays = 0;
    const int KDaysInWeek = 7;
    const int KHourMinuteSeparatorIndex = 1;
    const int KPassedEventTimeoutSecs = 1800; // Half hour
    
    //from DateIconLayoutHandler
    const HbFrameDrawer::FrameType DATE_ICON_BACKGROUND_FRAME_TYPE = HbFrameDrawer::OnePiece;
    const char *ICON_CONTAINER = "iconContainer";
    const char *DATE_ICON_LABEL = "iconLabel";
    const char *DATE_ICON_DAYNUMBER = "dayNumber";
    const char *DATE_ICON_MONTHLABEL = "month";
    const char *DATE_ICON_HIGHLIGHTLABEL = "iconHighlight";
    const char *NUMBER_LABEL_COLOR = "qtc_hs_cal";
    const char *MONTH_LABEL_COLOR = "qtc_hs_list_item_title_normal";
    //const char *HIGHLIGHT_LABEL_COLOR = "qtc_hs_list_item_pressed";
    const char *DATE_BACKGROUND = "qtg_large_calendar_dynamic";
    //const char *HIGHLIGHT_IMAGE_NAME = "qtg_fr_hsitems_pressed";
    
    //From MainLayoutHandler
    const char WIDGET[] = "CalendarWidget";
    const char *BACKGROUND_LABEL = "widgetBackground";
    const char *SEPARATOR_LABEL = "separatorLabel";
    const char *SEPARATOR_IMAGE_NAME = "qtg_graf_divider_v_thin";
    const char *BACKGROUND_IMAGE_NAME = "qtg_fr_hswidget_normal";
    const HbFrameDrawer::FrameType BACKGROUND_FRAME_TYPE = HbFrameDrawer::NinePieces;
    const HbFrameDrawer::FrameType SEPARATOR_FRAME_TYPE = HbFrameDrawer::OnePiece;
}

// ======== MEMBER FUNCTIONS ========

/*
 * ContentLayoutHandler::ContentLayoutHandler()
 */
ContentLayoutHandler::ContentLayoutHandler()
        : mContainer(0),
          mReminderLabel(0),
          mUpperLabelShort(0),
          mUpperLabelLong(0),
          mMiddleLabel(0),
          mLowerLabel(0),
          mIconContainer(0),
          mIconLabel(0),
          mNumberLabel(0),
          mMonthLabel(0),
          mHighlightLabel(0),
          mDateIconFrameDrawer(0),
          mReminderDrawer(0),
          mEventTimer(0),
          mAgendaUtil(0),
          mCalendarKeyManager(0),
          mDateHighlightActive(0),
          mContentHighlightActive(0),
          mDateObserver(0)
{
    LOGS("ContentLayoutHandler::ContentLayoutHandler");

    mCalendarKeyManager = new XQSettingsManager();
    unsigned long int key = (unsigned long int)ECalPubSubEventNotification;
    XQSettingsKey calendarKey(XQSettingsKey::TargetPublishAndSubscribe,
        KCalPubSubCategory.iUid, key);
    mCalendarKeyManager->startMonitoring(calendarKey);
    connect(mCalendarKeyManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(calendarChanged(XQSettingsKey, QVariant)));
}

/*
 * ContentLayoutHandler::~ContentLayoutHandler()
 */
ContentLayoutHandler::~ContentLayoutHandler()
{
    LOGS("ContentLayoutHandler::~ContentLayoutHandler");
    unsigned long int key = (unsigned long int)ECalPubSubEventNotification;
    XQSettingsKey calendarKey(XQSettingsKey::TargetPublishAndSubscribe,
        KCalPubSubCategory.iUid, key);
    mCalendarKeyManager->stopMonitoring(calendarKey);
    delete mCalendarKeyManager;
    if( mDateObserver ) {
        delete mDateObserver;   
    }
}

/*
 * ContentLayoutHandler::initializeLayout()
 */
void ContentLayoutHandler::initializeLayout(const HbDocumentLoader &layoutLoader,
    QObject *owner)
{
    this->setParent(owner);
    this->setObjectName("contentlayouthandler");
    
    //From MainLayoutHandler
    mWidget = (HbWidget*)layoutLoader.findWidget(WIDGET);
    mBgLabel = qobject_cast<HbLabel*>(layoutLoader.findWidget(BACKGROUND_LABEL));
    mSeparatorLabel = qobject_cast<HbLabel*>(layoutLoader.findWidget(QString(SEPARATOR_LABEL)));

    // Discontinue initialization of the main widget is missing
    if (!mWidget) {
        return;
    }

    addItem(mWidget);
    mWidget->setVisible(true);

    // Create main background drawer
    if (mBgLabel) {
        HbFrameDrawer* backgroundFrameDrawer = new HbFrameDrawer(BACKGROUND_IMAGE_NAME,
            BACKGROUND_FRAME_TYPE);
        mBackgroundLayoutItem = new HbFrameItem(backgroundFrameDrawer);
        mBgLabel->setBackgroundItem(mBackgroundLayoutItem);
    }
    
    // Create separator graphics drawer
    HbFrameDrawer* separatorFrameDrawer = new HbFrameDrawer(SEPARATOR_IMAGE_NAME,
        SEPARATOR_FRAME_TYPE);
    mSeparatorLayoutItem = new HbFrameItem(separatorFrameDrawer);
    mSeparatorLabel->setBackgroundItem(mSeparatorLayoutItem);

    setPreferredSize(mWidget->preferredSize());
    //END
    
    //From DateIconLayoutHandler
    mIconContainer = qobject_cast<HbWidget*>(layoutLoader.findWidget(QString(ICON_CONTAINER)));
    mIconLabel = qobject_cast<HbLabel*>(layoutLoader.findWidget(QString(DATE_ICON_LABEL)));
    mNumberLabel = qobject_cast<HbLabel*>(layoutLoader.findWidget(QString(DATE_ICON_DAYNUMBER)));
    mMonthLabel = qobject_cast<HbLabel*>(layoutLoader.findWidget(QString(DATE_ICON_MONTHLABEL)));
    mHighlightLabel = qobject_cast<HbLabel*>(layoutLoader.findWidget(QString(DATE_ICON_HIGHLIGHTLABEL)));

    // Create background drawer only if day container loading succeeded
    if (mIconLabel) {
        mDateIconFrameDrawer = new HbFrameDrawer(DATE_BACKGROUND, DATE_ICON_BACKGROUND_FRAME_TYPE);
        HbFrameItem* dateIconFrameItem = new HbFrameItem(mDateIconFrameDrawer);
        mIconLabel->setBackgroundItem(dateIconFrameItem);
    }

    setCalendarIconContent();
    setLabelsColor();

    // The icon label should never show text
    if (mIconLabel) {
        mIconLabel->clear();
    }
    if (mHighlightLabel) {
        mHighlightLabel->clear();
    }

    
    mDateObserver = new DateTimeObserver();
    connect(mDateObserver, SIGNAL(completed()), this, SIGNAL(dateChanged()));
    connect(this, SIGNAL(dateChanged()), this, SLOT(updateLayout()));
    //END
    
    // Get widget handles
    mContainer = qobject_cast<HbWidget*>(
        layoutLoader.findWidget(QString(CONTENT_CONTAINER)));
    mUpperLabelShort = qobject_cast<HbLabel*>(
        layoutLoader.findWidget(QString(UPPER_LABEL_SHORT)));
    mUpperLabelLong = qobject_cast<HbLabel*>(
        layoutLoader.findWidget(QString(UPPER_LABEL_LONG)));
    mMiddleLabel = qobject_cast<HbLabel*>(
        layoutLoader.findWidget(QString(MIDDLE_LABEL)));
    mLowerLabel = qobject_cast<HbLabel*>(
        layoutLoader.findWidget(QString(LOWER_LABEL)));
    mReminderLabel = qobject_cast<HbLabel*>(
        layoutLoader.findWidget(QString(REMINDER_ICON_LABEL)));

    // Create reminder icon drawer only if reminder label loading succeeded.
    if (mReminderLabel) {
        // Make sure there's no text in reminder icon label
        mReminderLabel->clear();
        // Create drawer for reminder icon
        mReminderDrawer = new HbFrameDrawer(REMINDER_IMAGE_NAME,
            REMINDER_FRAME_TYPE);
        //ownership transferred
        HbFrameItem* reminderFrameItem = new HbFrameItem(mReminderDrawer);
        //ownership transferred
        mReminderLabel->setBackgroundItem(reminderFrameItem);
    }

    setLabelsColor();
    hideLabels();
    createAgendaUtilsInstance();
    initializeTimer();
}

/*
 * ContentLayoutHandler::calendarChanged()
 */
void ContentLayoutHandler::calendarChanged(const XQSettingsKey& key,
    const QVariant& value)
{
    LOGS("ContentLayoutHandler::calendarChanged");
    Q_UNUSED(key);
    Q_UNUSED(value);
    updateLayout();
}

/*
 * ContentLayoutHandler::updateLayout()
 */
void ContentLayoutHandler::updateLayout()
{
    LOGS("ContentLayoutHandler::updateLayout");
    decorateContent();
    setTimerForNextUpdate();
    //From DateIconLayoutHandler
    setCalendarIconContent();
    
    //test signal
    emit calendarChanged();
}

/*
 * ContentLayoutHandler::onThemeChange()
 */
void ContentLayoutHandler::onThemeChange()
{
    LOGS("ContentLayoutHandler::onThemeChange");
    if (mReminderDrawer) {
        mReminderDrawer->themeChanged();
    }
    
    if (mDateIconFrameDrawer) {
        mDateIconFrameDrawer->themeChanged();
    }
    
    if (mBackgroundLayoutItem) {
        mBackgroundLayoutItem->frameDrawer().themeChanged();
    }
    if (mSeparatorLayoutItem) {
        mSeparatorLayoutItem->frameDrawer().themeChanged();
    }
    
    setLabelsColor();
    //test results
    mTestID = 1;
    emit themeChanged();
}

/*
 * ContentLayoutHandler::createAgendaUtilsInstance()
 */
void ContentLayoutHandler::createAgendaUtilsInstance()
{
    LOGS("ContentLayoutHandler::createAgendaUtilsInstance");
    mAgendaUtil = new AgendaUtil(this);
    connect(mAgendaUtil, SIGNAL(entryViewCreationCompleted(int)),
        this, SLOT(updateLayout()));
}

/*
 * ContentLayoutHandler::initializeTimer()
 */
void ContentLayoutHandler::initializeTimer()
{
    LOGS("ContentLayoutHandler::initializeTimer");
    mEventTimer = new QTimer();
    mEventTimer->setSingleShot(true);
    connect(mEventTimer, SIGNAL(timeout()), this, SLOT(eventTimerExpired()));
    setTimerForNextUpdate();
}

/*
 * ContentLayoutHandler::hideLabels()
 */
void ContentLayoutHandler::hideLabels()
{
    LOGS("ContentLayoutHandler::hideLabels");
    setLabelVisible(mMiddleLabel, false);
    setLabelVisible(mUpperLabelShort, false);
    setLabelVisible(mUpperLabelLong, false);
    setLabelVisible(mLowerLabel, false);
    setLabelVisible(mReminderLabel, false);
    mReminderVisible = false;
    mUpperLabelLongVisible = false;
    mUpperLabelShortVisible = false;
}

/*
 * ContentLayoutHandler::setLabelsColor()
 */
void ContentLayoutHandler::setLabelsColor()
{
    LOGS("ContentLayoutHandler::setLabelsColor");
    
    // --- Content layout ---
    
    // Choose text colors according to highlight state
    QString upperLabelColorAttribute;
    QString lowerLabelColorAttribute;
    if (mContentHighlightActive) {
        upperLabelColorAttribute = HIGHLIGHT_LABEL_COLOR;
        lowerLabelColorAttribute = HIGHLIGHT_LABEL_COLOR;
    }
    else {
        upperLabelColorAttribute = UPPER_LABEL_COLOR;
        lowerLabelColorAttribute = LOWER_LABEL_COLOR;
    }
    QColor upperLabelTextColor(HbColorScheme::color(upperLabelColorAttribute));
    QColor lowerLabelTextColor(HbColorScheme::color(lowerLabelColorAttribute));
    
    // Set upper and middle label text color
    if (upperLabelTextColor.isValid()) {
        if (mUpperLabelShort) {
            mUpperLabelShort->setTextColor(upperLabelTextColor);
        }
        if (mUpperLabelLong) {
            mUpperLabelLong->setTextColor(upperLabelTextColor);
        }
        if (mMiddleLabel) {
            mMiddleLabel->setTextColor(upperLabelTextColor);
        }
    }

    // Set lower label text color
    if (lowerLabelTextColor.isValid()) {
        if (mLowerLabel) {
            mLowerLabel->setTextColor(lowerLabelTextColor);
        }
    }
    
    // --- DateIconLayoutHandler ---
    QString numberLabelColorAttribute;
    QString monthLabelColorAttribute;
    if (mDateHighlightActive) {
        numberLabelColorAttribute = NUMBER_LABEL_COLOR;
        monthLabelColorAttribute = HIGHLIGHT_LABEL_COLOR;
    }
    else {
        numberLabelColorAttribute = NUMBER_LABEL_COLOR;
        monthLabelColorAttribute = MONTH_LABEL_COLOR;
    }
    QColor numberLabelTextColor(HbColorScheme::color(numberLabelColorAttribute));
    QColor monthLabelTextColor(HbColorScheme::color(monthLabelColorAttribute));
    
    // Set day number label text color
    if (numberLabelTextColor.isValid()) {
        if (mNumberLabel) {
            mNumberLabel->setTextColor(numberLabelTextColor);
        }
    }
    // Set month label text color
    if (monthLabelTextColor.isValid()) {
        if (mMonthLabel) {
            mMonthLabel->setTextColor(monthLabelTextColor);
        }
    }
}

/*
 * ContentLayoutHandler::eventTimerExpired
 */
void ContentLayoutHandler::eventTimerExpired()
{
    LOGS("ContentLayoutHandler::eventTimerExpired");
    decorateContent();
    setTimerForNextUpdate();
    //test signal
    emit timerExpired();
}

/*
 * ContentLayoutHandler::timeToNextCheck()
 */
int ContentLayoutHandler::timeToNextCheck()
{
    LOGS("ContentLayoutHandler::timeToNextCheck");
    QDateTime now = currentDateTime();
    int secondsToNextCheck = 0;
    int currentSeconds = 0;
    currentSeconds = now.toTime_t();
    int entryStartTimeSeconds = 0;
    int entryEndTimeSeconds = 0;
    int indexEntryToCheck = firstEntryIndex();

    QDateTime startTime = mAgendaEntryList.at(indexEntryToCheck).startTime();
    QDateTime endTime = mAgendaEntryList.at(indexEntryToCheck).endTime();
    entryStartTimeSeconds = startTime.toTime_t();
    entryEndTimeSeconds = endTime.toTime_t();

    //check if the event is bigger than half hour
    if ((entryEndTimeSeconds - entryStartTimeSeconds) < (KHalfHour * KSecondsInMinute)) {
        secondsToNextCheck = entryEndTimeSeconds - currentSeconds;
    }
    else {
        //check that it's not an all time event.
        //if it's an all day event there's no need to set the timer because there is already a date check and the widget content
        //will be updated anyway.
        if (!isAllDayEvent(mAgendaEntryList.at(indexEntryToCheck))) {
            secondsToNextCheck = entryStartTimeSeconds + KHalfHour * KSecondsInMinute
                - currentSeconds;
        }
    }

    int mSecsToNextCheck = 0;
    if (secondsToNextCheck > INT_MAX / KSecondInMillisec) {
        mSecsToNextCheck = INT_MAX;
    }
    else {
        //seconds to milliseconds
        mSecsToNextCheck = secondsToNextCheck * KSecondInMillisec;
    }

    return mSecsToNextCheck;
}

/*
 * ContentLayoutHandler::firstEntryIndex()
 */
int ContentLayoutHandler::firstEntryIndex()
{
    int indexEntryToCheck = 0;
    LOGS("ContentLayoutHandler::firstEntryIndex()");

    //let's find the first non all day event in the list
    while (isAllDayEvent(mAgendaEntryList.at(indexEntryToCheck))
        && (indexEntryToCheck < mAgendaEntryList.count() - 1)) {
        indexEntryToCheck++;
    }

    //if we found the first valid non all day event as last in the array then we have to keep that index
    if ((indexEntryToCheck == mAgendaEntryList.count() - 1)
        && isAllDayEvent(mAgendaEntryList.at(indexEntryToCheck))) {
        indexEntryToCheck = 0;
    }

    return indexEntryToCheck;
}

/*
 * ContentLayoutHandler::decorateContent()
 */
void ContentLayoutHandler::decorateContent()
{
    LOGS("ContentLayoutHandler::decorateContent");
    updateList();

    if (mAgendaEntryList.count()) {
        removeTodosFrom();
        //it removes the events that are shorter than 30 minutes and are already passed
        removePassedEvents();
    }

    //there might be only ToDos. Let's check again.
    if (mAgendaEntryList.count()) {
        int endEventIndex = 0;
        if (todayHasValidEvents()) {
            int numOverlappingEvents = checkOverlappingEvents(0, endEventIndex);
            if (!numOverlappingEvents) {
                showNextEvent();
            }
            else {
                showOverlappingEvents(KToday, numOverlappingEvents, endEventIndex);
            }
        }
        else {
            int numOverlappingEvents = checkOverlappingEvents(0, endEventIndex);
            if (!numOverlappingEvents) {
                showNextDaysEvent();
            }
            else {
                showOverlappingEvents(KComingDays, numOverlappingEvents, endEventIndex);
            }
        }
    }
    else {
        showNoEventsNextWeek();
    }
    
    QDateTime date;
    if (mAgendaEntryList.count()) {
        date = mAgendaEntryList.at(0).startTime();
    }
    else {
        date = QDateTime::currentDateTime();
    }
    
    emit currentDate(date);
}

/*
 * ContentLayoutHandler::updateList()
 */
void ContentLayoutHandler::updateList()
{
    LOGS("ContentLayoutHandler::updateList");
    
    // Calculate start of event fetching range
    QDateTime previousMidnight(currentDateTime().date(), QTime(0, 0));
    QDateTime passedEventStart(currentDateTime().addSecs(-KPassedEventTimeoutSecs));
    QDateTime rangeStart;
    if (previousMidnight < passedEventStart) {
        rangeStart = previousMidnight;
    }
    else {
        rangeStart = passedEventStart;
    }

    // Calculate end of event fetching range
    QDateTime rangeEnd(previousMidnight.addDays(KDaysInWeek+1));
    
    // Fetch events
    mAgendaEntryList = mAgendaUtil->fetchEntriesInRange(rangeStart, rangeEnd);
    
    QString agenda_count = "count: "
        + QString::number(mAgendaEntryList.count(), 10);
    LOGS(agenda_count);
    sortList();
}

/*
 * ContentLayoutHandler::removePassedEvents()
 */
void ContentLayoutHandler::removePassedEvents()
{
    LOGS("ContentLayoutHandler::removePassedEvents");
    for (int i = 0; i < mAgendaEntryList.count(); i++) {
        //add half hour
        QDateTime entryStartTime =
            mAgendaEntryList.at(i).startTime().addSecs(KHalfHour * KSecondsInMinute);
        LOGS(entryStartTime.toString());
        LOGS(currentDateTime());
        //if the entry start time plus half hour is less than current time or it is not but the entry duration is less than half hour (endtime < currenttime)
        //and the entry is not an all day event then remove it because it is passed.
        if (((entryStartTime < currentDateTime())
            || (mAgendaEntryList.at(i).endTime() < currentDateTime()))
            && !isAllDayEvent(mAgendaEntryList.at(i))) {
            mAgendaEntryList.removeAt(i);
            i--;
        }
    }
}

/*
 * ContentLayoutHandler::currentDateFormat()
 */
QString ContentLayoutHandler::currentDateFormat()
{
    LOGS("ContentLayoutHandler::currentDateFormat");
    QString format;
    HbExtendedLocale locale;
    switch (locale.dateStyle()) {
        case HbExtendedLocale::American:
        case HbExtendedLocale::Japanese:
            format = QString("MMM dd");
            break;
        case HbExtendedLocale::European:
            format = QString("dd MMM");
            break;
        default:
            format = QString("dd:MM");
            break;
    }        
    return format;
}

/*
 * ContentLayoutHandler::currentTimeFormat()
 */
QString ContentLayoutHandler::currentTimeFormat()
{
    LOGS("ContentLayoutHandler::currentTimeFormat");
    QString format;
    HbExtendedLocale locale;
    QChar separator = locale.timeSeparator(KHourMinuteSeparatorIndex);
    format = QString("hh") + separator + QString("mm");
    if (locale.timeStyle() == HbExtendedLocale::Time12) {
        if (locale.amPmSymbolPosition() == HbExtendedLocale::Before) {
            if (locale.amPmSpace()) {
                format.prepend(' ');
            }
            format.prepend("ap");
        }
        else {
            if (locale.amPmSpace()) {
                format.append(' ');
            }
            format.append("ap");
        }    
    }
    return format;
}

/*
 * ContentLayoutHandler::showNextDaysEvent()
 */
void ContentLayoutHandler::showNextDaysEvent()
{
    LOGS("ContentLayoutHandler::showNextDaysEvent");
    setLabelVisible(mMiddleLabel, false);
    setLabelVisible(mUpperLabelShort, true);
    setLabelVisible(mLowerLabel, true);
    QString day = mAgendaEntryList.at(0).startTime().date().toString(currentDateFormat());
    QString startTime = mAgendaEntryList.at(0).startTime().time().toString(currentTimeFormat());
    QString endTime = mAgendaEntryList.at(0).endTime().time().toString(currentTimeFormat());
    QString description = mAgendaEntryList.at(0).summary();
    if( description == "" ) {
        //TODO: replace Unnamed with the correct localisation string when it will be available
        description = "Unnamed";
    }
    if (!isAllDayEvent(mAgendaEntryList.at(0))) {
        //if not all day event show the start time
        if (mAgendaEntryList.at(0).startTime().date() == mAgendaEntryList.at(0).endTime().date()) {
            setLabelText(mUpperLabelShort, day + " " + startTime /*+ " " + description*/);
            setLabelText(mLowerLabel, description/*hbTrId("txt_calendar_widget_v_dblist_val_no_events_tod")*/);
        }
        else {
            //if the end date is different from the starting date show that information
            setLabelText(mUpperLabelShort, day + " " + startTime + "-"
                + mAgendaEntryList.at(0).endTime().date().toString(currentDateFormat()) /*+ " "
                + description*/);
            setLabelText(mLowerLabel, description/*hbTrId("txt_calendar_widget_v_dblist_val_no_events_tod")*/);
        }
    }
    else {
        // It's an all day event
        if (isOneDayAllDayEvent(mAgendaEntryList.at(0))) {
            // It's a one day all day event
            setLabelText(mUpperLabelShort, day /*+ " " + description*/);
            setLabelText(mLowerLabel, description /*hbTrId("txt_calendar_widget_v_dblist_val_no_events_tod")*/);
        }
        else {
            // It's a multiple day all day event
            setLabelText(mUpperLabelShort, day + " - "
                + mAgendaEntryList.at(0).endTime().date().addDays(-1).toString(currentDateFormat()) /*+ " "
                + description*/);
            setLabelText(mLowerLabel, description/*hbTrId("txt_calendar_widget_v_dblist_val_no_events_tod")*/);
        }
    }

    int timeoffset = mAgendaEntryList.at(0).alarm().timeOffset();
    if ( mAgendaEntryList.at(0).alarm().timeOffset() != -1 ) {
        setLabelVisible(mReminderLabel, true);
    }
    else {
        setLabelVisible(mReminderLabel, false);
    }
}

/*
 * ContentLayoutHandler::showOverlappingEvents()
 */
void ContentLayoutHandler::showOverlappingEvents(int when, int numOverlappingEvents,
    int aEndEventIndex)
{
    LOGS("ContentLayoutHandler::showOverlappingEvents");
    if (when == KToday) {
        setLabelVisible(mMiddleLabel, false);
        setLabelVisible(mUpperLabelShort, true);
        setLabelVisible(mLowerLabel, true);
        QString startTime = mAgendaEntryList.at(0).startTime().time().toString(currentTimeFormat());
        QString endTime = calculateEndTime(aEndEventIndex);
        QString day = mAgendaEntryList.at(0).startTime().date().toString(currentDateFormat());
        if (!isAllDayEvent(mAgendaEntryList.at(0))) {
            if (mAgendaEntryList.at(0).startTime().date()
                == mAgendaEntryList.at(aEndEventIndex).endTime().date()) {
                //if not all day event show the start time and end time
                setLabelText(mUpperLabelShort, startTime + "-" + endTime);
                setLowerLabelOverlapping(numOverlappingEvents);
            }
            else {
                setLabelText(mUpperLabelShort, startTime + "-"
                    + mAgendaEntryList.at(aEndEventIndex).endTime().date().toString(
                        currentDateFormat()) + " "
                    + mAgendaEntryList.at(aEndEventIndex).endTime().time().toString(
                        currentTimeFormat()));
                setLowerLabelOverlapping(numOverlappingEvents);
            }
        }
        else {
            // It's an all day event
            if (isOneDayAllDayEvent(mAgendaEntryList.at(0))) {
                // It's a one day all day event
                setLabelText(mUpperLabelShort, day);
                setLowerLabelOverlapping(numOverlappingEvents);
            }
            else {
                // It's a multiple day all day event
                setLabelText(mUpperLabelShort, day + " - "
                    + mAgendaEntryList.at(0).endTime().date().addDays(-1).toString(currentDateFormat()));
                setLowerLabelOverlapping(numOverlappingEvents);
            }
        }
    }
    else {
        setLabelVisible(mMiddleLabel, false);
        setLabelVisible(mUpperLabelShort, true);
        setLabelVisible(mLowerLabel, true);
        QString day = mAgendaEntryList.at(0).startTime().date().toString(currentDateFormat());
        QString startTime = mAgendaEntryList.at(0).startTime().time().toString(currentTimeFormat());
        QString endTime = calculateEndTime(aEndEventIndex);
        if (!isAllDayEvent(mAgendaEntryList.at(0))) {
            if (mAgendaEntryList.at(0).startTime().date()
                == mAgendaEntryList.at(aEndEventIndex).endTime().date()) {
                //if not all day event show the start time and end time
                setLabelText(mUpperLabelShort, day + " " + startTime + "-" + endTime);
                setLowerLabelOverlapping(numOverlappingEvents);
            }
            else {
                setLabelText(mUpperLabelShort, day + " " + startTime + "-"
                    + mAgendaEntryList.at(0).endTime().date().toString(currentDateFormat()));
                setLowerLabelOverlapping(numOverlappingEvents);
            }
        }
        else {
            // It's an all day event
            if (isOneDayAllDayEvent(mAgendaEntryList.at(0))) {
                // It's a one day all day event
                setLabelText(mUpperLabelShort, day);
                setLowerLabelOverlapping(numOverlappingEvents);
            }
            else {
                // It's a multiple day all day event
                setLabelText(mUpperLabelShort, day + " - "
                    + mAgendaEntryList.at(0).endTime().date().addDays(-1).toString(currentDateFormat()));
                setLowerLabelOverlapping(numOverlappingEvents);
            }
        }
    }

    // Never show reminder icon for overlapping events
    setLabelVisible(mReminderLabel, false);
}

/*
 * ContentLayoutHandler::calculateEndTime()
 */
QString ContentLayoutHandler::calculateEndTime(int& aEndEventIndex)
{
    LOGS("ContentLayoutHandler::calculateEndTime");
    QDateTime current = mAgendaEntryList.at(0).endTime();
    int endTimeIndex = 0;
    for (int i = 0; i < aEndEventIndex + 1; i++) {
        if (mAgendaEntryList.at(i).endTime() > current) {
            current = mAgendaEntryList.at(i).endTime();
            endTimeIndex = i;
        }
    }

    aEndEventIndex = endTimeIndex;
    return current.time().toString(currentTimeFormat());
}

/*
 * ContentLayoutHandler::showNextEvent()
 */
void ContentLayoutHandler::showNextEvent()
{
    LOGS("ContentLayoutHandler::showNextEvent");
    setLabelVisible(mMiddleLabel, false);
    setLabelVisible(mUpperLabelShort, true);
    setLabelVisible(mLowerLabel, true);
    QString startTime = mAgendaEntryList.at(0).startTime().time().toString(currentTimeFormat());
    QString endTime = mAgendaEntryList.at(0).endTime().time().toString(currentTimeFormat());
    QString description = mAgendaEntryList.at(0).summary();
    if( description == "" ) {
        //TODO: replace Unnamed with the correct localisation string when it will be available
        description = "Unnamed";
    }
    if (!isAllDayEvent(mAgendaEntryList.at(0))) {
        //if not all day event show the start time and end time
        if (mAgendaEntryList.at(0).startTime().date() == mAgendaEntryList.at(0).endTime().date()) {
            setLabelText(mUpperLabelShort, startTime + "-" + endTime);
            setLabelText(mLowerLabel, description);
        }
        else {
            //if the end date is different from the starting date show that information
            setLabelText(mUpperLabelShort, startTime + "-"
                + mAgendaEntryList.at(0).endTime().date().toString(currentDateFormat()) + " "
                + endTime);
            setLabelText(mLowerLabel, description);
        }
    }
    else {
        // It's an all day event
        if (isOneDayAllDayEvent(mAgendaEntryList.at(0))) {
            // It's a one day all day event
            setLabelText(mUpperLabelShort,
                mAgendaEntryList.at(0).startTime().date().toString(currentDateFormat()));
            setLabelText(mLowerLabel, description);
        }
        else {
            // It's a multiple day all day event
            setLabelText(mUpperLabelShort,
                mAgendaEntryList.at(0).startTime().date().toString(currentDateFormat())
                + " - " + mAgendaEntryList.at(0).endTime().date().addDays(-1).toString(
                currentDateFormat()));
            setLabelText(mLowerLabel, description);
        }
    }

    //check if reminder is set
    if ( mAgendaEntryList.at(0).alarm().timeOffset() != -1 ) {
        setLabelVisible(mReminderLabel, true);
    }
    else {
        setLabelVisible(mReminderLabel, false);
    }
}

/*
 * ContentLayoutHandler::showNoEventsNextWeek()
 */
void ContentLayoutHandler::showNoEventsNextWeek()
{
    LOGS("ContentLayoutHandler::showNoEventsNextWeek");
    setLabelText(mMiddleLabel, hbTrId("txt_calendar_widget_v_dblist_val_no_events_ox"));
    setLabelVisible(mMiddleLabel, true);
    setLabelVisible(mUpperLabelShort, false);
    setLabelVisible(mLowerLabel, false);
    setLabelVisible(mReminderLabel, false);
}

/*
 * ContentLayoutHandler::checkOverlappingEvents()
 */
int ContentLayoutHandler::checkOverlappingEvents(ulong id, int& aEndEventIndex)
{
    LOGS("ContentLayoutHandler::checkOverlappingEvents");
    int overlappingEvents = 0;
    AgendaEntry entry = mAgendaEntryList.at(id);
    for (int i = id + 1; i < mAgendaEntryList.count(); i++) {
        if ((entry.startTime() <= mAgendaEntryList.at(i).startTime())
            && (entry.endTime() > mAgendaEntryList.at(i).startTime())
            && entry.startTime().date() == mAgendaEntryList.at(i).startTime().date()) {
            aEndEventIndex = i;
            overlappingEvents++;
        }
    }
    if (overlappingEvents) {
        //if there are overlapping events we have to count one more
        //because the minumum is two.
        overlappingEvents++;
    }

    return overlappingEvents;
}

/*
 * ContentLayoutHandler::sortList()
 */
void ContentLayoutHandler::sortList()
{
    LOGS("ContentLayoutHandler::sortList");
    for (int i = 0; i < mAgendaEntryList.count() - 1; i++) {
        for (int j = i + 1; j < mAgendaEntryList.count(); j++) {
            if (mAgendaEntryList.at(i).startTime() > mAgendaEntryList.at(j).startTime()) {
                //TODO: Maintain an array of integer ?
                mAgendaEntryList.swap(i, j);
            }
        }
    }
}

/*
 * ContentLayoutHandler::todayHasValidEvents()
 */
bool ContentLayoutHandler::todayHasValidEvents()
{
    LOGS("ContentLayoutHandler::todayHasValidEvents");
    QDateTime currentTime = currentDateTime();
    bool hasValidEvents = false;
    for (int i = 0; i < mAgendaEntryList.count(); i++) {
        if ((mAgendaEntryList.at(i).endTime() >= currentTime)
            && (mAgendaEntryList.at(i).startTime().date() == currentTime.date())) {
            hasValidEvents = true;
        }
    }

    return hasValidEvents;
}

/*
 * ContentLayoutHandler::removeTodosFrom()
 */
void ContentLayoutHandler::removeTodosFrom()
{
    LOGS("ContentLayoutHandler::removeTodosFrom");
    for (int i = 0; i < mAgendaEntryList.count();) {
        if ((mAgendaEntryList.at(i).type() == AgendaEntry::TypeTodo)
            || (mAgendaEntryList.at(i).type() == AgendaEntry::TypeNote)
            || (mAgendaEntryList.at(i).type() == AgendaEntry::TypeUnknown)) {
            mAgendaEntryList.removeAt(i);
        }
        else {
            //nothing has been removed. Increment the counter.
            i++;
        }
    }
}

void ContentLayoutHandler::highlightOn(QPointF &point)
{
    LOGS("ContentLayoutHandler::highlightOn");
    // --- Content layout ---
    
    if (mContainer) {
        //traslate the container rect to right position according to the scene
        QRectF layoutRect( QPointF( mContainer->rect().topLeft().x() + mContainer->pos().x(), mContainer->rect().topLeft().y() + mContainer->pos().y() ), 
                           QPointF( mContainer->rect().bottomRight().x() + mContainer->pos().x(), mContainer->rect().bottomRight().y() + mContainer->pos().y() ));
        
        if ( layoutRect.contains(point) ) {
            LOGS("[[[Highlight signal in Content area]]]");
            if (!mContentHighlightActive) {
                HbFrameDrawer* highlightDrawer = new HbFrameDrawer(HIGHLIGHT_IMAGE_NAME,
                    HbFrameDrawer::NinePieces);
                HbFrameItem* highlightItem = new HbFrameItem(highlightDrawer);
                mContainer->setBackgroundItem(highlightItem);
                mContentHighlightActive = true; // must be done before setLabelsColor
                setLabelsColor();
                mContainer->update();
            }
        }
    }
    
    // --- Date layout ---
    
    // TODO Don't highlight if already highlighted
   
    if (mIconContainer) {
        //traslate the container rect to right position according to the scene        
        QRectF layoutRect( QPointF( mIconContainer->rect().topLeft().x() + mIconContainer->pos().x(), mIconContainer->rect().topLeft().y() + mIconContainer->pos().y() ), 
                           QPointF( mIconContainer->rect().bottomRight().x() + mIconContainer->pos().x(), mIconContainer->rect().bottomRight().y() + mIconContainer->pos().y() ));
        
        if ( layoutRect.contains(point) ) {
            LOGS("[[[Highlight signal in dateicon area]]]");
            if (!mDateHighlightActive) {
                HbFrameDrawer* highlightDrawer = new HbFrameDrawer(HIGHLIGHT_IMAGE_NAME,
                    HbFrameDrawer::NinePieces);
                HbFrameItem* highlightItem = new HbFrameItem(highlightDrawer);
                if (mHighlightLabel) {
                    mHighlightLabel->setBackgroundItem(highlightItem);
                }
                mDateHighlightActive = true; // must be done before setLabelsColor
                setLabelsColor();
                if (mHighlightLabel) {
                    mHighlightLabel->update();
                }
            }
        }
    }
}

void ContentLayoutHandler::highlightOff()
{
    LOGS("ContentLayoutHandler::highlightOff");
    // --- Content layout ---
    if (mContentHighlightActive) {
        QGraphicsItem* background1 = mContainer->backgroundItem();
        if (background1) {
            background1->hide();
        }
        mContentHighlightActive = false; // must be done before setLabelsColor
        setLabelsColor();
        mContainer->update();
    }
    
    // --- Date layout ---
    
    if (mDateIconFrameDrawer) {
        QGraphicsItem* background2 = mHighlightLabel->backgroundItem();
        if (background2) {
            background2->hide();
        }
        mDateHighlightActive = false; // must be done before setLabelsColor
        setLabelsColor();
        mHighlightLabel->update();
    }
}

//TEST FUNCTIONS

/*
 * ContentLayoutHandler::testId()
 */
int ContentLayoutHandler::testId()
{
    return mTestID;
}

/*
 * ContentLayoutHandler::setTestId()
 */
void ContentLayoutHandler::setTestId(int testID)
{
    mTestID = testID;
}

/*
 * ContentLayoutHandler::setTimerForNextUpdate()
 */
void ContentLayoutHandler::setTimerForNextUpdate()
{
    if (mAgendaEntryList.count()) {
        int msecsToNextEvent = timeToNextCheck();
        if (msecsToNextEvent > 0) {
            mEventTimer->start(msecsToNextEvent);
        }
    }
}

QDateTime ContentLayoutHandler::currentDateTime()
{
#ifdef TEST
    return QDateTime(QDate(2010, 1, 1), QTime(9, 0, 0));
#else
    return QDateTime::currentDateTime();
#endif
}

void ContentLayoutHandler::setLabelText(HbLabel* label, QString text)
{
    if (label) {
        label->setPlainText(text);

        if (label == mUpperLabelShort) {
            if (mUpperLabelLong) {
                mUpperLabelLong->setPlainText(text);
            }
        }
    }
}

void ContentLayoutHandler::setLabelVisible(HbLabel* label, bool visible)
{
    // State transition table
    //  R = reminder label, L = long upper label, S = short upper label
    //  rows = current state (label visibility), columns = action
    //          [hide R]  [show R]  [hide S]  [show S]
    // 0 (slr)   0 (slr)   1 (slR)   0 (slr)   2 (sLr)
    // 1 (slR)   0 (slr)   1 (slR)   1 (slR)   5 (SlR)
    // 2 (sLr)   2 (sLr)   5 (SlR)   0 (slr)   2 (sLr)
    // 3 (sLR)   2 (sLr)   5 (SlR)   1 (slR)   5 (SlR)
    // 4 (Slr)   2 (sLr)   5 (SlR)   0 (slr)   2 (sLr)
    // 5 (SlR)   2 (sLr)   5 (SlR)   1 (slR)   5 (SlR)
    // 6 (SLr)   2 (sLr)   5 (SlR)   0 (slr)   2 (sLr)
    // 7 (SLR)   2 (sLr)   5 (SlR)   1 (slR)   5 (SlR)
    const int stateTransition[] =
        {0,1,0,2,
         0,1,1,5,
         2,5,0,2,
         2,5,1,5,
         2,5,0,2, 
         2,5,1,5,
         2,5,0,2,
         2,5,1,5};
    
    // Logic for showing correct upper label
    if (label == mUpperLabelShort || label == mReminderLabel) {
        // Make sure all three labels exists
        if (mUpperLabelShort && mUpperLabelLong && mReminderLabel) {
            int currentState = 0;
            bool reminderVisible = mReminderLabel->isVisible();
            bool upperLabelLong = mUpperLabelLong->isVisible();
            bool upperLabelShort = mUpperLabelShort->isVisible();
            currentState += (/*mReminderLabel->isVisible()*/mReminderVisible ? 1 : 0);
            currentState += (/*mUpperLabelLong->isVisible()*/mUpperLabelLongVisible  ? 2 : 0);
            currentState += (/*mUpperLabelShort->isVisible()*/mUpperLabelShortVisible ? 4 : 0);

            int action = -1;
            if (label == mReminderLabel) {
                if (visible) {
                    action = 1;
                }
                else {
                    action = 0;
                }
            }
            else if (label == mUpperLabelShort) {
                if (visible) {
                    action = 3;
                }
                else {
                    action = 2;
                }
            }
            
            int newState = stateTransition[currentState * 4 + action];
            
            if (newState != currentState) {
                mReminderVisible = newState & 1;
                mUpperLabelLongVisible = newState & 2;
                mUpperLabelShortVisible = newState & 4;
                mReminderLabel->setVisible(newState & 1);
                mUpperLabelLong->setVisible(newState & 2);
                mUpperLabelShort->setVisible(newState & 4);
            }
        }
    }
    else { // Other labels
        if (label) {
            label->setVisible(visible);
        }
    }
}

bool ContentLayoutHandler::isAllDayEvent(AgendaEntry entry)
{
    if (entry.startTime().time() == QTime(0, 0)
        && entry.endTime().time() == QTime(0, 0)
        && entry.startTime().date() < entry.endTime().date()) {
        return true;
    }
    else {
        return false;
    }
}

bool ContentLayoutHandler::isOneDayAllDayEvent(AgendaEntry entry)
{
    if (isAllDayEvent(entry)
        && entry.startTime().date().addDays(1) == entry.endTime().date()) {
        return true;
    }
    else {
        return false;
    }
}

void ContentLayoutHandler::setLowerLabelOverlapping(int numberOfEvents)
{
    setLabelText(mLowerLabel, hbTrId("txt_calendar_widget_v_dblist_val_l1_events", numberOfEvents));
}

//from DateIconLayoutHandler
/*
 * ContentLayoutHandler::setCalendarIconContent()
 */
void ContentLayoutHandler::setCalendarIconContent()
{
    LOGS("ContentLayoutHandler::setCalendarIconContent");
    if (mNumberLabel) {
        mNumberLabel->setPlainText(QString::number(QDateTime::currentDateTime().date().day(), 10));
    }
    if (mMonthLabel) {
        mMonthLabel->setPlainText(QLocale::system().monthName(
            QDateTime::currentDateTime().date().month()));
    }
}

//End of file
