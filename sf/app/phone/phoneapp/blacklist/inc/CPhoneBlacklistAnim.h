/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It defines window animation service, which provides service
*                to clear blacklist upon next key event.
*
*/



#ifndef CPHONEBLACKLISTANIM_H
#define CPHONEBLACKLISTANIM_H

// INCLUDES
#include    <w32adll.h>        
#include    <etel.h>           
#include    <etelmm.h>         
#include    <rmmcustomapi.h>   

// CLASS DECLARATION

/**
* It is blacklist window animation class.
*
* @since 2.0 
*/
class CPhoneBlacklistAnim 
    : public CWindowAnim
    {
    public: // Constructors and destructors
        
        /** 
        * Constructor. 
        */
        CPhoneBlacklistAnim();
        
        /** 
        * Destructor.
        */
        ~CPhoneBlacklistAnim();
        
    public: // From base classes

        /** 
        * From CWindowAnim, handles commands.
        * @param aOpcode command.
        * @param aArgs parameters.
        * @return error code.
        */
        virtual TInt CommandReplyL( 
            TInt aOpcode, 
            TAny* aArgs );
        
        /** 
        * From CWindowAnim, handles commands. 
        * @param aOpcode command.
        * @param aArgs parameters.
        */
        virtual void Command(
            TInt aOpcode, 
            TAny* aArgs );
        
        /** 
        * From CWindowAnim, animate.
        * @param aDateTime NULL or current time.
        */
        virtual void Animate(
            TDateTime* aDateTime );
        
        /** 
        * From CWindowAnim, second phase constructor.
        * @param aArgs parameters.
        * @param aHasFocus focus information.
        */
        virtual void ConstructL(
            TAny* aArgs, 
            TBool aHasFocus );

        /** 
        * From CWindowAnim, handles redraw request.  
        */
        virtual void Redraw();

        /** 
        * From CWindowAnim, handles focus changes.
        *
        * @param new focus state.
        */
        virtual void FocusChanged(
            TBool aState );
        
        /** 
        * From CWindowAnim, handles keyevents. 
        *
        * @param aRawEvent event.
        * @return ETrue if event is consumed.
        */
        virtual TBool OfferRawEvent(
            const TRawEvent& aRawEvent );
        
    private:
        
        // inner class to clear blacklist.
        class CClearBlacklist;

        // ETel session.
        /*****************************************************
        *    Series 60 Customer / ETel
        *    Series 60  ETel API
        *****************************************************/
        RTelServer iServer;
        
        // Mobile phone session.
        /*****************************************************
        *    Series 60 Customer / ETel
        *    Series 60  ETel API
        *****************************************************/
        RMobilePhone iPhone;

        // Custom API session.
        /*****************************************************
        *    Series 60 Customer / ETel
        *    Series 60  ETel API
        *****************************************************/
        RMmCustomAPI iCustom;

        // If set true then next key event should cause black
        // list to be cleared.
        TBool iClearBlacklistOnNextEvent;

        // Owned blacklist clearer.
        CClearBlacklist* iActive;

    };

#endif // CPHONEBLACKLISTANIM_H

// End Of File
