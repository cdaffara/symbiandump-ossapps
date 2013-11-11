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

#ifndef RADIO_UI_ENGINE_EXPORT_H_
#define RADIO_UI_ENGINE_EXPORT_H_

// System includes
#include <QtGlobal>

#ifdef BUILD_UI_ENGINE_DLL
    #define UI_ENGINE_DLL_EXPORT Q_DECL_EXPORT
#else
    #define UI_ENGINE_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif // RADIO_UI_ENGINE_EXPORT_H_
