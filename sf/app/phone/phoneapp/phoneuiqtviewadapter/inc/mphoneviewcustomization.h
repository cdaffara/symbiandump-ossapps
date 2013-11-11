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
* Description:  Interface class for view control customization.
*
*/


#ifndef MPHONEVIEWCUSTOMIZATION_H
#define MPHONEVIEWCUSTOMIZATION_H

#include "phoneviewcommanddefinitions.h"
#include "tphonecommandparam.h"
#include "mphoneviewblockingdialogobserver.h"

// CLASS DECLARATION
/**
*  Interface class for view control customization.
*/
class MPhoneViewCustomization
    {
public:
    /**
    * Handles custom view commands.
    * @param aBlockingObserver Blocking dialog observer. Sets blocking dialog 
    *                          value (by this param) for view controller when
    *                          handle command needs to show blocking
    *                          dialogs on the screen.
    * @param aCmdId Command id.
    * @param aCommandParam Command param.
    * @return ETrue if command handled.
    */
    virtual TBool HandleCommandL(
                    MPhoneViewBlockingDialogObserver& aBlockingObserver,
                    TPhoneViewCommandId aCmdId,
                    TPhoneCommandParam* aCommandParam ) = 0;
    

    /**
    * Removes dialogs.
    */
    virtual void RemoveDialog() = 0;

    };
    
#endif // MPHONEVIEWCUSTOMIZATION_H
    
