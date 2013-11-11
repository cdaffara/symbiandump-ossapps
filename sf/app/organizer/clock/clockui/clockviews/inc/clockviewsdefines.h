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
* The clockviews common definition document.
*
*/

#ifndef CLOCKVIEWSDEFINES_H
#define CLOCKVIEWSDEFINES_H

// System includes
#include <qglobal.h>

#ifdef CLOCKVIEWS_LIB
#   define CLOCKVIEWS_EXPORT Q_DECL_EXPORT
#else
#   define CLOCKVIEWS_EXPORT Q_DECL_IMPORT
#endif

const QString clockMainView = "ClockMainView"; // Activity Id for clock activity to be save
// in Activity Manager DB

#endif // CLOCKVIEWSDEFINES_H

// End of file  --Don't remove this.
