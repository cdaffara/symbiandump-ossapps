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
* Description: 
*
*/

#include "glxlocalisationstrings.h"

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlerdelete.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandlerdeleteTraces.h"
#endif


GlxCommandHandlerDelete::GlxCommandHandlerDelete()
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERDELETE_GLXCOMMANDHANDLERDELETE_ENTRY );
    OstTraceFunctionExit0( GLXCOMMANDHANDLERDELETE_GLXCOMMANDHANDLERDELETE_EXIT );
    }

GlxCommandHandlerDelete::~GlxCommandHandlerDelete()
    {
    OstTraceFunctionEntry0( DUP1_GLXCOMMANDHANDLERDELETE_GLXCOMMANDHANDLERDELETE_ENTRY );
    OstTraceFunctionExit0( DUP1_GLXCOMMANDHANDLERDELETE_GLXCOMMANDHANDLERDELETE_EXIT );
    }

CMPXCommand* GlxCommandHandlerDelete::CreateCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& aConsume) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERDELETE_CREATECOMMANDL_ENTRY );
    Q_UNUSED(aCommandId);
    Q_UNUSED(aConsume);
    CMPXCollectionPath* path = aMediaList.PathLC(
            NGlxListDefs::EPathFocusOrSelection);
    CMPXCommand* command = TGlxCommandFactory::DeleteCommandLC(*path);
    CleanupStack::Pop(command);
    CleanupStack::PopAndDestroy(path);
    OstTraceFunctionExit0( GLXCOMMANDHANDLERDELETE_CREATECOMMANDL_EXIT );
    return command;
    }

QString GlxCommandHandlerDelete::CompletionTextL() const
    {
	return QString();
    }

QString GlxCommandHandlerDelete::ProgressTextL() const
    {
    return (GLX_DELETE_PROGRESS);
    }

QString GlxCommandHandlerDelete::ConfirmationTextL(bool multiSelection ) const
    {
    QString retString;
    if(multiSelection)
        {
        retString = GLX_MULTIPLE_DELETE;
        }
    else
        {
         
        retString = GLX_DIALOG_DELETE_ONE_ITEM ;
        }
    return  retString;   
    }    
    
