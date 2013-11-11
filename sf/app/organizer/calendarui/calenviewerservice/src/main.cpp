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
* Description:  main implementation.
*
*/

// System includes
#include <hbapplication.h>
#include <hbmainwindow.h>

// User includes
#include "calenviewerservice.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif


int main (int argc, char *argv[])
{
    OstTraceFunctionEntry0( _MAIN_ENTRY );
    
    HbApplication app(argc, argv);
	HbMainWindow mainWindow;
	
	// Create the service provider
	CalenViewerService service;
	
	mainWindow.show();
	
	int retVal = app.exec();
	
	OstTraceFunctionExit0( _MAIN_EXIT );
	
	return retVal;
}
