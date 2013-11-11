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

#ifndef EMAILCOMMONDEF_H_
#define EMAILCOMMONDEF_H_

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define EMAILCOMMON_EXPORT  __declspec(dllexport)
#else

#if defined(BUILD_EMAILCOMMON_DLL)
#define EMAILCOMMON_EXPORT  Q_DECL_EXPORT
#else
#define EMAILCOMMON_EXPORT  Q_DECL_IMPORT
#endif

#endif

#endif /* EMAILCOMMONDEF_H_ */
