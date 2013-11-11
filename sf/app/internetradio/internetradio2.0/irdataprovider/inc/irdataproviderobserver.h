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


#ifndef IRDATAPROVIDEROBSERVER_H
#define IRDATAPROVIDEROBSERVER_H

#include <e32cmn.h>

class CIRHttpResponseData;

/**
 * MIRDataProviderObserver
 * passes events and responses body data with this interface. 
 * An instance of this class must be provided for construction of 
 * CIRDataProvider.
 */

class MIRDataProviderObserver
    {
public:

    /** 
     *  void MIRDataProviderObserver::IRHttpGeneralError(TInt aErrCode)
     *  Used to determine the Private path of IRAPP
     *     
     */
    virtual void IRHttpGeneralError( TInt aErrCode ) = 0;
    
    /** 
     *  void MIRDataProviderObserver::IRHttpDataReceived(TDesC& aXmlPath)
     *  Used to determine the Private path of IRAPP
     */
    virtual void IRHttpDataReceived( const TDesC &aXmlPath,
            const CIRHttpResponseData& aResponseHeaders ) = 0;
    /** 
     *  void MIRDataProviderObserver::IRHttpContentNotChanged()
     *  Used to determine the Private path of IRAPP
     */
    virtual void IRHttpResponseCodeReceived( TInt aResponseCode,
            CIRHttpResponseData& aResponseHeaders  ) = 0;
    };
    
#endif // IRDATAPROVIDEROBSERVER_H


