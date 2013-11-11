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


#include "glxfadeeffectplugin.h"
#include "glxuistd.h"
#include "glxlocalisationstrings.h"

GlxFadeEffectPlugin::GlxFadeEffectPlugin() : GlxEffectPluginBase( SMOOTH_FADE )
{
    mEffectFileList.append(QString(":/data/opacity_deactivate.fxml"));
    mEffectFileList.append(QString(":/data/opacity_activate.fxml"));
}

void GlxFadeEffectPlugin::setUpItems( QList< QGraphicsItem * > &  items )
{
    if ( items.count() < 2 ) 
        return;
   
    items.at(0)->setPos(0,0);
    items.at(1)->setPos(0,0);
}

QString GlxFadeEffectPlugin::effectName() 
{ 
    return ( GLX_VAL_SMOOTH_FADE ) ; 
}

GlxFadeEffectPlugin::~GlxFadeEffectPlugin()
{
    mEffectFileList.clear();
}

