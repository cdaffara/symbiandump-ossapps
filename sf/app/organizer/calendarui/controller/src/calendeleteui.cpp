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
* Description:  Handles deletion
*
*/

// System includes
#include <QString>
#include <QtGui>
#include <hbdatetimepicker.h>
#include <hbdialog.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbradiobuttonlist.h>
#include <centralrepository.h>
#include <agendaentry.h>

// User includes
#include "calendarui_debug.h"
#include "calendeleteui.h"
#include "calencontext.h"
#include "calencontroller.h"
#include "CleanupResetAndDestroy.h"
#include "caleninstanceid.h"
#include "calenactionuiutils.h"
#include "calendateutils.h"
#include "calenagendautils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calendeleteuiTraces.h"
#endif


// Local constants
const TInt KEntriesToDelete = 1;

// ----------------------------------------------------------------------------
// CalenDeleteUi::NewL
// Two phased constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDeleteUi* CalenDeleteUi::NewL( CCalenController& aController )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_NEWL_ENTRY );
    
    CalenDeleteUi* self = new( ELeave ) CalenDeleteUi( aController, NULL );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CALENDELETEUI_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::CalenDeleteUi
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDeleteUi::CalenDeleteUi( CCalenController& aController, QObject *parent )
    :QObject(parent), iController( aController )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_CALENDELETEUI_ENTRY );
    
    iIsDeleting = false;
    
    OstTraceFunctionExit0( CALENDELETEUI_CALENDELETEUI_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::~CalenDeleteUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDeleteUi::~CalenDeleteUi()
    {
    OstTraceFunctionEntry0( DUP1_CALENDELETEUI_CALENDELETEUI_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CALENDELETEUI_CALENDELETEUI_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::ConstructL
// Second phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::ConstructL()
    {
    OstTraceFunctionEntry0( CALENDELETEUI_CONSTRUCTL_ENTRY );
    
    RArray<TCalenNotification> notifications;
    notifications.Append(ECalenNotifyCancelDelete);
    notifications.Append(ECalenNotifyCloseDialogs);
    notifications.Append(ECalenNotifySystemLocaleChanged);
    
    iController.RegisterForNotificationsL( this, notifications );
    
    notifications.Close();
    
    iMoreEntriesToDelete = EFalse;
    iDisplayQuery = EFalse;
    iEntriesToDelete = KEntriesToDelete;
    mDatePicker = 0;
    
    OstTraceFunctionExit0( CALENDELETEUI_CONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::HandleNotification
// Handles notifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::HandleNotification(const TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_HANDLENOTIFICATION_ENTRY );
    switch(aNotification)  {
    	case ECalenNotifyCancelDelete:  {
        	if(iMutlipleContextIdsCount)
           	 {
           		 // get the context
          		  MCalenContext& context = iController.context();
           		 // reset the multiple contexts
          		  context.resetMultipleContextIds();          
           	 }
		break;
        }
    	case ECalenNotifyCloseDialogs:  {
    		// Emit the signal to close all the dialogs which are already opened
    		emit closeDialogs();
		break;
        }
	case ECalenNotifySystemLocaleChanged: {
		//Refresh the date picker of Date query when deleting entries before date
		if(!(mDatePicker.isNull())) {
			mDatePicker->setDisplayFormat(CalenDateUtils::dateFormatString());
			mDatePicker->setDate(QDate::currentDate());
		}
		break;
	}
	default:
		break;
    }
    OstTraceFunctionExit0( CALENDELETEUI_HANDLENOTIFICATION_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::HandleCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CalenDeleteUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_HANDLECOMMANDL_ENTRY );
    
    TBool continueCommand(EFalse);
    
    switch( aCommand.Command() )
        {
        case ECalenDeleteCurrentEntry:
        	mDeleteCommand = ECalenDeleteCurrentEntry;
        	DeleteCurrentEntryL(); // Entry & instance
            break;

        case ECalenDeleteSeries:
        	mDeleteCommand = ECalenDeleteSeries;
        	DeleteThisOrAllL( AgendaUtil::ThisAndAll );
            break;
            
        case ECalenDeleteCurrentOccurrence:
        	mDeleteCommand = ECalenDeleteCurrentOccurrence;
        	DeleteThisOrAllL( AgendaUtil::ThisOnly );
            break;
            
        case ECalenDeleteEntryWithoutQuery:
        	//TODO:
        	// This case is not handled currently as no such commands
        	// has been issued. So this has to be handled once we start 
        	// issuing the command. 
        	/*continueCommand = DeleteEntryWithoutQueryL();*/
            break;

        case ECalenDeleteAllEntries:
        	mDeleteCommand = ECalenDeleteAllEntries;
        	DeleteAllEntriesL(); // EntryView & instance
            break;

        case ECalenDeleteEntriesBeforeDate:
            DeleteEntriesBeforeDateL(); // EntryView & instance
            break;

        default:
            // Controller decided this class was the place to handle this
            // command but it wasn't in our list; something has gone wrong.
            //ASSERT( EFalse );
            break;
        }
        
    OstTraceFunctionExit0( CALENDELETEUI_HANDLECOMMANDL_EXIT );
    return continueCommand;
    }


// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteThisOrAllL
// Deletes series repeating entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteThisOrAllL( AgendaUtil::RecurrenceRange aRepeatType )
{
    OstTraceFunctionEntry0( CALENDELETEUI_DELETETHISORALLL_ENTRY );

	if( iController.context().instanceId().mEntryLocalUid )
	{
		AgendaEntry instance = CalenActionUiUtils::findPossibleInstanceL(
									iController.context().instanceId(),
									iController.Services().agendaInterface() );
		if( !instance.isNull() )
		{
			DeleteSingleInstanceL( instance, aRepeatType );
		}
	}
	
	OstTraceFunctionExit0( CALENDELETEUI_DELETETHISORALLL_EXIT );
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntryWithoutQueryL()
// Deletes the current entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CalenDeleteUi::DeleteEntryWithoutQueryL()
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETEENTRYWITHOUTQUERYL_ENTRY );
    
	bool continueCommand(EFalse);
    
	// get the context
	MCalenContext& context = iController.context();
	
	// get the multliple context ids count
	iMutlipleContextIdsCount = context.mutlipleContextIdsCount();
	
	ASSERT( iMutlipleContextIdsCount );

	    if(!iMoreEntriesToDelete)
		    {
		    //iDisplayQuery = ShowMultipleEntriesDeleteQueryL(iMutlipleContextIdsCount);
		    }
		
		if(iDisplayQuery)
			{
			if(!iMoreEntriesToDelete)	
			    {
			    DisplayWaitDialogL();
			    }
			// get the multiple context instance ids
			QList<TCalenInstanceId>& multipleContextIds = context.getMutlipleContextIds();
			
			if(iMutlipleContextIdsCount <= iEntriesToDelete )
			    {
			    iMoreEntriesToDelete = EFalse; 
			    iEntriesToDelete = iMutlipleContextIdsCount;
			    }
			else
			    {
			    iMoreEntriesToDelete = ETrue;
	            // set the continue command flag if more entries are there to delete
			    continueCommand = ETrue;
			    }
			    
			TInt index(0);
			while(index<iEntriesToDelete)
			    {
			    // get the local uid of the entry through multiple context list
			    ulong entryLocalUid = multipleContextIds[0].mEntryLocalUid;
			    if(entryLocalUid)
			        {
			        DeleteEntryL(entryLocalUid);
			        }
			    // remove mutliple context based on the instanceid
			    context.removeMultipleContextId(multipleContextIds[0]);
			    index++;
			   }

			if(!iMoreEntriesToDelete)
			    {
			    MarkedEntriesDeletedL();    
			    }
			}
        else
            {
            context.resetMultipleContextIds();
            // notify delete failed
            iController.BroadcastNotification(ECalenNotifyDeleteFailed);    
            }
	
    OstTraceFunctionExit0( CALENDELETEUI_DELETEENTRYWITHOUTQUERYL_EXIT );
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteCurrentEntryL
// Deletes the current entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteCurrentEntryL()
{
    OstTraceFunctionEntry0( CALENDELETEUI_DELETECURRENTENTRYL_ENTRY );
    
	// Make sure we're focused on an entry.
	if (iController.context().instanceId().mEntryLocalUid) {
		// Fetch the entry
		AgendaEntry entry = iController.Services().agendaInterface()->fetchById(
				iController.context().instanceId().mEntryLocalUid);
			// Check if the entry is a To-Do
			if (AgendaEntry::TypeTodo == entry.type()) {
				showDeleteQuery(EDeleteToDo);
			} else {
				// Show the repeat entry delete query for repeating entries except Anniversary
				// Even though the anniversary is repeating 
				// all the instances will be deleted
				if ((entry.isRepeating() || !entry.recurrenceId().isNull()) 
							&& (AgendaEntry::TypeAnniversary != entry.type())) {
					// Show a confirmation note whether the user
					// wants to delete the single instance or all of them
					showRepeatingEntryDeleteQuery();
				} else if (CalenAgendaUtils::isAlldayEvent(entry)) {
					showDeleteQuery(EDeleteEvent);
				} else {
					// If the entry is not a repeating entry,
					// delete it directly
					// Save the entry for later reference in the slot
					showDeleteQuery(EDeleteEntry);
				}
			}
	}
	OstTraceFunctionExit0( CALENDELETEUI_DELETECURRENTENTRYL_EXIT );
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteAllEntriesL
// Deletes all entries
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteAllEntriesL()
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETEALLENTRIESL_ENTRY );
    
	if(iIsDeleting) {
		OstTraceFunctionExit0( CALENDELETEUI_DELETEALLENTRIESL_EXIT );
		return;
	}

    showDeleteQuery(EDeleteAll );
	OstTraceFunctionExit0( DUP1_CALENDELETEUI_DELETEALLENTRIESL_EXIT );
	}

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntriesBeforeDateL
// Deletes all entries before a set date.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteEntriesBeforeDateL()
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETEENTRIESBEFOREDATEL_ENTRY );
    
	if(iIsDeleting) {
		OstTraceFunctionExit0( CALENDELETEUI_DELETEENTRIESBEFOREDATEL_EXIT );
		return;
	}    
    // launch the datepicker
    dateQuery();
    OstTraceFunctionExit0( DUP1_CALENDELETEUI_DELETEENTRIESBEFOREDATEL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::dateQuery
// Launches the popup for the date selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::dateQuery()
	{
    OstTraceFunctionEntry0( CALENDELETEUI_DATEQUERY_ENTRY );
    
	// Create a popup with datepicker to select the date.
	HbDialog *popUp = new HbDialog();
	popUp->setParent(this);
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout );
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	popUp->setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_set_date")));
	
	QDateTime currentDateTime(CalenDateUtils::today());
	QDate currentDate(currentDateTime.date());
	if(mDatePicker) {
		mDatePicker = NULL;
	}
	mDatePicker = new  HbDateTimePicker(popUp);
	mDatePicker->setMinimumDate(CalenDateUtils::minTime().date());
	mDatePicker->setMaximumDate(currentDate);
	mDatePicker->setDisplayFormat(CalenDateUtils::dateFormatString());
	mDatePicker->setDate(currentDate);

	popUp->setContentWidget(mDatePicker);  
	mDeleteAction = new HbAction(
						hbTrId("txt_common_button_delete"), popUp);
	popUp->addAction(mDeleteAction);
	mCancelAction = new HbAction(hbTrId("txt_common_button_cancel"),popUp);
	popUp->addAction(mCancelAction);
	// Close the popup once closeDialogs() is received
	connect(this, SIGNAL(closeDialogs()), popUp, SLOT(close()));
	// Show the popup
	popUp->open(this, SLOT(handleDateQuery(HbAction*)));
	
	OstTraceFunctionExit0( CALENDELETEUI_DATEQUERY_EXIT );
	}

// ----------------------------------------------------------------------------
// CalenDeleteUi::handleDateQuery
// Handles the selection for the date query
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::handleDateQuery(HbAction* action)
{
    OstTraceFunctionEntry0( CALENDELETEUI_HANDLEDATEQUERY_ENTRY );
    
	if(action == mDeleteAction) {
		// User selected the date before which all the entries has to be deleted
		QDate selectedDate(mDatePicker->date());
		// Check if the date is within the range.
		if(selectedDate.isValid()) {
			QTime time(0,0,0,0);
			QDateTime dateTime;
			dateTime.setDate(selectedDate);
			dateTime.setTime(time);
			// Do delete only if inputted day is after beginning of range
			if(dateTime > AgendaUtil::minTime()) {
				// Two pass delete:
				// 1. pass
				// To prevent destroying entries starting and ending midnight
				// subtract one second and do delete on that range.
				dateTime = dateTime.addSecs(-1);
				dateTime = ( dateTime > AgendaUtil::minTime()? 
											dateTime :  AgendaUtil::minTime());

				HandleDeleteMultipleEventsL( AgendaUtil::minTime(), 
																 dateTime, 1);
			}else {
				iController.BroadcastNotification(ECalenNotifyDeleteFailed);
			}
		}
	}else {
		// User pressed cancel
		iController.BroadcastNotification(ECalenNotifyDeleteFailed);
	}
	// Reset the member variables
	mDeleteAction = NULL;
	mCancelAction = NULL;
	
	OstTraceFunctionExit0( CALENDELETEUI_HANDLEDATEQUERY_EXIT );
}
// ----------------------------------------------------------------------------
// CalenDeleteUi::showRepeatingEntryDeleteQuery
// Launches the popup for deleting the repeating entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::showRepeatingEntryDeleteQuery()
{
    OstTraceFunctionEntry0( CALENDELETEUI_SHOWREPEATINGENTRYDELETEQUERY_ENTRY );
    
    HbDialog *popUp = new HbDialog();
    popUp->setParent(this);
    popUp->setDismissPolicy(HbDialog::NoDismiss);
    popUp->setTimeout(HbDialog::NoTimeout);
    popUp->setAttribute( Qt::WA_DeleteOnClose, true );
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    HbWidget *editWidget = new HbWidget();
    editWidget->setLayout(layout);
    
    HbRadioButtonList *editButtonList = new HbRadioButtonList();
    
    QStringList list;
    list << hbTrId("txt_calendar_info_this_occurrence_only")
    		<< hbTrId("txt_calendar_info_all_occurences");

    editButtonList->setItems(list);

    layout->addItem(editButtonList);
    
    popUp->setContentWidget(editWidget);
    popUp->setHeadingWidget(new HbLabel(
						hbTrId("txt_calendar_title_delete_repeated_entry")));

    // Add cancel action
    mCancelAction = new HbAction(
					hbTrId("txt_common_button_cancel_singledialog"));
    popUp->addAction(mCancelAction);
    connect(editButtonList, SIGNAL(itemSelected(int)), this,
										SLOT(handleRepeatedEntryDelete(int)));
    connect(editButtonList, SIGNAL(itemSelected(int)), popUp, SLOT(close()));
    // Close the popup once closeDialogs() is received
    connect(this, SIGNAL(closeDialogs()), popUp, SLOT(close()));
    // Show the popup
    popUp->open(this, SLOT(handleCancelAndClose(HbAction*)));
    
    OstTraceFunctionExit0( CALENDELETEUI_SHOWREPEATINGENTRYDELETEQUERY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::handleCancelAndClose
// Handles the cancel action and the close of the popup
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::handleCancelAndClose(HbAction* action)
{
	OstTraceFunctionEntry0( CALENDELETEUI_HANDLECANCELANDCLOSE_ENTRY );
	if(action == mCancelAction || !action) {
		// If the user presses cancel button broadcast ECalenNotifyDeleteFailed
		// Action will be null if the popup is closed by itself 
		// without pressing any buttons. So in this case also notification 
		// has to be broadcasted as ECalenNotifyDeleteFailed
		
		iController.BroadcastNotification(ECalenNotifyDeleteFailed);
	}
	mCancelAction = NULL;
	OstTraceFunctionExit0( CALENDELETEUI_HANDLECANCELANDCLOSE_EXIT );
}

void CalenDeleteUi::handleRepeatedEntryDelete(int index)
{
    OstTraceFunctionEntry0( CALENDELETEUI_HANDLEREPEATEDENTRYDELETE_ENTRY );
    
	// Fetch the entry
	// Find all possible instances
	AgendaEntry instance = CalenActionUiUtils::findPossibleInstanceL(
									iController.context().instanceId(),
									iController.Services().agendaInterface());

	if (!instance.isNull()) {
		connect(iController.Services().agendaInterface(), 
									SIGNAL(entryDeleted(ulong)),
									this, SLOT(entryDeleted(ulong)));
		switch(index) {
			case 0:
				// User wants to delete only this occurence
				iController.Services().agendaInterface()->deleteRepeatedEntry(
									instance, AgendaUtil::ThisOnly);
				break;
			case 1:
				// User wants to delete all the occurences
				iController.Services().agendaInterface()->deleteRepeatedEntry(
									instance, AgendaUtil::ThisAndAll);
				break;
		}
	}else {
	    iController.BroadcastNotification(ECalenNotifyDeleteFailed);
	}
	OstTraceFunctionExit0( CALENDELETEUI_HANDLEREPEATEDENTRYDELETE_EXIT );
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::showDeleteQuery
// Launches the popup for deleting the instance/instances
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::showDeleteQuery(const TDeleteConfirmationType type,
                                                     const int count)
    {
    OstTraceFunctionEntry0( CALENDELETEUI_SHOWDELETEQUERY_ENTRY );
    
    HbMessageBox *popup = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    popup->setParent(this);
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);
    popup->setAttribute( Qt::WA_DeleteOnClose, true );
    
    QString text = 0;
    
    switch(type)
        {
        case EDeleteEntry:
            {
            text.append(hbTrId("txt_calendar_info_delete_meeting"));
            break;
            }
        case EDeleteToDo:
            {
            text.append(hbTrId("txt_calendar_info_delete_todo_note"));
            break;
            }
        case EDeleteToDos:
            {//"Delete %N to-do notes?"
            // TODO: Add the text id
            text.append("Delete %N to-do's?").arg(count);
            break;
            }
        case EDeleteAll:
            {
            text.append(hbTrId("txt_calendar_info_delete_all_calendar_entries"));
            break;
            }
        case EDeleteEvent:
            {
            text.append(hbTrId("txt_calendar_info_delete_allday_event"));
            break;
            }
        default:
            break;
        }
    
    popup->setText(text);
    	
    QList<QAction*> list = popup->actions();
    for(int i=0; i < list.count(); i++)
        {
        popup->removeAction(list[i]);
        }
    mDeleteAction = new HbAction(
						hbTrId("txt_common_button_delete"), popup);
    popup->addAction(mDeleteAction);
    mCancelAction = new HbAction(hbTrId("txt_common_button_cancel"), popup); 
    popup->addAction(mCancelAction);
    // Close the popup once closeDialogs() is received
    connect(this, SIGNAL(closeDialogs()), popup, SLOT(close()));
    // Show the popup
    popup->open(this, SLOT(handleDeletion(HbAction*)));
    
    OstTraceFunctionExit0( CALENDELETEUI_SHOWDELETEQUERY_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::handleDeletion
// Deletes the entries based on the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::handleDeletion(HbAction* action)
{
    OstTraceFunctionEntry0( CALENDELETEUI_HANDLEDELETION_ENTRY );
    
	TCalenNotification notification = ECalenNotifyDeleteFailed;
	
	if(action == mDeleteAction) {
		
		switch (mDeleteCommand) {
			
			case ECalenDeleteCurrentEntry:
			{
				// Get the entry id
				ulong id = iController.context().instanceId().mEntryLocalUid;
				connect(iController.Services().agendaInterface(), 
												SIGNAL(entryDeleted(ulong)),
												this, SLOT(entryDeleted(ulong)));
				// Delete the entry if the delete button is been pressed
				iController.Services().agendaInterface()->deleteEntry(id);
			}
				break;
			
			case ECalenDeleteSeries:
			case ECalenDeleteCurrentOccurrence:
			{
				AgendaEntry instance = 
						CalenActionUiUtils::findPossibleInstanceL(
								iController.context().instanceId(),
								iController.Services().agendaInterface() );
				if(!instance.isNull()) {
					QDateTime recId = instance.recurrenceId().toUTC();
					const bool child = recId.isNull();
					connect(iController.Services().agendaInterface(), 
					        SIGNAL(entryDeleted(ulong)),
					        this, SLOT(entryDeleted(ulong)));
					if( !child || mRecurrenceRange == AgendaUtil::ThisOnly 
							|| mRecurrenceRange == AgendaUtil::ThisAndAll) {
						iController.Services().agendaInterface()->deleteRepeatedEntry( 
								instance, mRecurrenceRange );
					}
				}else {
				    iController.BroadcastNotification(ECalenNotifyDeleteFailed); 
				}
			}
				break;
			
			case ECalenDeleteAllEntries:
			{
				HandleDeleteMultipleEventsL( AgendaUtil::minTime(), 
											AgendaUtil::maxTime(),1 );
			}
				break;
			
			default:
				break;
				
		}
	} else if(!action || action == mCancelAction ) {
		// Action will be null if the popup is closed by itself 
		// without pressing any buttons. So in this case also notification 
		// has to be broadcasted as ECalenNotifyDeleteFailed
		// If the user presses cancel button also the same has to happen
		// Notify the status
		iController.BroadcastNotification(notification);
	}
	
	// Reset the member variables
	mDeleteAction = NULL;
	mCancelAction = NULL;
	
	OstTraceFunctionExit0( CALENDELETEUI_HANDLEDELETION_EXIT );
}

void CalenDeleteUi::entryDeleted(ulong id)
{
    OstTraceFunctionEntry0( CALENDELETEUI_ENTRYDELETED_ENTRY );
    
    if (iController.context().instanceId().mEntryLocalUid == id) {
        iController.BroadcastNotification(ECalenNotifyEntryDeleted);
    }
    disconnect(iController.Services().agendaInterface(), SIGNAL(entryDeleted(ulong)),
               this, SLOT(entryDeleted(ulong)));
    
    OstTraceFunctionExit0( CALENDELETEUI_ENTRYDELETED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::HandleDeleteMultipleEventsL
// Handles multiple delete events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::HandleDeleteMultipleEventsL( const QDateTime& aFirstDay,
                                                 const QDateTime& aLastDay,
                                                 int aConfNoteId )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_HANDLEDELETEMULTIPLEEVENTSL_ENTRY );
    
	if(iIsDeleting) {
		OstTraceFunctionExit0( CALENDELETEUI_HANDLEDELETEMULTIPLEEVENTSL_EXIT );
		return;
	}
    iConfirmationNoteId = aConfNoteId;
    
    DeleteDayRangeL( aFirstDay, aLastDay );

    OstTraceFunctionExit0( DUP1_CALENDELETEUI_HANDLEDELETEMULTIPLEEVENTSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteDayRangeL
// Deletes all entries in a given range.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteDayRangeL( const QDateTime& aStart,
                                                      const QDateTime& aEnd )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETEDAYRANGEL_ENTRY );
    
    iStartTime = aStart;
    iEndTime = aEnd;
    
    iIsDeleting = ETrue;
    
    //AgendaUtil& agendaInterface = iController.agendaInterface();
    // Connect to the signal that gets generated when deletion is completed
    connect(iController.agendaInterface(), SIGNAL(entriesDeleted(int)), this,
            SLOT(doCompleted(int)));
    AgendaUtil::FilterFlags filter =
    	        AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries
    	                | AgendaUtil::IncludeAppointments
    	                | AgendaUtil::IncludeEvents
    	                | AgendaUtil::IncludeReminders
    	                | AgendaUtil::IncludeIncompletedTodos
    	                | AgendaUtil::IncludeCompletedTodos
    	                | AgendaUtil::IncludeAnniversaries);
    // 1: First pass, delete all entries.
    iController.agendaInterface()->deleteEntries(iStartTime, iEndTime, filter);

    OstTraceFunctionExit0( CALENDELETEUI_DELETEDAYRANGEL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DoCompletedL
// Handles delete callback
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::doCompleted( int aFirstPassError )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DOCOMPLETED_ENTRY );

    if( aFirstPassError == KErrNone )
        {
        // 2: Second pass, delete notes that end 00:00 of next day of iEndTime
        // We didn't delete them in first pass
        QDateTime nextMidnight = CalenDateUtils::beginningOfDay( iEndTime.addDays(1) );
        nextMidnight = ( nextMidnight < AgendaUtil::maxTime()? nextMidnight : AgendaUtil::maxTime() );
        PIM_TRAPD_HANDLE( deleteEntriesEndingAtMidnight( nextMidnight ) );
        }

    // 3. End deleting, close wait dialog, and show confirmation or error note
    iIsDeleting = EFalse;
    
    iController.BroadcastNotification( ECalenNotifyMultipleEntriesDeleted );

    OstTraceFunctionExit0( CALENDELETEUI_DOCOMPLETED_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntriesEndingAtMidnightL
// Deletes entries ending at midnight on the given day
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::deleteEntriesEndingAtMidnight( QDateTime aMidnight )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETEENTRIESENDINGATMIDNIGHT_ENTRY );

    QDateTime start, end;
    QTime startTime(aMidnight.time());
    startTime.setHMS(startTime.hour(), startTime.minute() - 1,
                    startTime.second(), startTime.msec());
    start.setDate(aMidnight.date());
    start.setTime(startTime);
    
    end = aMidnight;
    
    QList<AgendaEntry> instances = iController.Services().agendaInterface()->fetchEntriesInRange( start, end );

    // 2. loop through them and delete those entries that end at midnight
    for( int i=0; i < instances.count(); ++i )
        {
        AgendaEntry entry = instances[i];

        // Checking that if entry ends at midnight, is quite clumsy, but here goes:
        // EndsAtStartOfDay takes only CCalInstance, but here we mimic EndsAtStartOfDay
        // for CCalEntry type.

        // First check that if _instance_ ends at midnight, but starts earlier
        if( CalenAgendaUtils::endsAtStartOfDay( entry, aMidnight ) )
            {
            // Second, check that _entry's_ endtime is exactly the midnight
            // This prevents us from destroying repeating entries, that has one
            // instance falling on given midnight.
            if( entry.endTime() == aMidnight )
                {
                iController.Services().agendaInterface()->deleteRepeatedEntry(entry, AgendaUtil::ThisAndAll);
                }
            }
        }

    OstTraceFunctionExit0( CALENDELETEUI_DELETEENTRIESENDINGATMIDNIGHT_EXIT );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteSingleInstanceL
// Delete the given instance. Ask the user whether to delete the series or the instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteSingleInstanceL( AgendaEntry& aInstance )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETESINGLEINSTANCEL_ENTRY );
    
    DoDeleteSingleInstanceL( aInstance, EFalse, AgendaUtil::ThisAndAll );
    
    OstTraceFunctionExit0( CALENDELETEUI_DELETESINGLEINSTANCEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteSingleInstanceL
// Delete the given instance. Delete the entry range given by aRepeatType.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteSingleInstanceL( AgendaEntry& aInstance, 
                                AgendaUtil::RecurrenceRange aRepeatType )
    {
    OstTraceFunctionEntry0( DUP1_CALENDELETEUI_DELETESINGLEINSTANCEL_ENTRY );
    
    DoDeleteSingleInstanceL( aInstance, ETrue, aRepeatType );
    
    OstTraceFunctionExit0( DUP1_CALENDELETEUI_DELETESINGLEINSTANCEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DoDeleteSingleInstanceL
// Performs the deletion of the instance. If aHasRepeatType is EFalse, the user
// is prompted to delete either the instance or the entire series. In this case,
// aRepeatType is ignored. If aHasRepeatType is ETrue, aRepeatType determines
// whether to delete the instance or the entire series.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DoDeleteSingleInstanceL(
									AgendaEntry& aInstance,
									bool aHasRepeatType,
									AgendaUtil::RecurrenceRange aRepeatType )
{
    OstTraceFunctionEntry0( CALENDELETEUI_DODELETESINGLEINSTANCEL_ENTRY );
    
	bool repeating = aInstance.isRepeating();

	if( !repeating ) {
		// Even though there is no RRule, the entry might
		// have a list of rdates.
		QList<QDate> rDates = aInstance.rDates();
		repeating = ( rDates.count() > 0 );
	}
	QDateTime recId = aInstance.recurrenceId().toUTC();
	const bool child = recId.isNull();

	if( !aHasRepeatType ) {
		aRepeatType = AgendaUtil::ThisAndAll;
	}
	// For later reference in handleDeletion()
	mRecurrenceRange = aRepeatType;

	if( !aHasRepeatType && ( child || repeating ) && 
			( aInstance.type() != AgendaEntry::TypeAnniversary ) ) {
		showRepeatingEntryDeleteQuery();
	}
	else
	{
		showDeleteQuery(aInstance.type() == AgendaEntry::TypeTodo ?
														EDeleteToDo :
														EDeleteEntry );
	}
	OstTraceFunctionExit0( CALENDELETEUI_DODELETESINGLEINSTANCEL_EXIT );
}


// -----------------------------------------------------------------------------
// CalenDeleteUi::DialogDismissedL
// From MProgressDialogCallback
// Callback method
// called when a dialog is dismissed.
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DialogDismissedL( const TInt /*aButtonId*/ )
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DIALOGDISMISSEDL_ENTRY );
    
    // no more entries to delete
    iMoreEntriesToDelete = EFalse;
    iDisplayQuery = EFalse;
    
    // issue notification cancel delete
    iController.BroadcastNotification(ECalenNotifyCancelDelete);
    
    OstTraceFunctionExit0( CALENDELETEUI_DIALOGDISMISSEDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntryL
// Delete entry using entry local uid
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteEntryL(ulong& aEntryLocalUid)
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DELETEENTRYL_ENTRY );
    
    iController.Services().agendaInterface()->deleteEntry(aEntryLocalUid);
    
    OstTraceFunctionExit0( CALENDELETEUI_DELETEENTRYL_EXIT );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DisplayWaitDialogL
// Display wait dialog
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DisplayWaitDialogL()
    {
    OstTraceFunctionEntry0( CALENDELETEUI_DISPLAYWAITDIALOGL_ENTRY );
    
    OstTraceFunctionExit0( CALENDELETEUI_DISPLAYWAITDIALOGL_EXIT );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::MarkedEntriesDeletedL
// Dismiss wait dialog and show information note
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::MarkedEntriesDeletedL()
    {
    OstTraceFunctionEntry0( CALENDELETEUI_MARKEDENTRIESDELETEDL_ENTRY );
    
    // notify marked entries deleted
    iController.BroadcastNotification( ECalenNotifyMarkedEntryDeleted );
    
    OstTraceFunctionExit0( CALENDELETEUI_MARKEDENTRIESDELETEDL_EXIT );
    }

// End of File
