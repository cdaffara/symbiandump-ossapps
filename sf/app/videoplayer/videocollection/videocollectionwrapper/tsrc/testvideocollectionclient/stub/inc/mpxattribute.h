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
* Description:  stub mpx attribute classes for testing CVideoCollectionClient
*
*/


#ifndef MPXATTRIBUTE_H
#define MPXATTRIBUTE_H

#include <s32strm.h>


enum TMPXAttributeType
    {
    EMPXTypeUnknown,
    EMPXTypeTInt,
    EMPXTypeText,
    EMPXTypeTObject,
    EMPXTypeCObject,
    EMPXTypeError
    };



class TMPXAttributeData
{
public:
    /**
     * Overloadded operator&.
     */
    TBool operator& (const TMPXAttributeData& aData) const
    {
        return iAttributeId & aData.iAttributeId;
    }
    /**
     * Overloadded operator&.
     *
     */
    TBool operator& (TUint& aData) const
    {
        return iAttributeId & aData;
    }
    
    /**
     * Overloadded operator|.
     *
     */
     TMPXAttributeData operator| (const TMPXAttributeData& aData) const
     {
         TMPXAttributeData ret={iContentId, iAttributeId | aData.iAttributeId};
         return ret;
     }
     
public:
    TInt iContentId;
    TUint iAttributeId;
};


class TMPXAttribute
    {
public:     // Constructors and destructor
    /**
     * contructor
     */
    TMPXAttribute()
    {
        iData.iContentId=0;
        iData.iAttributeId=0;
    }

    /**
     * contructor
     */
    TMPXAttribute(TInt aContentId, TUint aAttributeId)
    {
        iData.iContentId=aContentId;
        iData.iAttributeId=aAttributeId;
    }

    /**
     * copy contructor
     */
    TMPXAttribute(const TMPXAttribute& aId)
    {
        iData.iContentId=aId.ContentId();
        iData.iAttributeId=aId.AttributeId();
    }

    /**
     * Copy constructor.
     */
    TMPXAttribute(const TMPXAttributeData& aData) : iData(aData)
    {        
    }

public:
    
    /**
     * Overloaded assignment operator.
     */
    TMPXAttribute& operator=(const TMPXAttribute& aId)
    {
        if (this != &aId)
        {
            iData.iContentId = aId.ContentId();
            iData.iAttributeId = aId.AttributeId();
        }
        return *this;
    }

    /**
    * Overloaded equal operator.
    */
    TBool operator==(const TMPXAttribute& aId) const
    {
        TBool ret = EFalse;
        if (iData.iAttributeId == aId.iData.iAttributeId && 
            iData.iContentId == aId.iData.iContentId)
        {
            ret = ETrue;
        }
        return ret;

    }

public:

    /**
    * Get content id.
    */
    TInt ContentId() const {return iData.iContentId;}

    /**
    * Attribute id.
    */
    TUint AttributeId() const {return iData.iAttributeId;}


private:     // Data
    TMPXAttributeData iData;
};

const TMPXAttributeData KMPXMediaNullAttribute={0x0,0x00};

#endif   // MPXATTRIBUTE_H

// End of File
