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

#ifndef BASEPLUGINDEF_H_
#define BASEPLUGINDEF_H_

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define BASEPLUGIN_EXPORT  __declspec(dllexport)
#else

#if defined(BUILD_BASEPLUGIN_DLL)
#define BASEPLUGIN_EXPORT  Q_DECL_EXPORT
#else
#define BASEPLUGIN_EXPORT  Q_DECL_IMPORT
#endif

#endif

#endif /* BASEPLUGINDEF_H_ */
