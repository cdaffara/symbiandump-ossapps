/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EMAILUTILITIESDEF_H_
#define EMAILUTILITIESDEF_H_

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define EMAIL_UTILITIES_EXPORT __declspec(dllexport)
#else

#if defined(BUILD_UTILITIES_DLL)
#define EMAIL_UTILITIES_EXPORT Q_DECL_EXPORT
#else
#define EMAIL_UTILITIES_EXPORT Q_DECL_IMPORT
#endif /* BUILD_UTILITIES_DLL */

#endif /* Q_OS_WIN */

#endif /* EMAILUTILITIESDEF_H_ */
