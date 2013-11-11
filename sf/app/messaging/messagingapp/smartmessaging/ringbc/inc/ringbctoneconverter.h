/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       For checking the validity of Smart messaging ringing tone and for 
*       ripping the ringing tone title from binary data. 
*
*/


#ifndef __RINGBCTONECONVERT_H
#define __RINGBCTONECONVERT_H

//  INCLUDES
#include <e32base.h>
#include "nsmringtone.h"

// FORWARD DECLARATIONS
NONSHARABLE_STRUCT( TInstruction )
    {
    TInt iValue;
    TInt iDuration;
    TInt iDurspecifier;
    TInt iScale;
    TInt iStyle;
    };

// CONSTANTS

NONSHARABLE_STRUCT( TCompData )
    {
	TInt iTempo;
	CArrayFix<TInstruction>* iSongData;
	TInt iSongLength;
	TBuf<KrtSongTitleTextLength> iSongTitle;
    };

//  CLASS DECLARATION 
NONSHARABLE_CLASS( RingBCNSMConverter ) : public CBase
    {
    public:     // New functions

        /**
        * Creates an instance of CRingBCNSMConverter and returns it.
        */
	    static RingBCNSMConverter* NewL();

    protected:
        /**
        * Second-phase constructor.
        */
        void ConstructL();

        /**
        * Default C++ constructor.
        */
	    RingBCNSMConverter();

    public:
        /**
        * Destructor.
        */
	    ~RingBCNSMConverter();

    public:     // New methods

        /**
        * Converts a Nokia Smart Messaging stream into TCompData.
        *
        * Smart Messaging:	<ringing-tone-programming-language>::=<command>+
        *					<command>::=
        *							<command-length><command-part>+ |
        *							<command-end>
        *					<command-length>::='binary[00000001..11111111], indicates
        *							how many command parts there are in the command.'
        *					<command-end>::='binary[00000000]. This indicates the end 
        *							of the ringing tone programming language.'

        * @param aFileData      The NSM stream.
        * @return               The song.
        */
	    TCompData* ConvertNsmToCompDataL( const TDesC8& aFileData );

        /**
        * Sets the title of the song.
        *
        * @param aFileName      The new title of the song.
        * @return               Error code
        */
	    void SetTitle( const TDesC& aFileName );

	    /**
        * Returns a pointer to the title of the ring tone.
        *
	    * Usage: 
	    *	1) Create converter(converter = CRingBCNSMConverter::NewL())
	    *	2) Call TitleL function to get title pointer(title = converter->TitleL(aFileData))
	    *	3) Use title pointer however necessary
	    *
        * @param aFileData          A pointer to the raw file data.
        * @return                   The title as a HBufC. The ownership is
        *                           transferred and it is left in cleanup stack.
	    *
        **/
	    HBufC* TitleLC(TPtr8& aFileData);

	    /**
        * Check if a NSM stream is a valid ringtone.
        *
	    * Usage: 
	    *	1) Create converter(converter = CRingBCNSMConverter::NewL())
	    *	2) Call function(booleanValue = converter->IsRingToneMimeTypeL(aFileData))
        *
        * @param aFileData		    A pointer to the raw file data.
        * @return                   ETrue if the tune is a ring tone, EFalse otherwise.
	    *
	    **/
        TBool IsRingToneMimeTypeL(TPtr8& aFileData);

    private:

	    struct TNsmRTPatterns
	    {
		    TInt iStart;
		    TInt iEnd;
	    };
    
        /**
        * Description:		This function processes Nokia Smart Messaging Ringing Tone
        *					<command-part> to generate Composer data.
        *
        * Smart Messaging:	<command-part>::=
        *							<ringing-tone-programming> |
        *							<unicode> |
        *							<cancel-command><cancel-command-specifier> |
        *							<sound><sound-command-specifier>
        *
        *					If necessary, filler bits are aded to ensure that 
        *					<command-part> is always octet-aligned.
        *
        *					The Ringing Tone programming requires that the order of 
        *					the command parts is the following: <ringing-tone-
        *					programming>, [<unicode>,] <sound>.
        */
	    void ProcessNsmRingToneCommandPartL();

        /**
        * Description:		This function processes Nokia Smart Messaging Ringing Tone
        *					<sound-command-specifier> to generate Composer data.
        *
        * Smart Messaging:	<sound-command-specifier>::=
        *							<basic-song-type><basic-song> |
        *							<temporary-song-type><temporary-song> |
        *							<midi-song-type><midi-song> |
        *							<digitized-song-type><digitized-song>
        */
	    void ProcessNsmRingToneSoundSpecL();

        /**
        * Description:		This function processes Nokia Smart Messaging Ringing Tone
        *					<song-title> 
        *
        * Smart Messaging:	<song-title>::=<text-length><text>
        *					<text>::=<default-char>+ |  'Uincode disabled'
        *							 <ISO-10646-char>+  'Uincode enabled'
        *					<text-length>::='binary[0000..1111]; Indicating how many
        *							characters are used for the following text.'	
        */
	    void ProcessNsmRingToneSongTitleL();

        /**
        * Description:		This function processes Nokia Smart Messaging Ringing Tone
        *					<temporary-song> to generate Composer data.
        *
        * Smart Messaging:	<temporary-song>::=<song-sequence-length><song-sequence>
        *					<song-sequence-length>::='binary[00000000..11111111]; 
        *							Indicates how many song patterns follow
        *					<song-sequence>::=<song-pattern>+
        *					<song-pattern>::=
        *							<pattern-header> |
        *							<pattern-header><pattern-instruction>+
        *					<pattern-header>::=
        *    						<pattern-header-id><pattern-id><loop-value><pattern-specifier>
        *					<loop-value>::='binary[0000..1111]; Indicates how many
        *							times the pattern should be repeated. Value zero 
        *							means no repeat. Value binary 1111 means infinite.
        *					<pattern-spacifier>::=
        *							<already-defined-pattern> | <length-of-new-pattern>
        *					<already-defined-pattern::='binary[00000000]; Indicates that
        *							an already defined pattern is used again.'
        *					<length-of-the-new-pattern>::='binary[00000001..11111111];
        *							Indicates how many pattern instructions there are
        *							in the song pattern. 
        */
	    void ProcessNsmRingToneSongL();

        /**
        * Description:		This function processes Nokia Smart Messaging Ringing Tone
        *					<pattern-instruction> to generate Composer data.
        *
        * Smart Messaging:	<pattern-instruction>::=
        *							<note-instruction> | <scale-instruction> |
        *							<style-instruction> | <tempo-instruction> |
        *							<volume-instruction>
        *					<scale-intruction>::=<scale-instruction-id><scale-value>
        *					<style-intruction>::=<style-instruction-id><style-value>
        *					<tempo-intruction>::=<tempo-instruction-id><tempo-value>
        *					<volume-intruction>::=<volume-instruction-id><volume-value>
        */
	    void ProcessNsmRingTonePatternInstructionL();

        /**
        * Description:		This function forms a composer symbol from the pattern
        *					instructions and places it into the CompData structure.
        *
        * Smart Messaging:	<note-instruction>::=<note-instruction-id><note-value>
        *							<note-duration><note-duration-specifier>
        */
	    void ProcessNsmRingToneNoteInstructionL(TInt aScale, TInt aStyle);

        /**
        * This function extracts the specified number of bits from 
        * the bit stream(returned int will contain both the instruction
        * bits and the data bits).
        *
        * @param aNumBits   The number of bits to get.
        * @return           The value.
        */
	    TInt GetBits(TInt aNumBits);

        /**
        */
	    void ResetForCompData();

    private:        // Data

        /// The data being converted from an NSM stream.
	    TPtrC8 iReceivedNsmRTData;

        /// The flags associated with an NSM stream.
	    TInt iNsmRTFlags;

        /// The current position in the stream.
	    TInt iNsmPosition;

        /// The current bit number in the NSM stream.
	    TInt iNsmPositionBit;

        /// Patterns of NSM data.
	    TFixedArray<TNsmRTPatterns,4> iNsmRTPatterns;

        /// The default scale of a tone.
	    TInt iScale;

        /// The default style of a tone.
	    TInt iStyle;
	    
        /// The song data created. Owns.
	    TCompData iCompData;

        /// The song data received for conversion. Refs.
	    TCompData* iReceivedCompData;

        /// The position in the song data.
	    TInt iCompDataPosition;

        /// If ETrue, the song data cannot be loaded to Composer.
        TBool iReadOnly;
};
#endif //__RINGBCTONECONVERT_H
