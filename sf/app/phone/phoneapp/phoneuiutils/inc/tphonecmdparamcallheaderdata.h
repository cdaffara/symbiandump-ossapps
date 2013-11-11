/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __TPHONECMDPARAMCALLHEADERDATA_H
#define __TPHONECMDPARAMCALLHEADERDATA_H

//  INCLUDES

#include <w32std.h>
#include <cntdef.h>
#include "tphonecommandparam.h"
#include "phoneconstants.h"

// DATA TYPES

// CLASS DECLARATION
class CFbsBitmap;

/**
*  A parameter class for call header information.
*/
class TPhoneCmdParamCallHeaderData : public TPhoneUICommandParam
    {
    public:
        // CLI used in conference call participant list.
        enum TPhoneParticipantListCLI
            {
            EPhoneParticipantCLIText,
            EPhoneParticipantCNAPText,                
            };

        enum TPhoneTextClippingDirection
            {
            ERight,
            ELeft
            };
           
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamCallHeaderData();

        /**
        * destructor.
        */
        IMPORT_C ~TPhoneCmdParamCallHeaderData();
    public: 
        
        /**
        * Sets the call header label text.
        * @param aLabel is the label
        */
        IMPORT_C void SetLabelText( const TDesC& aLabelText );

        /**
        * Sets the call header short label text
        * @param aShortLabel is the short label
        */
        IMPORT_C void SetShortLabelText( const TDesC& aShortLabelText );

        /**
        * Sets the call header call state.
        * @param aCallState is the call state
        */
        IMPORT_C void SetCallState( TInt aCallState );

        /**
        * Sets the call header call type.
        * @param aCallType is the call type
        */
        IMPORT_C void SetCallType( TInt aCallType );

        /**
        * Sets the call header number type.
        * @param aNumberType is the number type
        */
        IMPORT_C void SetNumberType( TInt aNumberType );

        /**
        * Sets the call header voice privacy status.
        * @param aCiphering is ETrue if voice privacy is on.
        */
        IMPORT_C void SetCiphering( TBool aCiphering );

        /**
        * Sets the call header CLI text
        * @param aCLI is the CLI text
        * @param aDirection Clipping direction of the CLI text.
        */
        IMPORT_C void SetCLIText( const TDesC& aCLIText, 
                                  TPhoneTextClippingDirection aDirection );

        /**
        * Sets the call header CNAP text
        * @param aCNAP is the CNAP text
        * @param aDirection Clipping direction of the CNAP text.
        */
        IMPORT_C void SetCNAPText( const TDesC& aCNAPText,
                                   TPhoneTextClippingDirection aDirection );

        /**
        * Sets the call header picture
        * @param aPicture is the picture data
        */
        IMPORT_C void SetPicture( const TDesC& aPicture );

        /**
        * Sets the call header diverted status.
        * @param call flag
        */
        IMPORT_C void SetDiverted( TBool aDiverted );
            
        /**
        * Sets the call header line2 status.
        * @param aLine2 is ETrue if line2 is active.
        */
        IMPORT_C void SetLine2( TBool aLine2 );
            
        /**
        * Returns the call header label text
        * @return Returns the label
        */
        IMPORT_C const TDesC& LabelText() const;

        /**
        * Returns the call header short label text.
        * @return Returns the short label
        */
        IMPORT_C const TDesC& ShortLabelText() const;

        /**
        * Returns the call header call state.
        * @return Returns the call state
        */
        IMPORT_C TInt CallState() const;

        /**
        * Returns the call header call type.
        * @return Returns the call type
        */
        IMPORT_C TInt CallType() const;

        /**
        * Returns the call header number type.
        * @return Returns the number type 
        */
        IMPORT_C TInt NumberType() const;

        /**
        * Returns the call header ciphering status.
        * @return Returns ETrue if ciphering is on 
        */
        IMPORT_C TBool Ciphering() const;

        /**
        * Returns the call header CLI text.
        * @return Returns the CLI text
        */
        IMPORT_C const TDesC& CLIText() const;

        /**
         * Returns the clipping direction of the CLI text.
         * @return  Clipping direction of the CLI text.
         */
        IMPORT_C TPhoneTextClippingDirection CLITextClippingDirection() const;

        /**
         * Returns the clipping direction of the CNAP text.
         * @return  Clipping direction of the CNAP text.
         */
        IMPORT_C TPhoneTextClippingDirection CNAPTextClippingDirection() const;
        
        /**
        * Returns the call header CNAP text.
        * @return Returns the CNAP text
        */
        IMPORT_C const TDesC& CNAPText() const;

        /**
        * Returns the call header picture.
        * @return Returns the picture
        */
        IMPORT_C const TDesC& Picture() const;
        
        /**
        * Returns call diverted status.
        * @return Returns ETrue if diverted call.
        */
        IMPORT_C TBool Diverted() const;
        
        /**
        * Returns the call header line 2 active.
        * @return Returns ETrue if line2 is on 
        */
        IMPORT_C TBool Line2() const;
        
        /**
        * Returns the Thumbnail.
        * @return Returns pointer to the thumbnail image 
        */
        IMPORT_C CFbsBitmap* Thumbnail() const;
        
        /**
        * Sets the Thumbnail
        * @param aThumbnail is pointer to the thumbnail image
        */
        IMPORT_C void SetThumbnail( CFbsBitmap* aThumbnail );
        
        /**
        * Sets the call header Caller text
        * @param aCallerText is the Caller text
        */
        IMPORT_C void SetCallerText( const TDesC& aCallerText );
        
        /**
        * Returns the call Caller text.
        * @return Returns the Caller text
        */
        IMPORT_C const TDesC& CallerText() const;

        /**
        * Sets CLI used in conference call participant list. 
        * @param aCLIType CLI type.
        */
        IMPORT_C void SetParticipantListCLI(
            TPhoneParticipantListCLI aCLIType );
            
        /**
        * Gets CLI type used in conference call participant list. 
        * @return CLI type.
        */
        IMPORT_C TPhoneParticipantListCLI ParticipantCLI() const;
        
        /**
        * Returns the status of Thumbnail.
        * @return Returns ETrue if thumbnail exist in contact
        */
        IMPORT_C TBool HasThumbnail() const;
        
        /**
        * Sets the Thumbnail status
        * @param aStatus status of thumbnail in contact
        */
        IMPORT_C void SetHasThumbnail( TBool aStatus );

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
        
        /**
         * Set service id. See Service Provider Settings API
         * @param Service id.
         */
        IMPORT_C void SetServiceId( TUint32 aServiceId );
        
        /**
         * Returns service id. See Service Provider Settings API
         * @return Service id.
         */
        IMPORT_C TUint32 ServiceId() const;
        
        /**
         * Sets contact store link. See Virtual Phonebook API
         * @param aContactStoreLink Contact store link
         */
        IMPORT_C void SetContactLink( const TDesC8& aContactLink );
        
        /**
         * Returns contact store link. See Virtual Phonebook API
         * @return Contact store link
         */
        IMPORT_C const TDesC8& ContactLink() const;
        
        /**
         * Sets remote end phone number.
         * @param aRemoteNumber Number to be set.
         */
        IMPORT_C void SetRemotePhoneNumber( const TDesC& aPhoneNumber );
        
        /**
         * Remote phone number.
         * @return Phone number for remote end.
         */
        IMPORT_C const TDesC& RemotePhoneNumber() const;
    private:    
        
        /**
        * Call header label text
        */
        HBufC *iLabelText;

        /**
        * Call header short label text
        */
        HBufC *iShortLabelText;

        /**
        * Call header call state
        */
        TInt iCallState;

        /**
        * Call header call type
        */
        TInt iCallType;

        /**
        * Call header number type
        */
        TInt iNumberType;

        /**
        * Call header ciphering status
        */
        TBool iCiphering;

        /**
        * Call header CLI Text
        */
        HBufC *iCLIText;

        /**
         * Clipping direction of the CLI text.
         */
        TPhoneTextClippingDirection iCLITextClippingDirection;
       
        /**
         * Clipping direction of the CLI text.
         */
//        CBubbleManager::TPhoneClippingDirection iCNAPTextClippingDirection;
        TPhoneTextClippingDirection iCNAPTextClippingDirection;
        
        /**
        * Call header CNAP Text
        */
        HBufC *iCNAPText;

        /**
        * Call header picture data
        */
        TPtrC iPicture;
        
        TBool iLine2;
        
        /**
        * Thumbnail image
        */
        CFbsBitmap* iThumbnail;
        
        /**
        * Caller text
        */
        TPtrC iCallerText;
        
        /**
        * Conference call participant list CLI.
        */
        TPhoneParticipantListCLI iParticipantListCLI;
        
        /**
        * Thumbnail contact status
        */
        TBool iHasThumbnail;
        
        /**
        * Ciphering indicator allowed status
        */
        TBool iCipheringIndicatorAllowed;
        
        /**
         * Service id
         * See Service Provider Settings API
         */
        TUint32 iServiceId;
        
        /**
         * Contact store link
         * See Virtual Phonebook API
         */
        TPtrC8 iContactLink;
        
        /**
         * Remote phone number
         */
        TPtrC iRemotePhoneNumber;

        /**
         * Call divert status.
         */
        TBool iDiverted;        
    };

#endif // __TPHONECMDPARAMCALLHEADERDATA_H   
            
// End of File
