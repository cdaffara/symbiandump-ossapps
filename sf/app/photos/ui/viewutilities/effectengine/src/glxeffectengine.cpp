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


#include "glxeffectengine.h"
#include "glxeffectpluginbase.h"
#include "glxtransitioneffect.h"
#include "glxsettinginterface.h"
#include "glxeffectpluginresolver.h"

#include <QDebug>

GlxEffectEngine::GlxEffectEngine( ) 
    : mNbrEffectRunning( 0 ), 
      mEffectPlugin( NULL ), 
      mTransitionEffect( NO_EFFECT ),
      mEffectResolver( NULL )
{
    qDebug( "GlxSlideShowEffectEngine::GlxSlideShowEffectEngine()" );
    mTransitionEffectList.clear();
}

GlxEffectEngine::~GlxEffectEngine()
{
    qDebug( "GlxSlideShowEffectEngine::~GlxSlideShowEffectEngine()" );
    delete mEffectResolver;
    mEffectResolver = NULL;
	
    cleanTransitionEfffect();
}

void GlxEffectEngine::registerEffect( const QString &itemType )
{
    GlxSettingInterface *settingObj = GlxSettingInterface::instance(); // NO ownership 
    if ( mEffectResolver == NULL ) {
        mEffectResolver = new GlxEffectPluginResolver();
    }
    /* read the selected effect through the centrep and get the effect plugin object through effect plugin resolver*/
    mEffectPlugin = mEffectResolver->effectPlugin( settingObj->slideShowEffectId( settingObj->slideShowEffectIndex ( ) ) ); //No ownership
    
    QList <QString > effectPathList = mEffectPlugin->effectFileList();
    
    qDebug( "GlxSlideShowEffectEngine::registerEffect() item type %s file path %s", itemType.utf16(), effectPathList[0].utf16() );
    for ( int i = 0; i < effectPathList.count() ; ++i ) {
        HbEffect::add( itemType, effectPathList.at( i ), QString( "Click%1" ).arg( i ) );
    }    
}

void GlxEffectEngine::deRegisterEffect( const QString &itemType )
{
    qDebug( "GlxSlideShowEffectEngine::deRegisterEffect() item type %s", itemType.utf16() );
    QList <QString > effectPathList = mEffectPlugin->effectFileList();
    for ( int i = 0; i < effectPathList.count() ; ++i ) {
        HbEffect::remove( itemType, effectPathList.at( i ), QString( "Click%1" ).arg( i ) );
    }
    mEffectPlugin = NULL;
}

void GlxEffectEngine::registerTransitionEffect()  
{
    qDebug( "GlxSlideShowEffectEngine::registerTransitionEffect()" );
    initTransitionEffect();
}

void GlxEffectEngine::deregistertransitionEffect()
{
    qDebug( "GlxSlideShowEffectEngine::deregisterTransitionEffect()" );
    cleanTransitionEfffect();
}

void GlxEffectEngine::runEffect( QGraphicsItem *  item, const QString &  itemType )
{
    qDebug( "GlxSlideShowEffectEngine::runEffect()1 item type %s", itemType.utf16() );
    HbEffect::start( item, itemType, QString( "Click1" ), this, "slideShowEffectFinished" );
    ++mNbrEffectRunning;
}

void GlxEffectEngine::runEffect( QList< QGraphicsItem * > &  items, const QString &  itemType )
{
    qDebug( "GlxSlideShowEffectEngine::runEffect()2 item Type %s", itemType.utf16() );
    mEffectPlugin->setUpItems(items);
    for ( int i = 0; i < items.count() ; ++i ) {
        if ( mEffectPlugin->isAnimationLater( i ) == FALSE ) {
            HbEffect::start( items.at( i ), itemType, QString( "Click%1" ).arg( i ), this, "slideShowEffectFinished" );
        }
        ++mNbrEffectRunning;
    }
}

void GlxEffectEngine::runEffect( QList< QGraphicsItem * > &  items, GlxEffect transitionEffect )
{
    qDebug( "GlxSlideShowEffectEngine::runEffect()3 effect type %d ", transitionEffect );
    
    GlxTransitionEffectSetting *effectSetting = mTransitionEffectList.value( transitionEffect );
    
    if ( effectSetting == NULL || items.count() != effectSetting->count() ) {
         return;
    }
    
    mTransitionEffect = transitionEffect;
    for ( int i = 0; i < effectSetting->count() ; ++i) {
        ++mNbrEffectRunning;
        if ( ( i == effectSetting->count() -1) && effectSetting->isTransitionLater() )
        {
            effectSetting->setAnimationItem( items.at( i ) );
            items.at( i )->hide();
        }
        else {
            HbEffect::start( items.at( i ), effectSetting->itemType().at( i ), effectSetting->eventType().at( i ), this, "transitionEffectFinished" );
        }
    }    
}

void GlxEffectEngine::cancelEffect( QGraphicsItem *  item )
{
    if ( HbEffect::effectRunning( item, QString( "Click1" ) ) ) {
        HbEffect::cancel( item, QString( "Click1" ) );
    }
}

void GlxEffectEngine::cancelEffect( const QList< QGraphicsItem * > &  items )
{
    for ( int i = 0; i < items.count() ; ++i ) {    
        if ( HbEffect::effectRunning( items.at( i ), QString( "Click%1" ).arg( i ) ) ) {
            HbEffect::cancel( items.at( i ), QString( "Click%1" ).arg( i ) );
        }
    }
}

void GlxEffectEngine::cancelEffect( QList< QGraphicsItem * > &  items, GlxEffect transitionEffect )
{
    GlxTransitionEffectSetting *effectSetting = mTransitionEffectList.value( transitionEffect );
    
    if ( effectSetting == NULL || items.count() != effectSetting->count() ) {
         return;
    }
    
    for ( int i = 0; i < effectSetting->count() ; ++i) {
        HbEffect::cancel(items.at( i ), effectSetting->eventType().at( i ) );
    }     
}

bool GlxEffectEngine::isEffectRuning( QGraphicsItem *  item )
{
    if ( HbEffect::effectRunning( item, QString( "Click1" ) ) ) {
        return true;
    }
    return false;
}

bool GlxEffectEngine::isEffectRuning( const QList< QGraphicsItem * > &  items )
{
    for ( int i = 0; i < items.count() ; ++i ) {    
        if ( HbEffect::effectRunning( items.at( i ), QString( "Click%1" ).arg( i ) ) ) {
            return true;
        }
    }
    return false;
}

void GlxEffectEngine::slideShowEffectFinished( const HbEffect::EffectStatus &status )
{
    Q_UNUSED( status )
    qDebug( "GlxSlideShowEffectEngine::slideShowEffectFinished() number of effect %d ", mNbrEffectRunning);
    
    --mNbrEffectRunning;
    
    if ( mEffectPlugin->isAnimationLater( mNbrEffectRunning) ) {
        HbEffect::start( mEffectPlugin->animationItem(), mEffectPlugin->ItemType(), QString( "Click%1" ).arg(mNbrEffectRunning), this, "slideShowEffectFinished" );
    }
    
    if (mNbrEffectRunning == 0) {
        emit effectFinished();
    }
}

void GlxEffectEngine::transitionEffectFinished( const HbEffect::EffectStatus &status )
{
    Q_UNUSED( status )
    qDebug( "GlxSlideShowEffectEngine::transitionEffectFinished() number of effect %d status %d", mNbrEffectRunning, status.reason);
    
    --mNbrEffectRunning;
    if ( mNbrEffectRunning == 1 ) {
        GlxTransitionEffectSetting *effectSetting = mTransitionEffectList.value( mTransitionEffect );
        if (  effectSetting->isTransitionLater() ){ 
            effectSetting->animationItem()->show();
            HbEffect::start( effectSetting->animationItem(), effectSetting->itemType().at( 1 ), effectSetting->eventType().at( 1 ), this, "transitionEffectFinished" );
            mTransitionEffect = NO_EFFECT;
        }    
    }
    
    if (mNbrEffectRunning == 0) {
        emit effectFinished();
    }
}

void GlxEffectEngine::initTransitionEffect()
{
    GlxTransitionEffectSetting *effectSetting = NULL;
    
    effectSetting = new GlxTransitionEffectSetting( GRID_TO_FULLSCREEN );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ GRID_TO_FULLSCREEN ] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( FULLSCREEN_TO_GRID );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ FULLSCREEN_TO_GRID ] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( FULLSCREEN_TO_GRID_PORTRAIT );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ FULLSCREEN_TO_GRID_PORTRAIT ] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( GRID_TO_ALBUMLIST );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ GRID_TO_ALBUMLIST ] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( ALBUMLIST_TO_GRID );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ ALBUMLIST_TO_GRID ] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( FULLSCREEN_TO_DETAIL );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ FULLSCREEN_TO_DETAIL ] = effectSetting; 
    
    effectSetting = new GlxTransitionEffectSetting( DETAIL_TO_FULLSCREEN );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at( i ), effectSetting->effectFileList().at( i ), effectSetting->eventType().at( i ) ) ;
    }
    mTransitionEffectList[ DETAIL_TO_FULLSCREEN ] = effectSetting;    
}

void GlxEffectEngine::cleanTransitionEfffect()
{	
    qDebug( "GlxSlideShowEffectEngine::cleanTrnastionEfffect()" );
    
    QHashIterator<GlxEffect, GlxTransitionEffectSetting *> iter( mTransitionEffectList );

    while ( iter.hasNext() ) {
        iter.next();
        GlxTransitionEffectSetting *list =  iter.value();
        for ( int i = 0; i < list->count(); ++i ) {
            HbEffect::remove( list->itemType().at( i ), list->effectFileList().at( i ), list->eventType().at( i ) ) ;
        }
        delete list;        
    }
    mTransitionEffectList.clear();
}

