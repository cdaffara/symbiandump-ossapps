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
* Description:  MDS to CPIX Document fetcher 
 *
*/


#ifndef CPIXDOCFETCHER_H_
#define CPIXDOCFETCHER_H_

#include <e32def.h>
#include <e32base.h>
#include <f32file.h>
#include "cdocumentfield.h"

//Forward declaration
class CSearchDocument;
class CMdEObject;
class CMdEObjectDef;

#define CPIX_MIMETYPE_FIELD      L"_mimetype"
#define LCPIX_MIMETYPE_FIELD     L"_mimetype"
const TInt KInitialExcerptLength = 5;
const TInt KStartPosition = 2; //Start position of absolute path
const TInt KDrivePosition = 0; //Drive position in object Uri
const TInt KMaxMediaLength = 14;
const TInt KMaxExtLength = 5;
//To unsigned decimal representation as UIds are out of range for signed decimal
_LIT(KFormat, "%u");
_LIT(KNameField, "Name");
_LIT(KMediaIdField, "MediaId");
_LIT(KExtensionField, "Extension");
_LIT(KExcerptDelimiter, " ");


class CCPIXDocFetcher : public CBase
    {
public:
    /**
     * Construction
     * @return instance of CCPIXDocFetcher
     */
    static CCPIXDocFetcher* NewL();
    /**
     * Construction
     * @return instance of CCPIXDocFetcher
     */
    static CCPIXDocFetcher* NewLC();
    /**
     * Destructor
     */    
    virtual ~CCPIXDocFetcher();
    /**
     * Get the search document from MDE object
     * @param CMdEObject& aObject mde object
     * @param TDesC& aAppClass app class require to create cpix 
     * @param CMdEObjectDef& aObjectDef object def of passed object
     * @return instance of CSearchDocument document
     */
    //Process base object properties here and let specific media handler "video" "music"
    //Overwride this in order to handle specific items client should call this method
    // from overwridden method to get common entries
    virtual CSearchDocument* GetCpixDocumentL(const CMdEObject& aObject, 
                                              const TDesC& aAppClass,
                                              CMdEObjectDef& aObjectDef);
    /**
     * Utility method Add field to created document
     * @param CSearchDocument& aSearchDoc search document
     * @param TDesC& aFiledName filed name to be added 
     * @param CTDesC& aValue field value
     * @return none
     */    
    void AddFiledtoDocumentL(CSearchDocument& aSearchDoc,const TDesC& aFiledName,
                             const TDesC& aValue,
                             TInt aConfig = CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);
    
    /*
     * Get the Absolute URI without path 
     * @return TDesC& URI absolute path
     */
    const TDesC& GetUri();
    
    /*
     * Convert Media ID from integer to descriptor for storage
     * @param aMediaId media id in integer
     * @param aMediaIDBuf reference of descriptor buffer to be filled
     */
    void GetMediaIdDescriptor(const TUint32& aMediaId,TDes& aMediaIDBuf);
    /*
     * Get the Media ID from the filesystem and convert Media ID from integer to descriptor for storage
     * @param TDesC& aURI Uri of the object 
     * @param aMediaIDBuf reference of descriptor buffer to be filled
     */    
    void GetMediaIdDescriptorFromFsL(const TDesC& aUri,TDes& aMediaIDBuf);
    /*
     * Get the file extension from uri
     * @param TDesC& aURI Uri of the object 
     * @param aExtension reference of descriptor buffer to be filled with file extension
     */
    void GetExtension(const TDesC& aUri, TDes& aExtension);
    
    /*
     * Add the value to excerpt field
     * @param TDesC& aExcerptValue excerpt value
     */
    void AddToFieldExcerptL(const TDesC& aExcerptValue);
protected:
    /*
     * second phase constructor ConstructL
     */
    void ConstructL();
    /*
     * Default constructor
     */
    CCPIXDocFetcher(); //Default constructor
    /*
     * reset the excerpt
     */
    void ResetExcerpt();
    
    // Excerpt field
    HBufC* iExcerpt;
private:
    /*
     * Convert URI to Absolute URI
     * exp. from f:\music\something.mp3 will be stored as \music\something.mp3
     */
    void SetAbsoluteURI(const TDesC& aUri);
private:
    // File system handle
    RFs iFs;
    TBuf<KMaxFileName> iUri; //Absolute URI of MDS Items  
    };

#endif /* CPIXDOCFETCHER_H_ */
