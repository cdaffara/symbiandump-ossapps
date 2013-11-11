/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HbLabel
*
*/

// Version : %version:  2 %


#include "hblabel.h"

// -------------------------------------------------------------------------------------------------
// HbLabel::HbLabel
// -------------------------------------------------------------------------------------------------
//
HbLabel::HbLabel()
{
}

// -------------------------------------------------------------------------------------------------
// HbLabel::~HbLabel
// -------------------------------------------------------------------------------------------------
//
HbLabel::~HbLabel()
{
}

// -------------------------------------------------------------------------------------------------
// HbLabel::setVisible
// -------------------------------------------------------------------------------------------------
//
void HbLabel::setVisible( bool visible )
{
    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// HbLabel::isVisible
// -------------------------------------------------------------------------------------------------
//
bool HbLabel::isVisible()
{
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// HbLabel::setPlainText
// -------------------------------------------------------------------------------------------------
//
void HbLabel::setPlainText( QString string )
{
    mString = string;
}

// End of file
