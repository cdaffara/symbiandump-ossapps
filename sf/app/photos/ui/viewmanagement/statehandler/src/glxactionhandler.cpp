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
#include <glxactionhandler.h>
#include <QDebug>
#include <glxcommandhandlers.hrh>
#include <glxcommandhandler.h>
#include<glxcommandhandlerfactory.h>
#include "glxmediaid.h"
#include <QModelIndexList>

GlxActionHandler::GlxActionHandler()
{
}

GlxActionHandler::~GlxActionHandler()
{
    qDeleteAll( mCommandHandlerList );
    mCommandHandlerList.clear();
}

void GlxActionHandler::handleAction(qint32 commandId,int collectionId,QModelIndexList indexList)
{
    GlxCommandHandler* cmdHandler = NULL;
	switch ( commandId ) {
	    case EGlxCmdContextAddToAlbum:
	        commandId = EGlxCmdAddToAlbum;
	        break;
	        
	    case EGlxCmdContextDelete:
	        commandId = EGlxCmdDelete;
	        break;
	        
	    case EGlxCmdContextAlbumDelete:
	        commandId = EGlxCmdDelete;
	        break;
	        
	    case EGlxCmdContextSend:
	         commandId = EGlxCmdSend;
	         break;

        case EGlxCmdContextRename:
	        commandId = EGlxCmdRename;
	        break;
        case EGlxCmdContextRemoveFrom:
            commandId = EGlxCmdRemoveFrom;
            break;
		default : break;	
		}
		
    if(mCommandHandlerList.contains(commandId)){
        cmdHandler = mCommandHandlerList.value(commandId);
    }
    else{
        cmdHandler = GlxCommandHandlerFactory::CreateCommandHandler(commandId);
        addCommandHandler(commandId,cmdHandler);
    }
    if ( cmdHandler ) {
        cmdHandler->executeCommand(commandId,collectionId,indexList);
    }
}

void GlxActionHandler::addCommandHandler(int commandId,GlxCommandHandler* commandHandler)
{
    if(!mCommandHandlerList.contains(commandId)){
        mCommandHandlerList.insert(commandId,commandHandler);
        }
}

