/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  OMA DS Folder object XML parser
*
*/


#ifndef __OMADSFOLDEROBJECT_H__
#define __OMADSFOLDEROBJECT_H__

#include <SenXmlReader.h>
#include <MSenContentHandlerClient.h>
#include "mmsdataproviderdefs.h"

const TInt KMaxElementLength = 255;
const TInt KMaxXmlLineLength = 1024;

class COMADSFolderObject : public CBase, public MSenContentHandlerClient
    {
public:

    static COMADSFolderObject* NewLC();
    static COMADSFolderObject* NewL();
    ~COMADSFolderObject();

    // Get methods
    inline const TDesC& GetName() const {return iName;}
    inline const TDateTime& GetCreatedDate() const {return iCreatedDate;}
    inline const TDateTime& GetModifiedDate() const {return iModifiedDate;}

    // Set methods
    inline void SetName(const TDesC& aName) {iName.Copy(aName);}
    inline void SetCreatedDate(const TDateTime& aDateTime) {iCreatedDate = aDateTime;}
    inline void SetModifiedDate(const TDateTime& aDateTime) {iModifiedDate = aDateTime;}

    // Import & export
    TInt ImportFolderXml(const TDesC8& aData);
    TInt ImportFolderXml(RFs& aFs, const TDesC& aFilename);
    void ExportFolderXmlL(TDes8& aBuffer);
    void ExportFolderXmlL(CBufBase& aBuffer);    

    // MSenContentHandlerClient virtual functionality
    TInt StartDocument();
    TInt EndDocument();
    TInt StartElement(const TDesC8& aURI, 
    				  const TDesC8& aLocalName, 
    				  const TDesC8& aName, 
    				  const RAttributeArray& apAttrs);
    TInt EndElement(const TDesC8& aURI, 
    				const TDesC8& aLocalName, 
    				const TDesC8& aName);
    TInt Characters(const TDesC8& aBuf, TInt aStart, TInt aLength);
    TInt Error(TInt aErrorCode);

private:

    CSenXmlReader* iXmlReader;
    void ConstructL();
    TBuf8<KMaxElementLength> iCurrentElement;
    TBuf8<KMaxXmlLineLength> iTemp;

    TBuf<KMaxFolderNameLength> iName;
    TDateTime iCreatedDate;
    TDateTime iModifiedDate;
    TInt iError;

    void Reset();
    TInt ParseDateString(const TDesC8& aString, TDateTime& aDateTime);	

    CBufBase* iBuffer;
    TInt iWriteBufPosition;
    TInt iWriteBufSize;
    TDes8* iDesc;

    void ExportL();	
    void WriteL(const TDesC &aData);
    void WriteEncodedXmlL(const TDesC &aData);
    };

#endif
