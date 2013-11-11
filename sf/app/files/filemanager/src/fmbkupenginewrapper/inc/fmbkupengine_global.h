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
 * 
 * Description:
 *      The header file of the backup engine wrapper export MACRO
 */

#ifndef FMBKUPENGINE_GLOBAL_H
#define FMBKUPENGINE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FMBKUPENGINE_LIBRARY)
#  define FMBKUPENGINE_EXPORT Q_DECL_EXPORT
#else
#  if defined(FMBKUPENGINE_NO_LIBRARY)
#    define FMBKUPENGINE_EXPORT
#  else
#    define FMBKUPENGINE_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // FMBKUPENGINE_GLOBAL_H
