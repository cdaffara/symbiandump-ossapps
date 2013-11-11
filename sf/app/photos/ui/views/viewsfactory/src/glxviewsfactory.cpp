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
* Description:   ?Description
*
*/


//Includes

//User Includes
#include "glxview.h"
#include "glxviewids.h"
#include "glxgridview.h"
#include "glxslideshowsettingsview.h"
#include "glxlistview.h"
#include "glxdetailsview.h"
#include "glxviewsfactory.h"
#include "glxviewdocloader.h"
#include "glxslideshowview.h"
#include "glxdocloaderdefs.h"
#include "glxfullscreenview.h"

GlxView * GlxViewsFactory::createView(qint32 viewId, HbMainWindow *window)
{
    switch ( viewId ) {
    case GLX_GRIDVIEW_ID :
        return new GlxGridView(window);

    case GLX_LISTVIEW_ID :
        return new GlxListView ( window );

    case GLX_FULLSCREENVIEW_ID :
	{
	    GlxFullScreenView *fullscreenView = NULL;
	    bool isloaded;
		
        GlxFullscreenViewDocLoader *docLoader = new GlxFullscreenViewDocLoader();
        // Load the   docml file.
        docLoader->load(GLXFULLSCREENDOCMLPATH, &isloaded);
                
        if(isloaded)
        {
        //retreive the full screen view
         fullscreenView  = static_cast<GlxFullScreenView *> (docLoader->findWidget(GLXFULLSCREEN_VIEW));
        }  
        return fullscreenView;
    }

   case GLX_DETAILSVIEW_ID :    
        return new GlxDetailsView( window );

    case GLX_SLIDESHOWVIEW_ID :
	{
	    GlxSlideShowView *slideShowView = NULL;
	    bool isloaded;
		
        GlxSlideShowViewDocLoader *docLoader = new GlxSlideShowViewDocLoader();
        // Load the   docml file.
       docLoader->load(GLXSLIDESHOW_PATH, &isloaded);
       if(isloaded)
	   {
	     //retreive the Slide show view
	     slideShowView  = static_cast<GlxSlideShowView *> (docLoader->findWidget(GLXSLIDESHOW_VIEW));
	   }  
        return slideShowView;
   }

    case GLX_SLIDESHOWSETTINGSVIEW_ID :
        return new GlxSlideShowSettingsView ( window );
    default :
        return 0;     
    }
}
