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
* Description:  ?Description
*
*/


#ifndef IRHTTPDATAPROVIDEROBSERVER_H
#define IRHTTPDATAPROVIDEROBSERVER_H

#include <e32cmn.h>

/**
 * MIRHttpDataProviderObserver
 * passes events and responses body data with this interface. 
 * An instance of this class must be provided for construction of 
 * CIRHttpDataProvider.
 */
class MIRHttpDataProviderObserver
    {
public:
    
    /** 
     *  void MIRHttpDataProviderObserver::HttpEventComplete()
     *  Used to indicate HTTP event completion
     */
    virtual void HttpEventComplete() = 0;
    
    /** 
     *  void MIRHttpDataProviderObserver::HttpHeaderReceived()
     *  Used to indicate HTTP header is received
     */
    virtual void HttpHeaderReceived( const TDesC8 &aHeaderData ) = 0;
        
    /** 
     *  HttpDateHeaderReceived();
     *  Used to indicate HTTP header is received
     *  to be set in the request header
     */
    virtual void HttpDateHeaderReceived( const TDesC8 &aHeader,
                                         const TTime& aTime ) = 0;

    /** 
     *  void MIRHttpDataProviderObserver::HttpBodyReceived()
     *  Used to indicate HTTP body is received
     */
    virtual void HttpBodyReceived( const TDesC8 &aBodyData ) = 0;
    
    /** 
     *  void MIRHttpDataProviderObserver::HttpTransactionError()
     *  Used to indicate HTTP Transaction error
     */
    virtual void HttpTransactionError( TInt aErrorCode ) = 0;
    
    /** 
     *  void MIRHttpDataProviderObserver::HttpResponseCodeRecieved(
     *  TInt aResponseCode)
     *  Used to indicate HTTP response code recieved
     */
    virtual void HttpResponseCodeRecieved( TInt aResponseCode ) = 0;
    
    };

#endif // IRHTTPDATAPROVIDEROBSERVER_H




