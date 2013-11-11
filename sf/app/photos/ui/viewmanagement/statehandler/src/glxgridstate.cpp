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
* Description:   ?Description
*
*/



#include <glxgridstate.h>
#include <glxstatemanager.h>
#include <glxcommandhandlers.hrh>
#include <QDebug>

GlxGridState::GlxGridState(GlxStateManager *stateManager, GlxState *preState ) : GlxState(GLX_GRIDVIEW_ID, preState)
{
     if ( preState)
         qDebug("GlxGridState::GlxGridState() state id = %d", preState->id());
     
     mState = ALL_ITEM_S;
     mStateManager = stateManager ;
     mIsMarkingMode = FALSE;
     mCommandId = EGlxCmdHandled;
}

void GlxGridState::eventHandler(qint32 &id)
{      
    qDebug("GlxGridState::eventHandler() action id = %d", id);
    defaultEventHandler (id );
    if ( id == EGlxCmdHandled ) {
        return;
    }
    
    switch ( mState ) {
    case ALL_ITEM_S :
	case FETCHER_ITEM_S :
        allItemEventHandler ( id ) ;
        break;
        
    case ALBUM_ITEM_S :
    case FETCHER_ALBUM_ITEM_S :
    	albumItemEventHandler ( id ) ;
    	break;
    	
    default :
    	break;
    }
}

void GlxGridState::defaultEventHandler ( qint32 &id )
{
   qDebug("GlxGridState::defaultEventHandler() action id = %d", id);
   
   switch(id) { 
   case EGlxCmdFetcherFullScreenOpen:
       id = EGlxCmdHandled;
       mStateManager->nextState( GLX_FULLSCREENVIEW_ID, FETCHER_S );
       break;
   case EGlxCmdBrowseFullScreenOpen:   
       id = EGlxCmdHandled;
       mStateManager->nextState( GLX_FULLSCREENVIEW_ID, BROWSE_S );
       break;

   case EGlxCmdAddToAlbum :
   case EGlxCmdDelete :
   case EGlxCmdRemoveFrom :
   case EGlxCmdSend :
       mCommandId = id;
       mStateManager->enterMarkingMode();
       mIsMarkingMode = TRUE;
       id = EGlxCmdHandled;
       break;
       
   case EGlxCmdBack :
       //here this command will be handled for only marking mode
       if (mIsMarkingMode == TRUE ) {
           mStateManager->exitMarkingMode();
           mIsMarkingMode = FALSE;
           mCommandId = id = EGlxCmdHandled;
       }
       break;
       
   case EGlxCmdSelect:
       if ( mStateManager->executeCommand(mCommandId) ) {
           mStateManager->exitMarkingMode();
           mIsMarkingMode = FALSE;
           mCommandId = EGlxCmdHandled;
       }
       id = EGlxCmdHandled;
       break;
       
   case EGlxCmdCancel :
       mStateManager->exitMarkingMode();
       mIsMarkingMode = FALSE;
       mCommandId = id = EGlxCmdHandled;
       break;
	        
    default:
       break;
	}
}

void GlxGridState::allItemEventHandler ( qint32 &id )
{
    qDebug("GlxGridState::allItemEventHandler() action id = %d", id);
    
    switch(id) {
    case EGlxCmdAllGridOpen :
    case EGlxCmdFetcherAllGridOpen :   
    	id = EGlxCmdHandled;
    	break ;
    case EGlxCmdFullScreenOpen :
        mStateManager->nextState( GLX_FULLSCREENVIEW_ID, ALL_FULLSCREEN_S );
        id = EGlxCmdHandled;
    	break;
    default :
    	break;    
    }
}
	
void GlxGridState::albumItemEventHandler ( qint32 &id )
{
    qDebug("GlxGridState::albumItemEventHandler() action id = %d", id);

    switch(id) {
    case EGlxCmdAllGridOpen :
        mStateManager->removeCurrentModel();
        mStateManager->goBack( GLX_GRIDVIEW_ID, ALL_ITEM_S );
        id = EGlxCmdHandled;
        break ;

    case EGlxCmdFetcherAllGridOpen :
        mStateManager->removeCurrentModel();
        mStateManager->goBack( GLX_GRIDVIEW_ID, FETCHER_ITEM_S );
        id = EGlxCmdHandled;
        break ;
        
    case EGlxCmdAlbumListOpen :
    case EGlxCmdFetcherAlbumListOpen:        
    case EGlxCmdBack :
        mStateManager->removeCurrentModel();
        mStateManager->previousState();
        id = EGlxCmdHandled;
        break;

    case EGlxCmdFullScreenOpen :
        mStateManager->nextState( GLX_FULLSCREENVIEW_ID, ALBUM_FULLSCREEN_S );
        id = EGlxCmdHandled;
    	break;
    default :
        break;
    }
}

void GlxGridState::setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect)
{
    qDebug("GlxListState::setTranstionParameter dir = %d", dir);
    if ( dir == NO_DIR || dir == FORWARD_DIR) {
        effect = ALBUMLIST_TO_GRID ;
        viewEffect = BOTH_VIEW ;
    }
    
    if ( dir == BACKWARD_DIR){
        effect = GRID_TO_ALBUMLIST ;
        viewEffect = BOTH_VIEW ;    
    }
}

