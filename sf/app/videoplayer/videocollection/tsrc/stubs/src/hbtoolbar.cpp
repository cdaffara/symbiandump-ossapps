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

#include "hbtoolbar.h"

int HbToolBar::mAddExtensionCallCount = 0;

HbToolBar::HbToolBar( QGraphicsItem *parent) 
 : HbWidget(parent), mToolBarExtensionAction( 0 )
{
    
}

HbToolBar::~HbToolBar()
{
    
}

HbAction *HbToolBar::addExtension( HbToolBarExtension *extension )
{
    Q_UNUSED(extension);
    mAddExtensionCallCount++;
    
    if(mToolBarExtensionAction)
    {
        delete mToolBarExtensionAction;
    }
    mToolBarExtensionAction = new HbAction();
    
    return mToolBarExtensionAction; 
}

