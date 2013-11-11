/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  plugin request handler
*
*/


#ifndef CFSMAILREQUESTHANDLER_H
#define CFSMAILREQUESTHANDLER_H

//  INCLUDES
//<cmail>
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailPluginData.h"

// <qmail>
// Entire class is now exported
#include "emailcommondef.h"
// </qmail>

// FORWARD DECLARATIONS
class CFSMailRequestObserver;

/**
 *  asynchronous request progress indicator
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class TFSPendingRequest
    {

public:

    /** request status */
    enum TFSRequestStatus
    {
        EFSRequestPending = 1,
        EFSRequestComplete,
        EFSRequestCancelled,
    };

public: // data

    /** request id */
    TInt                       iRequestId;

    /** plugin uid */
    TUid                       iPluginId;

    /** request status */
    TFSRequestStatus           iRequestStatus;

    /** request observer */
    CFSMailRequestObserver*    iObserver;
    };

/**
 *  email framework asynchronous request handler
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class EMAILCOMMON_EXPORT CFSMailRequestHandler : public CBase
	{

public:

    /**
     * destructor.
     */
     virtual ~CFSMailRequestHandler();

    /**
     * two based constructor
     *
     * @param aPluginInfo plugin information table
     * @param aPlugins plugin instances table
     */
      static CFSMailRequestHandler* NewL( RPointerArray<CImplementationInformation>&	aPluginInfo,
                                                  RPointerArray<CFSMailPlugin>& aPlugins );
    /**
     * two based constructor
     *
     * @param aPluginInfo plugin information table
     * @param aPlugins plugin instances table
     */
      static CFSMailRequestHandler* NewLC( RPointerArray<CImplementationInformation>& aPluginInfo,
                                                   RPointerArray<CFSMailPlugin>& aPlugins );
    /**
     * starts asynchronous request
     *
     * @param aPluginId id of the target plugin request is accessed to
     * @param aOperationObserver observer to forward request progress events
     * to user from plugin
     *
     * @return request data
     */
      TFSPendingRequest InitAsyncRequestL( TUid aPluginId,
                                                    MFSMailRequestObserver& aOperationObserver);

    /**
     * completes asynchronous request
     *
     * @param aRequestId id of the request to be completed
     */
      void CompleteRequest( TInt aRequestId );

    /**
     * cancels pending asynchronous request
     *
     * @param aRequestId id of the request to be cancelled
     */
      void CancelRequestL( TInt aRequestId );

    /**
     * cancels all pending asynchronous requests
     *
     */
      void CancelAllRequestsL( );

    /**
     * returns plugin pointer related to plugin id
     *
     * @param aPluginId plugin id
     * @return plugin pointer
     */
      CFSMailPlugin* GetPluginByUid( TFSMailMsgId aPluginId );

    /**
     * adds new plugin to plugin list
     *
     * @param aPluginId uid of the new plugin
     * @param aPlugin plugin instance
     */
      void AddPluginL( TUid aPluginId, CFSMailPlugin* aPlugin );

     /**
      * opens or creates file
      *
      * @param aFileName file name
      */
     // <qmail>
      RFile GetTempFileL( TFSMailMsgId aContentId, TFileName &aFileName );
     // </qmail>
protected:

    /**
     * C++ default constructor.
     */
  	 CFSMailRequestHandler();

    /**
     * two based constructor
     */
     void ConstructL( RPointerArray<CImplementationInformation> aPluginInfo,
                      RPointerArray<CFSMailPlugin> aPlugins );

protected: // data

    /** list of loaded plugins */
	 RPointerArray<CFSMailPluginData>    iPluginList;

private:

    /** removes all asynchronous requests */
    void RemoveAllRequests();

private: // data

    /** list of pending requests */
	 RArray<TFSPendingRequest>           iPendingRequests;

	/** request id counter       */
    TUint                                iRequestId;

    /** temp directory files handling        */
    RFs                                  iFs;
    HBufC                                *iTempDirName;
    };

#endif

// End of File
