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


#ifndef GLXEFFECTPLUGINRESOLVER_H
#define GLXEFFECTPLUGINRESOLVER_H

#include <hbeffect.h>
#include <QHash>

#include "glxuistd.h"

class GlxEffectPluginBase ;
class QString ;

class GlxEffectPluginResolver 
{ 

public :
    /*
     * Constructor
     */
    GlxEffectPluginResolver( );
    
    /*
     * Return the name of the effect
     */
    static QString effectName( int effectId );
    
    /*
     * Return the effect plugin instance to run the effect
     */
    GlxEffectPluginBase * effectPlugin( int effectId );
    
    /*
     * Destructor
     */
    ~GlxEffectPluginResolver( );
    
private :
    QHash< GlxEffect, GlxEffectPluginBase * > mEffectPluginList ;
};


#endif //GLXEFFECTPLUGINRESOLVER_H
