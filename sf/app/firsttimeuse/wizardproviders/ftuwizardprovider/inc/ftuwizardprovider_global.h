/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef FTUWIZARDPROVIDER_GLOBAL
#define FTUWIZARDPROVIDER_GLOBAL

#include <Qt/qglobal.h>


#ifdef FTUWIZARDPROVIDER_LIB
	#define FTUWIZARDPROVIDER_EXPORT Q_DECL_EXPORT
#else
    #ifdef FTUWIZARDPROVIDER_TEST
        #define FTUWIZARDPROVIDER_EXPORT
    #else
        #define FTUWIZARDPROVIDER_EXPORT Q_DECL_IMPORT
    #endif	
#endif

#ifdef LOGGING
#define QDEBUG(a)   qDebug() <<a
#else
#define QDEBUG(a)  
#endif //LOGGING


#endif // FTUWIZARDPROVIDER_GLOBAL
