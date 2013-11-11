/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for MTelDMAccessory class.
*                
*
*/


#ifndef MTELDMACCESSORY_H
#define MTELDMACCESSORY_H

// INCLUDES

//CLASS DECLARATION

/**
* Accessory mode 
*
*  @since 3.1
*/
class MTelDMAccessory 
    {
 
    public: // New functions
 
        /**
        * Is accessory attached ?
        * @param  none
        * @return True if accessory attached.
        */
        virtual TBool IsAccessoryAttached() = 0;
        
        /**
        * Is any active accessory ? 
        * @param  none
        * @return True if active accessory.
        */
        virtual TBool IsAnyActiveAccessory() = 0;

     };

#endif      // MTELDMACCESSORY_H
            
// End of File
