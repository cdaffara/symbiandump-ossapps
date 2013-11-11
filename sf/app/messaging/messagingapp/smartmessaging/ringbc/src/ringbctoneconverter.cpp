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



// SYSTEM INCLUDES
#include <e32svr.h>

// USER INCLUDES
#include "ringbctoneconverter.h"
#include "nsmringtone.h"
#include "debugtraces.h"


// LOCAL CONSTANTS AND MACROS
const TInt KCompMaxSongLength = 240; // maximum number of notes


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// NewL()
//
// ---------------------------------------------------------
RingBCNSMConverter* RingBCNSMConverter::NewL()
    {
	RingBCNSMConverter* self = new (ELeave) RingBCNSMConverter();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
    }

// ---------------------------------------------------------
// RingBCNSMConverter()
//
// ---------------------------------------------------------
RingBCNSMConverter::RingBCNSMConverter()
    {
	iScale = EScale2;
	iStyle = EStyleNatural;
    }

// ---------------------------------------------------------
// RingBCNSMConverter()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ConstructL()
    {
   
    
   
    }

// ---------------------------------------------------------
// ~RingBCNSMConverter()
//
// ---------------------------------------------------------
RingBCNSMConverter::~RingBCNSMConverter()
    {
	delete iCompData.iSongData; 	
    }


// ---------------------------------------------------------
// ConvertNsmToCompDataL()
//
// ---------------------------------------------------------
TCompData* RingBCNSMConverter::ConvertNsmToCompDataL( const TDesC8& aFileData )
    {
    QDEBUG_WRITE( "***BEGIN" );
    
    iReceivedNsmRTData.Set( aFileData );

    iReadOnly = EFalse;
	ResetForCompData();
	delete iCompData.iSongData;
	iCompData.iSongData = NULL;
	iCompData.iSongData = new(ELeave) CArrayFixFlat<TInstruction>( KCompMaxSongLength );

    QDEBUG_WRITE_FORMAT( "***NSM DATA LENGTH:", iReceivedNsmRTData.Length() );

    if( iReceivedNsmRTData.Length() == 0 )
        {
        QDEBUG_WRITE( "!!!ZERO LENGTH" );
        User::Leave( KErrCorrupt );
        }

	TInt commandlength( 1 );
	while( commandlength != KrtCommandEnd && 
        iNsmPosition < iReceivedNsmRTData.Length() )
        {
		commandlength = GetBits(KrtCommandLengthBits);
		

        QDEBUG_WRITE_FORMAT( "***NEXT CMD LENGTH :", commandlength );

		if (commandlength > 3)
            {
            QDEBUG_WRITE( "!!!CMD TOO LONG" );
            User::Leave( KErrCorrupt );
            }

		for (TInt i=0; i<commandlength; i++)
            {		

            QDEBUG_WRITE_FORMAT( "***CMD NUMBER ", i );

			ProcessNsmRingToneCommandPartL();
            }
        }

	return &iCompData;
    }

// ---------------------------------------------------------
// ProcessNsmRingToneCommandPartL()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ProcessNsmRingToneCommandPartL()
    {
	TInt commandpart( GetBits(KrtCommandPartBits) );

    QDEBUG_WRITE_FORMAT( "***CMD PART: ", commandpart );

	switch (commandpart)
	    {
	    case KrtCommandRTProgramming:
			{
			iNsmRTFlags |= KrtFlagRTProgramming;
		    break;
			}
	    case KrtCommandUnicode:
			{
			iNsmRTFlags |= KrtFlagUnicode;
		    break;
			}
	    case KrtCommandCancelCommand:
			{
		    if (GetBits(KrtCommandPartBits) == KrtCommandUnicode)
                {
			    iNsmRTFlags &= ~KrtFlagUnicode;
                }
		    break;
			}
	    case KrtCommandSound:
			{
			if ((iNsmRTFlags & KrtFlagRTProgramming) == 0)
                {
                QDEBUG_WRITE( "!!!CMD SOUND WITH NO RT PROGRAMMING" );
                User::Leave( KErrCorrupt );
                }
		    ProcessNsmRingToneSoundSpecL();
		    break;
			}
	    default:
			{
            // Ignore unknown commands
            QDEBUG_WRITE( "???CMD UNKNOWN" );
            iReadOnly = ETrue;
		    break;
			}
	    }

	GetBits(KrtFiller); // skip the filler bits
    }

// ---------------------------------------------------------
// ProcessNsmRingToneSoundSpecL()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ProcessNsmRingToneSoundSpecL()
    {
	TInt soundspec( GetBits(KrtSongTypeBits) );

    QDEBUG_WRITE_FORMAT( "***SONG TYPE :", soundspec );

	switch (soundspec)
	    {
	    case KrtSongTypeBasic:
			{
		    ProcessNsmRingToneSongTitleL();
		    ProcessNsmRingToneSongL();
		    break;
			}
	    case KrtSongTypeTemporary:
            {
		    ProcessNsmRingToneSongL();
		    break;
			}
	    default:
            {
		    iReadOnly = ETrue;
		    break;
			}
	    }

	iCompData.iSongLength = iCompDataPosition;
    }

// ---------------------------------------------------------
// ProcessNsmRingToneSongTitleL()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ProcessNsmRingToneSongTitleL()
{
	TInt titlelength( GetBits(KrtSongTitleTextLengthBits) );
    QDEBUG_WRITE_FORMAT( "***TITLE LENGTH ", titlelength );

    TInt charWidth( KrtDefaultCharBits );
	if( iNsmRTFlags & KrtFlagUnicode )
    	{
        charWidth = KrtUnicodeCharBits;
        }
    QDEBUG_WRITE_FORMAT( "***TITLE CHAR WIDTH: ", charWidth );

    TBuf<KrtSongTitleMaxLength> title;
	for( TInt i( 0 ); i < titlelength; i++ )
        {
        title.Append( GetBits(charWidth) );
        }
    QDEBUG_WRITE_FORMAT( "***TITLE: ", &title );

    SetTitle( title );
    }

// ---------------------------------------------------------
// ProcessNsmRingToneSong()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ProcessNsmRingToneSongL()
    {
	TInt songsequencelength( GetBits(KrtSongsequenceLengthBits) );
    QDEBUG_WRITE_FORMAT( "***SEQUENCE LENGTH ", songsequencelength );

	/*if( songsequencelength == 0 )
		{
		iReadOnly = ETrue;
		}*/

	for (TInt i( 0 ); i<songsequencelength; i++)
	    {
		TInt patternheaderid( GetBits(KrtInstructionIdLengthBit) );
        QDEBUG_WRITE_FORMAT( "***PATTERN HEADER ID:", patternheaderid );
		if (patternheaderid != EPatternHeaderId)
            {
            QDEBUG_WRITE( "!!!PATTERN HEADER ID INVALID" );
			User::Leave(KErrCorrupt);
            }

		TInt patternid( GetBits(KrtPatternIdLengthBit) );
        QDEBUG_WRITE_FORMAT( "***PATTERN ID:", patternid );
		TInt loopvalue( GetBits(KrtPatternLoopValueBits) );
        QDEBUG_WRITE_FORMAT( "***PATTERN LOOP VALUE:", loopvalue );
		TInt patternspec( GetBits(KrtPatternSpecBits) );
        QDEBUG_WRITE_FORMAT( "***PATTERN SPEC:", patternspec );
		
		if( patternspec != KrtPatternDefined )
		    {
            QDEBUG_WRITE( "***PATTERN NOT DEFINED" );
			iNsmRTPatterns[patternid].iStart = iCompData.iSongData->Count();
			
            for (TInt l( 0 ); l < patternspec && iCompDataPosition < KCompMaxSongLength; l++)
                {
				ProcessNsmRingTonePatternInstructionL();
                }
			iNsmRTPatterns[patternid].iEnd = iCompDataPosition - 1;
		    }
		// check if the pattern is really already defined before copying
		TInt start( iNsmRTPatterns[patternid].iStart );
		TInt end( iNsmRTPatterns[patternid].iEnd );
		if( start < end )
		    {
			if( loopvalue < KrtPatternRepeatInfinite )
			    {
				for( TInt loopNum( 0 ); loopNum < loopvalue; loopNum++ ) // handle repeat
				    {
					for (TInt k=start; (k<=end)&&(iCompDataPosition<KCompMaxSongLength); 
						k++, iCompDataPosition++)
                        {
						iCompData.iSongData->AppendL( iCompData.iSongData->At( k ) );
                        }
				    }
			    }
			else // handle infinite repeat
			    {
				while( iCompDataPosition<KCompMaxSongLength )
				    {
					for (TInt k=start; (k<=end)&&(iCompDataPosition<KCompMaxSongLength); 
						k++, iCompDataPosition++)
                        {
						iCompData.iSongData->AppendL( iCompData.iSongData->At( k ) );
                        }
				    }
			}
		}
	}
}

// ---------------------------------------------------------
// ProcessNsmRingTonePatternInstruction()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ProcessNsmRingTonePatternInstructionL()
    {
	TInt instructionid( GetBits(KrtInstructionIdLengthBit) );
    QDEBUG_WRITE_FORMAT( "***INSTRUCTION ID:", instructionid );
	switch (instructionid)
	    {
	    case ENoteInstructionId:
			{
            QDEBUG_WRITE_FORMAT( "***INSTRUCTION ID: NOTE", instructionid );
		    ProcessNsmRingToneNoteInstructionL(iScale, iStyle);
		    break;
			}
	    case EScaleInstructionId:
			{
			iScale = GetBits(KrtNoteScaleBits);
            QDEBUG_WRITE_FORMAT( "***INSTRUCTION SCALE:", iScale );
		    break;
			}
	    case EStyleInstructionId:
			{
			iStyle = GetBits(KrtNoteStyleBits);
            QDEBUG_WRITE_FORMAT( "***INSTRUCTION STYLE:", iStyle );
            if( iStyle == EStyleReserved )
                {
                QDEBUG_WRITE( "???INSTRUCTION STYLE NOT SUPPORTED" );
                iReadOnly = ETrue;
                }
		    break;
			}
	    case ETempoInstructionId:
			{
			iCompData.iTempo = GetBits(KrtTempoBits);
            QDEBUG_WRITE_FORMAT( "***INSTRUCTION TEMPO:", iCompData.iTempo );
            if( iCompData.iTempo > ETempo250 )
                {
                QDEBUG_WRITE( "???TEMPO TOO HIGH" );
                iReadOnly = ETrue;
                }
		    break;
			}
	    case EVolumeInstructionId:
            {
			QDEBUG_WRITE( "***INSTRUCTION VOLUME" );
		    GetBits(KrtVolumebits);
		    break;
			}
	    default:
            {
			QDEBUG_WRITE( "???INSTRUCTION NOT SUPPORTED" );
            iReadOnly = ETrue;
		    break;
			}
	    }
    }

// ---------------------------------------------------------
// ProcessNsmRingToneNoteInstruction()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ProcessNsmRingToneNoteInstructionL(TInt aScale, TInt aStyle)
    {
	TInt notevalue( GetBits(KrtNoteValueBits) );
    QDEBUG_WRITE_FORMAT( "***NOTE VALUE:", notevalue );
    if( notevalue != ENotePause && (notevalue > ENoteB 
        || iScale == EScale4 && notevalue > ENoteGis
        || iScale == EScale1 && notevalue < ENoteG ) )
        {
        QDEBUG_WRITE( "???NOTE VALUE OUT OF RANGE" );
        iReadOnly = ETrue;
        }

	TInt noteduration( GetBits(KrtNoteDurationBits) );
    QDEBUG_WRITE_FORMAT( "***NOTE DURATION:", noteduration );
    if( noteduration > EDurationThirtysecond )
        {
        QDEBUG_WRITE( "???NOTE DURATION OUT OF RANGE" );
        iReadOnly = ETrue;
        }

	TInt notedurspecifier( GetBits(KrtNoteDurSpecifierBits) );
    QDEBUG_WRITE_FORMAT( "***NOTE DURATION SPECIFIER:", notedurspecifier);
    if( notedurspecifier == EDurSpecifireDoubleDotted
        || notedurspecifier > EDurSpecifierTriplet )
        {
        QDEBUG_WRITE( "???NOTE DURATION SPECIFIER OUT OF RANGE" );
        iReadOnly = ETrue;
        }

	TInstruction symbol;
	symbol.iValue = notevalue;
	symbol.iDuration = noteduration;
	symbol.iDurspecifier = notedurspecifier;
	symbol.iScale = aScale;
	symbol.iStyle = aStyle;

	iCompData.iSongData->AppendL( symbol );
	iCompDataPosition ++;
    }

// ---------------------------------------------------------
// GetBits()
//
// ---------------------------------------------------------
TInt RingBCNSMConverter::GetBits(TInt aNumBits)
    {
	TUint32 buf( 0 );   
	if( aNumBits == 0 ) // handle byte alignment
	    {
		if (iNsmPositionBit != 0)
		    {
			iNsmPositionBit = 0;	// skip filler bits
			iNsmPosition ++;			
		    }
		return buf;
	    }

	for (TInt n=0; n<4; n++)
	    {
		buf <<= 8;
		if ( iNsmPosition+n < iReceivedNsmRTData.Length() )
            {
			buf |= iReceivedNsmRTData[iNsmPosition+n];
            }
	    }

	TUint32 filter = 0;
	buf = (buf >> (32 - iNsmPositionBit - aNumBits)) & ~(~filter << aNumBits);

	iNsmPositionBit += aNumBits;
	while (iNsmPositionBit > 7)
	    {
		iNsmPositionBit -= 8;
		iNsmPosition ++;
	    }
	return buf;
    }

// ---------------------------------------------------------
// ResetForCompData()
//
// ---------------------------------------------------------
void RingBCNSMConverter::ResetForCompData()
    {
	iNsmRTPatterns[EPatternIdA].iStart = 0;
	iNsmRTPatterns[EPatternIdA].iEnd = 0;
	iNsmRTPatterns[EPatternIdB].iStart = 0;
	iNsmRTPatterns[EPatternIdB].iEnd = 0;
	iNsmRTPatterns[EPatternIdC].iStart = 0;
	iNsmRTPatterns[EPatternIdC].iEnd = 0;
	iNsmRTPatterns[EPatternIdD].iStart = 0;
	iNsmRTPatterns[EPatternIdD].iEnd = 0;
	iNsmRTFlags = 0;
    iNsmPosition = 0;
	iNsmPositionBit = 0;
	iCompDataPosition = 0;

	iCompData.iTempo = KrtBpmDefault;
	iCompData.iSongLength = 0;
    }

// ---------------------------------------------------------
// SetTitle()
//
// ---------------------------------------------------------
void RingBCNSMConverter::SetTitle( const TDesC& aFileName )
    {
    iCompData.iSongTitle = aFileName.Left( KrtSongTitleTextLength );
    }


// ---------------------------------------------------------
// TitleLC()
//
// ---------------------------------------------------------
HBufC* RingBCNSMConverter::TitleLC(TPtr8& aFileData)
{
	iReceivedCompData = ConvertNsmToCompDataL(aFileData);

	HBufC* titlePtr = iReceivedCompData->iSongTitle.AllocLC();
	return titlePtr;
}


// ---------------------------------------------------------
// IsRingToneMimeTypeL()
//
// ---------------------------------------------------------
TBool  RingBCNSMConverter::IsRingToneMimeTypeL(TPtr8& aFileData)
    {
	TRAPD( returnError, 
        iReceivedCompData = ConvertNsmToCompDataL(aFileData) );
    if( returnError != KErrCorrupt 
        && returnError != KErrNotSupported )
        {
        User::LeaveIfError( returnError );
        }

    return returnError != KErrCorrupt;
    }

// end of file
