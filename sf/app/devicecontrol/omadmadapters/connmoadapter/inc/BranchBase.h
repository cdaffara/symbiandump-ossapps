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

#ifndef BRANCHBASE_H_
#define BRANCHBASE_H_

#include <cmmanagerext.h> 
#include "smldmadapter.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
#define KConnMoAdapterUid 0x2001FE5F

/**
 * Abstract base class for all ConnMO branches.
 */
class CBranchBase : public CBase
    {
public:

    /**
     * Builds DDF structure of DM settings it governs.
     * 
     * Implementations should build their structure using given node 
     * as the root node.
     * 
     * @param aDDF Root node
     */
    virtual void DDFStructureL( MSmlDmDDFObject& aDDF ) = 0;
    
    /**
     * Updates leaf node data.
     * 
     * @param aURI Root node
     * @param aLUID refrence to branch LUID
     * @param aObject Leaf object data
     * @param aType Type of the node
     * @param aStatusRef Status reference
     */
    virtual void UpdateLeafObjectL( const TDesC8& aURI, 
                                    const TDesC8& aLUID,
                                    const TDesC8& aObject, 
                                    const TDesC8& aType,
                                    TInt aStatusRef ) = 0;
    /**
     * Deletes object pointed by aURI.
     * 
     * @param aURI Root node
     * @param aLUID refrence to branch LUID
     * @param aStatusRef Status reference
     */    
    virtual void DeleteObjectL( const TDesC8& aURI, 
                                const TDesC8& aLUID,
                                TInt aStatusRef ) = 0;
        
    /**
     * Gets node data for leaf nodes and child list for parent nodes.
     * 
     * @param aURI Root node
     * @param aLUID refrence to branch LUID
     * @param aObject Leaf object data
     * @param aType Type of the node
     * @param aResultsRef Result reference
     * @param aStatusRef Status reference
     */
    virtual void FetchLeafObjectL( const TDesC8& aURI, 
                                   const TDesC8& aLUID,
                                   const TDesC8& aType, 
                                   TInt aResultsRef,
                                   TInt aStatusRef ) = 0;
        
    /**
     * Gets the size of the data in leaf node.
     */
    virtual void FetchLeafObjectSizeL( const TDesC8& aURI, 
                                       const TDesC8& aLUID,
                                       const TDesC8& aType, 
                                       TInt aResultsRef,
                                       TInt aStatusRef) = 0;

    /**
     * Gets all childs of the given node.
     */
    virtual void ChildURIListL( const TDesC8& aURI, 
                                const TDesC8& aLUID,
                                const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                TInt aResultsRef, 
                                TInt aStatusRef ) = 0;
        
    /**
     * Adds node to DM tree.
     */
    virtual void AddNodeObjectL( const TDesC8& aURI, 
                                 const TDesC8& aParentLUID,
                                 TInt aStatusRef )=0;

        
    virtual ~CBranchBase();

protected:
    
    CBranchBase();
    
    CBranchBase( MSmlDmCallback* aDmCallback );
            
    /**
     * This function gives all Connection Methods from Connection Method Manager.
     * 
     * @param aCmArray Result Array.
     * @param aCmManager CM Manager.
     */
    void GetAllConnectionMethodsL( RArray<TUint32>& aCmArray,
                                   RCmManagerExt& aCmManager );
    
    /**
     * This function checks if given CM ID is found from given URI segmentlist.
     * 
     * @param aPreviousURISegmentList Array where to search.
     * @param aCmId CM ID to be searched.
     * @param aURISeg If match is found, the URI segment matching the CM ID is copied here.
     * @return TBool ETrue if CM ID is found from array.
     */
    TBool CheckPreviousURIList( const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                const TUint32 aCmId,
                                TDes8& aURISeg );
    
    /**
     * This function gets list of dynamic nodes for the specified URI
     * 
     * @param aURI which to get the list for
     * @param aLuid refrence to branch LUID
     * @param aPreviousURISegmentList previous URI segment list
     * @param aResultRef result reference
     * @param aStatusRef status reference
     * @param aCmManagerExt CM Manager reference
     * @param aCurrentURISegmentList current URI segment list
     * @param aField branch name
     * @param aNodeCount number of dynamic nodes
     * @aCallback pointer to DM callback
     */
    void GetAllDynamicNodesL( const TDesC8& aURI, 
                              const TDesC8& aLUID,
                              const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                              TInt aResultsRef,
                              TInt aStatusRef,
                              RCmManagerExt& aCmManagerExt,
                              CBufBase& aCurrentURISegmentList,
                              const TDesC8& aField,
                              const TInt aNodeCount,
                              MSmlDmCallback* aCallback );
    
    /**
     * This function gets connection method
     * 
     * @param aCmManagerExt CM Manager
     * @param aCm CM Manager
     * @param aCmId CM ID
     * @return TBool ETrue if connection method found
     */
    TBool GetConnectionMethodLC( RCmManagerExt& aCmManagerExt,
                                 RCmConnectionMethodExt& aCm,
                                 TUint32 aCmId );
    
private:
    MSmlDmCallback* iCallback;
    
    };

#endif /* BRANCHBASE_H_ */
