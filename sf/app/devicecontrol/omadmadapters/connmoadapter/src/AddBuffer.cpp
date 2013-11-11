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

#include <e32base.h>

#include "AddBuffer.h"
#include "Utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "AddBufferTraces.h"
#endif

_LIT8( KChildNodeWildCardPattern, "/*" );
_LIT8( KNodeSeparator, "/" );

// ======== LOCAL FUNCTIONS =========

// ======== MEMBER FUNCTIONS ========
//------------------------------------------------------------------------------
// CConnMoNodeElement::CConnMoNodeElement()
//------------------------------------------------------------------------------
CConnMoNodeElement::CConnMoNodeElement() 
    {
    OstTraceFunctionEntry0( CCONNMONODEELEMENT_CCONNMONODEELEMENT_ENTRY );
    
    OstTraceFunctionExit0( CCONNMONODEELEMENT_CCONNMONODEELEMENT_EXIT );
    }

//------------------------------------------------------------------------------
// CAddBuffer::~CAddBuffer()
//------------------------------------------------------------------------------
CConnMoNodeElement::~CConnMoNodeElement()
    {
    OstTraceFunctionEntry0( CCONNMONODEELEMENT_DELETE_ENTRY );
    
    OstTraceFunctionExit0( CCONNMONODEELEMENT_DELETE_EXIT );
    }

//-----------------------------------------------------------------------------
// CAddBuffer* CAddBuffer::NewL( )
//-----------------------------------------------------------------------------
CAddBuffer* CAddBuffer::NewL()
    {
    OstTraceFunctionEntry0( CADDBUFFER_NEWL_ENTRY );
    
    CAddBuffer* self = new ( ELeave ) CAddBuffer();
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );

    OstTraceFunctionExit1( CADDBUFFER_NEWL_EXIT, ( TUint )( self ) );
    return self;
    }

//------------------------------------------------------------------------------
// CAddBuffer::CAddBuffer()
//------------------------------------------------------------------------------
CAddBuffer::CAddBuffer() 
    {
    OstTraceFunctionEntry0( CADDBUFFER_CADDBUFFER_ENTRY );
    
    OstTraceFunctionExit0( CADDBUFFER_CADDBUFFER_EXIT );
    }

//------------------------------------------------------------------------------
// CAddBuffer::~CAddBuffer()
//------------------------------------------------------------------------------
CAddBuffer::~CAddBuffer()
    {
    OstTraceFunctionEntry0( CADDBUFFER_DELETE_ENTRY );
    
    iBuffer.ResetAndDestroy();
    
    OstTraceFunctionExit0( CADDBUFFER_DELETE_EXIT );
    }

//------------------------------------------------------------------------------
// CAddBuffer::CAddBuffer()
//------------------------------------------------------------------------------
void CAddBuffer::ConstructL() 
    {
    OstTraceFunctionEntry0( CADDBUFFER_CONSTRUCTL_ENTRY );
    
    OstTraceFunctionExit0( CADDBUFFER_CONSTRUCTL_EXIT );
    }

//------------------------------------------------------------------------------
// CAddBuffer::AddNodeToBuffer()
//------------------------------------------------------------------------------
void CAddBuffer::AddNodeToBufferL( const TDesC8& aUri, 
                                   const TDesC8& aParentLUID,
                                   const TInt    aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CADDBUFFER_ADDNODETOBUFFERL_ENTRY, "ENTRY: CAddBuffer::AddNodeToBufferL;aUri=%s", aUri );
    
    CConnMoNodeElement* newNode = new (ELeave) CConnMoNodeElement();
    
    newNode->iUri.Copy( aUri );
    newNode->iParentLUID.Copy( aParentLUID );
    newNode->iStatusRef = aStatusRef;
    newNode->iProcessed = EFalse;
    newNode->iLeaf = EFalse;
    
    iBuffer.AppendL( newNode );
    OstTrace0( TRACE_NORMAL, CADDBUFFER_ADDNODETOBUFFERL_EXIT, "EXIT: CAddBuffer::AddNodeToBufferL");
    }

//------------------------------------------------------------------------------
// CAddBuffer::AddNodeToBuffer()
//------------------------------------------------------------------------------
void CAddBuffer::AddNodeToBufferL( const TDesC8& aUri, 
                                   const TDesC8& aLUID,
                                   const TDesC8& aObject,
                                   const TInt    aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CADDBUFFER_ADDNODETOBUFFERL_LEAF_ENTRY, "ENTRY: CAddBuffer::AddNodeToBufferL;aUri=%s", aUri );
    
    CConnMoNodeElement* newNode = new (ELeave) CConnMoNodeElement();
    
    newNode->iUri.Copy( aUri );
    newNode->iParentLUID.Copy( aLUID );
    newNode->iObject.Copy( aObject );
    newNode->iStatusRef = aStatusRef;
    newNode->iProcessed = EFalse;
    newNode->iLeaf = ETrue;
    
    iBuffer.AppendL( newNode );
    OstTrace0( TRACE_NORMAL, CADDBUFFER_ADDNODETOBUFFERL_LEAF_EXIT, "EXIT: CAddBuffer::AddNodeToBufferL");
    }

//------------------------------------------------------------------------------
// CAddBuffer::FindDynamicNode
//------------------------------------------------------------------------------
CConnMoNodeElement* CAddBuffer::FindDynamicNode( const TDesC8& aUri )
    {
    OstTraceExt1( TRACE_NORMAL, CADDBUFFER_FINDDYNAMICNODE_ENTRY, "ENTRY: CAddBuffer::FindDynamicNode;aUri=%s", aUri );

    CConnMoNodeElement* resultNode = NULL;

    for ( TInt i = 0; i < iBuffer.Count(); ++i )
        {
        if ( iBuffer[i]->iUri.Match(aUri) != KErrNotFound )
            {
            // 
            TBuf8<KMaxFieldLength> dynamicNodeChildUri(aUri);
            dynamicNodeChildUri.Append( KChildNodeWildCardPattern );
            
            if ( iBuffer[i]->iUri.Match( dynamicNodeChildUri ) == KErrNotFound )
                {
                // Correct dynamic node found.
                OstTrace0( TRACE_NORMAL, 
                           CADDBUFFER_FINDDYNAMICNODE_MATCH_FOUND, 
                           "CAddBuffer::FindDynamicNode;Matching node found.");
                resultNode = iBuffer[i];
                break;
                }
            }
        }
	if ( resultNode != NULL )
	    {
        OstTraceExt1( TRACE_NORMAL, 
                  CADDBUFFER_FINDDYNAMICNODE_EXIT, 
                  "EXIT: CAddBuffer::FindDynamicNode;resultNode=%s", 
                  resultNode->iUri );
	    }
    
    return resultNode;
    }
    

//------------------------------------------------------------------------------
// CAddBuffer::Find
//------------------------------------------------------------------------------
CConnMoNodeElement* CAddBuffer::CacheFind( const TDesC8& aUri )
    {
    OstTraceExt1( TRACE_NORMAL, CADDBUFFER_CACHEFIND_ENTRY, "ENTRY: CAddBuffer::CacheFind;aUri=%s", aUri );
    
    CConnMoNodeElement* resultNode = NULL;

    for ( TInt i = 0; i < iCache.Count(); ++i )
        {
        if ( iCache[i]->iUri.Match( aUri ) != KErrNotFound )
            {
            OstTrace0( TRACE_NORMAL, CADDBUFFER_CACHEFIND_MATCH_FOUND, "CAddBuffer::CacheFind; Matching URI found.");
            resultNode = iCache[i];
            
            OstTraceExt1( TRACE_NORMAL, 
                          CADDBUFFER_CACHEFIND_RESULT_NODE, 
                          "CAddBuffer::CacheFind; resultNode=%s", 
                          resultNode->iUri );
            break;
            }
        }
    
    OstTrace0( TRACE_NORMAL, CADDBUFFER_CACHEFIND_EXIT, "EXIT: CAddBuffer::CacheFind");
    return resultNode;
    }

//------------------------------------------------------------------------------
// CAddBuffer::FindDynamicNode
//------------------------------------------------------------------------------
CConnMoNodeElement* CAddBuffer::CacheFindDynamicNode( const TDesC8& aUri )
    {
    OstTraceExt1( TRACE_NORMAL, CADDBUFFER_CACHEFINDDYNAMICNODE_ENTRY, "ENTRY: CAddBuffer::CacheFindDynamicNode;aUri=%s", aUri );
    
    CConnMoNodeElement* resultNode = NULL;

    for ( TInt i = 0; i < iCache.Count(); ++i )
        {
        if ( iCache[i]->iUri.Match( aUri ) != KErrNotFound )
            {
            // 
            TBuf8<KMaxFieldLength> dynamicNodeChildUri( aUri );
            dynamicNodeChildUri.Append( KChildNodeWildCardPattern );
            
            if ( iCache[i]->iUri.Match(dynamicNodeChildUri) == KErrNotFound )
                {
                // Correct dynamic node found.
                OstTrace0( TRACE_NORMAL, 
                           CADDBUFFER_CACHEFINDDYNAMICNODE_MATCH_FOUND, 
                           "CAddBuffer::CacheFindDynamicNode; Matching node found.");
                          
                resultNode = iCache[i];
                break;
                }
            }
        }
    if ( resultNode != NULL )
        {
        OstTraceExt1( TRACE_NORMAL, 
                  CADDBUFFER_CACHEFINDDYNAMICNODE_EXIT, 
                  "EXIT: CAddBuffer::CacheFindDynamicNode;resultNode=%s", 
                  resultNode->iUri );
        }
    
    return resultNode;
    }
//------------------------------------------------------------------------------
// CAddBuffer::AddNodeToBuffer()
//------------------------------------------------------------------------------
TInt CAddBuffer::GetGroup( const TDesC8& aRoot, RPointerArray<CConnMoNodeElement>* aGroup )
    {
    OstTraceExt1( TRACE_NORMAL, CADDBUFFER_GETGROUP_ENTRY, "ENTRY: CAddBuffer::GetGroup;aRoot=%s", aRoot );

    iCache.Reset();
    TInt retval( KErrNotFound );
    TBool first = ETrue;
    TBuf8<KSmlMaxURISegLen> groupTag;
    groupTag.Copy( aRoot );
    groupTag.Append( KChildNodeWildCardPattern );

    for ( TInt i = 0; i < iBuffer.Count(); ++i )
        {
        if ( iBuffer[i]->iUri.Match( groupTag ) != KErrNotFound
             && !iBuffer[i]->iProcessed )
            {
            if ( first )
                {
                groupTag.Insert( aRoot.Length(), KNodeSeparator );
                groupTag.Insert( aRoot.Length() + 1, CUtils::GetSecondURISeg( iBuffer[i]->iUri ) );
                first = EFalse;
                retval = 0;
                }
            TInt err = KErrNone;
            TRAP(err,aGroup->AppendL(iBuffer[i]));
            if(err != KErrNone)
                {
				retval = err;
                break;
                }
            TRAP(err,iCache.AppendL(iBuffer[i]));
            if(err != KErrNone)
                {
                TInt indx = aGroup->Find(iBuffer[i]);
                if(indx != KErrNotFound)
                    aGroup->Remove(indx);
				retval = err;
                break;
                }
            retval++;
            }
        }
    OstTrace1( TRACE_NORMAL, CADDBUFFER_GETGROUP_EXIT, "EXIT: CAddBuffer::GetGroup;retval=%d", retval );

    return retval;
    }
//------------------------------------------------------------------------------
// CAddBuffer::~CAddBuffer()
//------------------------------------------------------------------------------
void CAddBuffer::Reset()
    {
    OstTraceFunctionEntry0( CADDBUFFER_Reset_ENTRY );
    
    iBuffer.ResetAndDestroy();
    
    OstTraceFunctionExit0( CADDBUFFER_Reset_EXIT );
    }

