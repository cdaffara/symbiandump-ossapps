#ifndef CALENIMPTESTCONSTANTS_H
#define CALENIMPTESTCONSTANTS_H/*
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
* Description:   Constants used by the AgnVersit2 Tests
*
*/


// Fixed buffer sizes
const TInt KICalMaxContentLineLength = 100;

//Type
_LIT( KTypeVCal, "vcal" );
_LIT( KTypeICal, "ical" );

//Directory
// FOR ARMV5
_LIT( KInputDirName,  "E://testing//data//unittest//calenimptest//input//" );

// FOR WINSCW enable below path and copy the vcs and ics files to this path
// and comment out the above path
// _LIT( KInputDirName,  "\\logs\\testframework\\" );

// These properties will be excluded from the import / export comparison tests
// as the dates they contain should be generated at run time.
_LIT8(KDtStamp, "DTSTAMP:");
_LIT8(KCreated, "CREATED:");
_LIT8(KExDate, "EXDATE:");
_LIT8(KRDate, "RDATE:");
_LIT8(KLastMod,"LAST-MODIFIED:");

const TUint KNewline('\n');

#endif // CALENIMPTESTCONSTANTS_H
