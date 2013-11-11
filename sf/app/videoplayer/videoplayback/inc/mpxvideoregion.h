/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Externalizable container for DSA region
*
*/

// Version : %version: 4.1 %


 

#ifndef CMPXVIDEOREGION_H
#define CMPXVIDEOREGION_H


#include <e32base.h>
#include <s32strm.h>

class CMPXVideoRegion: public CBase 
{  
public:    
    static CMPXVideoRegion* NewL();
    static CMPXVideoRegion* NewLC();
    ~CMPXVideoRegion();

    inline void Print();
    inline void SetRegion( RRegion& aRegion );
    inline TRegion& GetRegion();
   
    inline void ExternalizeL( RWriteStream& aStream ) const;
    inline void InternalizeL( RReadStream& aStream );
    
private:
    CMPXVideoRegion(); 
    void ConstructL();
    
private:
    RRegion* iRegion;
};

#include "mpxvideoregion.inl"

#endif // CMPXVIDEOREGION_H
