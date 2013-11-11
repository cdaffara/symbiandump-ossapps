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
* Description:
*
*/


#include "cxevideocontainer.h"
#include "cxutils.h"


CxeVideoContainer::CxeVideoContainer()
{
}

CxeVideoContainer::~CxeVideoContainer()
{
}

void CxeVideoContainer::ConstructL()
{
    CreateWindowL();
    ActivateL();
}

void CxeVideoContainer::Draw( const TRect& /*aRect*/ ) const
{
    CX_DEBUG_IN_FUNCTION();

    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    gc.SetBrushColor(TRgb::Color16MA(0));
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(Rect());
}


// end of file
