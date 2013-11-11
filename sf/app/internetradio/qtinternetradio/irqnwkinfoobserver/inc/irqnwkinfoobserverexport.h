/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  interface for updating network info
*
*/


#ifndef IRQNWKOBSERVEREXPORT_H_
#define IRQNWKOBSERVEREXPORT_H_

#include <QString>

#ifdef BUILD_IRQNWKINFOOBSERVER_DLL
    #define IRQNWKINFOOBSERVER_DLL_EXPORT Q_DECL_EXPORT
#else
    #define IRQNWKINFOOBSERVER_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif // IRQNWKOBSERVEREXPORT_H_
