/*
* ==============================================================================
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


#ifndef NAPBRANCH_H_
#define NAPBRANCH_H_

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <cmmanagerext.h> 
#include <smldmadapter.h>
#include "BranchBase.h"
#include "AddBuffer.h"

/**
 * Class for handling Network Access Point (NAP) settings in ConnMo DM tree
 */
class CNapBranch : public CBranchBase
    {
public:

    static CNapBranch* NewL(MSmlDmCallback* aDmCallback, 
                            CAddBuffer* aBuffer,
                            RCmManagerExt* aCmManagerExt );
    
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    
    void UpdateLeafObjectL( const TDesC8& aURI, 
                            const TDesC8& aLUID,
                            const TDesC8& aObject, 
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
                        TInt aResultsRef, 
                        TInt aStatusRef );
    
    void AddNodeObjectL( const TDesC8& aURI, 
                         const TDesC8& aParentLUID,
                         TInt aStatusRef);
    
    /**
     * Processes all Proxy related nodes from buffer
     */
    void ProcessBufferL();

    virtual ~CNapBranch(); 
    
private:
    
    //-----------------------------------------------------------------------------
    // Private functions
    //-----------------------------------------------------------------------------
    void ConstructL( CAddBuffer* aBuffer, RCmManagerExt* aCmManagerExt );
    CNapBranch( MSmlDmCallback* aDmCallback );
    
    CSmlDmAdapter::TError GetUnmodifiableFieldValueL(const TDesC8& aUri,
                                                    CBufBase* aResult );
    
    CSmlDmAdapter::TError GetLeafDataL(const TDesC8& aURI,
                                       RCmConnectionMethodExt& aCm,
                                       CBufBase* aResult );
    
    CSmlDmAdapter::TError GetUnmodifiableFieldAddResult( const TDesC8& aUri, 
                                                         const TDesC8& aObject );
    
    TUint32 MapUriToCmAttribute( const TDesC8& aUri );
    
    void SetLeafDataL( const TDesC8& aURI, 
                       const TDesC8& aObject, 
                       TInt aStatusRef, 
                       RCmConnectionMethodExt& aCm);
    
    void SetNodeDataL( CConnMoNodeElement* aNode, 
                       RCmConnectionMethodExt& aCm );
    
    TBool FindNewCmType( TUint32& aBearerType );
    
    TBool AllMandatoryNapFieldsFound();
    
    void NapChildURIListL( const TDesC8& aURI,
                           const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                           TInt aResultsRef, 
                           TInt aStatusRef,
                           CBufBase& aCurrentURISegmentList );
   
    void GetAllDynamicNapNodesL( const TDesC8& aURI, 
                                 const TDesC8& aLUID,
                                 const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                 TInt aResultsRef,
                                 TInt aStatusRef,
                                 CBufBase& aCurrentURISegmentList,
                                 const TDesC8& aField );
								 
    void IpBranchDDFStructureL( MSmlDmDDFObject& aIPDDF );
    
    CSmlDmAdapter::TError DeleteObjectL( const TDesC8& aURI, 
                                         const TDesC8& aLUID );
    
    TPtrC8 GetDynamicNAPNodeUri( const TDesC8& aURI );
    
    TBool IsNAPUriFormatMatchPredefined(const TDesC8 & aURI);
    
    TInt GetCmIdFromURIL(const TDesC8& aURI);
    //-----------------------------------------------------------------------------
    // Private member variables
    //-----------------------------------------------------------------------------
    CAddBuffer* iBuffer;
    MSmlDmCallback* iCallback;
    RCmManagerExt* iCmManager;
    };
    
    
    
#endif /* NAPBRANCH_H_ */
