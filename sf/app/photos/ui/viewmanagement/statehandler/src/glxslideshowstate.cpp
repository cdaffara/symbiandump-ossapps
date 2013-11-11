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

#include <QDebug>

#include "glxslideshowstate.h"
#include "glxstatemanager.h"
#include "glxcommandhandlers.hrh"
#include "glxorientationsensorfilter.h"

GlxSlideShowState::GlxSlideShowState(GlxStateManager *stateManager, GlxState *preState) : GlxState(GLX_SLIDESHOWVIEW_ID, preState)
{
    mStateManager = stateManager;
	mOrientSensorFilter = NULL;
    //mOrientSensorFilter = new GlxOrientationSensorFilter();
    mState = NO_SLIDESHOW_S;
}

void GlxSlideShowState::eventHandler( qint32 &id )
{
    qDebug("GlxSlideShowState::GlxSlideShowState() action id = %d", id);
    
    /*switch ( id ) {
    case EGlxCmdBack :
        if ( mOrientSensorFilter->orientation() == QOrientationReading::TopUp ) {
            id = EGlxCmdPlayBackAnim ;
        }
        break;
        
    default :
        break ;
    }*/
    
    if ( mState == SLIDESHOW_ALBUM_ITEM_S  ) {
        albumItemEventHandler( id );
    }   
}

void GlxSlideShowState::albumItemEventHandler( qint32 &commandId )
{
    switch ( commandId ) {
    /* in the case of slide show play through list view item specfice menu option, a new model was
     * created so first remove the current model and then go back to pervious state 
     */    
    case EGlxCmdSlideShowBack :
    case EGlxCmdBack :
    case EGlxCmdEmptyData : //memory card was removed ( no data )
        mStateManager->removeCurrentModel();
        mStateManager->previousState();
        commandId = EGlxCmdHandled;
        break;
        
    default :
        break;        
    }    
}

GlxSlideShowState::~GlxSlideShowState()
{
    delete 	mOrientSensorFilter;
}

