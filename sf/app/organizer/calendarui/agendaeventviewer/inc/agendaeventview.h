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
* Description: Declaration of the class AgendaEventView
*
*/

#ifndef AGENDAEVENTVIEW_H
#define AGENDAEVENTVIEW_H

// System includes
#include <QObject>
#include <QPointer>
#include <HbView>

// User includes.
#include <agendaentry.h>
#include "agendaeventviewer.h"

// Forward declarations
class QGraphicsLinearLayout;
class QTimer;
class QPluginLoader;
class HbView;
class HbAction;
class HbLabel;
class HbMainWindow;
class HbTranslator;
class MapTileService;
class AgendaEventViewerItem;
class AgendaEventViewerPrivate;
class AgendaEventViewerDocLoader;
class CalenEditor;

class AgendaEventView: public QObject
{
	Q_OBJECT

public:
	AgendaEventView(
			AgendaEventViewerPrivate *owner, QObject *parent=0);
	virtual ~AgendaEventView();
public:
	
	/*
	 * forcefully closing the editor.
	 * should be call if view is open,
	 */
	void saveAndCloseEditor();
    
	/*
     * forcefully closing the view.
     * should be call if view is open,
     */
    void closeAgendaEventView();
private:
	void execute(AgendaEntry entry, AgendaEventViewer::Actions action);
	void addViewerData();
	void addMenuItem();
	void addToolBarItem(AgendaEventViewer::Actions action);
	void addGroupBoxData();
	void addSubjectAndPriorityData();
	void addDateTimeData();
	void addLocationData();
	void addMapTileImage();
	void addReminderData();
	void addCompletedTodoData();
	void addRepeatData();
	void addDescriptionData();
	void getPriorityIcon(int priority, QString& priorityIcon);
	QString repeatRule();
	QString alarmTimeText() const;
	void removeWidget();
	void updateFieldsforTodoStatus();
	void removeAllWidgets();
	void addAllWidgets();
	void showDeleteOccurencePopup();
	void showDeleteConfirmationQuery();
	void getSubjectIcon(AgendaEntry::Type type, QString &subjectIcon);
	void getProgressIndicatorstatus(QString &progressIcon);
	void updateSubjectandPriorityData();

private slots:
	void markTodoStatus();
	void edit();
	void deleteAgendaEntry();
	void saveAgendaEntry();
	void close();
	void handleEntryUpdation(ulong id);
	void handleEntryDeletion(ulong id);
	void handleNoteEditorClosed(bool status);
	void handleCalendarEditorClosed();
	void handleDeleteOccurence(int index);
	void handleDeleteAction();
	void updateProgressIndicator();
	void receiveMapTileStatus(int entryid,int addressType, int status); 
	void changedOrientation(Qt::Orientation orientation);

private:
	HbMainWindow* mMainWindow;
	HbView *mViewer;
	QPointer<AgendaEventViewerItem> mSubjectWidget;
	QPointer<AgendaEventViewerItem> mDateTimeWidget;
	QPointer<AgendaEventViewerItem> mLocationWidget;
	HbLabel *mMaptileLabel ;
	QPointer<AgendaEventViewerItem> mReminderWidget;
	QPointer<AgendaEventViewerItem> mRepeatWidget;
	QPointer<AgendaEventViewerItem> mDescriptionWidget;

	QPointer<HbAction> mMarkTodoAction;

	AgendaEventViewerPrivate *mOwner;
	AgendaEventViewerDocLoader *mDocLoader;
	AgendaEntry mOriginalAgendaEntry;
	AgendaEntry mAgendaEntry;
	QPointer<QPluginLoader> mNotesEditorPluginLoader;
	QPointer<CalenEditor> mCalenEditor;
	QGraphicsLinearLayout *mLinearLayout;

	HbTranslator *mTranslator;
	bool mReminderWidgetAdded;
	HbAction *mBackAction;
	
	bool mLocationFeatureEnabled;
	QString mMaptilePath;
	MapTileService *mMaptileService;
	QTimer *mProgressTimer;
	int mProgressIconCount;  
	bool mMaptileStatusReceived;
	bool mCalenEditorClosed ;
	int mMaptileStatus; 
	bool mNotesPluginLoaded;
	ulong mParentId;
	ulong mEntryIdDeleted;

private:
	friend class AgendaEventViewerPrivate;
};
#endif // AGENDAEVENTVIEW_H

// End of file
