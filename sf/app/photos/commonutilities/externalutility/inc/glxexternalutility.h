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


#ifndef GLXEXTERNALUTILITY_H
#define GLXEXTERNALUTILITY_H

#include <hbmainwindow.h>
#include "glxexternalinterfacedefs.h"
#ifdef BUILD_EXTERNALUTILITY
#define GLX_EXTERNALUTILITY_EXPORT Q_DECL_EXPORT
#else
#define GLX_EXTERNALUTILITY_EXPORT Q_DECL_IMPORT
#endif

class GLX_EXTERNALUTILITY_EXPORT GlxExternalUtility
{

   public:
	 static GlxExternalUtility* instance();
      
     void setMainWindow(HbMainWindow* mainWindow);
	 HbMainWindow* getMainWindow();
	 void setExternalData(GlxExternalData * externalData);
	 GlxExternalData* getExternalData();
 
    private:
       GlxExternalUtility() { mMainWindow = NULL; }
	   HbMainWindow* mMainWindow;
	   GlxExternalData * mExternalData;
		      
};

#endif //GLXEXTERNALUTILITY_H