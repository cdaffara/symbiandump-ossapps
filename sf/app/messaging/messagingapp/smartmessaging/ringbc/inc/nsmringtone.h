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
* Description:  	This file contains enums for Nokia Smart Messaging
*				Ringing Tone file format.
*
*/



#ifndef __NSMRINGTONE_H
#define __NSMRINGTONE_H

enum // Song Pattern Header
{
	EPatternIdA = 0x00,
	EPatternIdB = 0x01,
	EPatternIdC = 0x02,
	EPatternIdD = 0x03
};

enum // Instruction Id
{
	EPatternHeaderId = 0x00,
	ENoteInstructionId = 0x01,
	EScaleInstructionId = 0x02,
	EStyleInstructionId = 0x03,
	ETempoInstructionId = 0x04, 
	EVolumeInstructionId = 0x05
};

enum //Note Value
{
	ENotePause = 0x00,
	ENoteC = 0x01,
	ENoteCis = 0x02,
	ENoteD = 0x03,
	ENoteDis = 0x04,
	ENoteE = 0x05,
	ENoteF = 0x06,
	ENoteFis = 0x07,
	ENoteG = 0x08,
	ENoteGis = 0x09,
	ENoteA = 0x0a,
	ENoteAis = 0x0b,
	ENoteB = 0x0c
};

enum // Note Duration
{
	EDurationFull = 0x00,
	EDurationHalf = 0x01,
	EDurationQuarter = 0x02,
	EDurationEighth = 0x03,
	EDurationSixteenth = 0x04,
	EDurationThirtysecond = 0x05
};

enum // Duration Specifier
{
	EDurSpecifierNone = 0x00,
	EDurSpecifierDotted = 0x01,
	EDurSpecifireDoubleDotted =0x02, // not supported in composer
	EDurSpecifierTriplet = 0x03
};

enum // Scale
{
	EScale1 = 0x00,
	EScale2 = 0x01,
	EScale3 = 0x02,
	EScale4 = 0x03
};

enum // Style
{
	EStyleNatural = 0x00,
	EStyleLegato = 0x01,
	EStyleStaccato = 0x02,
    EStyleReserved = 0x03
};

enum // Tempo index
{
	ETempo25 = 0x00,
	ETempo28 = 0x01,
	ETempo31 = 0x02,
	ETempo35 = 0x03,
	ETempo40 = 0x04,
	ETempo45 = 0x05,
	ETempo50 = 0x06,
	ETempo56 = 0x07,
	ETempo63 = 0x08,
	ETempo70 = 0x09,
	ETempo80 = 0x0a,
	ETempo90 = 0x0b,
	ETempo100 = 0x0c,
	ETempo112 = 0x0d,
	ETempo125 = 0x0e,
	ETempo140 = 0x0f,
	ETempo160 = 0x10,
	ETempo180 = 0x11,
	ETempo200 = 0x12,
	ETempo225 = 0x13,
	ETempo250 = 0x14,
	ETempo285 = 0x15,
	ETempo320 = 0x16,
	ETempo355 = 0x17,
	ETempo400 = 0x18,
	ETempo450 = 0x19,
	ETempo500 = 0x1a,
	ETempo565 = 0x1b,
	ETempo635 = 0x1c,
	ETempo715 = 0x1d,
	ETempo800 = 0x1e,
	ETempo900 = 0x1f 
};

enum // Volume levels
{
	EVolumeToneOff = 0x00,
	EVolumeLevel1 = 0x01,
	EVolumeLevel2 = 0x02,
	EVolumeLevel3 = 0x03,
	EVolumeLevel4 = 0x04,
	EVolumeLevel5 = 0x05,
	EVolumeLevel6 = 0x06,
	EVolumeLevel7 = 0x07,
	EVolumeLevel8 = 0x08,
	EVolumeLevel9 = 0x09,
	EVolumeLevel10 = 0x0a,
	EVolumeLevel11 = 0x0b,
	EVolumeLevel12 = 0x0c,
	EVolumeLevel13 = 0x0d,
	EVolumeLevel14 = 0x0e,
	EVolumeLevel15 = 0x0f
};

// Ringing Tones Command
const TInt KrtCommandLengthBits = 8;
const TInt KrtCommandEnd = 0x00;
const TInt KrtCommandEndBits = 8;
const TInt KrtCommandFillerBits = 0;
const TInt KrtCommandPartBits = 7;
const TInt KrtCommandCancelCommand = 0x05;
const TInt KrtCommandRTProgramming = 0x25;
const TInt KrtCommandSound = 0x1d;
const TInt KrtCommandUnicode = 0x22;

// Ringing Tones Song Type
const TInt KrtSongTypeBits = 3;
const TInt KrtSongTypeBasic = 0x01;
const TInt KrtSongTypeTemporary = 0x02;
const TInt KrtSongTitleTextLengthBits = 4;
const TInt KrtSongTitleTextLength = 1<<KrtSongTitleTextLengthBits;
const TInt KrtSongTitleMaxLength = 0x0f;
const TInt KrtDefaultCharBits = 8;
const TInt KrtUnicodeCharBits = 16;
const TInt KrtSongsequenceLengthBits = 8;

// Ringing Tones Pattern
const TInt KrtPatternIdLengthBit = 2;
const TInt KrtPatternLoopValueBits = 4;
const TInt KrtPatternSpecBits = 8;
const TInt KrtPatternDefined = 0x00;
const TInt KrtPatternRepeatInfinite = 0x0f;

// Ringing Tones Instruction
const TInt KrtInstructionIdLengthBit = 3;
const TInt KrtNoteValueBits = 4;
const TInt KrtNoteDurationBits = 3;
const TInt KrtNoteDurSpecifierBits = 2;
const TInt KrtNoteScaleBits = 2;
const TInt KrtNoteStyleBits = 2;
const TInt KrtTempoBits = 5;
const TInt KrtVolumebits = 4;

// Default values 
const TInt KrtVolumeDefault = EVolumeLevel7;
const TInt KrtBpmDefault = ETempo160;

const TInt KRingingToneBmpTable[] =
{
	25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90, 100, 112, 125,
	140, 160, 180, 200, 225, 250, 258, 320, 355, 400, 450, 500, 
	565, 635, 715, 800, 900
};

const TInt KrtFlagRTProgramming = 1;
const TInt KrtFlagUnicode = 2;
const TInt KrtFiller = 0;

const TInt KrtMaxNotesInPattern = 80;

#endif

