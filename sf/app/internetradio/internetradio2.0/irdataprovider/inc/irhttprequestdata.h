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


#ifndef IRHTTPREQUESTDATA_H
#define IRHTTPREQUESTDATA_H

#include <e32base.h>

// Constants
/**
 * Used to specify the maximum header-field value length
 */
const TInt KMAXHEADERLENGTH = 255;

/**
 * Used to specify the maximum URI length
 */
const TInt KMAXURILENGTH = 256;

// Enumerations
/**
 * Enum specifying the type of HTTP request
 */
enum TIRHttpDataProvider
    {
    EIRHttpGET = 1, EIRHttpPOST
    };

/**
 * Class to hold the request data
 */
class CIRHttpRequestData : public CBase
    {
public:
    
    /** 
     *  void CIRHttpRequestData::CIRHttpRequestData()
     *  Default C++ Constructor
     */
    inline CIRHttpRequestData()
        {
        // Implementation not required
        }
        
    /** 
     *  void CIRHttpRequestData::~CIRHttpRequestData()
     *  Default C++ Destructor
     *     
     */
    virtual inline ~CIRHttpRequestData()
        {
        // Implementation not required        
        }
public:
    
    /**
     * This value indicates type of request method ( GET / POST )
     */ 
    TInt iMethod;
    
    /**
     * IfModifiedSince header info
     */    
    TDateTime iIfModifiedSince;
    
    /**
     * Set as ETrue if the IfModifiedSince header is set
     */ 
    TBool isIfModifiedSet;
    
    /**
     * Used to set the Accept-Language Header
     */ 
    TBuf8 < KMAXHEADERLENGTH > iAcceptLanguage; 
    
    /**
     * Used to set the Content-Type Header
     */
    TBuf8<KMAXHEADERLENGTH> iContentType;

    /**
     * Used to specify the URI
     */
    TBuf8<KMAXURILENGTH> iUri;
    };


class CIRHttpResponseData: public CBase
    {
public:

    /** 
     *  void CIRHttpResponseData::CIRHttpResponseData()
     *  Default C++ Constructor
     *     
     */
    inline CIRHttpResponseData()
        {
        // Implementation not required
        }
        
    /** 
     *  void CIRHttpResponseData::~CIRHttpResponseData()
     *  Default C++ Destructor
     */
    inline ~CIRHttpResponseData()
        {
        // Implementation not required
        }
public:
    
    /**
     * Content type of the data
     */ 
    TBuf8 < KMAXHEADERLENGTH > iContentType;
    
    /**
     * Date header value of the response
     */ 
    TTime iDate;
    
    /**
     * Max age header value
     */
    TBuf8 < KMAXHEADERLENGTH > iMaxAge;
    
    /**
     * Content-Length header value
     */
    TBuf8 < KMAXHEADERLENGTH > iContentLength;

    /**
     * LastModified header value
     */ 
    TTime iLastModified;
    
    /**
     * Expires header value
     */ 
    TBuf8 < KMAXHEADERLENGTH > iExpires;
    
    /**
     * ETag header value, Used in logo downloads
     */
    TBuf8 < KMAXHEADERLENGTH > iETag;
    
    };

#endif // IRHTTPREQUESTDATA_H




