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


#ifndef TPHONECMDPARAMRINGTONE_H
#define TPHONECMDPARAMRINGTONE_H

//  INCLUDES

#include "tphonecommandparam.h"

// DATA TYPES

enum TPhoneRingToneType
    {
    EPhoneRingToneProfile,
    EPhoneRingTonePersonal
    };

// CLASS DECLARATION

/**
*  A parameter class for ring tone information.
*/
class TPhoneCmdParamRingTone : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamRingTone();

    public: 
        
        /**
        * Sets the ring tone type
        * @param aRingToneType is the ring tone type
        */
        IMPORT_C void SetType( const TPhoneRingToneType aType );

        /**
        * Sets the ring tone volume
        * @param aVolume is the ring tone volume
        */
        IMPORT_C void SetVolume( TInt aVolume );
       
        /**
        * Sets the ring tone ringing type.
        * @param aRingingType is the ring tone ringing type
        */
        IMPORT_C void SetRingingType( TInt aRingingType );
       
        /**
        * Sets the ring tone 
        * @param aRingTone is the ring tone
        */
        IMPORT_C void SetRingTone( const TDesC& aRingTone );

        /**
        * Sets the text to say
        * @param aTextToSay is the text to say
        */
        IMPORT_C void SetTextToSay( const TDesC& aTextToSay );

        /**
        * Returns the ring tone type
        * @return Returns the ring tone type
        */
        IMPORT_C TPhoneRingToneType Type() const;

        /**
        * Returns the ring tone volume
        * @return Returns the ring tone volume
        */
        IMPORT_C TInt Volume() const;

        /**
        * Returns the ring tone ringing type.
        * @return Returns the ring tone ringing type 
        */
        IMPORT_C TInt RingingType() const;

        /**
        * Returns the ring tone.
        * @return Returns the ring tone
        */
        IMPORT_C const TDesC& RingTone() const;

        /**
        * Returns the text to say.
        * @return Returns the text to say
        */
        IMPORT_C const TDesC& TextToSay() const;
        
        /**
        * Sets status of caller text
        * @param aStatus, caller text status 
        */
        IMPORT_C void SetCallerTextStatus( TBool aStatus );

        /**
        * Returns status of caller text
        * @return Returns ETrue if used
        */
        IMPORT_C TBool IsCallerText();

        /**
        * Sets status of caller image
        * @param aStatus, caller image status
        */
        IMPORT_C void SetCallerImageStatus( TBool aStatus );

        /**
        * Returns status of caller image
        * @return Returns ETrue if used
        */
        IMPORT_C TBool IsCallerImage();

    private:    
        
        /**
        * Ring tone
        */
        TPtrC iRingTone;

        /**
        * Ring tone text to say
        */
        TPtrC iTextToSay;

        /**
        * Ring tone type
        */
        TPhoneRingToneType iType;

        /**
        * Ring tone volume
        */
        TInt iVolume;

        /**
        * Ring tone ringing type
        */
        TInt iRingingType;

        /**
        * Status of caller text
        */
        TBool iIsCallerText;
        
        /**
        * Status of caller image
        */
        TBool iIsCallerImage;
    };

#endif // __TPHONECMDPARAMCALLHEADERDATA_H   
            
// End of File
