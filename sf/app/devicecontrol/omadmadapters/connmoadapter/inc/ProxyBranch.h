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

#ifndef PROXYBRANCH_H_
#define PROXYBRANCH_H_


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <smldmadapter.h>
#include "BranchBase.h"
#include "cmmanagerext.h"
#include "AddBuffer.h"

/**
 * Class for handling Proxy settings in ConnMo DM tree
 */
class CProxyBranch : public CBranchBase
    {
public:

    static CProxyBranch* NewL(MSmlDmCallback* aDmCallback, 
                              CAddBuffer* aBuffer,
                              RCmManagerExt* aCmManagerExt );
    
    // Adapter branch interface from CProxyBranch
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
                               TInt aStatusRef);

    void ChildURIListL( const TDesC8& aURI, 
                        const TDesC8& aLUID,
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                        TInt aResultsRef, 
                        TInt aStatusRef );
    
    void AddNodeObjectL( const TDesC8& aURI, 
                         const TDesC8& aParentLUID,
                         TInt aStatusRef );
    
    /**
     * The function processes all proxy nodes from add buffer.
     * @param aCmManagerExt Reference to CmManager session. 
     */
    void ProcessBufferL();
    
    virtual ~CProxyBranch(); 
    
private:
    
    //-----------------------------------------------------------------------------
    // Private functions
    //-----------------------------------------------------------------------------
    CProxyBranch(MSmlDmCallback* aDmCallback, 
                 CAddBuffer* aBuffer, 
                 RCmManagerExt* aCmManagerExt );
        
    /**
     * The function returns URI list of dynamic Proxy nodes. The list is returned by 
     * calling the SetResultsL function of MSmlCallback callback interface.
     * @param aURI URI of the parent object.
     * @param aPreviousURISegmentList URI list with mapping LUID information.
     * @param aResultsRef Reference to correct results.
     * @param aStatusRef  Reference to correct command.
     * @param aCmManagerExt Reference to CmManager session. 
     */
    void GetAllDynamicProxyNodesL( const TDesC8& aURI,
                                   const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                   TInt aResultsRef, 
                                   TInt aStatusRef,
                                   CBufBase& aCurrentURISegmentList );
    
    /**
     * The function returns URI list of requested dynamic Proxy node.
     * @param aLUID LUID of the parent object.
     * @param aResultsRef Reference to correct results.
     * @param aStatusRef  Reference to correct command.
     * @param aCmManagerExt Reference to CmManager session. 
     */
    void GetURIListForProxyNodeL( const TDesC8& aLUID,
                                  TInt aResultsRef, 
                                  TInt aStatusRef );
    
    /**
     * The function adjust DM operation return values for hardcoded fields.
     * @param aURI URI of the object.
     * @return CSmlDmAdapter::TError Status value for DM operation.
     */
    CSmlDmAdapter::TError CheckForUnmodifiableField( const TDesC8& aUri );
    
    /**
     * The function returns CMManager's attribute enum that corresponds to uri.
     * @param aURI URI of the object.
     * @return TUint32 Attribute enum for CM.
     */
    TUint32 MapUriToCmAttribute( const TDesC8& aUri );
    
    /**
     * The function searches for connection method that is referenced from proxy buffered
     * in the addbuffer.
     * @param aCmManagerExt Reference to CmManager session. 
     * @return TUint32 CM ID.
     */
    TInt32 FindConnectionMethodL();
    
    /**
     * The function checs that the addbuffer contains all necessary fields 
     * to create new proxy entry.
     * @return TBool True if all mandatory fields found.
     */
    TBool AllMandatoryProxyFieldsFound();
    
    /**
     * The function writes information from the addbuffer to connection method.
     * @param aURI URI of the object.
     * @param aLUID Local UID of the leaf.
     * @param aObject objecto to be written in CM.
     * @param aStatusRef  Reference to correct command.
     * @param aCm Reference to Connection Method. 
     */
    void SetLeafDataL( const TDesC8& aURI, 
                       const TDesC8& aLUID,
                       const TDesC8& aObject, 
                       TInt aStatusRef, 
                       RCmConnectionMethodExt& aCm);
    
    /**
     * The function informs framework about new nodes.
     * @param aNode Node to be saved.
     * @param aCm Reference to Connection Method.
     */
    void SetNodeDataL( CConnMoNodeElement* aNode, RCmConnectionMethodExt& aCm );
    
    /**
     * Fethces leaf object data from connection settings
     * @param aURI Target node
     * @param aLuid Local UID for target node
     * @param buffer for returning object data
     * @return CSmlDmAdapter::TError Return value informs completion status of the operation
     */
    CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI,
                                            const TDesC8& aLUID,
                                            CBufBase& aLeafObject );
    
    TPtrC8 GetDynamicProxyNodeUri( const TDesC8& aURI );
    
    TBool IsProxyUriFormatMatchPredefined(const TDesC8 & aURI);
 
    TInt GetCmIdFromURIL(const TDesC8& aURI);
    
    //-----------------------------------------------------------------------------
    // Private member variables
    //-----------------------------------------------------------------------------
    MSmlDmCallback* iCallback;
    CAddBuffer* iBuffer;
    RCmManagerExt* iCmManager;
    };
    
#endif /* PROXYBRANCH_H_ */
