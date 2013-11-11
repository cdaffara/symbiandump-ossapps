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
* Description:  The implementation for presentation elements.
*
*/


#ifndef IRXMLCONTENTHANDLER_H
#define IRXMLCONTENTHANDLER_H

#include <contenthandler.h>

class CIRAdvertisement;
class CIRBrowseCatagoryItems;
class CIRBrowseChannelItems;
class CIRCacheMgmt;
class CIRHttpResponseData;
class CIRIsdsPreset;
class CIROTAUpdate;
class MIRCacheObserver;
class MIRParserObserver;

namespace Xml
    {
    class CParser;
    }

//constant declaration   
_LIT8(KFile,"text/xml");    
const TInt KSessionLogResponse = 5;

using namespace Xml;

/**
* Parse data from xml file
*
* @code 
*
* classA* ParserObserver;//class classA : public MIRParserObserver
* classB* CacheObserver;//class classB : public MIRCacheObserver
* CIRHttpResponseData* ResponseHeader;
* CIRHttpResponseData* parser = CIRXMLContentHandler::
* NewL (*ParserObserver,*CacheObserver); 
* parser->ParseXmlL(FilePath,CachePath,ResponseHeader);
*
* @endcode
*
*/

class CIRXMLContentHandler : public CBase,
                        public MContentHandler
    {
public:             
    /**
    * Gets the Filepath of the XML file
    * @param aFilePath Contains the filepath of the file
    * @return ErrorCode if the file cannot be opened
    */
  IMPORT_C void ParseXmlL( const TDesC& aFilePath, const TDesC& aCachePath,
             const CIRHttpResponseData& aResponseHeaders );

    /**
    * Default Constructor
    * @param aObserver Contains the reference to the MClass Observer
    */    
    IMPORT_C static CIRXMLContentHandler* NewL 
    ( MIRParserObserver &aObserver, MIRCacheObserver &aCacheObserver );       
    /**
    * Destructing the object
    */  
    ~CIRXMLContentHandler();
    /**
    * This method is a callback to indicate the start of the document
    * @param aDocParam Specifies the various parameters of the document
    * @param aErrorCode KErrNone if successfull. 
    * @return None
    */         
    virtual void OnStartDocumentL( const RDocumentParameters &aDocParam,
       TInt aErrorCode );
    /**
    * This method is a callback to indicate the end of the document
    * @param aErrorCode KErrNone if successfull. 
    * @return None
    */                  
    virtual void OnEndDocumentL( TInt aErrorCode );
    /**
    * This method is a callback to indicate an element has been parsed. 
    * @param aElement is a handle to the element's details. 
    * @param aAttributes contains the attributes for the element
    * @param aErrorCode KErrNone if successfull. 
    */    
    virtual void OnStartElementL( const RTagInfo &aElement, 
      const RAttributeArray &aAttributes, TInt aErrorCode );
    /**
    * This method is a callback to indicate the end of 
    * the element has been reached. 
    * @param aElement is a handle to the element's details. 
    * @param aErrorCode KErrNone if successfull. 
    */   
    virtual void OnEndElementL( const RTagInfo &aElement, TInt aErrorCode );
    /**
    * This method is a callback that sends the content of the element.
    * The data may be sent in chunks
    * @param aBytes is the raw content data for the element. 
    * @param aErrorCode KErrNone if successfull. 
    */  
    virtual void OnContentL( const TDesC8 &aBytes, TInt aErrorCode );
    /**
    * This method is a notification of the beginning of the scope 
    *of a prefix-URI Namespace mapping.
    * @param aPrefix is the Namespace prefix being declared 
    * @param aUri is the Namespace URI the prefix is mapped to
    * @param aErrorCode KErrNone if successfull. 
    */  
    virtual void OnStartPrefixMappingL( const RString &aPrefix, 
      const RString &aUri, TInt aErrorCode );
    /**
    * This method is a notification of the end of the scope 
    * of a prefix-URI mapping
    * @param aPrefix is the Namespace prefix that was mapped. 
    * @param aErrorCode KErrNone if successfull. 
    */  
    virtual void OnEndPrefixMappingL( const RString &aPrefix, TInt aErrorCode );
    /**
    * This method is a notification of ignorable whitespace in element content. 
    * @param aBytes are the ignored bytes from the document being parsed.. 
    * @param aErrorCode KErrNone if successfull. 
    */  
    virtual void OnIgnorableWhiteSpaceL( const TDesC8 &aBytes, TInt aErrorCode );
    /**
    * This method is a notification of a skipped entity
    * @param aName is the name of the skipped entity. 
    * @param aErrorCode KErrNone if successfull. 
    */ 
    virtual void OnSkippedEntityL( const RString &aName, TInt aErrorCode );
    /**
    * This method is a receive notification of a processing instruction.
    * @param aTarget is the processing instruction target. 
    * @param aData is the processing instruction data. 
    * If empty none was supplied.. 
    * @param aErrorCode KErrNone if successfull. 
    */ 
    virtual void OnProcessingInstructionL( const TDesC8 &aTarget, 
      const TDesC8 &aData, TInt aErrorCode );
    /**
    * This method indicates an error has occurred.
    * @param aErrorCode contains the errorcode. 
    */ 
    virtual void OnError( TInt aErrorCode );
    /**
    * This method obtains the interface matching the specified uid. 
    * @param aUid the uid identifying the required interface 
    * @return 0 if no interface matching the uid is found. 
    * Otherwise, the this pointer cast to that interface
    */ 
    virtual TAny *GetExtendedInterface( const TInt32 aUid ); 
    /**
    * This method maps the errorcode
    * @param aError conatins the errorcode
    * @return the type of the error
    */ 
    TInt ParseError( TInt aError );
    /**
    * This method stringpool handle
    * @param aError conatins the errorcode
    * @return the type of the error
    */ 
    RStringPool& StringPool ();
    
    void ResetValues();

protected:
    
    /**
    * Default Constructor
    * @param aObserver Contains the reference to the MClass Observer
    */  
    CIRXMLContentHandler( MIRParserObserver &aObserver,
        MIRCacheObserver &aCacheObserver );

    /**
    * Two-Phase Constructor
    */
    void ConstructL();
    
   private :
    
    // Buffer for the URL
    HBufC* iBufGetOperation;
    
    // Buffer for Size
    HBufC* iSize;  
  
    // Count of the bitrates
    TBool iIRID;

    TInt iBitrateCounter;   
    
    // Indicating for Channels/Preset
    TInt iFlag;
    
    // Indicating for Channels
    TInt iChannelFlag;
    
    // Bool Value to indicate the class
    TBool iPresets;
    TBool iCategories;
    TBool iCat;
    TBool iChannels;
    TBool iChan;
    TBool iAdvertisement;
    TBool iOTAUpdate;
    TBool iTimeStamp; //checks the header has a time stamp
    TInt  iEOD;
    // Pointer to the PresetData
    CIRIsdsPreset *iPreset;
    
    // Pointer to the CategoryData
    CIRBrowseCatagoryItems *iCategory;
    
    // Pointer to the ChannelData
    CIRBrowseChannelItems *iChannel;
    
    // Pointer to the Advertisement
    
    CIRAdvertisement *iAdv;
    
    //! An Handle to Stringpool
      
    RStringPool    iStringPool;
    
    //! Reference to the CallBack Class
    
    MIRParserObserver& iParseObserver;
    
    CParser* iParser;

    //Caching 
    CIRCacheMgmt *iCache;
    
    MIRCacheObserver &iCacheObserver;
    
    TBuf<256> iFileToCache;

    // For Advertisement
    TBool iAdvts;
    TBool iAdvertisementTagFlag;
    HBufC*   iUrlContentForAds;  //to store the content of the tag



public:

    // Count of the bitrates
    
    TBuf<30>      iRID;

    // Array of Pointers to PresetClass
    
    CArrayPtrFlat<CIRIsdsPreset>* iPtrPreset;
    
    // Array of Pointers to CategoryClass
    
    CArrayPtrFlat<CIRBrowseCatagoryItems>* iPtrCategory;
    
    // Array of Pointers to BrowseCategoryClass
    
    CArrayPtrFlat<CIRBrowseChannelItems>* iPtrChannel;
    // Pointer to the OTAUpdate
    
    CIROTAUpdate *iOTA;
    
    
      
};

#endif //IRXMLCONTENTHANDLER_H
