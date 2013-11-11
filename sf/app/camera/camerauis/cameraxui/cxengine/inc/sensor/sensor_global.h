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
#ifndef SENSOR_GLOBAL_H
#define SENSOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XQSENSOR_LIBRARY)
#  define XQSENSOR_EXPORT Q_DECL_EXPORT
#else
#  if defined(XQSENSOR_NO_LIBRARY)
#    define XQSENSOR_EXPORT
#  else
#    define XQSENSOR_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // SENSOR_GLOBAL_H
