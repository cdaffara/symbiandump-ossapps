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


#ifndef __TPHONECMDPARAMNOTE_H
#define __TPHONECMDPARAMNOTE_H

//  INCLUDES

#include <w32std.h>
#include <aknnotedialog.h> 
#include "tphonecommandparam.h"

// DATA TYPES

/**
* Enumerates note types
*
* EPhoneNoteError - error note
* EPhoneNoteWarning - warning note
* EPhoneNoteInformation - information note
* EPhoneNoteConfirmation - confirmation note
* EPhoneNoteCustom - custom note
* EPhoneNotePermanent - custom permanent note
* EPhoneNoteDtmfSending - Dtmf sending note
* EPhoneNoteSecurity - security note
*/
enum TPhoneNoteType
{
    EPhoneNoteError,
    EPhoneNoteWarning,
    EPhoneNoteInformation,
    EPhoneNoteConfirmation,
    EPhoneNoteCustom,
    EPhoneNotePermanent,
    EPhoneNoteDtmfSending,
    EPhoneNoteSecurity,
    EPhoneNoteUIDisabled
};

// CLASS DECLARATION

/**
*  A parameter class for note dialog information.
*/
class TPhoneCmdParamNote : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamNote();

    public: 
        
        /**
        * Sets the note type
        * @param aType is the note type
        */
        IMPORT_C void SetType( TPhoneNoteType aType );

        /**
        * Sets the resource ID for the note
        * @param aResourceId is the note resource ID
        */
        IMPORT_C void SetResourceId( TInt aResourceId );

        /**
        * Sets the text resource ID for the note
        * @param aResourceId is the note resource ID
        */
        IMPORT_C void SetTextResourceId( TInt aTextResourceId );

        /**
        * Sets the text for the note
        * @param aText is the note text
        */
        IMPORT_C void SetText( const TDesC& aText );

        /**
        * Sets the note tone.
        * @param aTone is the note tone
        */
        IMPORT_C void SetTone( CAknNoteDialog::TTone aTone );
       
        /**
        * Sets the note timeout.
        * @param aTimeout is the note timeout
        */
        IMPORT_C void SetTimeout( CAknNoteDialog::TTimeout aTimeout );

        /**
        * Returns the note type
        * @return Returns the type
        */
        IMPORT_C TPhoneNoteType Type() const;

        /**
        * Returns the note resource ID.
        * @return Returns the note resource ID.
        */
        IMPORT_C TInt ResourceId() const;

        /**
        * Returns the note text resource ID.
        * @return Returns the note text resource ID.
        */
        IMPORT_C TInt TextResourceId() const;

        /**
        * Returns the note text.
        * @return Returns the note text.
        */
        IMPORT_C const TDesC& Text() const;

        /**
        * Returns the note tone.
        * @return Returns the note tone
        */
        IMPORT_C CAknNoteDialog::TTone Tone() const;

        /**
        * Returns the note timeout.
        * @return Returns the note timeout 
        */
        IMPORT_C CAknNoteDialog::TTimeout Timeout() const;

    private:    
        
        /**
        * Note type
        */
        TPhoneNoteType iType;

        /**
        * Note resource ID
        */
        TInt iResourceId;

        /**
        * Note text resource ID
        */
        TInt iTextResourceId;

        /**
        * Note text
        */
        TPtrC iText;

        /**
        * Note tone
        */
        CAknNoteDialog::TTone iTone;

        /**
        * Note timeout
        */
        CAknNoteDialog::TTimeout iTimeout;
    };

#endif // __TPHONECMDPARAMNOTE_H   
            
// End of File
