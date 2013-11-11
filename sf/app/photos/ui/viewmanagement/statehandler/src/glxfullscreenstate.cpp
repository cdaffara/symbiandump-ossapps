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


#include <hbmainwindow.h>
#include <hbinstance.h>

#include "glxfullscreenstate.h"
#include "glxstatemanager.h"
#include "glxcommandhandlers.hrh"

GlxFullScreenState::GlxFullScreenState(GlxStateManager *stateManager, GlxState *preState) : GlxState(GLX_FULLSCREENVIEW_ID, preState)
{
    mStateManager = stateManager ;
}

void GlxFullScreenState::eventHandler(qint32 &id)
{
    switch ( id ){
    case EGlxCmdDetailsOpen :
        if ( mState == IMAGEVIEWER_S ) {

            mStateManager->nextState( GLX_DETAILSVIEW_ID, IMAGEVIEWER_DETAIL_S );
        }
        else {

            mStateManager->nextState( GLX_DETAILSVIEW_ID, NO_DETAIL_S );
        }
        id = EGlxCmdHandled;
        break ;
    default :
        break ;
    }
}

void GlxFullScreenState::setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect)
{
    GlxState *preState = previousState();
    effect = NO_EFFECT ;
    viewEffect = NO_VIEW ;
    
    if ( preState == NULL ) {
        return ;        
    }
    
    if ( dir == FORWARD_DIR ){
         if ( preState->compare( GLX_GRIDVIEW_ID ) ) {
             effect = GRID_TO_FULLSCREEN ;
             viewEffect = BOTH_VIEW ;
         }
    }    
    else if ( dir == BACKWARD_DIR ) {
        if ( preState->compare( GLX_GRIDVIEW_ID ) ) {
            HbMainWindow *window = hbInstance->allMainWindows().first();
            effect = window->orientation() == Qt::Vertical ? FULLSCREEN_TO_GRID_PORTRAIT : FULLSCREEN_TO_GRID ;
            viewEffect = LAUNCH_VIEW;
        }       
    }    
}

