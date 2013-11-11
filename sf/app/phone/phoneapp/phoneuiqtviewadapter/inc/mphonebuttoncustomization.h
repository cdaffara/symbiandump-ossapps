/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class for touch button customization.
*
*/

#ifndef MPHONEBUTTONCUSTOMIZATION_H
#define MPHONEBUTTONCUSTOMIZATION_H

// CLASS DECLARATION
/**
*  Interface class for touch button customization.
*/
class MPhoneButtonCustomization
    {
    
public:
    /**
    * Customizes touch pane buttons.
    * @param aTouchPane Interface to add and dimm
    *                   bubble touch pane buttons.
    */
    virtual void CustomizeTouchPaneButtons() = 0;
    };
    
#endif // MPHONEBUTTONCUSTOMIZATION_H
    
