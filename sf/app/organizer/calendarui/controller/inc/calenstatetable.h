/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar controller
*
*/


#ifndef CALENSTATES_H
#define CALENSTATES_H

// INCLUDES
#include <e32base.h>
#include "hb_calencommands.hrh"      // command ids
#include "calennotificationhandler.h"      // TCalenNotification

// ENUMERATIONS

// Enumeration of the various possible states that the controller can be
// be in.
enum TControllerState
    {
    EIdle                       = 1 << 0,
    EWaitingForInstanceView     = 1 << 1,
    EWaitingForEntryView        = 1 << 2,
    EConstructingNativeViews    = 1 << 3,
    EConstructingPlugins        = 1 << 4,
    EExiting                    = 1 << 5,

    ESending                    = 1 << 6,
    EDeleting                   = 1 << 7,
    EEditing                    = 1 << 8,
    EViewing                    = 1 << 9,
    ESaving                     = 1 << 10,
    EPrinting                   = 1 << 11,
    EDisplayingSettings         = 1 << 12,
    EDisplayingHelp             = 1 << 13
    };


// Each command has an associated state table entry, ( which is a structure
// than a class in order for it to be used as elements within a constant
// array by removing the need for a constructor). The state table entry
// contains details of what states it is valid to call the command in,
// what the state should be changed to as the command is processed, and
// what notifications will revert the controller to its previous state if
// applicable.
struct TStateTableEntry
    {
    // Bit field for what states the command it is valid to call the command
    TInt    iValidStates;

    // The state the contoller should be placed in after the command is called
    TControllerState    iNextState;

    // Bit field containing the notifications that will move the contoller to
    // its previous state if applicable.
    TInt    iCompleteNotification;
    };

// CONSTANTS

// Bitmask to represent any notifications
const TInt KAnyCalenNotification = -1;

const TInt KCalenNoNotification = 0;

// Bitmask representing the four notifications that
// may be received when the editor is closed.
const TInt KCalenEditorClosedNotification = ECalenNotifyEntrySaved
                                            | ECalenNotifyEntryDeleted
                                            | ECalenNotifyInstanceDeleted
                                            | ECalenNotifyEntryClosed;

// Bitmask represents completion of a delete command
const TInt KCalenDeleteComplete = ECalenNotifyEntryDeleted 
                                            | ECalenNotifyDeleteFailed
                                            | ECalenNotifyMultipleEntriesDeleted;


// State table entry that represents the controllers idle state
const TStateTableEntry KControllerIdleState
                     = { EIdle, EIdle, KCalenNoNotification};

// State table entry representing the controller state when the entry view is
// being created asyncronously.
const TStateTableEntry KControllerWaitingForEntryView
                = { EIdle, EWaitingForEntryView, ECalenNotifyViewCreated  };

// State table entry representing the controller state when the instance view is
// being created asyncronously.
const TStateTableEntry KControllerWaitingForInstanceView
                = { EIdle, EWaitingForInstanceView, ECalenNotifyViewCreated };

const TInt KCalenLaunchEditorStates 
                        = EIdle | EWaitingForInstanceView | EWaitingForEntryView;

// State lookup table for view manager commands
// It is assumed that all view commands return immediately, therefore they will not
// change the state of the controller.
const TStateTableEntry KStateTable[] =
    {
    /* ECalenMonthView */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenDayView */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenAgendaView */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenForwardsToDayView */
    { EIdle , EIdle, KCalenNoNotification },
    
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenNextView */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenPrevView */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenCancelViewCycling */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenShowToolbar */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenHideToolbar */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenUpdateToolbar */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenGotoToday */
    { EIdle , EIdle, KCalenNoNotification },

    /* ECalenGotoDate */
    { EIdle , EIdle, KCalenNoNotification },

    // State lookup table for edit ui commands

    /* ECalenNewMeeting */
    { KCalenLaunchEditorStates , EEditing, KCalenEditorClosedNotification  },

    /* ECalenNewTodo */
    { KCalenLaunchEditorStates , EEditing, KCalenEditorClosedNotification },

    /* ECalenNewAnniv */
    { KCalenLaunchEditorStates , EEditing, KCalenEditorClosedNotification },

    /* ECalenNewDayNote */
    { KCalenLaunchEditorStates , EEditing, KCalenEditorClosedNotification },

    /* ECalenNewReminder */
    { KCalenLaunchEditorStates , EEditing, KCalenEditorClosedNotification },

    /* ECalenNewMeetingRequest */
    { KCalenLaunchEditorStates, EEditing, KCalenEditorClosedNotification },

    /* ECalenNewEntry */
    { KCalenLaunchEditorStates , EEditing, KCalenEditorClosedNotification },

    /* ECalenEditCurrentEntry */
    { EIdle , EEditing, KCalenEditorClosedNotification },

    /* ECalenEditSeries */
    { EIdle , EEditing, KCalenEditorClosedNotification },

    /* ECalenEditOccurrence */
    { EIdle , EEditing, KCalenEditorClosedNotification },

    /* ECalenViewCurrentEntry */
    { EIdle , EViewing, KCalenEditorClosedNotification },

    /* ECalenNotifyFocusChange */
    { EIdle , EIdle, KCalenNoNotification }, 

    /* ECalenCompleteTodo */
    { EIdle , EIdle, ECalenNotifyEntrySaved | ECalenNotifyCommandFailed }, 

    /* ECalenRestoreTodo */
    { EIdle , EIdle, ECalenNotifyEntrySaved | ECalenNotifyCommandFailed }, 

    /* ECalenSend */
    { EIdle | EViewing | EEditing, ESending, KAnyCalenNotification }, 

    // State lookup table for delete Ui commands

    /* ECalenDeleteCurrentEntry */
    { EIdle | EViewing | EEditing , EDeleting, KCalenDeleteComplete  },

    /* ECalenDeleteEntryWithoutQuery */
    { EIdle | EViewing | EEditing , EDeleting, KCalenDeleteComplete },

    /* ECalenDeleteSeries */
    { EIdle | EViewing | EEditing , EDeleting, KCalenDeleteComplete },

    /* ECalenDeleteOccurrence */
    { EIdle | EViewing | EEditing , EDeleting, KCalenDeleteComplete },

    /* ECalenDeleteSingleInstance */
    {EIdle | EViewing | EEditing , EDeleting, KCalenDeleteComplete  },

    /* ECalenDeleteSingleInstanceWithRepeatType */
    { EIdle | EViewing | EEditing , EDeleting, KCalenDeleteComplete },

    /* ECalenDeleteAllEntries */
    { EIdle , EDeleting, KCalenDeleteComplete },

    /* ECalenDeleteEntriesBeforeDate */
    { EIdle , EDeleting, KCalenDeleteComplete },

    /* ECalenShowSettings */
     { EIdle , EDisplayingSettings, ECalenNotifySettingsClosed }
    };

// The number of commands in each of the state tables can be calculated from the size of the
// table at compilation.
const TInt KNumberCommands = sizeof( KStateTable ) / sizeof( TStateTableEntry );

// A TFixedArray wrapper class has been typedef'ed for the four
// lookup tables for out of bounds safety and readablity of
// the code.
typedef TFixedArray< TStateTableEntry, KNumberCommands > TStateTable;

#endif // CALENCONTROLLERSTATES_H

// End of file

