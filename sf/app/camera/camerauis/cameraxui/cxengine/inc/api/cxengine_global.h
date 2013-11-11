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

#ifndef CXENGINE_GLOBAL_H
#define CXENGINE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CAMERAX_ENGINE_LIBRARY)
#  define CAMERAX_ENGINE_EXPORT Q_DECL_EXPORT
#else
#  if defined(CAMERAX_ENGINE_NO_LIBRARY)
#    define CAMERAX_ENGINE_EXPORT
#  else
#    define CAMERAX_ENGINE_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // CXENGINE_GLOBAL_H
