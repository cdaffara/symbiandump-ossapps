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
* Description:  This plugin enables the Catalogs PC client to communicate 
*                with the Catalogs engine
*
*/


#ifndef C_CATALOGSPCCONNECTIVITYPLUGIN_H
#define C_CATALOGSPCCONNECTIVITYPLUGIN_H

#include "catalogspcconnectivityclient.h"

/**
 *  Plugin enables Catalogs PC client to communicate with the Catalogs engine
 *
 *  This class get requests from Catalogs PC client. It passes the requests
 *  to client CatalogsPCConnectivityClient
 *
 *  @lib catalogspcconnectivityplugin
 *  @since S60 v3.1
 */
class CCatalogsPCConnectivityPlugin : public CBase
    {
public:

    /**
     * Standard symbian object construction first phase
     *
     * @since S60 v3.1
     * @return CCatalogsPCConnectivityPlugin pointer
     */
    IMPORT_C static  CCatalogsPCConnectivityPlugin* NewL();

    /**
     * Standard symbian object constructing first phase
     *
     * @since S60 v3.1
     * @return CCatalogsPCConnectivityPlugin pointer
     */
    IMPORT_C static  CCatalogsPCConnectivityPlugin* NewLC();

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    IMPORT_C virtual ~CCatalogsPCConnectivityPlugin();

    /**
     * A connection is formed to the Catalogs engine pc client front-end.
     * This will start up the front-end server and connect to it 
     *
     * @return TInt Standard error code, KErrNone if connection is formed
     * @since S60 v3.1
     */
    IMPORT_C TInt Connect();

    /**
     * The connection to the Catalogs engine front-end is closed
     *
     * @since S60 v3.1
     */
    IMPORT_C void Close();

    /**
     * All MIME types supported by this component can be asked using this method
     *
     * @return RPointerArray<TDataType> contains all supported MIME types. 
     *         The ownership is transferred to the caller.
     *         Use ResetAndDestroy method to free all allocated memory
     * @exeption Leaves with standard error codes
     * @since S60 v3.1
     */
    IMPORT_C RPointerArray<TDataType> SupportedMimeTypesL();

    /**
     * A Data packet of max length 65535 (0xFFFF) bytes is synchronously
     * sent to the Catalogs engine front-end.
     * Usage: After PutDataL has been called, you must call GetDataL
     *        before you can call PutDataL again
     *
     * @param aMimeType Data packet MIME type
     * @param aData Data buffer to read from
     * @exeption Leaves with standard error codes
     *           KErrNotSupported if not supported MIME type
     * @since S60 v3.1
     */
    IMPORT_C void PutDataL( const TDataType& aMimeType, 
                            const CBufFlat& aData );

    /**
     * A data packet is synchronously transfered from the Catalogs front-end
     * and returned to caller
     * Usage: Everytime before calling GetDataL, you have to call PutDataL.
     *        You may not call GetData in sequentially.
     *
     * @since S60 v3.1
     * @param aMimeType MIME type of the received data packet
     * @param aData Data buffer to write to. ExpandL function will be called
     *        with the correct size for aData, but not more than max length
     *        65535 (OxFFFF)
     * @return The reference parameters contain the return data
     * @exeption Leaves with standard error codes
     */                                                          
    IMPORT_C void GetDataL( TDataType& aMimeType, CBufFlat& aData );
    
    
private:

// private functions


    /**
     * Constructor
     *
     * @since S60 v3.1
     */
    CCatalogsPCConnectivityPlugin();

    /**
     * Standard symbian object construction second phase
     */
    void ConstructL();


private: // data

    /**
     * Client that communicate with the CatalogsEnginePCClientFrontEnd
     */
    RCatalogsPCConnectivityClient iClient;
    
    };

#endif // C_CATALOGSPCCONNECTIVITYPLUGIN_H
