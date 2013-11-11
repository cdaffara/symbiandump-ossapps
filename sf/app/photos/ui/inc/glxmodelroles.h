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



#ifndef GLXMODELROLES_H
#define GLXMODELROLES_H
enum
{
    //For ItemImageData Function role START
    GlxHgVisibilityRole = Qt::UserRole + 1, // VisibilityRole For MediaWall ,  Dont Change This Value - Depedency To HgWidget
    GlxFsImageRole,                         //to get the fullscreen image
    GlxDefaultImage,
    GlxQImageSmall,                     //To Get Grid Thumbnail as QImage
    GlxQImageLarge,                     //To Get FullScreen Thumbnail as QImage
    GlxHdmiBitmap,                      //to get the pre decoded thumbnail for HDMI
    GlxItemImageDataEnd = 0x100,
    //For ItemImageData Function role END
    
    //For CollectionProperty Function role START
    GlxFocusIndexRole,                  //to get and set the selected item index
    GlxSubStateRole,                    //to publish the sub state of a SM states
    GlxPopulated,
    GlxViewTitle,
    GlxVisualWindowIndex,               //to set the visual window index in media list
    GlxPaintPageFlag,
    GlxCollectionPropertyEnd = 0x200,
    //For CollectionProperty Function role END
    
    //For ItemProperty Function role START
    GlxFrameCount,                       //to get the number of frame in an image
    GlxImageCorruptRole,                 //To get the corrupt image status
    GlxUriRole,                          //to get the URI of the image
    GlxSizeRole,                         //to get the size of the image
    GlxImageTitle,                        //to fetch the title of the image
    GlxDimensionsRole,                  
    GlxDateRole,                         //to get the image created or modified date 
    GlxTimeRole,                         //to get the time image was modified
    GlxDescRole,                         //to get the description of the images
    GlxItemPropertyEnd = 0x300,                       
    //For ItemProperty Function role
 
    GlxContextRole,                      //to get and set the context    
    GlxSelectedIndexRole,               //to get and set the seleceted item list
    GlxListItemCount,                   //to get the number of item in the album list
    GlxSystemItemRole,                   //to get the item is system item or not
    GlxFavorites,                       //to get the image is in favorites folder or not
    GlxRemoveContextRole,                 //to remove the context
    GlxTempVisualWindowIndex,              //to store the visual index obtained from AM temporarily
};


#endif 
