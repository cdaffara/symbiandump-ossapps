/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains template tags and sections used to handle XHTML template file.
*
*/


#ifndef XFCTEMPLATES_H
#define XFCTEMPLATES_H


#include "cxhtmlfilecomposer.h"

	_LIT( KXHTMLTemplate, "z:\\resource\\imageprintdata\\protocols\\xhtmltempl\\xhtmlfiletemplate.txt" );

_LIT8(KPageWidth, "<<pageWidth>>");
_LIT8(KPageHeight, "<<pageHeight>>");
_LIT8(KPageMargin, "<<pageMargin>>");

_LIT8(KImgWidth, "<<imgWidth>>");
_LIT8(KImgHeight, "<<imgHeight>>");
_LIT8(KImgMargin, "<<imgMargin>>");
_LIT8(KImgOrientation, "<<imgOrientation>>");

_LIT8(KPositionTop, "<<positionTop>>");
_LIT8(KPositionLeft, "<<positionLeft>>");

_LIT8(KImgFrameWidth, "<<imgFrameWidth>>");
_LIT8(KImgFrameHeight, "<<imgFrameHeight>>");

_LIT8(KId, "<<id>>");
_LIT8(KPosId, "<<posId>>");
_LIT8(KDetId, "<<detId>>");

_LIT8(KUnit, "<<unit>>");
_LIT8(KTop, "top:");
_LIT8(KLeft, "left:");

_LIT8(KDeg, "%ddeg");

_LIT8(KFramePosition, "<<framePosition>>");
_LIT8(KPositionStart, "<<positionStart>>");
_LIT8(KPositionEnd, "<<positionEnd>>");

_LIT8(KImgDetails, "<<imageDetails>>");
_LIT8(KImgStart, "<<imgStart>>");
_LIT8(KImgEnd, "<<imgEnd>>");

_LIT8(KEndTag, "</html>");

_LIT8(KPage, "<<page>>");

_LIT8(KPageType, "<<pageType>>");
_LIT8(KPageFirsts, "page");
_LIT8(KPageLast, "page_last");

_LIT8(KPageStart, "<<pageStart>>");
_LIT8(KPageEnd, "<<pageEnd>>");

_LIT8(KFrame, "<<frame>>");
_LIT8(KFrameStart, "<<frameStart>>");
_LIT8(KFrameEnd, "<<frameEnd>>");

_LIT8(KImgName, "<<imgName>>");


#endif // XFCTEMPLATES_H

//  End of File
