/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "emailtrace.h"

// <qmail>
// Exports removed because entire class is exported from DLL
#include <nmcommonheaders.h>
// </qmail>

#include "CFSMailRequestHandler.h"
#include "CFSMailRequestObserver.h"


// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CFSMailRequestHandler
// -----------------------------------------------------------------------------
 CFSMailRequestHandler::CFSMailRequestHandler() : iRequestId(0)
{
    NM_FUNCTION;

    // store pointer to TLS
    TInt err = Dll::SetTls(static_cast<TAny*>(this));

    // reset
    iPendingRequests.Reset();
    iPluginList.Reset();

    iTempDirName =NULL;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::~CFSMailRequestHandler
// -----------------------------------------------------------------------------
 CFSMailRequestHandler::~CFSMailRequestHandler()
    {
    NM_FUNCTION;

    TRAP_IGNORE( CancelAllRequestsL() );
    RemoveAllRequests();
    iPendingRequests.Reset();
    iPluginList.ResetAndDestroy();

    if(iTempDirName)
        {
        delete iTempDirName;
        // close file server connection
        iFs.Close();
        }

    // clear TLS
    Dll::FreeTls();
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::NewLC
// -----------------------------------------------------------------------------
 CFSMailRequestHandler* CFSMailRequestHandler::NewLC(
                                    RPointerArray<CImplementationInformation>& aPluginInfo,
                                    RPointerArray<CFSMailPlugin>& aPlugins )
{
    NM_FUNCTION;

    CFSMailRequestHandler* pluginHandler = new (ELeave) CFSMailRequestHandler();
    CleanupStack:: PushL(pluginHandler);
    pluginHandler->ConstructL( aPluginInfo, aPlugins );
    return pluginHandler;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::NewL
// -----------------------------------------------------------------------------
 CFSMailRequestHandler* CFSMailRequestHandler::NewL(
                                    RPointerArray<CImplementationInformation>& aPluginInfo,
                                    RPointerArray<CFSMailPlugin>& aPlugins )
{
    NM_FUNCTION;

    CFSMailRequestHandler* pluginHandler =
        CFSMailRequestHandler::NewLC( aPluginInfo, aPlugins );
    CleanupStack:: Pop(pluginHandler);
    return pluginHandler;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::ConstructL
// -----------------------------------------------------------------------------
void CFSMailRequestHandler::ConstructL(
                                RPointerArray<CImplementationInformation> /*aPluginInfo*/,
                                RPointerArray<CFSMailPlugin> /*aPlugins*/ )
{
    NM_FUNCTION;
}

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetPluginByUid
// -----------------------------------------------------------------------------
 CFSMailPlugin* CFSMailRequestHandler::GetPluginByUid( TFSMailMsgId aObjectId )
    {
    NM_FUNCTION;

    for(TInt i=0;i<iPluginList.Count();i++)
        {
        if(iPluginList[i]->iPluginId.iUid == aObjectId.PluginId().iUid)
            {
            return iPluginList[i]->iPlugin;
            }
        }


    return NULL;
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::InitAsyncRequestL
// -----------------------------------------------------------------------------
 TFSPendingRequest CFSMailRequestHandler::InitAsyncRequestL(
                                                    TUid aPluginId,
                                                    MFSMailRequestObserver& aOperationObserver)
    {
    NM_FUNCTION;

        TFSPendingRequest newRequest;
        for(TInt i=0;i<iPendingRequests.Count();i++)
            {
            if(iPendingRequests[i].iRequestStatus != TFSPendingRequest::EFSRequestPending)
                {
                iPendingRequests[i].iPluginId = aPluginId;
                iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestPending;
                iPendingRequests[i].iObserver->SetUserObserver(aOperationObserver);
                newRequest = iPendingRequests[i];
                return newRequest;
                }
            }

        newRequest.iRequestId = iPendingRequests.Count();
        newRequest.iPluginId = aPluginId;
        newRequest.iRequestStatus = TFSPendingRequest::EFSRequestPending;
        CFSMailRequestObserver* observer =
            CFSMailRequestObserver::NewL(*this, aOperationObserver);
        CleanupStack::PushL(observer);
        newRequest.iObserver = observer;
        iPendingRequests.AppendL(newRequest);
        CleanupStack::Pop(observer);
        
        return newRequest;
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CompleteRequest
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CompleteRequest( TInt aRequestId )
    {
    NM_FUNCTION;

        for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestId == aRequestId)
            {
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestComplete;
            break;
            }
        }

    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CancelRequestL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CancelRequestL( TInt aRequestId )
    {
    NM_FUNCTION;

        for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        if(iPendingRequests[i].iRequestId == aRequestId &&
            iPendingRequests[i].iRequestStatus == TFSPendingRequest::EFSRequestPending)
            {
            TFSMailMsgId pluginId(iPendingRequests[i].iPluginId,0);
            if(CFSMailPlugin* plugin = GetPluginByUid(pluginId))
                {
                plugin->CancelL(aRequestId);
                }
            iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestCancelled;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::CancelAllRequestsL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::CancelAllRequestsL( )
    {
    NM_FUNCTION;

        for(TInt i=0;i<iPendingRequests.Count();i++)
        {
            if(iPendingRequests[i].iRequestStatus == TFSPendingRequest::EFSRequestPending)
                {
                TFSMailMsgId pluginId(iPendingRequests[i].iPluginId,0);
                if(CFSMailPlugin* plugin = GetPluginByUid(pluginId))
                    {
                    plugin->CancelL(iPendingRequests[i].iRequestId);
                    }
                iPendingRequests[i].iRequestStatus = TFSPendingRequest::EFSRequestCancelled;
                }
        }
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::AddPluginL
// -----------------------------------------------------------------------------
 void CFSMailRequestHandler::AddPluginL( TUid aPluginId, CFSMailPlugin* aPlugin )
    {
    NM_FUNCTION;

    CFSMailPluginData* pluginData = new (ELeave) CFSMailPluginData;
    pluginData->iPluginId = aPluginId;
    pluginData->iPlugin = aPlugin;
    iPluginList.AppendL(pluginData);
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::RemoveAllRequests
// -----------------------------------------------------------------------------
void CFSMailRequestHandler::RemoveAllRequests()
    {
    NM_FUNCTION;

    for(TInt i=0;i<iPendingRequests.Count();i++)
        {
        delete iPendingRequests[i].iObserver;
        iPendingRequests[i].iObserver = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CFSMailRequestHandler::GetTempFileL( )
// -----------------------------------------------------------------------------
 RFile CFSMailRequestHandler::GetTempFileL( TFSMailMsgId aContentId, TFileName &aFileName )
{
    NM_FUNCTION;

    // get temp dir path
    if(!iTempDirName)
        {
        // open file server connection
        User::LeaveIfError(iFs.Connect());
        iFs.CreatePrivatePath(EDriveC);
        iTempDirName = HBufC::NewL(20);
        TPtr ptr = iTempDirName->Des();
        iFs.PrivatePath(ptr);
        }

    // create temp file
    RFile file;
    aFileName.Append(iTempDirName->Des());
    aFileName.AppendNum(aContentId.PluginId().iUid);
    aFileName.AppendNum(aContentId.Id());
    file.Replace(iFs,aFileName,EFileWrite);
    return file;
    }

