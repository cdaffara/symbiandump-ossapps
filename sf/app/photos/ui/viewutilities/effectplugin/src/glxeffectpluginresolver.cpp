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

#include <QString>

#include "glxeffectpluginresolver.h"
#include "glxeffectpluginbase.h"
#include "glxfadeeffectplugin.h"
#include "glxflipeffectplugin.h"
#include "glxzoominoutplugin.h"
#include "glxhelixeffectplugin.h"

GlxEffectPluginResolver::GlxEffectPluginResolver()
{
    mEffectPluginList.clear();
}

QString GlxEffectPluginResolver::effectName( int effectId )
{
    qDebug( " GlxEffectPluginResolver::effectName %d ", effectId );
    switch ( effectId ) {
    case FLIP_EFFECT :
        return GlxFlipEffectPlugin::effectName();
        
    case SMOOTH_FADE :
        return GlxFadeEffectPlugin::effectName() ;
    
    case ZOOM_TO_FACE :
        return GlxZoomInOutEffectPlugin::effectName();
        
    case HELIX_EFFECT :
        return GlxHelixEffectPlugin::effectName();
        
    default :
        return GlxFadeEffectPlugin::effectName() ;
    }
}

GlxEffectPluginBase *GlxEffectPluginResolver::effectPlugin( int effectId )
{
    qDebug( " GlxEffectPluginResolver::effectPlugin %d ", effectId );
    GlxEffectPluginBase * effectPlugin = mEffectPluginList.value( ( GlxEffect  )effectId );
    if ( effectPlugin ) {
        return  effectPlugin ;
    }
    
    switch ( effectId ) {
        case FLIP_EFFECT :
            effectPlugin = new GlxFlipEffectPlugin();
            mEffectPluginList[ FLIP_EFFECT ] = effectPlugin ;
            break ;
            
        case ZOOM_TO_FACE :
            effectPlugin = new GlxZoomInOutEffectPlugin() ;
            mEffectPluginList[ ZOOM_TO_FACE ] = effectPlugin ;
            break ;
            
        case HELIX_EFFECT :
            effectPlugin = new GlxHelixEffectPlugin() ;
            mEffectPluginList[ HELIX_EFFECT ] = effectPlugin ;
            break ;
        
        case SMOOTH_FADE :   
        default :
            effectPlugin = mEffectPluginList.value( SMOOTH_FADE );
            if ( effectPlugin == NULL ) {
                effectPlugin = new GlxFadeEffectPlugin();
                mEffectPluginList[ SMOOTH_FADE ] = effectPlugin ;
            }
            break ;
    } 

    return effectPlugin ;
}

GlxEffectPluginResolver::~GlxEffectPluginResolver()
{
    foreach( GlxEffectPluginBase *list, mEffectPluginList) {
        delete list;
        list = NULL;
    }
    mEffectPluginList.clear();
}
