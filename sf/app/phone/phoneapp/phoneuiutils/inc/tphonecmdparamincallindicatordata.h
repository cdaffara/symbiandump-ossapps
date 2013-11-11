/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMINCALLINDICATORDATA_H
#define __TPHONECMDPARAMINCALLINDICATORDATA_H

//  INCLUDES

#include <w32std.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for call header information.
*/
class TPhoneCmdParamIncallIndicatorData : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamIncallIndicatorData();

    public: 
        
        /**
        * Sets the incall indicator call state.
        * @param aCallState is the call state
        */
        IMPORT_C void SetCallState( TInt aCallState );
       
        /**
        * Sets the incall indicator mode.
        * @param aMode is the mode status
        */
        IMPORT_C void SetMode( TInt aMode );
       
        /**
        * Sets the incall indicator mute status.
        * @param aMute is the mute status
        */
        IMPORT_C void SetMute( TBool aMute );

        /**
        * Sets the incall indicator ciphering status.
        * @param aCiphering is ETrue if ciphering is on.
        */
        IMPORT_C void SetCiphering( TBool aCiphering );

        /**
        * Sets the incall indicator emergency status.
        * @param aEmergency is ETrue if emergency mode is on.
        */
        IMPORT_C void SetEmergency( TBool aEmergency );

        /**
        * Sets the incall indicator line2 status.
        * @param aLine2 is the line2 status
        */
        IMPORT_C void SetLine2( TBool aLine2 );
        
        /**
        * Returns the incall indicator call state.
        * @return Returns the call state
        */
        IMPORT_C TInt CallState() const;

        /**
        * Returns the incall indicator mode.
        * @return Returns the call state
        */
        IMPORT_C TInt Mode() const;

        /**
        * Returns the incall indicator mute status.
        * @return Returns ETrue if mute is on 
        */
        IMPORT_C TBool Mute() const;

        /**
        * Returns the incall indicator ciphering status.
        * @return Returns ETrue if ciphering is on 
        */
        IMPORT_C TBool Ciphering() const;

        /**
        * Returns the incall indicator emergency status.
        * @return Returns ETrue if emergency mode is on 
        */
        IMPORT_C TBool Emergency() const;
        
        /**
        * Returns the incall indicator line2 status.
        * @return Returns ETrue if call is in line2 
        */
        IMPORT_C TBool Line2() const;
        
        /**
        * Sets the little bubble visible or hidden
        * @param aVisible EFalse to prevent little bubble from showing
        *                 ETrue to set visible
        */
        IMPORT_C void SetLittleBubbleVisible( TBool aVisible );
        
        /**
        * Returns the little bubble visible status
        * @return Returns EFalse when little bubble is wanted to be hidden
        *         ETrue to be visible
        */
        IMPORT_C TBool LittleBubbleVisible() const;

        /**
        * Sets the ciphering indicator allowed or hidden
        * @param aAllowed EFalse to prevent ciphering indicator from showing
        *                 ETrue to enable
        */
        IMPORT_C void SetCipheringIndicatorAllowed( TBool aAllowed );
        
        /**
        * Returns the ciphering indicator allowed status
        * @return Returns EFalse when ciphering indicator is wanted to be disabled
        *         ETrue to be allowed
        */
        IMPORT_C TBool CipheringIndicatorAllowed() const;
        
    private:    
        
        /**
        * Incall indicator call state
        */
        TInt iCallState;

        /**
        * Incall indicator mode
        */
        TInt iMode;

        /**
        * Incall indicator mute status
        */
        TBool iMute;

        /**
        * Incall indicator ciphering status
        */
        TBool iCiphering;

        /**
        * Incall indicator emergency status
        */
        TBool iEmergency;
        
        /**
        * Incall indicator line2 status
        */
        TBool iLine2;
        
        /**
        * Should the little bubble be visible or not
        */
        TBool iLittleBubbleVisible;

        /**
        * Should the ciphering indicator be visible or not
        */
        TBool iCipheringIndicatorAllowed;
        
    };

#endif // __TPHONECMDPARAMINCALLINDICATORDATA_H   
            
// End of File
