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

#ifndef SIMUTILITYGLOBAL_H
#define SIMUTILITYGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(SIMUTILITY_LIBRARY)
#  define SIMUTILITYLIB_EXPORT Q_DECL_EXPORT
#else
#  define SIMUTILITYLIB_EXPORT Q_DECL_IMPORT
#endif

#endif
