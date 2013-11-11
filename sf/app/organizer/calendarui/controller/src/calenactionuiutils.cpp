/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of UI utils
*
*/

// System Includes
#include <QtGui>
#include <hblabel.h>
#include <hbaction.h>
#include <hbinputdialog.h>
#include <HbMessageBox>
#include <agendautil.h>
#include <agendaentry.h>

// User includes
#include "calendarui_debug.h"
#include "calenactionuiutils.h"
#include "calendar.hrh"
#include "hb_calencommands.hrh"
#include "caleninstanceid.h"
#include "CleanupResetAndDestroy.h"
#include "calendateutils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenactionuiutilsTraces.h"
#endif

// -----------------------------------------------------------------------------
// CCalenCommonUI::FindPossibleInstanceL
// Finds an instance with the given instance id and instance view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
AgendaEntry CalenActionUiUtils::findPossibleInstanceL(const TCalenInstanceId& id,
                                                         AgendaUtil* agendaUtil )
    {
    OstTraceFunctionEntry0( CALENACTIONUIUTILS_FINDPOSSIBLEINSTANCEL_ENTRY );
    
    AgendaUtil::FilterFlags filter = 
    		AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries |
									 AgendaUtil::IncludeAppointments | 
									 AgendaUtil::IncludeEvents |
									 AgendaUtil::IncludeReminders |
									 AgendaUtil::IncludeIncompletedTodos);
    QList<AgendaEntry> instances = 
				agendaUtil->createEntryIdListForDay(id.mInstanceTime, filter);
    AgendaEntry result;

    // For instances finishing the next day (now possible with unified DateTime editor),
    // we have to do our best to match the instance time exactly - otherwise we could
    // match the LocalUid to the incorrect instance in a series.
    for ( TInt i=0; i < instances.count() && (result.isNull()); ++i )
        {
        if( instances[i].id() == id.mEntryLocalUid )
            {
            // Check the instance time matches.
            if( instances[i].startTime() == id.mInstanceTime )
                {
                result = instances[i];
                instances.removeAt(i);
                }
            }
        }

    if( result.isNull() )
        {
        // Couldn't match the instance time exactly - just use the instance
        // with the same LocalUid as the one we're looking for.
        for ( TInt i=0; i < instances.count() && (result.isNull()); ++i )
            {
            if( instances[i].id() == id.mEntryLocalUid )
                {
                result = instances[i];
                instances.removeAt(i);
                }
            }
        }

    OstTraceFunctionExit0( CALENACTIONUIUTILS_FINDPOSSIBLEINSTANCEL_EXIT );
    return result;
    }


// ----------------------------------------------------------------------------
// CalenActionUiUtils::SetToDoCompleteStatusL
// Mark to-do entry as completed or restore a completed to-do and save.
// This is not in the engine layer because it shows an error mesage on failure.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenActionUiUtils::setToDoCompleteStatus( AgendaUtil* agendaUtil,
                                                 AgendaEntry& entry,
                                                 const bool status )
    {
    OstTraceFunctionEntry0( CALENACTIONUIUTILS_SETTODOCOMPLETESTATUS_ENTRY );
    
    ASSERT( !entry.isNull() );
    QDateTime now = QDateTime::currentDateTime();

    // set as completed or restore and update the entry in the database
    agendaUtil->setCompleted(entry, status, now);

    OstTraceFunctionExit0( CALENACTIONUIUTILS_SETTODOCOMPLETESTATUS_EXIT );
    }

// End of file
