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
 * Description: This is the domain header for defining DLL EXPORT 
 *               MACRO
 */

#ifndef MSG_UTILS_API_DEFINES_H
#define MSG_UTILS_API_DEFINES_H

#include <QObject>

#ifdef BUILD_MSG_UTILS_DLL
#define MSG_UTILS_API_EXPORT Q_DECL_EXPORT
#else
#define MSG_UTILS_API_EXPORT Q_DECL_IMPORT
#endif

#endif // MSG_UTILS_API_DEFINES_H
