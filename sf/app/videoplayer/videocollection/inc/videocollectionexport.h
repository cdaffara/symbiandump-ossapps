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
* Description:   DLL export macro for videocollection.
* 
*/

#ifndef _VIDEOCOLLECTIONEXPORT_H
#define _VIDEOCOLLECTIONEXPORT_H

// System includes
#include <qglobal.h>

#ifdef BUILD_VIDEOCOLLECTION_DLL
    #define VIDEOCOLLECTION_DLL_EXPORT Q_DECL_EXPORT
#else
    #define VIDEOCOLLECTION_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif // _VIDEOCOLLECTIONEXPORT_H

// End of file.
