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
* Description: stub CMPXMedia for unit testing CVideocollectionClient
*
*/

#ifndef CMPXMEDIA_H
#define CMPXMEDIA_H

// INCLUDES
#include <s32strm.h>
#include <mpxcmn.h>
#include <mpxattribute.h>

class CMPXMedia : public CBase
{
public:

    /**
     * Returns new object 
     */
    static CMPXMedia* NewL() {return new CMPXMedia();};

    /**
     * Constructor.
     */
    CMPXMedia() {};
    
    /**
     * Destructor.
     */
    ~CMPXMedia() {};

 public:

    /**
     * NOP  
     */
    template<typename T>
    inline void SetTObjectValueL(const TMPXAttribute& /*aAttribute*/, T /*aValue*/)
    {
        // NOP
    }
    
    /**
     * NOP  
     */
    template<typename C>
    inline void SetCObjectValueL(const TMPXAttribute& /*aAttribute*/,C* /*aValue*/)
    {
            // NOP
    }
    
    /**
     * NOP
     */
    template<typename T>
    inline T ValueTObjectL(const TMPXAttribute& /*aAttribute*/) const
    {
        if(mValueTObjectLeaves)
        {
            User::Leave(KErrGeneral);
        }
        return mIdFromValueTObject;
    }

    /**
     * NOP
     */
    inline void SetTextValueL(const TMPXAttribute& /*aAttribute*/,
                                     const TDesC& /*aValue*/)
    {
        // NOP
    }
    
    /**
     * NOP
     */
    inline bool IsSupported(const TMPXAttribute& /*aAttribute*/)
    {
        // NOP
        return mIsSupported;
    }
    
    static bool mIsSupported;
    
    static bool mValueTObjectLeaves;

    static TMPXItemId mIdFromValueTObject;
    
};


#endif // CMPXMEDIA_H
