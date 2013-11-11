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

#include <glxliststate.h>
#include <glxcommandhandlers.hrh>
#include <QDebug>

GlxListState::GlxListState(GlxState *preState) : GlxState(GLX_LISTVIEW_ID, preState)
{
    if ( preState )
        qDebug("GlxListState::GlxListState pre state id = %d", preState->id());
}

void GlxListState::eventHandler(qint32 &id)
{
    qDebug("GlxListState::eventHandler action id = %d", id);
    switch(id) {
    case EGlxCmdAlbumListOpen :
    case EGlxCmdFetcherAlbumListOpen:
	    id = EGlxCmdHandled;
	    break ;
	
    default :
	    break;
	}	
}

void GlxListState::setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect)
{
    qDebug("GlxListState::setTranstionParameter dir = %d", dir);
    if ( dir == NO_DIR) {
        effect = GRID_TO_ALBUMLIST ;
        viewEffect = BOTH_VIEW ;
    }
    else {
        effect = NO_EFFECT;
        viewEffect = NO_VIEW;
    }
}

