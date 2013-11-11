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
* Description:  hbpopup stub
*
*/

#ifndef HBPOPUP_H
#define HBPOPUP_H

#include "hbwidget.h"

class HbPopup : public HbWidget
{
    Q_OBJECT
 
public:

    HbPopup(QGraphicsItem *parent = 0) : HbWidget(parent){}
    
    enum Placement
    {
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner,
        TopEdgeCenter,
        RightEdgeCenter,
        BottomEdgeCenter,
        LeftEdgeCenter,
        Center
    };
        
    enum DismissPolicy
    {
        NoDismiss   = 0,
        TapInside   = 1,
        TapOutside  = 2,
        TapAnywhere = TapInside | TapOutside
    };
    enum DefaultTimeout
    {
        NoTimeout,
        ConfirmationNoteTimeout,
        StandardTimeout,
        ContextMenuTimeout,
    };
        
    
};

#endif

