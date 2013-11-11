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
* Description: stub hblabel
* 
*/

#include "hblabel.h"

int HbLabel::initializeCount = 0;

// ---------------------------------------------------------------------------
// setPlainText
// ---------------------------------------------------------------------------
//
void HbLabel::setPlainText(QString txt)
{
    mPlainTxt = txt;
}

// ---------------------------------------------------------------------------
// setAlignment
// ---------------------------------------------------------------------------
//
void HbLabel::setAlignment(Qt::AlignmentFlag alignment)
{
    mAlignment = alignment;
}

// ---------------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------------
//
void HbLabel::clear()
{
    mPlainTxt = "";   
}

