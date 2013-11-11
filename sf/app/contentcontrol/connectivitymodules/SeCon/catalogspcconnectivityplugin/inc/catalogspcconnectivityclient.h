/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is a client which communicate with the server 
*                CatalogsEnginePCClientFrontEnd 
*
*/


#ifndef R_CATALOGSPCCONNECTIVITYCLIENT_H
#define R_CATALOGSPCCONNECTIVITYCLIENT_H

#include <e32base.h>
#include <apmstd.h>

/**
 *  This client communicate with the server CatalogsEnginePCClientFrontEnd
 *
 *  This class get requests from CatalogsPCConnectivityPlugin. 
 *  Class itself is client in client-server solution. When request 
 *  is received it will start the server (CatalogsEnginePCClientFrontEnd). 
 *  This client class is dummy, the server have all the functionality.
 *
 *  @lib catalogspcconnectivityplugin
 *  @since S60 v3.1
 */
class RCatalogsPCConnectivityClient : public RSessionBase
    {
public:

    /**
     * Constructor
     *
     * @since S60 v3.1
     */
    RCatalogsPCConnectivityClient();

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    ~RCatalogsPCConnectivityClient();

    /**
     * A connection is formed to the Catalogs engine pc client front-end.
     * This will start up the front-end server and connect to it 
     *
     * @since S60 v3.1
     * @return TInt Standard error code, KErrNone if connection is formed
     *         or the connection was formed already     
     */
    TInt Connect();

    /**
     * The connection to the Catalogs engine front-end is closed
     *
     * @since S60 v3.1
     */
    void Close();

    /**
     * All MIME types supported by this component can be asked using this method
     *
     * @since S60 v3.1
     * @return RPointerArray< TDataType > contain all supported MIME types
     *         RPointerArray TDataTypes have allocated using new, caller
     *         have to remember to delete them properly
     * @exeption Leaves with standard error codes
     */
    RPointerArray<TDataType> SupportedMimeTypesL();

    /**
     * A data packet of max length 65535 (0xFFFF) bytes is synchronously
     * sent to the Catalogs engine front-end
     * Usage: After PutDataL have been called, you have to call GetDataL
     *        before you can call PutDataL again
     *
     * @since S60 v3.1
     * @param aMimeType data packet MIME type
     * @param aData data buffer to read from
     * @exeption Leaves with standard error codes
     */
    void PutDataL( const TDataType& aMimeType, 
        const CBufFlat& aData );

    /**
     * A data packet is synchronously transfered from the Catalogs front-end
     * and returned to caller 
     * Usage: Everytime before calling GetDataL, you have to call PutDataL.
     *        So you can't call GetData in sequentially.
     *
     * @since S60 v3.1
     * @param aMimeType data packet MIME type
     * @param aData data buffer to write to. Will call ExpandL function
     *        for aData, but never more than max length 65535 (OxFFFF)
     * @return nothing, but parameter aData work as return buffer
     * @exeption Leaves with standard error codes
     */                                                          
    void GetDataL( TDataType& aMimeType, CBufFlat& aData );

    
private:

// private functions

    /**
     * Returns the earliest version number of the server that we can talk to
     *
     * @since S60 v3.1
     * @return TVersion Earliest version number we can talk to
     */
    TVersion Version() const;

    /**
     * Start server if it isn't running already
     *
     * @since S60 v3.1
     * @return TInt Standard error code, KErrNone if server was running
     *         already or start was succesfull     
     */
    TInt StartServer();

    /**
     * Create server process
     *
     * @since S60 v3.1
     * @return TInt standard error code, KErrNone when creation was success
     */
    TInt CreateServerProcess();

    };

#endif // R_CATALOGSPCCONNECTIVITYCLIENT_H

