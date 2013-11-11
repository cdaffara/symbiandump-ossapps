/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Parses Skype id from UUI message.
*
*/


#ifndef TCSPSKYPEIDPARSER_H
#define TCSPSKYPEIDPARSER_H

#include <e32std.h>

/**
 *  Skype id parser.
 *
 *  @lib phoneengine.lib
 *  @since S60 v3.2
 */
class TCSPSkypeIdParser 
    {
public:

    TCSPSkypeIdParser();
    
    /**
     * Creates right size buffer to be used with parse method.
     * 
     * @since S60 v3.2
     * @return Buffer. Ownership given to caller.
     */
    static HBufC* CreateSkypeIdBufferL();
    

    /**
     * Parses Skype if from UUI message.
     *
     * @since S60 v3.2
     * @param aUUIMessage Message.
     * @param aSkypeId Parser skype id is appended here.
     * @return Error code. KErrNotSupported if data is not understood by
     * parser.
     */
    TInt Parse( const TDesC& aUUIMessage, TDes& aSkypeId );
    

private: // data

    };

#endif // CSPSKYPEIDPARSER_H
