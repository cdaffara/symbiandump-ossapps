/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef CPHONERINGINGTONE_H
#define CPHONERINGINGTONE_H

// INCLUDES
#include <e32base.h>
#include <Profile.hrh>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Ringing tone
*  Wrapper for file name. In addtion, provides methods
*  for ringing tone type identification. 
*
*  @lib Phone.app
*  @since Series 60 3.1
*/
class CPhoneRingingTone : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFileName Ringing tone file name with path.
        * @param aDrmInPlayback ETrue to use IsFileDrmProtected().
        */
        static CPhoneRingingTone* NewL( 
            const TDesC& aFileName,
            TBool aDrmInPlayback = EFalse );
        
        /**
        * Destructor.
        */
        virtual ~CPhoneRingingTone();

    public: // New functions
        
        
        /**
        * Sets the file name including path.
        * @since Series 60 3.1
        * @param aFileName file name plus path.
        */
        void SetFileName( const TDesC& aFileName );
        
        /**
        * Returns file name including path.
        * @since Series 60 3.1
        * @return File name.
        */
        const TDesC& FileName() const;
        
        /**
        * Returns MIME type.
        * @since Series 60 3.1
        * @return MIME type.
        */
        const TDesC& MimeType() const;
        
        /**
        * Checks if video ringing tone.
        * @since Series 60 3.1
        * @return ETrue if video ringing tone.
        */
        TBool IsVideoRingingTone();
        
        /**
        * Checks if the file is a DCF file.
        * @since Series 60 3.1
        * @return ETrue if files is DRM protected.
        */
        TBool IsFileDrmProtected() const;
        
        /**
        * Checks if the file is in ROM.
        * @since Series 60 3.1
        */
        TBool IsFileInRom() const;

        /**
        * Set profile's ringing type  
        * @since Series 60 5.0
        */        
        void SetRingingType( TProfileRingingType aRingingType);

        /**
        * Return ringing type of current ringingtone  
        * @since Series 60 5.0
        */            
        TProfileRingingType RingingType() const;        
        
        /**
        * Set profile's ringingtone volume  
        * @since Series 60 5.0
        */            
        void SetVolume( const TInt aVolume );
        
        /**
        * Return volume of current ringingtone  
        * @since Series 60 5.0
        */         
        TInt Volume() const;
        
        /**
        * Set profile's TTS value
        * @since Series 60 5.0
        */         
        void SetTtsToneToBePlayed( TBool aTtsToneToBePlayed );

        /**
        * Return TTS status of current profile  
        * @since Series 60 5.0
        */         
        TBool TtsToneToBePlayed() const;       
        
        /**
        * Checks if ringing tone file's size is larger than the size limit.
        * If size limit is exceeded, default tone is played instead.
        * @since 3.1
        * @param aRingingTone  Tone to check.
        * @return EFalse if the limit was exceeded and thus default tone played.
        *         ETrue  if tone file's size was valid and the checked tone 
        *                can be played.
        */
        TBool CheckAndHandleToneSizeLimit();
        
        /**
        * Checks if ringing tone file's size is larger than the size limit.
        * @since 3.1
        * @param aFile   File to check.
        *        aSizeLimitKB  the size limit in kB is set to this variable.
        * @return KErrNone        if the file can be used as a ringing tone.
        *         KErrTooBig        if the file size limit is exceeded.
        *         Other error value if error happened during size checking.
        */
        TInt CheckToneFileSize( const TDesC& aFile, TInt aSizeLimitKB );
        
        /**
        * Reads from central repository the file size limit of following tones:
        * -contact's personal ringing tone
        * -voice call line1
        * -voice call line2
        * Value 0 means all sizes are allowed.
        * @since 3.1
        * @return leaves on error.
        */
        void GetMaxToneFileSize();      
        
    private:

        /**
        * Refresh MIME type. This must be called
        * before using MimeType().
        */
        TInt RefreshMime();    
        
        /**
        * Refresh MIME type.
        */
        void RefreshMimeL();
        
        /**
        * Checks if file is located in video directory.
        */
        TBool IsFileInVideoDirectory() const;

    private:

        /**
        * C++ default constructor.
        */
        CPhoneRingingTone( TBool aDrmInPlayback );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aFileName );

    private:    // Data

        // File name with path
        HBufC* iFileName;
        
        // MIME type
        HBufC* iMimeType;
        
        // Extended security
        TBool iDrmInPlayback;
        
        TProfileRingingType iRingingType;
    
        TInt iVolume;
        
        // Voice call ringing tone file size max value.
        TInt iToneFileSizeLimitKB;
        
        TBool iTtsToneToBePlayed;
    };

#endif      // CPHONERINGINGTONE_H   
            
// End of File
