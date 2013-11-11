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

#ifndef GLXDETAILSICON_H
#define GLXDETAILSICON_H

#include <hbwidget.h>


#ifdef BUILD_DETAILSCUSTOM
#define MY_EXPORT Q_DECL_EXPORT
#else
#define MY_EXPORT Q_DECL_IMPORT
#endif

class HbIconItem;
class HbIcon;

class MY_EXPORT GlxDetailsIcon : public HbWidget 
{	
    Q_OBJECT
    
public:
    GlxDetailsIcon(QGraphicsItem *parent = NULL);
    ~GlxDetailsIcon();
    	
	/*
	 * Sets the  Icon of Favourite IconItem.
	 */
	void setItemIcon(const HbIcon &icon);
	
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);	
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
    
signals :
     void updateFavourites();

private:
   HbIconItem *mFavIcon;   
};

#endif // GLXDETAILSICON_H
