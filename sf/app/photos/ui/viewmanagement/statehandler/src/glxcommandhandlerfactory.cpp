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

#include<glxcommandhandlerfactory.h>
#include <glxcommandhandlers.hrh>

#include<glxcommandhandlerdelete.h>
#include <glxcommandhandleraddtocontainer.h>
#include <glxcommandhandlerrotate.h>
#include <glxcommandhandlereditimage.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxcommandhandlernewmedia.h>
#include <glxcommandhandlersend.h>
#include <glxcommandhandlerrename.h>
#include <glxcommandhandlercomment.h>

GlxCommandHandler* GlxCommandHandlerFactory::CreateCommandHandler(int commandId)
{
    GlxCommandHandler* cmdHandler = NULL;
    switch(commandId)
        {
        case EGlxCmdDelete: cmdHandler = new GlxCommandHandlerDelete();
           break;
        case EGlxCmdAddToAlbum: 
        case EGlxCmdAddToFav:
                            cmdHandler = new GlxCommandHandlerAddToContainer();
           break;
		case EGlxCmdRotate: cmdHandler = new GlxCommandHandlerRotate();
           break;
        case EGlxCmdRemoveFrom:
        case EGlxCmdRemoveFromFav: 
                         cmdHandler = new GlxCommandHandlerRemoveFrom();  
           break;
        case EGlxCmdAddMedia:  cmdHandler = new GlxCommandHandlerNewMedia();
            break;
		
		case EGlxCmdSend : cmdHandler = new GlxCommandHandlerSend();  
	         break;

		case EGlxCmdRenameFile:     
        case EGlxCmdRename: cmdHandler = new GlxCommandHandlerRename();
           break;
           
        case EGlxCmdComment: cmdHandler = new GlxCommandHandlerComment();
                   break;
        case EGlxCmdRotateImgCW:
        case EGlxCmdRotateImgCCW:
		case EGlxCmdRotateImgCrop:
		case EGlxCmdSetWallpaper:
            				cmdHandler = new GlxCommandHandlerEditImage();
            break;
        default:
           break;
        }
    return cmdHandler;
}
