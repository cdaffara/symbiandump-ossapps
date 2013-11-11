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

#ifndef GLXSETTINGINTERFACE_H
#define GLXSETTINGINTERFACE_H

#include <qglobal.h>

#ifdef BUILD_GLXVIEWUTILITIES
#define GLX_VIEWUTILITIES_EXPORT Q_DECL_EXPORT
#else
#define GLX_VIEWUTILITIES_EXPORT Q_DECL_IMPORT
#endif

class QStringList ;

/*
 * This is an interface for read and write the photos application related setting from the centrep.
 */
class GLX_VIEWUTILITIES_EXPORT GlxSettingInterface
{

public :
    /*
     * return the instance of the GlxSetting
     */
	static GlxSettingInterface * instance();
    
    /* 
     * Returns the index to the selected transition effect
     * This value is fetched from Central repository 
     */
    virtual int slideShowEffectIndex() = 0 ;
    
    /*
     * SlideShowSettingsView will call this API to set the index of the transition effect chosen. 
     * This data will be written to the Central Repository
     */
    virtual void setslideShowEffectIndex( int index ) = 0 ;
    
    /* 
     * Returns the index to the selected transition delay 
     * This value is fetched from Central repository 
     */
    virtual int slideShowDelayIndex() = 0 ;
    
    /*
     * SlideShowSettingsView will call this API to set the index of the transition delay chosen. 
     * This data will be written to the Central Repository
     */
    virtual void setSlideShowDelayIndex( int index ) = 0;
    
    /*
     * It will return the slide show delay time for playing the slide show.
     * This value is fetched for central repository
     */
    virtual int slideShowDelayTime() = 0;
    
    /*
     * This will return the list of effects as available in the central repository 
     */
    virtual QStringList slideShowEffectList() = 0;
    
    /*
     * This will return the effect ID based on the effect index 
     * Effect index will be 0,1,2... and effect ID is GLX_EFFECT
     * Effect id is store in the centrep with respect to index.
     */
    virtual int slideShowEffectId( int index ) = 0;
    
    
    /* 
     * Returns Current Media Wall 3D effect Status
     * This value is fetched from Central repository 
     */
    virtual int mediaWall3DEffect() = 0 ;
    
    /*
     * Set Media Wall 3D effect. True = 3D Effect On / False = 3D Effect Off 
     * This data will be written to the Central Repository
     */
    virtual void setmediaWall3DEffect( int index ) = 0 ;
    
protected :
    /*
     * Constructor
     */
    GlxSettingInterface( );
    
    /*
     * Copy Constructor
     */
    GlxSettingInterface( GlxSettingInterface & );
    
    /*
     * Destructor
     */
    virtual ~GlxSettingInterface() ;
};

#endif

