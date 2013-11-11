/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File contains defines, and literal strings of global scope.  
*
*/


#ifndef RSCONSTANTS_H
#define RSCONSTANTS_H

#include <e32std.h>

const TInt KMaxPrintedPathLength=16;
const TInt KMinWidthOrHeight=200;
const TInt KStickersHelpPage = 1;
const TInt KImagesHelpPage = 2;


// log to file of heap useage
//#define _HEAP_LOGGING_ 1

// file logging EX-cluding mimeout.txt
//#define _LOGGING_ 1

// Out of memory tests when generating XHtml and sending
//#define __OOM_TEST_SENDING__ 1

// Allows debugging of the mime package and print job on the emulator with no bluetooth present.
//#define __PRINT_TO_FILE__ 1

// mimeout.txt logging
//#define _MIMEOUT_LOGGING_ 1

#ifdef __PRINT_TO_FILE__
#ifndef _MIMEOUT_LOGGING_
#define _MIMEOUT_LOGGING_ 1
#endif
#endif // __PRINT_TO_FILE__

/// Extension for temporary files.
_LIT(KTmpExtension, ".tmp");

#endif // RSCONSTANTS_H

//  End of File
