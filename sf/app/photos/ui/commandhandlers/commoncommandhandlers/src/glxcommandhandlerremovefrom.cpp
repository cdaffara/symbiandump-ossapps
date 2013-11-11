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

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxcollectionpluginalbums.hrh>
#include <glxcommandhandlers.hrh>
#include "glxlocalisationstrings.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandlerremovefromTraces.h"
#endif


GlxCommandHandlerRemoveFrom::GlxCommandHandlerRemoveFrom()
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERREMOVEFROM_GLXCOMMANDHANDLERREMOVEFROM_ENTRY );
    OstTraceFunctionExit0( GLXCOMMANDHANDLERREMOVEFROM_GLXCOMMANDHANDLERREMOVEFROM_EXIT );
    }

GlxCommandHandlerRemoveFrom::~GlxCommandHandlerRemoveFrom()
    {
    OstTraceFunctionEntry0( DUP1_GLXCOMMANDHANDLERREMOVEFROM_GLXCOMMANDHANDLERREMOVEFROM_ENTRY );
    OstTraceFunctionExit0( DUP1_GLXCOMMANDHANDLERREMOVEFROM_GLXCOMMANDHANDLERREMOVEFROM_EXIT );
    }

CMPXCommand* GlxCommandHandlerRemoveFrom::CreateCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& aConsume) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERREMOVEFROM_CREATECOMMANDL_ENTRY );
    Q_UNUSED(aCommandId);
    Q_UNUSED(aConsume);

    CMPXCommand* command = NULL;
    CMPXCollectionPath* path = aMediaList.PathLC(
             NGlxListDefs::EPathFocusOrSelection);
           
    if(aCommandId == EGlxCmdRemoveFromFav)
        {
          // adding to the favourites
          TGlxMediaId favId(KGlxCollectionFavoritesId);     
          command = TGlxCommandFactory::RemoveFromContainerCommandLC(favId, *path);
        }
    else
        {
          command = TGlxCommandFactory::RemoveFromContainerCommandLC(*path);
        }

    CleanupStack::Pop(command);
    CleanupStack::PopAndDestroy(path);
    OstTraceFunctionExit0( GLXCOMMANDHANDLERREMOVEFROM_CREATECOMMANDL_EXIT );
    return command;
    }

QString GlxCommandHandlerRemoveFrom::CompletionTextL() const
    {
	return QString();
    }

QString GlxCommandHandlerRemoveFrom::ProgressTextL() const
    {
    return GLX_REMOVE_IMAGES;
    }
