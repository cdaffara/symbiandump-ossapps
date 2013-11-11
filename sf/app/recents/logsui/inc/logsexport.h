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
#ifndef LOGSEXPORT_H
#define LOGSEXPORT_H

#include <qglobal.h>

#if !defined(LOGSENGINE_EXPORT)
#if defined(LOGSENGINE_LIB)
#define LOGSENGINE_EXPORT Q_DECL_EXPORT
#else
#define LOGSENGINE_EXPORT Q_DECL_IMPORT
#endif
#endif

#if !defined(LOGSSERVICES_EXPORT)
#if defined(LOGSSERVICES_LIB)
#define LOGSSERVICES_EXPORT Q_DECL_EXPORT
#else
#define LOGSSERVICES_EXPORT Q_DECL_IMPORT
#endif
#endif


#endif // LOGSEXPORT_H
