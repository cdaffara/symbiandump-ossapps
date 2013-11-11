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


#include "glxflipeffectplugin.h"
#include "glxuistd.h"
#include "glxlocalisationstrings.h"

GlxFlipEffectPlugin::GlxFlipEffectPlugin() : GlxEffectPluginBase( FLIP_EFFECT )
{
    mEffectFileList.append(QString(":/data/uphide.fxml"));
    mEffectFileList.append(QString(":/data/downshow.fxml"));
}

void GlxFlipEffectPlugin::setUpItems( QList< QGraphicsItem * > &  items )
{
    if ( items.count() < 2 ) 
        return;
   
    items.at(0)->setPos(0,0);
    items.at(1)->setPos(0,0);
}

QString GlxFlipEffectPlugin::effectName() 
{ 
    return GLX_EFFECTS_FLIP ; 
}

GlxFlipEffectPlugin::~GlxFlipEffectPlugin()
{
    mEffectFileList.clear();
}


