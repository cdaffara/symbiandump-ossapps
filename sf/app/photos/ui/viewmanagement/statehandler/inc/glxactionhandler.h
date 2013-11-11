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


 
#ifndef GLXACTIONHANDLER_H
#define GLXACTIONHANDLER_H

#include <QObject>
#include <QHash>
#include <QModelIndex>

class GlxCommandHandler;

class GlxActionHandler : public QObject
{
Q_OBJECT

public :
	GlxActionHandler();
    ~GlxActionHandler();
    void addCommandHandler(int commandId,GlxCommandHandler* commandHandler);
	    
signals :

public :
    void handleAction(qint32 commandId,int collectionId,QList<QModelIndex> indexList = QList<QModelIndex>());

private://functions

private://data 
QHash<int , GlxCommandHandler*> mCommandHandlerList;

};


#endif /* GLXACTIONHANDLER_H */
