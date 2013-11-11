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


#include "tcspskypeidparser.h"

const TInt KMinSkypeIdUUILength = 3;
const TInt KMaxSkypeIdUUILength = 33;
const TInt KMaxSkypeIdLength = 32;
const TInt KStartOf31To32EncodingByte = 26;
const TInt KUUIMTDicriminator = 0x44;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TCSPSkypeIdParser::TCSPSkypeIdParser() 
    {
    }

// ---------------------------------------------------------------------------
// Creates buffer to be used by client with parse method.
// ---------------------------------------------------------------------------
//  
HBufC* TCSPSkypeIdParser::CreateSkypeIdBufferL()
    {
    return HBufC::NewL( KMaxSkypeIdLength );
    }
    
// ---------------------------------------------------------------------------
// Parses skype id.
// SkypeID format:
// byte 0: Protocol discriminator: Always 0x00
// byte 1: User-User Information Discriminator.
//         For MT call this has to be 0x44
// Byte 2: First skype id character a.k.a payload. MSB bit is zero.
// Byte 26-33 May have MSB bit set, which are bits of the last character.
// The 26 byte's MSB is the last characters MSB bit.
// ---------------------------------------------------------------------------
//
TInt TCSPSkypeIdParser::Parse( 
    const TDesC& aUUIMessage, 
    TDes& aSkypeId )
    {
    const TInt messageLength( aUUIMessage.Length() );
    TInt error = KErrNone;
    
    const TBool messageSizeOk = (messageLength >= KMinSkypeIdUUILength &&
                                 messageLength <= KMaxSkypeIdUUILength);
                                 
    if( messageSizeOk &&
       ( aUUIMessage[1] ) == KUUIMTDicriminator ) // address type
        {
        
        TUint lastChar = 0;
        
        // skypeId payload starts from the second byte.
        TInt i = 2;
        for( ; i < messageLength; i++ )
            {
            TUint byte = aUUIMessage[i];
            if( i >= KStartOf31To32EncodingByte )
                {
                // 31 to 32 encoding: last character is build from
                // 7 last bytes.
                lastChar = lastChar << 1;
                lastChar |= (byte>>7)&0x01;
                }
            // MSB bit is not part of the character.
            aSkypeId.Append( byte & 0x7F ); 
                
            
            }
            
        // Append the last char only if the aUUIMessage size was the maximum and there
        // is content in the last char.
        if( messageLength == KMaxSkypeIdUUILength && lastChar != 0 )
            {
            aSkypeId.Append(lastChar);
            }

        }
    else
        {
        error = KErrNotSupported;
        }
        
    return error;
    }
