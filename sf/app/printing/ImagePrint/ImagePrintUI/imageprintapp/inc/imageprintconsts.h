/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef IMAGEPRINTCONSTS_H
#define IMAGEPRINTCONSTS_H

// High resolution / Low resolution ratio. In LR mode all UI pixel
// parameter are half of corresponding HR parameter.
const TInt HLRatio( 2 );

// Preview Pane PORTRAIT
const TInt KPreviewPaneWidthPort( 256 );
const TInt KPreviewPaneHeightPort( 256 );
const TInt KPreviewPanePositionXPort( 0 );
const TInt KPreviewPanePositionYPort( 0 );
const TInt KTemplateIndLeftPosX1Port( 22 );
const TInt KTemplateIndLeftPosY1Port( 228 );
const TInt KTemplateIndLeftPosX2Port( 22 );
const TInt KTemplateIndLeftPosY2Port( 252 );
const TInt KTemplateIndLeftPosX3Port( 10 );
const TInt KTemplateIndLeftPosY3Port( 240 );
const TInt KTemplateIndRightPosX1Port( 236 );
const TInt KTemplateIndRightPosY1Port( 228 );
const TInt KTemplateIndRightPosX2Port( 236 );
const TInt KTemplateIndRightPosY2Port( 252 );
const TInt KTemplateIndRightPosX3Port( 248 );
const TInt KTemplateIndRightPosY3Port( 240 );
const TInt KPreviewPaneFontSizePort( 12 );
const TInt KPortPaperWidthPort( 192 );
const TInt KportPaperHeightPort( 256 );
const TInt KPortPaperPositionXPort( 32 );
const TInt KPortPaperPositionYPort( 0 );
const TInt KLandPaperWidthPort( 256 );
const TInt KLandPaperHeightPort( 192 );
const TInt KLandPaperPositionXPort( 0 );
const TInt KLandPaperPositionYPort( 32 );
const TInt KPageNumberBoxPositionPortX( 170 );
const TInt KPageNumberBoxSizePortX( 86 );
const TInt KPageNumberBoxSizePortY( 35 );

// Preview Pane PORTRAIT QVGA
const TInt KPreviewPaneWidthPortQVGA( 200 );   // pane width
const TInt KPreviewPaneHeightPortQVGA( 200 );  // pane height
const TInt KPreviewPanePositionXPortQVGA( 0 ); // leftmost pos of pane
const TInt KPreviewPanePositionYPortQVGA( 0 ); // top pos of pane
const TInt KTemplateIndLeftPosX1PortQVGA( 20 ); //coordinates for left scroll arrow
const TInt KTemplateIndLeftPosY1PortQVGA( 176 );
const TInt KTemplateIndLeftPosX2PortQVGA( 20 );
const TInt KTemplateIndLeftPosY2PortQVGA( 196 );
const TInt KTemplateIndLeftPosX3PortQVGA( 10 );
const TInt KTemplateIndLeftPosY3PortQVGA( 186 );
const TInt KTemplateIndRightPosX1PortQVGA( 180 ); //coordinates for right scroll arrow
const TInt KTemplateIndRightPosY1PortQVGA( 176 );
const TInt KTemplateIndRightPosX2PortQVGA( 180 );
const TInt KTemplateIndRightPosY2PortQVGA( 196 );
const TInt KTemplateIndRightPosX3PortQVGA( 190 );
const TInt KTemplateIndRightPosY3PortQVGA( 186 );
const TInt KPortPaperWidthPortQVGA( 150 );     // portrait paper width
const TInt KportPaperHeightPortQVGA( 200 );    // portrait paper height
const TInt KPortPaperPositionXPortQVGA( 25 );  // portrait paper leftmost pos
const TInt KPortPaperPositionYPortQVGA( 0 );   // portrait paper top pos
const TInt KLandPaperWidthPortQVGA( 200 );     // landscape paper width
const TInt KLandPaperHeightPortQVGA( 150 );    // landscape paper height
const TInt KLandPaperPositionXPortQVGA( 0 );   // landscape paper leftmost pos
const TInt KLandPaperPositionYPortQVGA( 25 );  // landscape paper top pos
const TInt KPageNumberBoxPositionPortXQVGA( 128 ); // page number box leftmost pos
const TInt KPageNumberBoxSizePortXQVGA( 72 );      // page number box width
const TInt KPageNumberBoxSizePortYQVGA( 18 );      // page number box height

// Preview Pane LANDSCAPE QVGA
const TInt KPreviewPaneWidthLandscapeQVGA( 192 );   // pane width
const TInt KPreviewPaneHeightLandscapeQVGA( 192 );  // pane height
const TInt KPreviewPanePositionXLandscapeQVGA( 0 ); // leftmost pos of pane
const TInt KPreviewPanePositionYLandscapeQVGA( 0 ); // top pos of pane
const TInt KTemplateIndLeftPosX1LandscapeQVGA( 20 ); //coordinates for left scroll arrow
const TInt KTemplateIndLeftPosY1LandscapeQVGA( 168 );
const TInt KTemplateIndLeftPosX2LandscapeQVGA( 20 );
const TInt KTemplateIndLeftPosY2LandscapeQVGA( 188 );
const TInt KTemplateIndLeftPosX3LandscapeQVGA( 10 );
const TInt KTemplateIndLeftPosY3LandscapeQVGA( 178 );
const TInt KTemplateIndRightPosX1LandscapeQVGA( 172 ); //coordinates for right scroll arrow
const TInt KTemplateIndRightPosY1LandscapeQVGA( 168 );
const TInt KTemplateIndRightPosX2LandscapeQVGA( 172 );
const TInt KTemplateIndRightPosY2LandscapeQVGA( 188 );
const TInt KTemplateIndRightPosX3LandscapeQVGA( 182 );
const TInt KTemplateIndRightPosY3LandscapeQVGA( 178 );
const TInt KportPaperWidthLandscapeQVGA( 144 );     // portrait paper width
const TInt KportPaperHeightLandscapeQVGA( 192 );    // portrait paper height
const TInt KPortPaperPositionXLandscapeQVGA( 24 );  // portrait paper leftmost pos
const TInt KPortPaperPositionYLandscapeQVGA( 0 );   // portrait paper top pos
const TInt KLandPaperWidthLandscapeQVGA( 192 );    // landscape paper width
const TInt KLandPaperHeightLandscapeQVGA( 144 );    // landscape paper height
const TInt KLandPaperPositionXLandscapeQVGA( 0 );   // landscape paper leftmost pos
const TInt KLandPaperPositionYLandscapeQVGA( 24 );  // landscape paper top pos
const TInt KPageNumberBoxPositionXLandscapeQVGA( 124 ); // page number box leftmost pos
const TInt KPageNumberBoxSizeXLandscapeQVGA( 68 );      // page number box width
const TInt KPageNumberBoxSizeYLandscapeQVGA( 18 );      // page number box height

// Preview Pane LANDSCAPE
const TInt KPreviewPaneWidthLand( 288 );
const TInt KPreviewPaneHeightLand( 288 );
const TInt KPreviewPanePositionXLand( 0 );
const TInt KPreviewPanePositionYLand( 0 );
const TInt KTemplateIndLeftPosX1Land( 24 );
const TInt KTemplateIndLeftPosY1Land( 258 );
const TInt KTemplateIndLeftPosX2Land( 24 );
const TInt KTemplateIndLeftPosY2Land( 282 );
const TInt KTemplateIndLeftPosX3Land( 12 );
const TInt KTemplateIndLeftPosY3Land( 270 );
const TInt KTemplateIndRightPosX1Land( 264 );
const TInt KTemplateIndRightPosY1Land( 258 );
const TInt KTemplateIndRightPosX2Land( 264 );
const TInt KTemplateIndRightPosY2Land( 282 );
const TInt KTemplateIndRightPosX3Land( 276 );
const TInt KTemplateIndRightPosY3Land( 270 );
const TInt KPreviewPaneFontSizeLand( 12 );
const TInt KPortPaperWidthLand( 216 );
const TInt KPortPaperHeightLand( 288 );
const TInt KPortPaperPositionXLand( 36 );
const TInt KPortPaperPositionYLand( 0 );
const TInt KLandPaperWidthLand( 288 );
const TInt KLandPaperHeightLand( 216 );
const TInt KLandPaperPositionXLand( 0 );
const TInt KLandPaperPositionYLand( 36 );
const TInt KPageNumberBoxPositionLandX( 202 );
const TInt KPageNumberBoxSizeLandX( 86 );
const TInt KPageNumberBoxSizeLandY( 35 );

// Browser Pane PORTRAIT
const TInt KBrowserPaneWidthPort( 96 );
const TInt KBrowserPaneHeightPort( 256 );
const TInt KBrowserPanePositionXPort( 256 );
const TInt KBrowserPanePositionYPort( 0 );
const TInt KDefaultCursorFillPositionXPort( 264 );
const TInt KDefaultCursorFillPositionYPort( 20 );
const TInt KCursorFillWidthPort( 79 );
const TInt KCursorFillHeightPort( 69 );
const TInt KMaxCursorRightSidePort( 346 );
const TInt KMinCursorLeftSidePort( 60 );
const TInt KCursorStepPort( 72 ); //42
const TInt KDefaultThumbnailPositionXPort( 12 );
const TInt KDefaultThumbnailPositionYPort( 24 );
const TInt KThumbnailImgWidthPort( 72 ); // 72
const TInt KThumbnailImgHeightPort( 62 ); // 65
const TInt KCursorFillPenSizePort( 4 );
const TInt KCursorPenSizePort( 4 );
const TInt KMaxCursorTopPositionPort( 78 ); //check
const TInt KMaxCursorBottomPositionPort( 232 );

const TInt KImageIndUpPosX1Port( 292 );
const TInt KImageIndUpPosY1Port( 14 );
const TInt KImageIndUpPosX2Port( 316 );
const TInt KImageIndUpPosY2Port( 14 );
const TInt KImageIndUpPosX3Port( 304 );
const TInt KImageIndUpPosY3Port( 2 );
const TInt KImageIndDownPosX1Port( 292 );
const TInt KImageIndDownPosY1Port( 242 );
const TInt KImageIndDownPosX2Port( 316 );
const TInt KImageIndDownPosY2Port( 242 );
const TInt KImageIndDownPosX3Port( 304 );
const TInt KImageIndDownPosY3Port( 254 );

// Browser Pane PORTRAIT QVGA
const TInt KBrowserPaneWidthPortQVGA( 40 );         // browser pane width
const TInt KBrowserPaneHeightPortQVGA( 200 );       // browser pane height
const TInt KBrowserPanePositionXPortQVGA( 200 );    // browser pane leftmost pos
const TInt KBrowserPanePositionYPortQVGA( 0 );      // browser pane top pos

const TInt KDefaultCursorFillPositionXPortQVGA( 201 );  // cursor initial leftmost pos
const TInt KDefaultCursorFillPositionYPortQVGA( 43 );   // cursor initial top pos
const TInt KCursorFillWidthPortQVGA( 38 );              // cursor width
const TInt KCursorFillHeightPortQVGA( 33 );             // cursor height
const TInt KCursorStepPortQVGA( 40 );                   // height pos diff between thumbnails
const TInt KDefaultThumbnailPositionXPortQVGA( 1 );     // 1st thumbnail initial leftmost pos
const TInt KDefaultThumbnailPositionYPortQVGA( 45 );    // 1st thumbnail initial top pos
const TInt KThumbnailImgWidthPortQVGA( 38 );        // thumbnail image width
const TInt KThumbnailImgHeightPortQVGA( 29 );       // thumbnail image height
const TInt KCursorPenSizePortQVGA( 4 );             // cursor frame size
const TInt KMaxCursorTopPositionPortQVGA( 55 );     // cursor top limit
const TInt KMaxCursorBottomPositionPortQVGA( 155 ); // cursor bottom limit

const TInt KImageIndUpPosX1PortQVGA( 210 ); //coordinates for up scroll arrow
const TInt KImageIndUpPosY1PortQVGA( 25 );
const TInt KImageIndUpPosX2PortQVGA( 230 );
const TInt KImageIndUpPosY2PortQVGA( 25 );
const TInt KImageIndUpPosX3PortQVGA( 220 );
const TInt KImageIndUpPosY3PortQVGA( 15 );
const TInt KImageIndDownPosX1PortQVGA( 210 ); //coordinates for down scroll arrow
const TInt KImageIndDownPosY1PortQVGA( 175 );
const TInt KImageIndDownPosX2PortQVGA( 230 );
const TInt KImageIndDownPosY2PortQVGA( 175 );
const TInt KImageIndDownPosX3PortQVGA( 220 );
const TInt KImageIndDownPosY3PortQVGA( 185 );

// Browser Pane LANDSCAPE QVGA
const TInt KBrowserPaneWidthLandscapeQVGA( 128 );       // browser pane width
const TInt KBrowserPaneHeightLandscapeQVGA( 192 );      // browser pane height
const TInt KBrowserPanePositionXLandscapeQVGA( 192 );   // browser pane leftmost pos
const TInt KBrowserPanePositionYLandscapeQVGA( 0 );     // browser pane top pos
const TInt KDefaultCursorFillPositionXLandscapeQVGA( 216 );// cursor initial leftmost pos
const TInt KDefaultCursorFillPositionYLandscapeQVGA( 3 );   // cursor initial top pos
const TInt KCursorFillWidthLandscapeQVGA( 79 );             // cursor width
const TInt KCursorFillHeightLandscapeQVGA( 60 );            // cursor height
const TInt KCursorStepLandscapeQVGA( 61 );                  // height pos diff between thumbnails
const TInt KDefaultThumbnailPositionXLandscapeQVGA( 26 );   // 1st thumbnail initial leftmost pos
const TInt KDefaultThumbnailPositionYLandscapeQVGA( 5 );    // 1st thumbnail initial top pos
const TInt KThumbnailImgWidthLandscapeQVGA( 77 );           // thumbnail image width
const TInt KThumbnailImgHeightLandscapeQVGA( 58 );          // thumbnail image height
const TInt KCursorPenSizeLandscapeQVGA( 4 );                // cursor frame size
const TInt KMaxCursorTopPositionLandscapeQVGA( 10 );        // cursor top limit
const TInt KMaxCursorBottomPositionLandscapeQVGA( 155 );    // cursor bottom limit
const TInt KImageIndUpPosX1LandscapeQVGA( 199 ); //coordinates for up scroll arrow
const TInt KImageIndUpPosY1LandscapeQVGA( 30 );
const TInt KImageIndUpPosX2LandscapeQVGA( 209 );
const TInt KImageIndUpPosY2LandscapeQVGA( 30 );
const TInt KImageIndUpPosX3LandscapeQVGA( 204 );
const TInt KImageIndUpPosY3LandscapeQVGA( 20 );
const TInt KImageIndDownPosX1LandscapeQVGA( 199 ); //coordinates for down scroll arrow
const TInt KImageIndDownPosY1LandscapeQVGA( 172 );
const TInt KImageIndDownPosX2LandscapeQVGA( 209 );
const TInt KImageIndDownPosY2LandscapeQVGA( 172 );
const TInt KImageIndDownPosX3LandscapeQVGA( 204 );
const TInt KImageIndDownPosY3LandscapeQVGA( 182 );

// Browser Pane LANDSCAPE
const TInt KBrowserPaneWidthLand( 128 );
const TInt KBrowserPaneHeightLand( 288 );
const TInt KBrowserPanePositionXLand( 288 );
const TInt KBrowserPanePositionYLand( 0 );
const TInt KDefaultCursorFillPositionXLand( 298 );
const TInt KDefaultCursorFillPositionYLand( 21 );
const TInt KCursorFillWidthLand( 108 );
const TInt KCursorFillHeightLand( 82 );
const TInt KMaxCursorRightSideLand( 346 );
const TInt KMinCursorLeftSideLand( 60 );
const TInt KCursorStepLand( 82 ); //42
const TInt KDefaultThumbnailPositionXLand( 14 );
const TInt KDefaultThumbnailPositionYLand( 25 );
const TInt KThumbnailImgWidthLand( 100 );
const TInt KThumbnailImgHeightLand( 74 );
const TInt KCursorFillPenSizeLand( 4 );
const TInt KCursorPenSizeLand( 4 );
const TInt KMaxCursorTopPositionLand( 78 ); //check
const TInt KMaxCursorBottomPositionLand( 232 );

const TInt KImageIndUpPosX1Land( 340 );
const TInt KImageIndUpPosY1Land( 14 );
const TInt KImageIndUpPosX2Land( 364 );
const TInt KImageIndUpPosY2Land( 14 );
const TInt KImageIndUpPosX3Land( 352 );
const TInt KImageIndUpPosY3Land( 2 );
const TInt KImageIndDownPosX1Land( 340 );
const TInt KImageIndDownPosY1Land( 274 );
const TInt KImageIndDownPosX2Land( 364 );
const TInt KImageIndDownPosY2Land( 274 );
const TInt KImageIndDownPosX3Land( 352 );
const TInt KImageIndDownPosY3Land( 286 );

// Browser pane general
const TInt KMaxTemplatesOnScreen( 3 ); // max image thumbnails to see in browse pane
const TInt KNoBrowserIndicators( 0 );
const TInt KLeftBrowserIndicator( 1 );
const TInt KRightBrowserIndicator( 2 );
const TInt KBothBrowserIndicators( 3 );
const TInt KCursorPositionOne( 1 );
const TInt KCursorPositionTwo( 2 );
const TInt KCursorPositionThree( 3 );
const TInt KCursorFillColorR( 102 );
const TInt KCursorFillColorG( 153 );
const TInt KCursorFillColorB( 255 );
const TInt KBrowserPaneCursorUp( -1 );
const TInt KBrowserPaneCursorDown( 1 );
const TInt KBrowserPaneCursorLeft( -1 );
const TInt KBrowserPaneCursorRight( 1 );

//Info Pane - PORTRAIT
const TInt KInfoPaneWidth(352);
const TInt KInfoPaneHeight(32);
const TInt KInfoPanePositionX( 0 );
const TInt KInfoPanePositionY( 256 );

//Info Pane - PORTRAIT QVGA
const TInt KInfoPaneWidthQVGA( 240 );       // info pane width
const TInt KInfoPaneHeightQVGA( 24 );       // info pane height
const TInt KInfoPanePositionXQVGA( 0 );     // info pane leftmost pos
const TInt KInfoPanePositionYQVGA( 200 );   // info pane top pos

//Tooltip - GENERAL
const TInt KTooltipDelay( 3000000 );
const TUint32 KTooltipTransparency( 0x333333 );
const TInt KNumberInputInterval( 750000 );

//Tooltip - PORTRAIT
const TInt KTooltipWidthPort( 288 );
const TInt KTooltipHeightPort( 226 );
const TInt KTooltipPositionXPort( 32 );
const TInt KTooltipPositionYPort( 15 );
const TInt KTooltipPaddingPort( 10 );

//Tooltip - PORTRAIT QVGA
const TInt KTooltipWidthPortQVGA( 200 );
const TInt KTooltipHeightPortQVGA( 170 );
const TInt KTooltipPositionXPortQVGA( 20 );
const TInt KTooltipPositionYPortQVGA( 15 );
const TInt KTooltipPaddingPortQVGA( 10 );

//Tooltip - LANDSCAPE
const TInt KTooltipWidthLand( 352 );
const TInt KTooltipHeightLand( 226 );
const TInt KTooltipPositionXLand( 32 );
const TInt KTooltipPositionYLand( 31 );
const TInt KTooltipPaddingLand( 10 );

//Tooltip - LANDSCAPE QVGA
const TInt KTooltipWidthLandQVGA( 240 );
const TInt KTooltipHeightLandQVGA( 170 );
const TInt KTooltipPositionXLandQVGA( 35 );
const TInt KTooltipPositionYLandQVGA( 11 );
const TInt KTooltipPaddingLandQVGA( 10 );

//SliderQuery - GENERAL
const TUint32 KSliderQueryTransparency( 0x333333 );

//SliderQuery - PORTRAIT
const TInt KSliderQueryWidthPort( 352 );
const TInt KSliderQueryHeightPort( 128 );
const TInt KSliderQueryPositionXPort( 0 );
const TInt KSliderQueryPositionYPort( 160 );

const TInt KSliderQueryButtonLeftXPort( 10 );
const TInt KSliderQueryButtonLeftYPort( 75 );
const TInt KSliderQueryBarXPort( 40 );
const TInt KSliderQueryBarYPort( 84 );
const TInt KSliderQueryButtonRightXPort( 322 );
const TInt KSliderQueryButtonRightYPort( 75 );

const TInt KSliderQueryButtonSizeXPort( 20 );
const TInt KSliderQueryButtonSizeYPort( 20 );
const TInt KSliderQueryBarSizeXPort( 272 );
const TInt KSliderQueryBarSizeYPort( 2 );
const TInt KSliderQueryHandleSizeXPort( 42 );
const TInt KSliderQueryHandleSizeYPort( 42 );

const TInt KSliderQueryTitlePosXPort( 10 );
const TInt KSliderQueryTitlePosYPort( 0 );
const TInt KSliderQueryTitleSizXPort( 282 );
const TInt KSliderQueryTitleSizYPort( 64 );

const TInt KSliderQueryHandlePositionYPort( 64 );

//SliderQuery - PORTRAIT QVGA
const TInt KSliderQueryWidthPortQVGA( 240 );
const TInt KSliderQueryHeightPortQVGA( 100 );
const TInt KSliderQueryPositionXPortQVGA( 0 );
const TInt KSliderQueryPositionYPortQVGA( 124 );

const TInt KSliderQueryButtonLeftXPortQVGA( 8 );
const TInt KSliderQueryButtonLeftYPortQVGA( 59 );
const TInt KSliderQueryBarXPortQVGA( 25 );
const TInt KSliderQueryBarYPortQVGA( 65 );
const TInt KSliderQueryButtonRightXPortQVGA( 218 );
const TInt KSliderQueryButtonRightYPortQVGA( 59 );

const TInt KSliderQueryButtonSizeXPortQVGA( 14 );
const TInt KSliderQueryButtonSizeYPortQVGA( 14 );
const TInt KSliderQueryBarSizeXPortQVGA( 190 );
const TInt KSliderQueryBarSizeYPortQVGA( 2 );
const TInt KSliderQueryHandleSizeXPortQVGA( 32 );
const TInt KSliderQueryHandleSizeYPortQVGA( 32 );

const TInt KSliderQueryTitlePosXPortQVGA( 8 );
const TInt KSliderQueryTitlePosYPortQVGA( 0 );
const TInt KSliderQueryTitleSizXPortQVGA( 190 );
const TInt KSliderQueryTitleSizYPortQVGA( 50 );

const TInt KSliderQueryHandlePositionYPortQVGA( 50 );

//SliderQuery - LANDSCAPE
const TInt KSliderQueryWidthLand( 416 );
const TInt KSliderQueryHeightLand( 144 );
const TInt KSliderQueryPositionXLand( 0 );
const TInt KSliderQueryPositionYLand( 144 );

const TInt KSliderQueryButtonLeftXLand( 19 );
const TInt KSliderQueryButtonLeftYLand( 86 );
const TInt KSliderQueryBarXLand( 58 );
const TInt KSliderQueryBarYLand( 95 );
const TInt KSliderQueryButtonRightXLand( 377 );
const TInt KSliderQueryButtonRightYLand( 86 );

const TInt KSliderQueryButtonSizeXLand( 20 );
const TInt KSliderQueryButtonSizeYLand( 20 );
const TInt KSliderQueryBarSizeXLand( 299 );
const TInt KSliderQueryBarSizeYLand( 2 );
const TInt KSliderQueryHandleSizeXLand( 42 );
const TInt KSliderQueryHandleSizeYLand( 42 );

const TInt KSliderQueryTitlePosXLand( 19 );
const TInt KSliderQueryTitlePosYLand( 0 );
const TInt KSliderQueryTitleSizXLand( 299 );
const TInt KSliderQueryTitleSizYLand( 73 );

const TInt KSliderQueryHandlePositionYLand( 73 );

//SliderQuery - LANDSCAPE QVGA
const TInt KSliderQueryWidthLandQVGA( 320 );
const TInt KSliderQueryHeightLandQVGA( 96 );
const TInt KSliderQueryPositionXLandQVGA( 0 );
const TInt KSliderQueryPositionYLandQVGA( 96 );

const TInt KSliderQueryButtonLeftXLandQVGA( 13 );
const TInt KSliderQueryButtonLeftYLandQVGA( 57 );
const TInt KSliderQueryBarXLandQVGA( 40 );
const TInt KSliderQueryBarYLandQVGA( 63 );
const TInt KSliderQueryButtonRightXLandQVGA( 293 );
const TInt KSliderQueryButtonRightYLandQVGA( 57 );

const TInt KSliderQueryButtonSizeXLandQVGA( 14 );
const TInt KSliderQueryButtonSizeYLandQVGA( 14 );
const TInt KSliderQueryBarSizeXLandQVGA( 240 );
const TInt KSliderQueryBarSizeYLandQVGA( 2 );
const TInt KSliderQueryHandleSizeXLandQVGA( 32 );
const TInt KSliderQueryHandleSizeYLandQVGA( 32 );

const TInt KSliderQueryTitlePosXLandQVGA( 13 );
const TInt KSliderQueryTitlePosYLandQVGA( 0 );
const TInt KSliderQueryTitleSizXLandQVGA( 240 );
const TInt KSliderQueryTitleSizYLandQVGA( 48 );

const TInt KSliderQueryHandlePositionYLandQVGA( 48 );

// GENERAL
const TInt KScreenModePortrait = 0;
const TInt KScreenModeLandscape = 1;
const TInt KScreenModePortraitQVGA = 2;
const TInt KScreenModeLandscapeQVGA = 3;

const TInt KScreenWidthPortrait = 352;
const TInt KScreenWidthLandscape = 416;
const TInt KScreenWidthPortraitQVGA = 240;
const TInt KScreenWidthLandscapeQVGA = 320;

#endif // IMAGEPRINTCONSTS_H

//  End of File
