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

#ifndef GLXTRANSITIONEFFECT_H
#define GLXTRANSITIONEFFECT_H

#include <QList>

#include "glxuistd.h"

class QString;
class QGraphicsItem ;

/*
 * This class have the views transition effect information
 */
class GlxTransitionEffectSetting
{
public :
    GlxTransitionEffectSetting(GlxEffect effect);
    ~GlxTransitionEffectSetting();
    
    QList <QString > effectFileList() { return mEffectFileList ; }
    QList <QString > itemType() { return mItemType ; }
    QList <QString > eventType() { return mEventType ; }
    bool isTransitionLater() { return mTransitionLater ; }
    QGraphicsItem * animationItem() { return mItem ; }
    
    void setEffectFileList( QList <QString > & effectFileList) { mEffectFileList = effectFileList ; }
    void setItemTypes( QList <QString > & itemType) { mItemType = itemType; }
    void setEventTypes( QList <QString > & eventType) { mEventType = eventType ; }
    void setTransitionLater(bool transitionLater) { mTransitionLater = transitionLater ; }
    void setAnimationItem(QGraphicsItem *item) { mItem = item ;}
    int count() { return mEffectFileList.count() ; }
    
private :
    void init();
    
private :
    GlxEffect mEffect;     //transition effect type
    bool mTransitionLater; //second animation will run same time(false) or later (true)
    QGraphicsItem *mItem; //it will used in the case of animation run later
    QList <QString > mEffectFileList; //list of fxml file used for animation
    QList <QString > mItemType; //list of item type 
    QList <QString > mEventType; //list of event type
};

#endif //GLXTRANSITIONEFFECT_H
