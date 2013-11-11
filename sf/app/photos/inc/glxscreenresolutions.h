/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Screen resolutions
 *
*/



#ifndef T_GLXSCREENRESOLUTIONSINFO_H
#define T_GLXSCREENRESOLUTIONSINFO_H
    
/*
* Device supports 360x640 resolution (QHD portrait, touch screen)
*/
#ifdef __LAYOUT_360_640_TOUCH
    // Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 111;
    const TInt KGlxThumbnailSmallHeight = 83;
    const TInt KGlxThumbnailLargeWidth = 640;
    const TInt KGlxThumbnailLargeHeight = 360;
    const TInt KGlxThumbnailPortraitWidth = 360;
    const TInt KGlxThumbnailPortraitHeight = 640;

    
/*
 * Device supports 640x360 resolution (QHD landscape, touch screen)
 */
#elif defined  __LAYOUT_640_360_TOUCH
    // Persistent thumbnail sizes   
    const TInt KGlxThumbnailSmallWidth = 83;
    const TInt KGlxThumbnailSmallHeight = 111;
    const TInt KGlxThumbnailLargeWidth = 640;
    const TInt KGlxThumbnailLargeHeight = 360;
    const TInt KGlxThumbnailPortraitWidth = 360;
    const TInt KGlxThumbnailPortraitHeight = 640;

/*
* Device supports 240x320 resolution (QVGA)
*/
#elif defined __LAYOUT_240_320
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 56;
    const TInt KGlxThumbnailSmallHeight = 42;
    const TInt KGlxThumbnailLargeWidth = 320;
    const TInt KGlxThumbnailLargeHeight = 240;
    const TInt KGlxThumbnailPortraitWidth = 240;
    const TInt KGlxThumbnailPortraitHeight = 320;

/*
* Device supports 320x240 resolution (QVGA landscape)
*/
#elif defined __LAYOUT_320_240
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 42;
    const TInt KGlxThumbnailSmallHeight = 56;
    const TInt KGlxThumbnailLargeWidth = 240;
    const TInt KGlxThumbnailLargeHeight = 320;
    const TInt KGlxThumbnailPortraitWidth = 320;
    const TInt KGlxThumbnailPortraitHeight = 240;

/*
* Device supports 176x208 resolution
*
* Feature not supported, this flag must be always undefined. Do not edit this !
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined  __LAYOUT176_208
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 36;
    const TInt KGlxThumbnailSmallHeight = 28;
    const TInt KGlxThumbnailLargeWidth = 208;
    const TInt KGlxThumbnailLargeHeight = 176;
    const TInt KGlxThumbnailPortraitWidth = 176;
    const TInt KGlxThumbnailPortraitHeight = 208;
    
/*
* Device supports 208x176 resolution
*
* Feature not supported, this flag must be always undefined. Do not edit this !
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT208_176    
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 28;
    const TInt KGlxThumbnailSmallHeight = 36;
    const TInt KGlxThumbnailLargeWidth = 176;
    const TInt KGlxThumbnailLargeHeight = 208;
    const TInt KGlxThumbnailPortraitWidth = 208;
    const TInt KGlxThumbnailPortraitHeight = 176;

/*
* Device supports 208x208 resolution (square)
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT208_208    
 //to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 36;
    const TInt KGlxThumbnailSmallHeight = 36;
    const TInt KGlxThumbnailLargeWidth = 208;
    const TInt KGlxThumbnailLargeHeight = 208;
    const TInt KGlxThumbnailPortraitWidth = 208;
    const TInt KGlxThumbnailPortraitHeight = 208;
    
/*
* Device supports 352x416 resolution (double)
*
* Feature not supported, this flag must be always undefined. Do not edit this !
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_352_416     
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 72;
    const TInt KGlxThumbnailSmallHeight = 55;
    const TInt KGlxThumbnailLargeWidth = 416;
    const TInt KGlxThumbnailLargeHeight = 352;
    const TInt KGlxThumbnailPortraitWidth = 352;
    const TInt KGlxThumbnailPortraitHeight = 416;


/*
* Device supports 416x352 resolution (double landscape)
*
* Feature not supported, this flag must be always undefined. Do not edit this !
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_416_352
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 55;
    const TInt KGlxThumbnailSmallHeight = 72;
    const TInt KGlxThumbnailLargeWidth = 352;
    const TInt KGlxThumbnailLargeHeight = 416;
    const TInt KGlxThumbnailPortraitWidth = 416;
    const TInt KGlxThumbnailPortraitHeight = 352;
    
/*
* Device supports 480x640 resolution (VGA portrait)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_480_640
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 146;
    const TInt KGlxThumbnailSmallHeight = 110;
    const TInt KGlxThumbnailLargeWidth = 640;
    const TInt KGlxThumbnailLargeHeight = 480;
    const TInt KGlxThumbnailPortraitWidth = 480;
    const TInt KGlxThumbnailPortraitHeight = 640;
    
/*
* Device supports 640x480 resolution (VGA landscape)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_640_480
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 110;
    const TInt KGlxThumbnailSmallHeight = 146;
    const TInt KGlxThumbnailLargeWidth = 480;
    const TInt KGlxThumbnailLargeHeight = 640;
    const TInt KGlxThumbnailPortraitWidth = 640;
    const TInt KGlxThumbnailPortraitHeight = 480;
    
  
/*
* Device supports 320x480 resolution (HVGA portrait)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_320_480
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 56;
    const TInt KGlxThumbnailSmallHeight = 42;
    const TInt KGlxThumbnailLargeWidth = 480;
    const TInt KGlxThumbnailLargeHeight = 320;
    const TInt KGlxThumbnailPortraitWidth = 320;
    const TInt KGlxThumbnailPortraitHeight = 480;
    

/*
* Device supports 480x320 resolution (HVGA landscape)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_480_320
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 42;
    const TInt KGlxThumbnailSmallHeight = 56;
    const TInt KGlxThumbnailLargeWidth = 320;
    const TInt KGlxThumbnailLargeHeight = 480;
    const TInt KGlxThumbnailPortraitWidth = 480;
    const TInt KGlxThumbnailPortraitHeight = 320;
    
    
/*
* Device supports 800x352 resolution (WVGA landscape)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_800_352
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 45;
    const TInt KGlxThumbnailSmallHeight = 140;
    const TInt KGlxThumbnailLargeWidth = 352;
    const TInt KGlxThumbnailLargeHeight = 800;
    const TInt KGlxThumbnailPortraitWidth = 800;
    const TInt KGlxThumbnailPortraitHeight = 352;
    
/*
* Device supports 352x800 resolution (WVGA portrait)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_352_800
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 140;
    const TInt KGlxThumbnailSmallHeight = 45;
    const TInt KGlxThumbnailLargeWidth = 800;
    const TInt KGlxThumbnailLargeHeight = 352;
    const TInt KGlxThumbnailPortraitWidth = 352;
    const TInt KGlxThumbnailPortraitHeight = 800;
    
/*
* Device supports 480x640 resolution (VGA portrait, touch screen)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_480_640_TOUCH
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 112;
    const TInt KGlxThumbnailSmallHeight = 85;
    const TInt KGlxThumbnailLargeWidth = 640;
    const TInt KGlxThumbnailLargeHeight = 480;
    const TInt KGlxThumbnailPortraitWidth = 480;
    const TInt KGlxThumbnailPortraitHeight = 640;
    

/*
* Device supports 640x480 resolution (VGA landscape, touch screen)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_640_480_TOUCH
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 85;
    const TInt KGlxThumbnailSmallHeight = 112;
    const TInt KGlxThumbnailLargeWidth = 480;
    const TInt KGlxThumbnailLargeHeight = 640;
    const TInt KGlxThumbnailPortraitWidth = 640;
    const TInt KGlxThumbnailPortraitHeight = 480;
    

/*
* Device supports 240x320 resolution (QVGA2 for larger screens, portrait)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_240_320_LARGE_SCREEN
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 56;
    const TInt KGlxThumbnailSmallHeight = 42;
    const TInt KGlxThumbnailLargeWidth = 320;
    const TInt KGlxThumbnailLargeHeight = 240;
    const TInt KGlxThumbnailPortraitWidth = 240;
    const TInt KGlxThumbnailPortraitHeight = 320;
    

/*
* Device supports 320x240 resolution (QVGA2 for larger screens, landscape)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_320_240_LARGE_SCREEN
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 42;
    const TInt KGlxThumbnailSmallHeight = 56;
    const TInt KGlxThumbnailLargeWidth = 240;
    const TInt KGlxThumbnailLargeHeight = 320;
    const TInt KGlxThumbnailPortraitWidth = 320;
    const TInt KGlxThumbnailPortraitHeight = 240;
    

/*
* Device supports 240x320 resolution (QVGA, touch screen)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_240_320_TOUCH
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 56;
    const TInt KGlxThumbnailSmallHeight = 42;
    const TInt KGlxThumbnailLargeWidth = 320;
    const TInt KGlxThumbnailLargeHeight = 240;
    const TInt KGlxThumbnailPortraitWidth = 240;
    const TInt KGlxThumbnailPortraitHeight = 320;
    
/*
* Device supports 320x240 resolution (QVGA landscape, touch screen)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_320_240_TOUCH
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 42;
    const TInt KGlxThumbnailSmallHeight = 56;
    const TInt KGlxThumbnailLargeWidth = 240;
    const TInt KGlxThumbnailLargeHeight = 320;
    const TInt KGlxThumbnailPortraitWidth = 320;
    const TInt KGlxThumbnailPortraitHeight = 240;
    

/*
* Device supports 320x480 resolution (HVGA portrait, touch screen)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_320_480_TOUCH
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 85;
    const TInt KGlxThumbnailSmallHeight = 56;
    const TInt KGlxThumbnailLargeWidth = 480;
    const TInt KGlxThumbnailLargeHeight = 320;
    const TInt KGlxThumbnailPortraitWidth = 320;
    const TInt KGlxThumbnailPortraitHeight = 480;
    

/*
* Device supports 480x320 resolution (HVGA landscape, touch screen)
*
* For Smaller thumbnail, we need to modify with actual values when we get so.
* The values are calculated roughly, should be modified with real values later
*/
#elif defined __LAYOUT_480_320_TOUCH
//to-do add the values to constants according to the resolutions and requirements
		// Persistent thumbnail sizes
    const TInt KGlxThumbnailSmallWidth = 56;
    const TInt KGlxThumbnailSmallHeight = 85;
    const TInt KGlxThumbnailLargeWidth = 320;
    const TInt KGlxThumbnailLargeHeight = 480;
    const TInt KGlxThumbnailPortraitWidth = 480;
    const TInt KGlxThumbnailPortraitHeight = 320;
#else
// making 640x360 resolution as default, 
    const TInt KGlxThumbnailSmallWidth = 85;
    const TInt KGlxThumbnailSmallHeight = 112;
    const TInt KGlxThumbnailLargeWidth = 640;
    const TInt KGlxThumbnailLargeHeight = 360;
    const TInt KGlxThumbnailPortraitWidth = 360;
    const TInt KGlxThumbnailPortraitHeight = 640;

#endif    

#endif // T_GLXSCREENRESOLUTIONSINFO_H
