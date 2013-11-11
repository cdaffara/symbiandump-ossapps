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

#ifndef NMUIDEF_H_
#define NMUIDEF_H_

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define NMUI_EXPORT __declspec(dllexport)
#else

#if defined(UI_EXTENSION_INTERFACE)
#define NMUI_EXPORT Q_DECL_EXPORT
#else
#define NMUI_EXPORT Q_DECL_IMPORT
#endif

#endif

#endif /* NMUIDEF_H_*/
