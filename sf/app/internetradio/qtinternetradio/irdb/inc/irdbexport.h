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

#ifndef IRDBEXPORT_H_
#define IRDBEXPORT_H_

#include <QtGlobal>

#ifdef BUILD_IRDB_DLL
    #define IRDB_DLL_EXPORT Q_DECL_EXPORT
#else
    #define IRDB_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif /* IRDBEXPORT_H_ */
