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
* Description:  conversation view defines 
 *
*/


#ifndef CONVERSATION_VIEW_DEFINES_H
#define CONVERSATION_VIEW_DEFINES_H

#include <QObject>

#ifdef  CONVERSATIONVIEW_DLL
#define CONVERSATION_VIEW_EXPORT Q_DECL_EXPORT
#else
#define CONVERSATION_VIEW_EXPORT Q_DECL_IMPORT
#endif

#endif // CONVERSATION_VIEW_DEFINES_H
