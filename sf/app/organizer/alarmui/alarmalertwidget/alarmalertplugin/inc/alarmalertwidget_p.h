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
// alarmalertwidget_p.h

// System includes
#include <QDateTime>
#include <QPointer>
#include <hbdialog.h>
#include <hbdevicedialoginterface.h>
#include <hbeffect.h>

// User includes
#include "alarmalert.h"

// Forward declarations
class QGraphicsWidget;
class QEvent;
class HbTranslator;
class HbMainWindow;
class HbDocumentLoader;
class HbLabel;
class HbAction;
class AlarmAlertDocLoader;

const QString alarmNormalUICalendarDocml(
		":/xml/alarmalert_timed_calendar_unlocked.docml");
const QString alarmNormalUINonTimedCalendarDocml(
		":/xml/alarmalert_nontimed_calendar_unlocked.docml");
const QString alarmNormalUIClockDocml(":/xml/alarmalert_clock_unlocked.docml");
const QString alarmNormalUITodoDocml(":/xml/alarmalert_todo_unlocked.docml");

// Class declaration
/**
 * @class AlarmAlertDialogPrivate
 * @brief The device dialog implementation for alarm alert
 */
class AlarmAlertDialogPrivate : public HbDialog, public HbDeviceDialogInterface
{
Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parameters The list of arguments sent by the client
     */
    AlarmAlertDialogPrivate(const QVariantMap &parameters);
    
    /**
     * @brief Destructor
     */
    ~AlarmAlertDialogPrivate();

    /**
     * @brief Sets the device dialog parameters to this dialog
     * @param parameters List of parameters to set
     */
    bool setDeviceDialogParameters(const QVariantMap &parameters);
    
    /**
     * @brief Called from the device dialog server to check
     * for any errors in dialog creation/display
     */
    int deviceDialogError() const;
    
    /**
     * @brief Called from the device dialog server.
     * Closes the dialog if its being shown
     * @param byClient true if the close event is being
     * triggered by the client, false otherwise
     */
    void closeDeviceDialog(bool byClient);
    
    /**
     * @brief Returns the device dialog instance
     */
    HbDialog *deviceDialogWidget() const;
    
    /**
     * @brief Sets up the normal ui for alarms.
     * This UI is shown when the screen is unlocked
     */
    void setupNormalUI(AlarmAlertDocLoader *alertDocLoader);
    
protected:
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    void showEvent(QShowEvent *event);
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    void hideEvent(QHideEvent *event);
    
signals:

    /**
     * @brief Emitted when the dialog is about to be closed
     */
    void deviceDialogClosed();
    
    /**
     * @brief Used to send any information back to the client
     */
    void deviceDialogData(QVariantMap data);

private slots:

    /**
     * @brief Callback function for handling orientation changes
     */
    void handleOrientationChange(Qt::Orientation orientation);
    
    /**
     * @brief Called when the user snoozes the alarm
     */
    void snoozed();
    
    /**
     * @brief Called when the user dismisses the alarm
     */
    void dismissed();
    
    /**
     * @brief Called when the user silences the alarm
     */
    void silenced();
    
	/**
     * @brief Called when the alarmui is about to display.
     */
    void aboutToDisplay();
    
private:
    /**
     * @brief Extracts the parameters sent by the client
     * @var parameters List of parameters 
     */
    void parseAndFetchParams(const QVariantMap &parameters);
    
    /**
     * @brief handles the calendar alarms
     */
    void handleCalendarAlarms();
    
    /**
     * @brief handles the clock alarms
     */
    void handleClockAlarms();
    
    /**
     * @brief handles to-do alarms
     */
    void handleToDoAlarms();

private:
    Q_DISABLE_COPY(AlarmAlertDialogPrivate)
    
    /**
     * @var mSlider
     * @brief Points to the slider icon
     */
    QGraphicsWidget *mSlider;
    
    /**
     * TODO: Remove this when gestures are implemented
     */
    QPointF mTapPoint;
    
    /**
     * @var mSubject
     * @brief Holds the alarm subject
     */
    QString mSubject;
    
    /**
     * @var mLocation
     * @brief Holds the alarm location (for calendar alerts only)
     */
    QString mLocation;
    
    /**
     * @var mAlarmTime
     * @brief Holds the alarm expiry time
     */
    QString mAlarmTime;
    
    /**
     * @var mAlarmDate
     * @brief Holds the alarm expiry time
     */
    QString mAlarmDate;
    
    /**
     * @var mTranslator
     * @brief translator for the localisation of strings
     */
    HbTranslator *mTranslator;
	
    /**
     * @var mAlarmDescription
     * @brief Points to the alarm description label
     */
    QPointer<HbLabel> mAlarmDescription;
    
    /**
    * @var mAlarmDateTime
    * @brief Points to the alarm expiry time label
    */
    QPointer<HbLabel> mAlarmDateTime;
    
    /**
     * @var mAlarmIcon
     * @brief Points to the alarm icon label
     */
    QPointer<HbLabel> mAlarmIcon;
    
    /**
     * @var mMainWindow
     * @brief The global main window reference.
     * Required for getting orientation change events
     */
    HbMainWindow *mMainWindow;
    
    /**
     * @var mAlertDocLoader
     * @brief The document loader for loading docml's
     */
    AlarmAlertDocLoader* mAlertDocLoader;
    
	/**
     * @var mSnoozeAction
     * @brief snooze softkey for alarm
     */
    HbAction *mSnoozeAction;
	   
    /**
     * @var mUserResponse
     * @brief Stores the user response
     */
    int mUserResponse;
    
    /**
     * @var mAlarmAlertType
     * @brief Tells if the type of alert being displayed
     * is a clock /calendar/to-do alert
     */
    AlarmType mAlarmAlertType;
    
    /**
     * @var mCanSnooze
     * @brief Tells whether the current alert can be snoozed.
     * true->alarm can be snoozed
     * false->alarm cannot be snoozed
     */
    bool mCanSnooze;
    
    /**
     * @var mIsSilent
     * @brief Indicates if the alarm is silent or not
     */
    bool mIsSilent;
    
    /**
     * @var mIsTimedAlarm
     * @brief Indicates if the alarm is has time info or not
     */
    bool mIsTimedAlarm;
    
    /**
     * @var mClosedByClient
     * @brief Indicates if the alarm dialog is closed by the client
     */
    bool mClosedByClient;
    
    /**
     * @var mIsSilenceKey
     * @brief Indicates if the silence softkey is shown for alarm
     */
    bool mIsSilenceKey;
};
