/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef IRCHANNELSERVERURL_H
#define IRCHANNELSERVERURL_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
 * This class provides the basic structure to hold the preset's url data
 * information that come from the isds server.
 * It also provides the basic getter and setter functions to 
 * manage the preset's url information.
 * for url selection for a particular bitrate some functionality 
 * has been provided
 * copy constructor and assignment operator has been provided to 
 * override the default ones 
 *
 * @code
 * CIRChannelServerUrl* presetUrl = CIRChannelServerUrl::NewL()
 * preset->SetXXX();
 * preset->GetXXX();
 * @endcode
 *   
 */
class CIRChannelServerUrl:public CBase
    {
public:
    
    /**
     * CIRChannelServerUrl::NewL()
     * Static function
     * standard two phased constructor
     * @return *CIRChannelServerUrl
     */
    IMPORT_C static CIRChannelServerUrl* NewL();
    
    /**
     * CIRChannelServerUrl::NewLC()
     * Static function
     * standard two phased constructor
     * @return *CIRChannelServerUrl
     */
    IMPORT_C static CIRChannelServerUrl* NewLC();
    
    /**
     * CIRChannelServerUrl::SetServerName()
     * sets server name 
     * @param TDesC
     */
    IMPORT_C void SetServerName( const TDesC& aServerName );
    
    /**
     * CIRChannelServerUrl::SetServerUrl()
     * sets server url 
     * @param TDesC
     */
    IMPORT_C void SetServerUrl( const TDesC& aServerUrl );
    
    /**
     * CIRChannelServerUrl::SetBitRate()
     * sets server bitrate 
     * @param TInt
     */
    IMPORT_C void SetBitRate( TInt aBitrate );
    
    /**
     * CIRChannelServerUrl::GetServerName()
     * gets server name 
     * @return TDesC
     */
    IMPORT_C const TDesC& GetServerName() const;
    
    /**
     * CIRChannelServerUrl::GetServerUrl()
     * gets server url 
     * @return TDesC
     */
    IMPORT_C const TDesC& GetServerUrl() const;
    
    /**
     * CIRChannelServerUrl::GetBitRate()
     * gets server bitrate
     * @return TInt
     */
    IMPORT_C TInt GetBitRate() const;
    
    /**
     * CIRChannelServerUrl::~CIRChannelServerUrl()
     * standard C++ destructor 
     */    
    ~CIRChannelServerUrl();
    
    /**
     * CIRChannelServerUrl::=()
     * standard C++ copy constructor(deep copy) 
     * @param CIRChannelServerUrl
     */    
    IMPORT_C CIRChannelServerUrl& operator=( const CIRChannelServerUrl& churl );
        
    /**
     * CIRChannelServerUrl::ExternalizeL()
     * function to externalize url data
     * @param CIRChannelServerUrl
     */
    IMPORT_C void ExternalizeL( RWriteStream& aWriteStream );    
    
    /**
     * CIRChannelServerUrl::InternalizeL()
     * function to internalize url data
     * @param CIRChannelServerUrl
     */
    IMPORT_C void InternalizeL( RReadStream& aReadStream );

protected:
    /**
     * CIRChannelServerUrl::ConstructL()
     * standard second phase construction
     */    
    void ConstructL() const;
    
public:
    //server name
    HBufC *iServerName;
    HBufC *iURL;
    //bit rate
    TInt iBitrate;
    };
    
#endif //IRCHANNELSERVERURL_H

