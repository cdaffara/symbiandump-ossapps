/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CGI parser and generator
*
*/



// INCLUDE FILES
#include "nsmlcgiscriptparser.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParseStateInfo::TNSmlCGIScriptParseStateInfo
// -----------------------------------------------------------------------------
//
TNSmlCGIScriptParser::TNSmlCGIScriptParseStateInfo::TNSmlCGIScriptParseStateInfo(
    const TDesC* aCGIScript,
    const CArrayPtr<TNSmlDataTypesForCGIScriptNames>* aDatatypes) : 

    iState(ENSmlCGIScriptParseStateKeyWord), 
    iNextState(ENSmlCGIScriptParseStateKeyWord),
    iCGIScript(aCGIScript),
    iDatatypes(aDatatypes),
    iStartPos(0),
    iCurrPos(0){}

//  End of File  
