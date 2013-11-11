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

//INCLUDES
#include <hbdocumentloader.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbinstance.h>
#include <QDebug>

//USER INCLUDES
#include "glxdocloaderdefs.h" //contains the path and names of the view
#include "glxviewdocloader.h"
#include "glxfullscreenview.h"
#include "glxcoverflow.h"
#include "glxslideshowwidget.h"
#include "glxslideshowview.h"
#include "glxzoomwidget.h"

#include "glxdetailstextedit.h"
#include "glxdetailsicon.h"

//----------------------------------------------------------------------------------------
// createObject:creates the custom widget and views of fullscreen view
//----------------------------------------------------------------------------------------
QObject* GlxFullscreenViewDocLoader::createObject(const QString &type, const QString &name)
    {
    qDebug() <<"GlxFullscreenViewDocLoader::createObject -->";

    if (GLXFULLSCREEN_VIEW == name)
        {
        qDebug() << "GlxFullscreenViewDocLoader::createObject:FULLSCREEN_VIEW -->";

        //Passes the docloader ownership to the view
        HbMainWindow *window = hbInstance->allMainWindows().first();
        QObject *object = new GlxFullScreenView(window,this);
        object->setObjectName(name);
        return object;
        }

    if (GLXFULLSCREEN_COVERFLOW == name) 
        {
        qDebug() << "GlxFullscreenViewDocLoader::createObject:CoverFLOW -->";

        QObject *object = new GlxCoverFlow();
        object->setObjectName(name);
        return object;
        }
       
       if (GLXFULLSCREENZOOMWIDGET == name) 
        {
        qDebug() << "GlxFullscreenViewDocLoader::createObject:ZOOM -->";
        QObject *object = new GlxZoomWidget();
        object->setObjectName(name);
        return object;
        }
    return HbDocumentLoader::createObject(type, name);
    }


//----------------------------------------------------------------------------------------
// createObject:creates the custom widget and views of slide show
//----------------------------------------------------------------------------------------
QObject* GlxSlideShowViewDocLoader::createObject(const QString &type, const QString &name)
    {
    qDebug() <<"GlxSlideShowViewDocLoader::createObject -->";


    if (GLXSLIDESHOW_VIEW == name)
        {
        qDebug() << "GlxSlideShowViewDocLoader::createObject:SLIDESHOW -->";

        //Passes the docloader ownership to the view
        HbMainWindow *window = hbInstance->allMainWindows().first();
        QObject *object = new GlxSlideShowView(window,this);
        object->setObjectName(name);
        return object;
        }

    if (GLXSLIDESHOW_WIDGET == name)
        {
        qDebug() << "GlxSlideShowViewDocLoader::createObject:WIDGET -->";
        QObject *object = new GlxSlideShowWidget();
        object->setObjectName(name);
        return object;
        }
    return HbDocumentLoader::createObject(type, name);
    }

	//----------------------------------------------------------------------------------------
// createObject:creates the custom widget and views of details view
//----------------------------------------------------------------------------------------
QObject* GlxDetailsViewDocLoader::createObject(const QString &type, const QString &name)
    {
    qDebug() <<"GlxDetailsViewDocLoader::createObject -->";
   
    if (GLX_DETAILSVIEW_FAVICON == name)
        {
        qDebug() << "GlxDetailsViewDocLoader::createObject:ICON -->";

        QObject *object = new GlxDetailsIcon();
        object->setObjectName(name);
        return object;
        }
    
    
    return HbDocumentLoader::createObject(type, name);
    }

