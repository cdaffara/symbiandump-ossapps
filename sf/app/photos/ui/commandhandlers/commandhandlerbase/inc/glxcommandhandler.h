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

#ifndef GLXCOMMANDHANDLER_H
#define GLXCOMMANDHANDLER_H

#include <QObject>
#include <QModelIndex>

#ifdef BUILD_COMMANDHANDLERBASE
#define GLX_COMMANDHANDLERBASE_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMANDHANDLERBASE_EXPORT Q_DECL_IMPORT
#endif

class GLX_COMMANDHANDLERBASE_EXPORT GlxCommandHandler : public QObject
{
public:
    GlxCommandHandler();
    virtual ~GlxCommandHandler();
    virtual void executeCommand(int commandId,int collectionId ,QList<QModelIndex> indexList = QList<QModelIndex>() ) = 0;
    
};
#endif //GLXCOMMANDHANDLER_H
