/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LAUNCHERDLLPARSER_H_
#define LAUNCHERDLLPARSER_H_

#include <e32base.h>
#include <f32file.h>
#include "launcherdllelement.h"

#define UID1_OFFSET 0x00
#define UID2_OFFSET 0x04
#define UID3_OFFSET 0x08
#define SID_OFFSET_ROM 0x44
#define SID_OFFSET_ROFS 0x80
#define CAPABILITY_OFFSET_ROM 0x4c
#define CAPABILITY_OFFSET_ROFS 0x88

class CLauncherDLLParser : public CBase
    {
public:

    virtual ~CLauncherDLLParser();
    static CLauncherDLLParser* NewL();
    static CLauncherDLLParser* NewLC();
    
    /**
     * Reads data from the given DLL-file and sets parsed data to given object.
     * @param aFileSession Reference to an open file session     
     * @param aFile Reference to the DLL file object
     * @param aElement Reference to the element, which will contain the parsed data
     */
    void ParseL( RFs& aFileSession, RFile& aFile, CLauncherDLLElement& aElement );
    
private:

    CLauncherDLLParser();
    void ConstructL();
    
    /**
     * Reads 32-bit unsigned integer from the given file.
     * @return 32-bit unsigned integer value.
     * @param aFile File handle.
     */
    TUint32 ReadUint32L( RFile& aFile );
    };

#endif /* LAUNCHERDLLPARSER_H_ */
