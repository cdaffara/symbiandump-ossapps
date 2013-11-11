/*
* ==============================================================================
*  Name        : ConnMoAdapter.h
*  Part of     : ConnMoAdapter
*  Interface   : ecom / ConnMOAdapter
*  Description : Connection Management Object
*  Version     : $Revision: 61 $ $Modtime: 29.09.05 8:17 $
*
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
* Description:
*
* ==============================================================================
*/

#ifndef CONNMOADAPTER_H_
#define CONNMOADAPTER_H_

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <smldmadapter.h>
#include <cmmanagerext.h>

class CNapBranch;
class CProxyBranch;
class CAddBuffer;
class CBranchBase;

_LIT8( KCMDdfNAP,                   "NAP" );
_LIT8( KCMDdfProxy,                 "Proxy" );

/**
 * Implements ConnMO partition of the DM tree
 */
class CConnMoAdapter : public CSmlDmAdapter
    {
public:

    static CConnMoAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CConnMoAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CConnMoAdapter(); 
    
    // Adapter Interface inherited from CSmlDmAdapter
    void DDFVersionL( CBufBase& aVersion );
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    
    void UpdateLeafObjectL( const TDesC8& aURI, 
                            const TDesC8& aLUID,
                            const TDesC8& aObject, 
                            const TDesC8& aType,
                            TInt aStatusRef );
    
    void UpdateLeafObjectL( const TDesC8& aURI, 
                            const TDesC8& aLUID,
                            RWriteStream*& aStream, 
                            const TDesC8& aType,
                            TInt aStatusRef );
    
    void DeleteObjectL( const TDesC8& aURI, 
                        const TDesC8& aLUID,
                        TInt aStatusRef );
    
    void FetchLeafObjectL( const TDesC8& aURI, 
                           const TDesC8& aLUID,
                           const TDesC8& aType, 
                           TInt aResultsRef,
                           TInt aStatusRef );
    
    void FetchLeafObjectSizeL( const TDesC8& aURI, 
                               const TDesC8& aLUID,
                               const TDesC8& aType, 
                               TInt aResultsRef,
                               TInt aStatusRef );
    
    void ChildURIListL( const TDesC8& aURI, 
                        const TDesC8& aLUID,
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                        TInt aResultsRef, TInt aStatusRef );
    
    void AddNodeObjectL( const TDesC8& aURI, 
                         const TDesC8& aParentLUID,
                         TInt aStatusRef );

    void ExecuteCommandL( const TDesC8& aURI, 
                          const TDesC8& aLUID,
                          const TDesC8& aArgument, 
                          const TDesC8& aType,
                          TInt aStatusRef );

    void ExecuteCommandL( const TDesC8& aURI, 
                          const TDesC8& aLUID,
                          RWriteStream*& aStream, 
                          const TDesC8& aType,
                          TInt aStatusRef );

    void CopyCommandL( const TDesC8& aTargetURI, 
                       const TDesC8& aTargetLUID,
                       const TDesC8& aSourceURI, 
                       const TDesC8& aSourceLUID,
                       const TDesC8& aType, 
                       TInt aStatusRef );

    void StartAtomicL(); 
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport( TInt& aItemSize );
    void StreamCommittedL();
    void CompleteOutstandingCmdsL();
    
private:
    
    //-----------------------------------------------------------------------------
    // Private functions
    //-----------------------------------------------------------------------------
    void ConstructL();
    CConnMoAdapter( TAny* aEcomArguments );
    
    CBranchBase* GetBranch( const TDesC8& aUri );
    
    MSmlDmDDFObject& AddBaseUriL( MSmlDmDDFObject& aRootDDF);
    
    MSmlDmDDFObject& AddBranchL( MSmlDmDDFObject& aRootDDF, 
                                const TDesC8& aNodeName);
    
    //-----------------------------------------------------------------------------
    // Private member variables
    //-----------------------------------------------------------------------------
    /** Callback to framework */
    MSmlDmCallback*     iCallBack;
    
    /** branches of ConnMo structure */
    CNapBranch*         iNapBranch;
    CProxyBranch*       iProxyBranch;
    
    /** Buffer for node addition */
    CAddBuffer*         iBuffer;
    
    /** CmMager for manipulating CM settings */
    RCmManagerExt       iCmManager;
    };


#endif /* CONNMOADAPTER_H_ */
