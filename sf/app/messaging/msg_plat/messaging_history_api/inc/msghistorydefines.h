/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Message history defines.
 *
 */

#ifndef __MSG_HISTORY_DEFINES_H__
#define __MSG_HISTORY_DEFINES_H__

#include <QObject>

#ifdef  MSGHISTORY_DLL
#define MSG_HISTORY_EXPORT Q_DECL_EXPORT
#else
#define MSG_HISTORY_EXPORT Q_DECL_IMPORT
#endif

#endif // __MSG_HISTORY_DEFINES_H__
