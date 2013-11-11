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

#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <QStringList>

#include "glxsetting.h"
#include "glxeffectpluginresolver.h"
#include "glxuistd.h"


const TUint32 KGlxTransitionEffect     = 0x1;
const TUint32 KGlxTransitionDelay  = 0x2;
const TUint32 KGlxSlow  = 0x3;
const TUint32 KGlxMeduim  = 0x4;
const TUint32 KGlxFast  = 0x5;
const TUint32 KGlxSlideShowEffect[ ]  = { 0x6, 0x7, 0x8, 0x9 };
const TUint32 KGlx3DEffect  = 0xA;
const TUint32 KCRUidGallery = 0x20007194;

GlxSetting GlxSetting::mObj ;


GlxSetting::GlxSetting(  ) 
{
    mSettingsManager = new XQSettingsManager();
    mTransitionEffectCenrepKey = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery , KGlxTransitionEffect );
    mTransitionDelayCenrepKey = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery , KGlxTransitionDelay );
    mSlowCenRepKey = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxSlow );
    mMediumCenRepKey = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxMeduim );
    mFastCenRepKey = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxFast );
    m3DEffectCenRepKey = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlx3DEffect );

    for ( int i = 0; i < NBR_SLIDESHOW_EFFECT; i++ ) {
        mSlideShowEffectCenRepKey[ i ] = new XQSettingsKey( XQSettingsKey::TargetCentralRepository, KCRUidGallery , KGlxSlideShowEffect[ i ] );
    }
}

GlxSetting::GlxSetting( GlxSetting & )
{

}

GlxSetting::~GlxSetting( )
{            
    delete mFastCenRepKey;
    delete mMediumCenRepKey;
    delete mSlowCenRepKey;
    delete mTransitionDelayCenrepKey;
    delete mTransitionEffectCenrepKey;
    delete mSettingsManager;
    delete m3DEffectCenRepKey;
    
    for ( int i = 0 ; i < NBR_SLIDESHOW_EFFECT ; i++ ) {
        delete mSlideShowEffectCenRepKey[ i ];
    }
}

GlxSettingInterface * GlxSetting::instance()
{
    return &mObj ;
}

int GlxSetting::slideShowEffectIndex()
{
    QVariant effectvalue = mSettingsManager->readItemValue(*mTransitionEffectCenrepKey);
    return effectvalue.toInt();
}

void GlxSetting::setslideShowEffectIndex( int index )
{
    mSettingsManager->writeItemValue( *mTransitionEffectCenrepKey, index ) ;
}

int GlxSetting::slideShowDelayIndex()
{
    QVariant effectvalue = mSettingsManager->readItemValue(*mTransitionDelayCenrepKey);
    return effectvalue.toInt();
}

void GlxSetting::setSlideShowDelayIndex( int index )
{
    mSettingsManager->writeItemValue(*mTransitionDelayCenrepKey, index);
}

int GlxSetting::slideShowDelayTime()
{
    QVariant effectvalue = mSettingsManager->readItemValue(*mTransitionDelayCenrepKey);
    switch ( effectvalue.toInt() ) {
    case SLOW:
        return mSettingsManager->readItemValue(*mSlowCenRepKey).toInt();
        
    case MEDIUM:
        return mSettingsManager->readItemValue(*mMediumCenRepKey).toInt();

    case FAST:
        return mSettingsManager->readItemValue(*mFastCenRepKey).toInt();

     default:
        return 3000;
     }
}

QStringList GlxSetting::slideShowEffectList()
{
    QStringList effectList;
    effectList.clear();
    int effectId = 0;
    
    for ( int i = 0 ; i < NBR_SLIDESHOW_EFFECT ; i++ ) {
        effectId = mSettingsManager->readItemValue( * mSlideShowEffectCenRepKey[ i ] ).toInt() ;
        effectList << GlxEffectPluginResolver::effectName ( effectId );
    }
    
    return effectList;
}

int GlxSetting::slideShowEffectId( int index )
{
    if ( index >= NBR_SLIDESHOW_EFFECT || index < 0 ) {
       return NO_EFFECT ;
    }
    
    return mSettingsManager->readItemValue( * mSlideShowEffectCenRepKey[ index ] ).toInt() ;
}

int GlxSetting::mediaWall3DEffect()
{
    QVariant effectvalue = mSettingsManager->readItemValue(*m3DEffectCenRepKey);
    return effectvalue.toInt();
}

void GlxSetting::setmediaWall3DEffect( int index )
{
    mSettingsManager->writeItemValue( *m3DEffectCenRepKey, index ) ;
}



