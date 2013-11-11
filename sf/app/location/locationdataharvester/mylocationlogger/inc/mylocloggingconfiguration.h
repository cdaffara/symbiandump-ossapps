/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MYLOCCONFIGURATION_H
#define MYLOCCONFIGURATION_H

//#ifdef _DEBUG // logging to file in debug only

/**
* Have this line active if you want to have logging for this component.
* Comment this line out if you don't want any logging for this component.
*/
//#define _MYLOCLOGGING

/**
* Have this line active if you want to have logging to file.
* Comment this line out if you want to have logging via RDebug::Print.
*/
#define _MYLOC_LOGGING_TO_FILE

/**
* Have this line active in case you want to have plugin loading performance
* measurements activated.
*/
//#define _MYLOC_PERFORMANCE_TRACES

//#endif // _DEBUG

#endif // MYLOCCONFIGURATION_H
