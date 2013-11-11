/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Queues commands to MDS.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include "vcxmyvideosmdscmdqueue.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosmdsalbums.h"


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsCmdQueue::CVcxMyVideosMdsCmdQueue( CVcxMyVideosMdsDb& aMdsDb )
: iMdsDb( aMdsDb )
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::ConstructL()
    {
    iQueue.Reset();    
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsCmdQueue* CVcxMyVideosMdsCmdQueue::NewL( CVcxMyVideosMdsDb& aMdsDb )
    {
    CVcxMyVideosMdsCmdQueue* self = new(ELeave) CVcxMyVideosMdsCmdQueue( aMdsDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsCmdQueue::~CVcxMyVideosMdsCmdQueue()
    {
    for ( TInt i = 0; i < iQueue.Count(); i++ )
        {
        delete iQueue[i];
        }
    iQueue.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::Cancel
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::Cancel( CVcxMyVideosMdsDb::TRequestType aType )
    {
    TInt count = iQueue.Count()-1;
    for ( TInt i = count; i >= 0; i-- )
        {
        if ( iQueue[i]->iCmdType == aType || aType == CVcxMyVideosMdsDb::EAll )
            {
            delete iQueue[i];
            iQueue[i] = NULL;
            iQueue.Remove( i );
            }
        }    
    iQueue.Compress();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::ExecuteCmdL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::ExecuteCmdL( CVcxMyVideosMdsCmd* aCmd )
    {    
    if ( iQueue.Count() == 0 && !iCmdInProgress )
        {
        DoExecuteCmdL( aCmd ); //ownership does not move
        delete aCmd;
        }
    else
        {
        iQueue.AppendL( aCmd );
        }    
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::CmdFinished
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::CmdFinished()
    {
    iCmdInProgress = EFalse;

    TInt err( KErrGeneral );
    while ( err != KErrNone )
        {
        if ( iQueue.Count() == 0 )
            {
            break;
            }
                            
        TInt cmdIndexToExecute;
        // Let other commads bypass EGetVideoList
        if ( iQueue.Count() > 1 && iQueue[0]->iCmdType == CVcxMyVideosMdsDb::EGetVideoList )
            {
            cmdIndexToExecute = 1;
            }
        else
            {
            cmdIndexToExecute = 0;
            }
        TRAP( err, DoExecuteCmdL( iQueue[cmdIndexToExecute] ) ); //ownership does not move
        delete iQueue[cmdIndexToExecute];
        iQueue[cmdIndexToExecute] = NULL;
        iQueue.Remove( cmdIndexToExecute );
        iQueue.Compress();
        
#ifdef _DEBUG
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosMdsCmdQueue:: MDS cmd failed: %d", err);
            }
#endif
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::DoExecuteCmdL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::DoExecuteCmdL( CVcxMyVideosMdsCmd* aCmd )
    {    
    switch ( aCmd->iCmdType )
        {
        case CVcxMyVideosMdsDb::EGetVideoList:
            {
            CVcxMyVideosMdsCmdGetVideoList* cmd =
                    static_cast<CVcxMyVideosMdsCmdGetVideoList*>( aCmd );
            iMdsDb.DoCreateVideoListL( cmd->iSortingOrder,
                    cmd->iAscending,
                    cmd->iFullDetails,
                    *cmd->iVideoList );
            iCmdInProgress = ETrue;
            }
            break;
        
        case CVcxMyVideosMdsDb::EGetAlbums:
            {
            CVcxMyVideosMdsCmdGetAlbums* cmd = static_cast<CVcxMyVideosMdsCmdGetAlbums*>( aCmd );
            iMdsDb.iAlbums->DoGetAlbumsL( cmd->iAlbumList, *cmd->iClient );
            iCmdInProgress = ETrue;
            }
            break;

        case CVcxMyVideosMdsDb::EGetAlbumContentIds:
            {
            CVcxMyVideosMdsCmdGetAlbumContentIds* cmd =
                    static_cast<CVcxMyVideosMdsCmdGetAlbumContentIds*>( aCmd );
            iMdsDb.iAlbums->DoGetAlbumContentIdsL( cmd->iAlbumId, *cmd->iAlbumContent,
                    *cmd->iClient );
            iCmdInProgress = ETrue;
            }
            break;

        case CVcxMyVideosMdsDb::EGetAlbumContentVideos:
            {
            CVcxMyVideosMdsCmdGetAlbumContentVideos* cmd =
                    static_cast<CVcxMyVideosMdsCmdGetAlbumContentVideos*>( aCmd );
            iMdsDb.iAlbums->DoGetAlbumContentVideosL( cmd->iAlbumId, *cmd->iAlbumContentVideos,
                    *cmd->iClient );
            iCmdInProgress = ETrue;
            }
            break;
            
        case CVcxMyVideosMdsDb::EAddVideosToAlbum:
            {
            CVcxMyVideosMdsCmdAddVideosToAlbum* cmd =
                    static_cast<CVcxMyVideosMdsCmdAddVideosToAlbum*>( aCmd );
            iMdsDb.iAlbums->DoAddVideosToAlbumL( cmd->iMpxCmd,
                    *cmd->iClient );
            iCmdInProgress = ETrue;
            }
            break;

        case CVcxMyVideosMdsDb::ERemoveRelations:
            {
            CVcxMyVideosMdsCmdRemoveRelations* cmd =
                    static_cast<CVcxMyVideosMdsCmdRemoveRelations*>( aCmd );
            iMdsDb.iAlbums->DoRemoveRelationsL( *cmd->iRelationIds, *cmd->iResults,
                    *cmd->iClient );
            iCmdInProgress = ETrue;
            }
            break;
            
        case CVcxMyVideosMdsDb::ERemoveAlbums:
            {
            CVcxMyVideosMdsCmdRemoveAlbums* cmd =
                    static_cast<CVcxMyVideosMdsCmdRemoveAlbums*>( aCmd );
            iMdsDb.iAlbums->DoRemoveAlbumsL( cmd->iMpxCmd, *cmd->iClient );
            iCmdInProgress = ETrue;
            }
            break;

        default:
            break;
        }    
    }    
// END OF FILE
