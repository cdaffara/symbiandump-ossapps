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

#ifndef GLXSETTING_H
#define GLXSETTING_H


#include <glxsettinginterface.h>

class XQSettingsKey;
class XQSettingsManager;
class QStringList;

#define NBR_SLIDESHOW_EFFECT 4

/*
 * This class is used for read and write the photos application related setting from the centrep.
 */
class GlxSetting : public GlxSettingInterface
{

public :
    /*
     * It will return the singlton object of this class
     */
	static GlxSettingInterface * instance(); 
	    
    /* 
     * Returns the index to the selected transition effect
     * This value is fetched from Central repository 
     */
    int slideShowEffectIndex() ;
    
    /*
     * SlideShowSettingsView will call this API to set the index of the transition effect chosen. 
     * This data will be written to the Central Repository
     */
     void setslideShowEffectIndex( int index );
    
    /* 
     * Returns the index to the selected transition delay /
     * This value is fetched from Central repository 
     */
    int slideShowDelayIndex();
    
    /*
     * SlideShowSettingsView will call this API to set the index of the transition delay chosen. 
     * This data will be written to the Central Repository
     */
    void setSlideShowDelayIndex( int index );
    
    /*
     * It will return the slide show delay time for playing the slide show.
     * This value is fetched for central repository
     */
    int slideShowDelayTime() ;
    
    /*
     * This will return the list of effects as available in the central repository 
     */
    QStringList slideShowEffectList();
    
    /*
     * This will return the effect ID based on the effect index 
     * Effect index will be 0,1,2... and effect ID is GLX_EFFECT
     * Effect id is store in the centrep with respect to index.
     */
    int slideShowEffectId( int index ) ;
    
    /* 
     * Returns Current Media Wall 3D effect Status
     * This value is fetched from Central repository 
     */
    int mediaWall3DEffect();
    
    /*
     * Set Media Wall 3D effect. True = 3D Effect On / False = 3D Effect Off 
     * This data will be written to the Central Repository
     */
    void setmediaWall3DEffect( int index );
    
protected :
    /*
     * Constructor
     */
    GlxSetting( );
    
    /*
     * Copy Constructor
     */
    GlxSetting( GlxSetting & );
    
    /*
     * Destructor
     */
    ~GlxSetting();
	
private :    
    XQSettingsManager *mSettingsManager;
    XQSettingsKey *mTransitionEffectCenrepKey;
    XQSettingsKey *mTransitionDelayCenrepKey;
    
    XQSettingsKey *mSlideShowEffectCenRepKey[ NBR_SLIDESHOW_EFFECT ];
    
    
    XQSettingsKey *mSlowCenRepKey;
    XQSettingsKey *mMediumCenRepKey;
    XQSettingsKey *mFastCenRepKey;
    
    XQSettingsKey *m3DEffectCenRepKey;
    
    static GlxSetting mObj;
};



#endif //GLXSETTING_H
