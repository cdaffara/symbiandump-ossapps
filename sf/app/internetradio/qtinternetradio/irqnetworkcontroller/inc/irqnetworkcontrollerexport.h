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

#ifndef IRQNETWORKCONTROLLEREXPORT_H_
#define IRQNETWORKCONTROLLEREXPORT_H_

#include <QtGlobal>

#ifdef BUILD_IRQNETWORKCONTROLLER_DLL
    #define IRQNETWORKCONTROLLER_DLL_EXPORT Q_DECL_EXPORT
#else
    #define IRQNETWORKCONTROLLER_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif /* IRQNETWORKCONTROLLEREXPORT_H_ */
