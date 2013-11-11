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
* Description:  The dll header file.
*
*/

#ifndef SREENSAVERMODEL_GLOBAL_H
#define SREENSAVERMODEL_GLOBAL_H

#include <Qt/qglobal.h>

#ifdef SREENSAVERMODEL_LIB
	#define SREENSAVERMODEL_EXPORT Q_DECL_EXPORT
#else
    #ifdef SREENSAVERMODEL_TEST
        #define SREENSAVERMODEL_EXPORT
    #else
        #define SREENSAVERMODEL_EXPORT Q_DECL_IMPORT
    #endif	
#endif

#endif // SREENSAVERMODEL_GLOBAL_H
