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



#ifndef GLXCOMMANDHANDLERFACTORY_H
#define GLXCOMMANDHANDLERFACTORY_H

class GlxCommandHandler;

class GlxCommandHandlerFactory 
{
public :
    GlxCommandHandlerFactory(){};
	static GlxCommandHandler* CreateCommandHandler(int commandId);
};


#endif /* GLXCOMMANDHANDLERFACTORY_H */
