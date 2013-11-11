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



#ifndef GLXVIEWDOCLOADER_H
#define GLXVIEWDOCLOADER_H

#include <hbdocumentloader.h>

//Custom Docloader to load the fullscreen view
class GlxFullscreenViewDocLoader : public HbDocumentLoader
{
protected:
    
    //This creates the custom widget glxfullscreen view and cover flow.
	QObject* createObject(const QString &type, const QString &name);
	
	private: //
};


//Custom Docloader to load the slide show view
class GlxSlideShowViewDocLoader : public HbDocumentLoader
{
protected:
    //This creates the custom widget slideshow view and slide show widget
    QObject* createObject(const QString &type, const QString &name);
    
    private: //
};

//Custom Docloader to load the details  view
class GlxDetailsViewDocLoader : public HbDocumentLoader
{
protected:
    //This creates the custom widget for details
    QObject* createObject(const QString &type, const QString &name);
    
    private: //
};

#endif /* GLXVIEWDOCLOADER_H */
