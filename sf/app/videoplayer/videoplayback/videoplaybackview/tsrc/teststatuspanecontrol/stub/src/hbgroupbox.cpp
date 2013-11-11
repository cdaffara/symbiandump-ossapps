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
* Description:  Implementation of HbGroupBox
*
*/

// Version : %version:   1 %


#include "hbgroupbox.h"

// -------------------------------------------------------------------------------------------------
// HbGroupBox::HbGroupBox
// -------------------------------------------------------------------------------------------------
//
HbGroupBox::HbGroupBox()
{
}

// -------------------------------------------------------------------------------------------------
// HbGroupBox::~HbGroupBox
// -------------------------------------------------------------------------------------------------
//
HbGroupBox::~HbGroupBox()
{
}

// -------------------------------------------------------------------------------------------------
// HbGroupBox::setVisible
// -------------------------------------------------------------------------------------------------
//
void HbGroupBox::setVisible( bool visible )
{
    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// HbGroupBox::isVisible
// -------------------------------------------------------------------------------------------------
//
bool HbGroupBox::isVisible()
{
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// HbGroupBox::setHeading
// -------------------------------------------------------------------------------------------------
//
void HbGroupBox::setHeading( QString string )
{
    mString = string;
}

// End of file
