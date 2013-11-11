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



#include <hbmainwindow.h>
#include "glxexternalutility.h"
#include "glxexternalinterfacedefs.h"


GlxExternalUtility* GlxExternalUtility::instance()
{ 
	static GlxExternalUtility theExternalUtility;
	return &theExternalUtility;
}
void GlxExternalUtility::setMainWindow(HbMainWindow* mainWindow)
{
	mMainWindow = mainWindow;
}

HbMainWindow* GlxExternalUtility::getMainWindow()
{
	return mMainWindow;
}
void GlxExternalUtility::setExternalData(GlxExternalData* externalData)
{
	mExternalData = externalData;
}

GlxExternalData* GlxExternalUtility::getExternalData()
{
	return mExternalData;
}
