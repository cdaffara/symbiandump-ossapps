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

#ifndef NMAILUIWIDGETSDEF_H_
#define NMAILUIWIDGETSDEF_H_

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define NMAILUIWIDGETS_EXPORT __declspec(dllexport)
#else

#if defined(BUILD_NMAILUIWIDGETS_DLL)
#define NMAILUIWIDGETS_EXPORT Q_DECL_EXPORT
#else
#define NMAILUIWIDGETS_EXPORT Q_DECL_IMPORT
#endif

#endif

#endif /* NMAILUIWIDGETSDEF_H_ */
