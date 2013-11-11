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


#ifndef GLXZOOMINOUTEFFECTPLUGIN_H
#define GLXZOOMINOUTEFFECTPLUGIN_H

#include "glxeffectpluginbase.h"
#include "glxlocalisationstrings.h"

class GlxZoomInOutEffectPlugin : public GlxEffectPluginBase
{
public :
    /*
     *  Constructor
     */
    GlxZoomInOutEffectPlugin();
    
    /*
     *  Destructor
     */
    ~GlxZoomInOutEffectPlugin();
    
    /*
     *  Get the flip hide and Flip Show file list
     */
    QList <QString > effectFileList() { return mEffectFileList; }
    
    /*
     * setup the item postion and set the mItem value
     */
    void setUpItems( QList< QGraphicsItem * > &  items );
    
    /*
     * get the name of the effect, use to shown in the slide show setting view
     */
    static QString effectName() { return   (GLX_EFFECTS_ZOOMIN_ZOOMOUT) ; }
    
private :
    QList <QString > mEffectFileList;
    QGraphicsItem *mItem;
};

#endif /*GLXZOOMINOUTEFFECTPLUGIN_H*/
