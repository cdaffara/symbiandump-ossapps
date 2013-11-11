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
* Description:  COMADSFolderObject class implementation
*
*/



#ifndef __OMADSFOLDEROBJECT_H__
#define __OMADSFOLDEROBJECT_H__

#include <e32base.h>

const TInt KMaxElementLength = 255;
const TInt KMaxXmlLineLength = 1024;

class COMADSFolderObject : public CBase
    {
public:

    static COMADSFolderObject* NewLC();
    ~COMADSFolderObject();

    // Set methods
    inline void SetName( const TDesC& aName ) {iName.Copy(aName);};
    inline void SetCreatedDate( const TDateTime& aDateTime ) {iCreatedDate = aDateTime;};
    inline void SetModifiedDate( const TDateTime& aDateTime ) {iModifiedDate = aDateTime;};
    
    void ExportFolderXmlL( CBufBase& aBuffer );
    
private:
    void ExportL(); 
    void WriteL( const TDesC &aData );
    
private:
    
    TBuf8<KMaxXmlLineLength>    iTemp;
    TPath                       iName;
    TDateTime                   iCreatedDate;
    TDateTime                   iModifiedDate;
    CBufBase*                   iBuffer; // Not owned
    TInt                        iWriteBufPosition;
    TInt                        iWriteBufSize;
    };

#endif
