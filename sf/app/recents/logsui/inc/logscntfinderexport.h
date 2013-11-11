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
#ifndef LOGSCNTFINDEREXPORT_H
#define LOGSCNTFINDEREXPORT_H

#include <qglobal.h>

#if !defined(LOGSCNTFINDER_EXPORT)
#if defined(LOGSCNTFINDER_LIB)
#define LOGSCNTFINDER_EXPORT Q_DECL_EXPORT
#else
#define LOGSCNTFINDER_EXPORT Q_DECL_IMPORT
#endif
#endif



#endif // LOGSCNTFINDEREXPORT_H
