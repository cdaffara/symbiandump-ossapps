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
* Description:  Interface class for view blocking dialogs.
*
*/


#ifndef MPHONEVIEWBLOCKINGDIALOGOBSERVER_H
#define MPHONEVIEWBLOCKINGDIALOGOBSERVER_H


// CLASS DECLARATION
/**
*  Interface class for view blocking dialogs.
*/
class MPhoneViewBlockingDialogObserver
    {
public:
    /**
    * Sets the value is blocking dialogs in screen.
    * @param aBlockingDialog ETrue if blocking dialog is
    *                        displayed.
    */
    virtual void SetBlockingDialogIsDisplayed( TBool aBlockingDialog ) = 0;
    

    };
    
#endif // MPHONEVIEWBLOCKINGDIALOGOBSERVER_H
