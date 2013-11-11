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
// alarmalertwidget_p.cpp

// System includes
#include <QApplication>
#include <QGesture>
#include <QGestureEvent>
#include <QGraphicsSceneMouseEvent>

#include <hbdocumentloader.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbinstance.h>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <hbaction.h>
#include <hbtranslator.h>

// User includes
#include "alarmalertwidget_p.h"
#include "alarmalert.h"
#include "alarmalertdocloader.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "alarmalertwidget_pTraces.h"
#endif


// ---------------------------------------------------------
// AlarmAlertDialogPrivate::AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertDialogPrivate::AlarmAlertDialogPrivate(const QVariantMap &parameters):
	mTranslator(new HbTranslator("alarmui")),
	mClosedByClient(false),
	mIsSilenceKey(false)
	{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_ALARMALERTDIALOGPRIVATE_ENTRY );
	// Extract all the parameters sent by the client
	parseAndFetchParams(parameters);

	// Set the dismiss policy and timeout property
	setDismissPolicy(HbPopup::NoDismiss);
	setTimeout(HbPopup::NoTimeout);
	
	// Listen the signal when alarmui is about to display so that we can start playing the alarm tone.
	connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToDisplay()) );

	// Initialize the user response
	mUserResponse = Other;

	// TODO: Gestures not working. Integrate once support is available from Qt
	grabGesture(Qt::SwipeGesture);
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_ALARMALERTDIALOGPRIVATE_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::~AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertDialogPrivate::~AlarmAlertDialogPrivate()
{
    OstTraceFunctionEntry0( DUP1_ALARMALERTDIALOGPRIVATE_ALARMALERTDIALOGPRIVATE_ENTRY );
    // Cleanup
	if (mAlertDocLoader) {
		delete mAlertDocLoader;
	}
    // Cancel any ongoing animations
    HbEffect::cancel(mSlider, "slideDownPortrait", false, false, false);
    HbEffect::cancel(mSlider, "slideDownLandscape", false, false, false);
    
    // Remove the effect
    HbEffect::remove(mSlider);

	// Remove the translator
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	OstTraceFunctionExit0( DUP1_ALARMALERTDIALOGPRIVATE_ALARMALERTDIALOGPRIVATE_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::setDeviceDialogParameters
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertDialogPrivate::setDeviceDialogParameters(const QVariantMap &parameters)
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_SETDEVICEDIALOGPARAMETERS_ENTRY );
    Q_UNUSED(parameters);
    
    //TODO: Handle parameters to be set
    OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_SETDEVICEDIALOGPARAMETERS_EXIT );
    return 0;
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::deviceDialogError
// rest of the details are commented in the header
// ---------------------------------------------------------
//
int AlarmAlertDialogPrivate::deviceDialogError() const
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_DEVICEDIALOGERROR_ENTRY );
    // TODO: Return any errors that might have occured
    OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_DEVICEDIALOGERROR_EXIT );
    return 0;
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::closeDeviceDialog(bool byClient)
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_CLOSEDEVICEDIALOG_ENTRY );
    Q_UNUSED(byClient);
    mClosedByClient = byClient;
	close();
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_CLOSEDEVICEDIALOG_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::deviceDialogWidget
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HbDialog *AlarmAlertDialogPrivate::deviceDialogWidget() const
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_DEVICEDIALOGWIDGET_ENTRY );
    return const_cast<AlarmAlertDialogPrivate*> (this);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::showEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::showEvent(QShowEvent *event)
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_SHOWEVENT_ENTRY );
	HbDialog::showEvent(event);
	QVariantMap param;
	param.insert(alarmCommand, mUserResponse);
	emit deviceDialogData(param);
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_SHOWEVENT_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::hideEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::hideEvent(QHideEvent *event)
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_HIDEEVENT_ENTRY );
    HbDialog::hideEvent(event);
    emit deviceDialogClosed();
    OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_HIDEEVENT_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleOrientationChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleOrientationChange(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_HANDLEORIENTATIONCHANGE_ENTRY );
    QString section;
    QString filename;
    if (orientation == Qt::Horizontal) {
    	section = "landscape";
    } else {
    	section = "portrait";
    }
    bool loadSuccess = false;
    switch (mAlarmAlertType) {
    	case ClockAlarm:
    		mAlertDocLoader->load(alarmNormalUIClockDocml, section, &loadSuccess);
    		break;
    	case TodoAlarm:
    		mAlertDocLoader->load(alarmNormalUITodoDocml, section, &loadSuccess);
    		break;
    	case CalendarAlarm:
    		if (mIsTimedAlarm) {
    			filename = alarmNormalUICalendarDocml;
	            section = "timedAlarmportrait";
	            if(orientation == Qt::Horizontal) {
	            	section = "timedAlarmLandscape";
	            }
            } else {
            	filename = alarmNormalUINonTimedCalendarDocml;
            	section = "nonTimedAlarmPortrait";
            	if(orientation == Qt::Horizontal) {
            		section = "nonTimedAlarmLandscape";
            	}
            }
    		mAlertDocLoader->load(filename, section, &loadSuccess);
    		break;
    	default:
    		break;
    }
    
    OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_HANDLEORIENTATIONCHANGE_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::snoozed
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::snoozed()
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_SNOOZED_ENTRY );
	mUserResponse = Snooze;
    QVariantMap param;
    param.insert(alarmCommand, mUserResponse);
    emit deviceDialogData(param);    
	
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_SNOOZED_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::silenced
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::silenced()
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_SILENCED_ENTRY );
	if (mIsSilenceKey) {
		mUserResponse = Silence;
		mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
		mIsSilenceKey = false;
		setDismissOnAction(true);
		QVariantMap param;
		param.insert(alarmCommand, mUserResponse);
		emit deviceDialogData(param);
	}else {
        snoozed();

	}
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_SILENCED_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::aboutToDisplay
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::aboutToDisplay()
    {
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_ABOUTTODISPLAY_ENTRY );
    mUserResponse = Shown;
    QVariantMap param;
    param.insert(alarmCommand, mUserResponse);
    emit deviceDialogData(param);
    OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_ABOUTTODISPLAY_EXIT );
    }
	
// ---------------------------------------------------------
// AlarmAlertDialogPrivate::dismissed
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::dismissed()
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_DISMISSED_ENTRY );
	mUserResponse = Stop;
    QVariantMap param;
    param.insert(alarmCommand, mUserResponse);
    emit deviceDialogData(param);
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_DISMISSED_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::parseAndFetchParams
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::parseAndFetchParams(const QVariantMap &parameters)
{
    OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_PARSEANDFETCHPARAMS_ENTRY );
    // Iterate thru the list and extract all relevant parameters
    QVariantMap::const_iterator iter = parameters.constBegin();
    int count = parameters.size();
    while (iter != parameters.constEnd()) {
        QString key(iter.key());
        if (alarmSubject == key) {
            mSubject = iter.value().toString();
        } else if (alarmLocation == key) {
            mLocation = iter.value().toString();
        } else if (alarmTime == key) {
            mAlarmTime = iter.value().toString();
        } else if (alarmDate == key) {
            mAlarmDate = iter.value().toString();
        } else if (alarmType == key) {
            mAlarmAlertType = static_cast <AlarmType> (iter.value().toInt());
        } else if (alarmCanSnooze == key) {
            mCanSnooze = iter.value().toBool();
        } else if (alarmRingingType == key) {
            mIsSilent = iter.value().toBool();
        } else if (alarmIsTimed == key) {
            mIsTimedAlarm = iter.value().toBool();
        }else {
            break;
        }
        iter++;
    }
    OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_PARSEANDFETCHPARAMS_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::setupNormalUI
// rest of the details are commented in the header
// ---------------------------------------------------------
//

void AlarmAlertDialogPrivate::setupNormalUI(AlarmAlertDocLoader *alertDocLoader)
    {
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_SETUPNORMALUI_ENTRY );
	if(!alertDocLoader) {
		// Nothing can be done. Simply return
		OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_SETUPNORMALUI_EXIT );
		return;
	}
	mAlertDocLoader = alertDocLoader;
	
	mMainWindow = hbInstance->allMainWindows().first();
	connect(
			mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(handleOrientationChange(Qt::Orientation)));

	if (mAlarmAlertType == ClockAlarm) {
        handleClockAlarms();
    }else if(mAlarmAlertType == TodoAlarm) {
        handleToDoAlarms();
    }else if(mAlarmAlertType == CalendarAlarm) {
    	handleCalendarAlarms();
    }
	OstTraceFunctionExit0( DUP1_ALARMALERTDIALOGPRIVATE_SETUPNORMALUI_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleClockAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleClockAlarms()
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_HANDLECLOCKALARMS_ENTRY );
	
	handleOrientationChange(mMainWindow->orientation());
	
	QGraphicsWidget *headingWidget = mAlertDocLoader->findWidget("heading");
	if (!headingWidget) {
		qFatal("Unable to load the heading Widget");
	}

	mAlarmDateTime = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmTime"));
	if (!mAlarmDateTime) {
		qFatal("Unable to load the alarmTime label");
	}
	HbExtendedLocale locale = HbExtendedLocale::system();

	mAlarmDateTime->setPlainText(mAlarmTime);
	
	mAlarmDescription = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmDescription"));
	if (!mAlarmDescription) {
		qFatal("Unable to load the alarmDescription label");
	}
	mAlarmDescription->setPlainText(mSubject);

	mAlarmIcon = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmIcon"));
	if (!mAlarmIcon) {
		qFatal("Unable to load the alarm icon");
	}

	HbAction *snoozeAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("snoozeAction"));
	if (!snoozeAction) {
		qFatal("Unable to load the snoozeAction softkey");
	}
	snoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
	connect(snoozeAction, SIGNAL(triggered()), this, SLOT(snoozed()));

	HbAction *stopAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("stopAction"));
	if (!stopAction) {
		qFatal("Unable to load the stopAction softkey");
	}
	stopAction->setText(hbTrId("txt_calendar_button_alarm_stop"));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(dismissed()));

	if (!mCanSnooze) {
		snoozeAction->setVisible(false);
	}
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_HANDLECLOCKALARMS_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleCalendarAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleCalendarAlarms()
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_HANDLECALENDARALARMS_ENTRY );
	HbExtendedLocale locale = HbExtendedLocale::system();
	// Check if the alarm has the time info or not.
	// For all day events the time label has to be hidden
	// So load the proper sections accordingly
	if ( mIsTimedAlarm ) {
		handleOrientationChange(mMainWindow->orientation());
		mAlarmDateTime = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmTime"));
		if (!mAlarmDateTime) {
			qFatal("Unable to find the alarmTime label");
		}
		mAlarmDateTime->setPlainText(mAlarmTime);
		HbLabel *alarmDate = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmDate"));
		if (!alarmDate) {
			qFatal("Unable to find the alarmDate label");
		}
		alarmDate->setPlainText(mAlarmDate);
	}else {
		handleOrientationChange(mMainWindow->orientation());
		HbLabel *alarmDate = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmDate"));
		if (!alarmDate) {
			qFatal("Unable to find the alarmDate label");
		}
		alarmDate->setPlainText(mAlarmDate);
	}

	mAlarmDescription = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmDescription"));
	if (!mAlarmDescription) {
		qFatal("Unable to find the alarmDescription label");
	}
	mAlarmDescription->setPlainText(mSubject);

	HbLabel *alarmLocation = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmLocation"));
	if (!alarmLocation) {
		qFatal("Unable to find the alarmLocation label");
	}
	alarmLocation->setPlainText(mLocation);
	
	mAlarmIcon = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmIcon"));
	if (!mAlarmIcon) {
		qFatal("Unable to find the alarm Icon");
	}

	mSnoozeAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("snoozeAction"));
	if (!mSnoozeAction) {
		qFatal("Unable to find the snoozeAction softkey");
	}
	
	if (!mCanSnooze) {
		mSnoozeAction->setVisible(false);
	}else { 
		if(!mIsSilent) {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_silence"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(silenced()));
			mIsSilenceKey = true;
			setDismissOnAction(false);
		} 	else {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(snoozed()));
		}
	}
	HbAction *stopAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("stopAction"));
	if (!stopAction) {
		qFatal("Unable to find the stopAction softkey");
	}
	stopAction->setText(hbTrId("txt_calendar_button_alarm_stop"));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(dismissed()));
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_HANDLECALENDARALARMS_EXIT );
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleToDoAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleToDoAlarms()
{
	OstTraceFunctionEntry0( ALARMALERTDIALOGPRIVATE_HANDLETODOALARMS_ENTRY );
	
	handleOrientationChange(mMainWindow->orientation());
	
	QGraphicsWidget *headingWidget = mAlertDocLoader->findWidget("heading");
	if (!headingWidget) {
		qFatal("Unable to load the heading widget");
	}
	HbExtendedLocale locale = HbExtendedLocale::system();
	
	HbLabel *alarmDate = qobject_cast<HbLabel*> (
							mAlertDocLoader->findWidget("alarmDate"));
	if (!alarmDate) {
		qFatal("Unable to load the alarmDate label");
	}
	alarmDate->setPlainText(mAlarmDate);
	
	mAlarmDescription = qobject_cast<HbLabel*> (
							mAlertDocLoader->findWidget("alarmDescription"));
	if (!mAlarmDescription) {
		qFatal("Unable to load the alarmDescription label");
	}
	mAlarmDescription->setPlainText(mSubject);
	
	mAlarmIcon = qobject_cast<HbLabel*> (
									mAlertDocLoader->findWidget("alarmIcon"));
	if (!mAlarmIcon) {
		qFatal("Unable to load the alarm Icon");
	}
	mSnoozeAction = qobject_cast<HbAction*> (
								mAlertDocLoader->findObject("snoozeAction"));
	if (!mSnoozeAction) {
		qFatal("Unable to load the snoozeAction softkey");
	}

	if (!mCanSnooze) {
		mSnoozeAction->setVisible(false);
	}else { 
		if(!mIsSilent) {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_silence"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(silenced()));
			mIsSilenceKey = true;
			setDismissOnAction(false);
		} 	else {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(snoozed()));
		}
	}
	HbAction *stopAction = qobject_cast<HbAction*> (
									mAlertDocLoader->findObject("stopAction"));
	if (!stopAction) {
		qFatal("Unable to load the stopAction softkey");
	}
	stopAction->setText(hbTrId("txt_calendar_button_alarm_stop"));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(dismissed()));
	OstTraceFunctionExit0( ALARMALERTDIALOGPRIVATE_HANDLETODOALARMS_EXIT );
}

// End of file  --Don't remove this.
