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
 * Description: Header file for class CalenEditorPrivate.
 *
 */

#ifndef CalenEditorPrivate_P_H
#define CalenEditorPrivate_P_H

// System includes
#include <QObject>
#include <QPointer>
#include <hbmainwindow.h>
#include <QHash>
#include <QDateTime>

//user includes
#include <AgendaEntry>
#include <caleneditor.h>
#include "calennotificationhandler.h"

#include "caleneditorcommon.h"

//forward declarations
class QFile;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbAction;
class HbLineEdit;
class HbPushButton;
class HbCheckBox;
class HbComboBox;
class HbView;
class HbTranslator;
class MCalenServices;
class AgendaEntry;
class AgendaUtil;
class CalenEditor;
class CalenEditorDocLoader;
class CalenEditorCustomItem;
class CalenEditorReminderField;
class CalenEditorRepeatField;
class CalenEditorDataHandler;
class CEnvironmentChangeNotifier;		// Receive system event notifications

// Constants
const int KNoOfDaysInWeek = 7;

class CalenEditorPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(CalenEditor)
	
public:	
	enum Error
	        {
	        CalenEditorErrorNone,
	        CalenEditorErrorAlarmTimeLaterThanNote,
	        CalenEditorErrorAlarmTimePast,
	        CalenEditorErrorAlarmDateTooManyDaysBeforeNote,
	        CalenEditorErrorRepeatUntilEarlierThanNote,
	        CalenEditorErrorDurationGreaterThanRepeatInterval,
	        CalenEditorErrorStopTimeEarlierThanStartTime,
	        CalenEditorErrorDbConflictEntryDeleted,//ToDo ::need to handle
	        CalenEditorErrorDbConflictEntryModified,//ToDo ::need to handle
	        CalenEditorErrorOutOfSequence,//ToDo ::need to handle
	        CalenEditorErrorOverlapsExistingInstance,//ToDo ::need to handle
	        CalenEditorErrorInstanceAlreadyExistsOnThisDay//ToDo ::need to handle
	        };
	
	enum Action 
	        {
	        ActionNothing,
	        ActionSave,
	        ActionDelete
	        };
	
	enum EditorItemIndex 
			{
			SubjectItem = 0,
			AllDayItem,
			DateTimeFromItem,
			DateTimeToItem,
			LocationItem,
			ReminderItem,
			ReminderTimeForAllDayItem,
			RepeatItem,
			RepeatUntilItem
			};
public:
	CalenEditorPrivate(AgendaUtil *agendaUtil,
	                   QObject *parent);
	virtual ~CalenEditorPrivate();
	void addDiscardAction();
	AgendaEntry* editedEntry();
	AgendaEntry* originalEntry();
	bool isNewEntry();
	HbDataFormModelItem* allDayCheckBoxItem();
	bool isReminderTimeForAllDayAdded();
	bool isRepeatUntilItemAdded();
	bool isAllDayEvent();
	void updateReminderChoices();
	int currentIndexOfReminderField();
	void setCurrentIndexOfReminderField(int index);
	int getReminderCount();
	bool isEditRangeThisOnly();
	bool isAllDayFieldAdded();
	void forcedSaveEntry();

	/**
	 * Called from CEnvironmentChangeNotifier when the
	 * system environment changes
	 * @param aThisPtr self pointer
	 * @return EFalse
	 */
	static TInt EnvChangeCallbackL( TAny* aThisPtr );
	TInt DoEnvChange();
	
private:
	void edit(const QFile &handle, bool launchCalendar);
	void edit(AgendaEntry entry, bool launchCalendar);
	void edit(ulong id, bool launchCalendar);
	void create(CalenEditor::CreateType type, QDateTime newEntryDateTime, 
	            bool launchCalendar);
	void create(CalenEditor::CreateType type, AgendaEntry entry, 
		            bool launchCalendar);
	void openEditor(AgendaEntry entry);
	void showEditor(AgendaEntry entry);
	void showEditOccurencePopup();
	void setUpView();
	void initModel();
	void addSubjectItem();
	void addAllDayCheckBoxItem();
	void addCustomItemFrom();
	void addCustomItemTo();
    void addCustomItemLocation();
	void addReminderItem();
	void addRepeatItem();
	void insertRepeatUntilItem();
	
	void populateModel();
	void populateSubjectItem();
	void populateAllDayItem();
	void populateCustomItemDateTime();
	void populateCustomItemLocation();
	void populateLocationItem();
	void populateRepeatItem();
	void populateDescriptionItem();
	void removeDescriptionItem();
		
	bool isChild() const ;
	
	Action handleDone();
	bool saveEntry();
	void deleteEntry(bool close = false);
	bool handleAllDayToSave();
	void enableFromTotimeFileds(bool, QDateTime, QDateTime);
	QDateTime defaultTimeSameDay( );
	void refreshTimeForUncheckAllDay();
	void launchDialog(QString title);
	
private slots:
	void handleSubjectChange(const QString subject);
	void handleAllDayChange(int state);
	void saveFromDateTime(QDateTime& fromDateTime);
	void saveToDateTime(QDateTime& toDateTime);
	void handleLocationChange(const QString location);
	void handleLocationChange(const QString location, 
            const double geoLatitude, const double geoLongitude);
	void handleDescriptionChange(const QString description);
	void saveAndCloseEditor();
	void showDeletePopup();
	void showDeleteConfirmationQuery();
	void showDeleteOccurencePopup();
	void handleDeleteAction();
	void handleDeleteOccurence(int index);
	void launchSettingsView();
	void discardChanges();
	void handleDescriptionAction();
	void handleEditOccurence(int index);
	void handleCancel();
	void handleCalendarLaunchError(int error);
	void closeEditor();
	void handleLocationEditingFinished();
	void selectEditingFinishedAction(HbAction* action);	
	
private:
	enum EditRange {
		ThisOnly,
		ThisAndAll,
		UserCancelled
	};
	
	CalenEditor *q_ptr;
	QPointer<AgendaUtil> mAgendaUtil;
	
	CalenEditorDocLoader *mEditorDocLoader;
	HbView *mEditorView;
	CalenEditorDataHandler* mDataHandler;
	HbDataForm *mCalenEditorForm;
	HbDataFormModel *mCalenEditorModel;
	
	HbDataFormModelItem *mSubjectItem;
	CalenEditorCustomItem *mViewFromItem;
	CalenEditorCustomItem *mViewToItem;
	CalenEditorCustomItem *mViewLocationItem;
	HbDataFormModelItem *mAllDayCheckBoxItem;

	CalenEditorReminderField *mReminderField;
	CalenEditorRepeatField *mRepeatField;
	HbDataFormModelItem *mDescriptionItem;
	
	EditRange mEditRange;
	
	QDateTime mNewEntryDateTime;
	
	AgendaEntry mEntry;
	AgendaEntry *mOriginalEntry;
	AgendaEntry *mEditedEntry;

	HbAction *mSoftKeyAction;
	HbAction *mDescriptionAction;
	HbMainWindow *mMainWindow;
	HbTranslator *mTranslator;
	
	bool mNewEntry;
	bool mDescriptionItemAdded;
	bool mIsChild;
	bool mIsAllDayItemAdded;
	bool mOwnsAgendaUtil;
	bool mLaunchCalendar;
	bool mMenuItemAdded;

	// Notifications about locale and time changes
	CEnvironmentChangeNotifier* iEnvChangeNotifier;
	bool iIgnoreFirstLocaleChange;
private:
	friend class CalenEditor;
#ifdef TESTCALENEDITOR
	friend class TestCalenEditor;
#endif

};

#endif // CalenEditorPrivate_P_H

// End of file	--Don't remove this.
