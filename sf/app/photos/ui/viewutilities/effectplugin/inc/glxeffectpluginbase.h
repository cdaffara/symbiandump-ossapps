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

#ifndef GLXEFFECTPLUGINBASE_H
#define GLXEFFECTPLUGINBASE_H

#include <QList>
#include <QString>
#include <QGraphicsItem>

/*
 * It is based class of diffetent animation plugin.
 * These Plugin mostly used for playing the animation in slide show
 */

class GlxEffectPluginBase 
{
public :
    /*
     * Constructor
     */
    GlxEffectPluginBase( int effectId ) { mEffectId = effectId ;  }
    
    /*
     * Destructor
     */
    virtual ~GlxEffectPluginBase() {}
    
    /*
     * return the list of effect file's 
     */
    virtual QList <QString > effectFileList() = 0;
    
    /*
     * Setup the item's postion and other properties before plaing the animation
     */
    virtual void setUpItems( QList< QGraphicsItem * > &  items ) { Q_UNUSED( items ) }

    /*
     * second animation will run same time(false) or later (true)
     */    
    virtual bool isAnimationLater(int index) 
    { 
        Q_UNUSED( index )
        return false ; 
    }
    
    /*
     * Return the item type of animated object
     */
    virtual QString ItemType() { return QString("HbIconItem") ; }
    
    /*
     * Return the animation object
     */
    virtual QGraphicsItem * animationItem() { return NULL; }
    
private :
    int mEffectId ;

};

#endif /*GLXEFFECTPLUGINBASE_H*/
