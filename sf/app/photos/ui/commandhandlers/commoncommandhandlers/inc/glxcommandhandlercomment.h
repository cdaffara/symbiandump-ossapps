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

#ifndef GLXCOMMANDHANDLERCOMMENT_H
#define GLXCOMMANDHANDLERCOMMENT_H

#include <glxmpxcommandhandler.h>

#ifdef BUILD_COMMONCOMMANDHANDLERS
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_IMPORT
#endif


class GLX_COMMONCOMMANDHANDLERS_EXPORT GlxCommandHandlerComment : public GlxMpxCommandHandler
	{
public:
    GlxCommandHandlerComment();
    ~GlxCommandHandlerComment();
    CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const ;
private:
    QString CompletionTextL() const;
    QString GetName(MGlxMediaList& aMediaList) const;

	};

#endif // GLXCOMMANDHANDLERCOMMENT_H
