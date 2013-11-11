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


#include <hbiconitem.h>
#include <hbframeitem.h>
#include "glxdetailsicon.h"
#include <hbstackedlayout.h>
#include <QEvent>

//--------------------------------------------------------------------------------------------------------------------------------------------
//mousePressEvent
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsIcon::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearCurrentModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsIcon::mouseReleaseEvent (QGraphicsSceneMouseEvent *event) 
{
  Q_UNUSED(event)  
  emit updateFavourites();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsIcon
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsIcon::GlxDetailsIcon(QGraphicsItem *parent) : HbWidget(parent)
{
  mFavIcon = new HbIconItem(this);
  //add the layout to the icon or else the icon is not showed.
  HbStackedLayout *stackedLayout = new HbStackedLayout;
  stackedLayout->addItem( mFavIcon );
  setLayout( stackedLayout );
  
  mFavIcon->setOpacity(0.9);
  HbFrameItem* frame = new HbFrameItem(this);
  frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
  frame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
  frame->graphicsItem()->setOpacity(0.1); 
  setBackgroundItem(frame->graphicsItem(),-1);   
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsIcon
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsIcon::~GlxDetailsIcon()
{
  delete mFavIcon;
  mFavIcon = NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemIcon
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsIcon::setItemIcon(const HbIcon &icon)
{
    mFavIcon->setIcon(icon);    
} 
