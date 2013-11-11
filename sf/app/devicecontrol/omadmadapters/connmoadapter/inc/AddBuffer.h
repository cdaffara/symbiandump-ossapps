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

#ifndef ADDBUFFER_H_
#define ADDBUFFER_H_

TInt const KMaxFieldLength = 128;

/**
* CConnMoNodeElement is array element for ConnMo node objects.  
*/
class CConnMoNodeElement : public CBase
    {
public:
    CConnMoNodeElement();
    ~CConnMoNodeElement();
    
    /** Node address */
    TBuf8<KMaxFieldLength>  iUri;
    /** Unique identifier of the parent node */
    TBuf8<KMaxFieldLength>  iParentLUID;
    /** Leaf object's data */
    TBuf8<KMaxFieldLength>  iObject;
    /** Refenrence for giving commands return value to framework */
    TInt  iStatusRef;
    /** Information whether adapter has processed the node or not */
    TBool iProcessed;
    /** Specifies if the node leaf or not */
    TBool iLeaf;
private:
    };

/**
* CAddBuffer is array for ConnMo nodes to be added in .
* 
* CConnMoAdapter uses this class to buffer node and leaf data 
* addition DM commands. 
*/
class CAddBuffer : public CBase
    {
public:
    
    /**
     * Creates new instance of CAddBuffer
     */
    static CAddBuffer* NewL();
    
    /**
     * Adds node to buffer
     * 
     * @param aUri target node URI
     * @param aParentLUID refrence to parent node
     * @param aStatusRef Status reference
     */
    void AddNodeToBufferL( const TDesC8& aUri, 
                           const TDesC8& aParentLUID, 
                           const TInt    aStatusRef );
    
    /**
     * Adds leaf node to buffer
     * 
     * @param aUri target node URI
     * @param aParentLUID refrence to parent node
     * @param aObject Leaf object data
     * @param aStatusRef Status reference
     */
    void AddNodeToBufferL( const TDesC8& aUri, 
                           const TDesC8& aLUID, 
                           const TDesC8& aObject,
                           const TInt    aStatusRef );
    
    /**
     * Searches buffer for given dynamic node
     * 
     * @param aUri target node URI
     */
    CConnMoNodeElement* FindDynamicNode( const TDesC8& aUri );
    
    /**
     * Gets all unprocessed nodes associated to given root node
     * 
     * Function searches buffer for all unprocessed nodes that 
     * belong to given group and are under same dynamic node 
     * 
     * @param aRoot target group
     * @param aGroup Array for result nodes
     */
    TInt GetGroup( const TDesC8& aRoot, RPointerArray<CConnMoNodeElement>* aGroup );
    
    /**
     * Searches buffer for givennode
     * 
     * @param aUri target node URI
     */
    CConnMoNodeElement* CacheFind( const TDesC8& aUri );
    
    /**
     * Searches buffer for given dynamic node
     * 
     * @param aUri target node URI
     */
    CConnMoNodeElement* CacheFindDynamicNode( const TDesC8& aUri );
    
    /**
     * Empties buffer and frees all allocated memory
     */
    void Reset();

    virtual ~CAddBuffer(); 
    
private:
    
    //-----------------------------------------------------------------------------
    // Private functions
    //-----------------------------------------------------------------------------
    void ConstructL();
    CAddBuffer();
          
    //-----------------------------------------------------------------------------
    // Private member variables
    //-----------------------------------------------------------------------------
    /** Contains all nodes to be added */
    RPointerArray<CConnMoNodeElement> iBuffer;
    /** Contains group of nodes that user has previously asked */
    RPointerArray<CConnMoNodeElement> iCache;
        
    };

#endif /* ADDBUFFER_H_ */
