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


#include "glxhelixeffectplugin.h"
#include "glxuistd.h"
#include "glxlocalisationstrings.h"

GlxHelixEffectPlugin::GlxHelixEffectPlugin() : GlxEffectPluginBase( HELIX_EFFECT )
{
    mEffectFileList.append(QString(":/data/helix.fxml"));
    mEffectFileList.append(QString(":/data/fadein.fxml"));
}

void GlxHelixEffectPlugin::setUpItems( QList< QGraphicsItem * > &  items )
{
    if ( items.count() < 2 ) 
        return;
   
    items.at(0)->setPos(0,0);
    items.at(1)->setPos(0,0);
}

QString GlxHelixEffectPlugin::effectName() 
{ 
    return ( GLX_EFFECTS_ZOOM_AND_PAN ) ; 
}

GlxHelixEffectPlugin::~GlxHelixEffectPlugin()
{
    mEffectFileList.clear();
}

