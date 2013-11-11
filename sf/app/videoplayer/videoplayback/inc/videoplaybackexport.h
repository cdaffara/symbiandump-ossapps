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
* Description:   DLL export macro for videoplayback
* 
*/

// Version : %version:  2 %


#ifndef _VIDEOPLAYBACKEXPORT_H
#define _VIDEOPLAYBACKEXPORT_H

#include <qglobal.h>

#ifdef BUILD_VIDEOPLAYBACK_DLL
    #define VIDEOPLAYBACK_DLL_EXPORT Q_DECL_EXPORT
#else
    #define VIDEOPLAYBACK_DLL_EXPORT Q_DECL_IMPORT
#endif

#endif // _VIDEOPLAYBACKEXPORT_H

// End of file.
