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

#ifndef GLXMODELCOMMANDHANDLER_H
#define GLXMODELCOMMANDHANDLER_H

#include <QObject>
#include<glxcommandhandler.h>

class GlxMediaModel;
#ifdef BUILD_COMMANDHANDLERBASE
#define GLX_COMMANDHANDLERBASE_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMANDHANDLERBASE_EXPORT Q_DECL_IMPORT
#endif

class GLX_COMMANDHANDLERBASE_EXPORT GlxModelCommandHandler : public GlxCommandHandler
{
public:
    GlxModelCommandHandler();
    virtual ~GlxModelCommandHandler();
    virtual void executeCommand(int commandId,int collectionId, QList<QModelIndex> indexList = QList<QModelIndex>() );

    protected: // From derived class    
    virtual void doHandleUserAction(GlxMediaModel* model,QList<QModelIndex>  indexList) const = 0;

};
#endif //GLXMODELCOMMANDHANDLER_H
