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
* Description:  Parameter class for custom dialer creation
*
*/


#ifndef TPHONECMDPARAMCUSTOMDIALER_H_
#define TPHONECMDPARAMCUSTOMDIALER_H_

//  INCLUDES
#include "tphonecommandparam.h"

class MPhoneDialerController;

// CLASS DECLARATION

/**
*  A concrete parameter class for one integer value.
*  
*/
class TPhoneCmdParamCustomDialer : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamCustomDialer();

    public: 
        
        /**
        * Sets the custom dialer 
        * @param aDialerController Reference to custom dialer controller
        */
        IMPORT_C void SetDialerController( MPhoneDialerController* aDialerController );

        /**
        * Returns pointer to custom dialer controller
        */
        IMPORT_C MPhoneDialerController* DialerController();

    private:   

        MPhoneDialerController* iCustomDialer;
    };


#endif /*TPHONECMDPARAMCUSTOMDIALER_H_*/

