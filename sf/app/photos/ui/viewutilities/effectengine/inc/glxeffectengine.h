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


#ifndef GLXEFFECTENGINE_H
#define GLXEFFECTENGINE_H

#ifdef BUILD_GLXVIEWUTILITIES
#define GLX_VIEWUTILITIES_EXPORT Q_DECL_EXPORT
#else
#define GLX_VIEWUTILITIES_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <hbeffect.h>
#include <QString>
#include <QHash>
#include <QList>
#include "glxuistd.h"


class QStringList ;
class GlxTransitionEffectSetting ;
class GlxEffectPluginBase;
class GlxEffectPluginResolver;

/*
 * It is customise class for Photo Application to run the effect.
 */

class GLX_VIEWUTILITIES_EXPORT GlxEffectEngine : public QObject
{
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    GlxEffectEngine( );
    /*
     * Destructor
     */
    ~GlxEffectEngine();
    
    /*
     * Register the item type with selected effect
     * User can change the effect at run time so it is required to register and deregister the effect
     * before playing the effect.
     */
    void registerEffect(const QString &itemType);
    
    /*
     * Deregister the current register slide show effect
     */
    void deRegisterEffect(const QString &itemType);
    
   /*  
    *  To register a set of transition effect 
    *  e.g "grid to full screeen", "fullscreen to grid"  
    */  
    void registerTransitionEffect();
    
    /*
     * Deregister the set of register transition effect
     */
    void deregistertransitionEffect();
    
    /*
     * Run the effect on single item
     */
    void runEffect(QGraphicsItem *  item, const QString &  itemType ); 
    
    /*
     * Run the effect on multiple item
     */
    void runEffect(QList< QGraphicsItem * > &  items, const QString &  itemType );
    
    /*
     * Run the transition effect 
     */
    void runEffect(QList< QGraphicsItem * > &  items, GlxEffect transitionEffect);
    
    /*
     * Cancel the runing effect on the given item
     */
    void cancelEffect(QGraphicsItem *  item);
    
    /*
     * Cancel the running effect on the given items
     */
    void cancelEffect( const QList< QGraphicsItem * > &  items );
    
    /*
     * cancel the runing transition effect
     */
    void cancelEffect(QList< QGraphicsItem * > &  items, GlxEffect transitionEffect);
    
    /*
     * Return the running status of the given item
     */
    bool isEffectRuning(QGraphicsItem *  item);
    
    /*
     * return the running status of given items
     */
    bool isEffectRuning(const QList< QGraphicsItem * > &  items);
    
signals:
    /*
     * emit the signal of effect finished
     */
    void effectFinished();
    
public slots:
    /*
     * Slot to monitor the slide show effect finished
     */
    void slideShowEffectFinished( const HbEffect::EffectStatus &status );
    
    /*
     * Slot to monitor the transi
     */
    void transitionEffectFinished( const HbEffect::EffectStatus &status );

private:
    /*
     * Initialise the all transition effect
     */
    void initTransitionEffect();
    
    /*
     * Clear the all transition effect
     */
    void cleanTransitionEfffect();
    
private:
    int mNbrEffectRunning;               //containe the number of effect running
    GlxEffectPluginBase *mEffectPlugin;  //No ownership
    QHash<GlxEffect, GlxTransitionEffectSetting *> mTransitionEffectList;
    GlxEffect mTransitionEffect;
    GlxEffectPluginResolver *mEffectResolver;
};

#endif /*GLXEFFECTENGINE_H*/
