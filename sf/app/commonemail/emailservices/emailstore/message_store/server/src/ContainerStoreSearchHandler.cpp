/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store search handler implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreSearchHandler.h"
#include "ContainerStore.h"
#include "ContainerStoreDefs.h"
#include "ContainerStoreContentManager.h"
#include "ContainerStoreSearchResultTable.h"
#include "MsgStoreSortResultRowSet.h"
#include "PropertiesSerializer.h"
//<cmail>
#include "MsgStorePropertyKeys.h"
//</cmail>

// =========
// CONSTANTS
// =========

// The heap buffer size for search operations.
//WMT: This size needs to be >= KNormalBlockSize defined in ContainerStoreContentManager.cpp
const TUint KSearchBufferSize = 4096;

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreSearchHandler::CContainerStoreSearchHandler( CContainerStore&                  aContainerStore,
                                                            CContainerStoreContentManager&    aContentManager, 
                                                            CContainerStoreSearchResultTable& aSearchResultTable,
                                                            CContainerStoreSortingTable&      aSortingTable,
                                                            TContainerId                      aType, 
                                                            MContainerStoreSearchClient&      aClient,
                                                            TInt                              aPriority ) :
    CActive( aPriority ),
    iContainerStore( aContainerStore ),
    iContentManager( aContentManager ),
    iSearchResultTable( aSearchResultTable ),
    iSortingTable( aSortingTable ),
    iClient( aClient ),
    iType( aType ),
    iSortResultSet( NULL ),
    iIsNewSearch( ETrue )
    {
    __LOG_CONSTRUCT( "msg", "CContainerStoreSearchHandler" )
    
   	CActiveScheduler::Add(this);    
    } // end constructor

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CContainerStoreSearchHandler::ConstructL( RPointerArray<HBufC>&     aSearchStrings,
                                               TMsgStoreSearchCmdParams& aCmdParam,
                                               RArray<TContainerId>&     aFolderIds,
                                               RPointerArray<HBufC8>&    aPropertyNames )
    {
    __LOG_ENTER( "ConstructL" )
    
    CreateSearchPatternL( aSearchStrings, iSearchStrings );
    
    TInt maxSSLength = 0;
    for ( TInt i = 0 ; i < aSearchStrings.Count() ; i++ )
        {
        if ( aSearchStrings[i]->Length() > maxSSLength )
            {
            maxSSLength = aSearchStrings[i]->Length();
            }
        }
    
    iSearchBuffer.CreateL( KSearchBufferSize + maxSSLength * 2 );
    
    InitializeL( aCmdParam, aFolderIds, aPropertyNames );
    
    iSearchResultTable.DeleteAllL();
    
    __LOG_EXIT
    } // end ConstructL
    
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreSearchHandler::~CContainerStoreSearchHandler()
    {
    __LOG_DESTRUCT
    
	Cancel();
    TRAP_IGNORE( iSearchResultTable.DeleteAllL() );
	
//    iSearchIds.Reset();
    iSearchBuffer.Close();
    iFolderIds.Close();
    iSearchStrings.ResetAndDestroy();
    iPropertyNames.ResetAndDestroy();
    delete iSortResultSet;
    iPropertyBuf.Close();
    } // end destructor

// ==========================================================================
// FUNCTION: StartL
// ==========================================================================
void CContainerStoreSearchHandler::StartL()
    {
    __LOG_ENTER( "StartL" )
    
    iCurFolderIdIndex = -1;

    SortNextFolderL( );
    
    DoNextSearchL();
    
    __LOG_EXIT
    } // end StartL

// ==========================================================================
// FUNCTION: IsSameSearchCriteriaL
// ==========================================================================
TBool CContainerStoreSearchHandler::IsSameSearchCriteriaL( RPointerArray<HBufC>& aSearchStrings )
    {
    //
    RPointerArray<HBufC> newSearchPatterns;
    
    CreateSearchPatternL( aSearchStrings, newSearchPatterns );
    
    TBool isSame = newSearchPatterns.Count() == iSearchStrings.Count();
    
    if ( isSame )
        {
        for ( TInt i = 0 ; i < iSearchStrings.Count() ; i++ )
            {
            isSame = newSearchPatterns[i]->CompareC( *(iSearchStrings[i]) ) == 0;
            if ( !isSame )
                {
                break;
                }
            }
        }
    
    newSearchPatterns.ResetAndDestroy();
    
    return isSame;
    }

// ==========================================================================
// FUNCTION: RestartL
// ==========================================================================
void CContainerStoreSearchHandler::RestartL( TMsgStoreSearchCmdParams& aCmdParam, 
                                             RArray<TContainerId>&     aFolderIds, 
                                             RPointerArray<HBufC8>&    aPropertyNames )
    {
    InitializeL( aCmdParam, aFolderIds, aPropertyNames );
    iIsNewSearch = EFalse;
    StartL();
    }

// ==========================================================================
// FUNCTION: DoNextSearchL
// ==========================================================================
void CContainerStoreSearchHandler::DoNextSearchL()
    {
    __LOG_ENTER_SUPPRESS( "DoNextSearch" )
    
    TContainerId messageId = KContainerInvalidId;
    
    if ( iSortResultSet->HasMoreNextL() )
        {
        messageId = iSortResultSet->NextL();
        }
    else
        {
        while ( SortNextFolderL() )
            {
            if ( iSortResultSet->HasMoreNextL() )
                {
                messageId = iSortResultSet->NextL();
                break;
                }
            }
        }
    
    if( messageId != KContainerInvalidId  )
        {
        TBool found = EFalse;
        
        TRAP_IGNORE( found = SearchL( messageId ) );
        
        if( found )
            {
            // Remove the properties that are not needed.
            TPropertiesSerializer serializer( iPropertyBuf );        
            TBool moreProperties = serializer.First();        
            while( moreProperties )        
                {
                TBool found = EFalse;            
                for( TInt i = 0; !found && (i < iPropertyNames.Count()); i++ )
                    {
                    found = (iPropertyNames[i]->Compare( serializer.Name() ) == 0);
                    } // end for                               
                if( found )
                    {
                    moreProperties = serializer.Next();
                    }
                else
                    {
                    moreProperties = serializer.RemovePropertyL();
                    } // end if                
                } // end while   
            
            
            iClient.MatchFound( messageId, iSortResultSet->FolderId(), iPropertyBuf );
            } // end if
        
        // Schedule next search (yield, in case higher priority active objects have something to do).
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );      
        }
    else
        {
        // Done!
        __LOG_WRITE_INFO( "Search complete" )
        iClient.SearchComplete( KErrNone );
        } // end if
        
    } // end DoNextSearch
    
// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CContainerStoreSearchHandler::RunL()
    {
    DoNextSearchL();
    } // end RunL
    
// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CContainerStoreSearchHandler::DoCancel()
    {
    // Nothing special to do.
    } // end DoCancel

// ==========================================================================
// FUNCTION: InitializeL
// ==========================================================================
void CContainerStoreSearchHandler::InitializeL( TMsgStoreSearchCmdParams& aCmdParams, 
                                                RArray<TContainerId>&     aFolderIds, 
                                                RPointerArray<HBufC8>&    aPropertyNames )
    {
    iSearchParams = aCmdParams;
    
    iFolderIds.Reset();
    for( TInt i = 0 ; i < aFolderIds.Count() ; i++ )
        {
        iFolderIds.AppendL( aFolderIds[i] );
        }

    iPropertyNames.ResetAndDestroy();
    for ( TInt i = 0 ; i < aPropertyNames.Count() ; i++ )
        {
        iPropertyNames.AppendL( aPropertyNames[i] );
        }
    aPropertyNames.Reset();
    
    delete iSortResultSet;
    iSortResultSet = NULL;
    }

// ==========================================================================
// FUNCTION: SortNextFolderL
// ==========================================================================
TBool CContainerStoreSearchHandler::SortNextFolderL()
    {
    if ( ++iCurFolderIdIndex >= iFolderIds.Count() )
        {
        //no more folder to sort/search
        return EFalse;
        }
    
    TMsgStoreSortCriteria sortCriteria;
    
    sortCriteria.iFolderId = iFolderIds[ iCurFolderIdIndex ];
    sortCriteria.iSortBy = iSearchParams.iSortBy;
    sortCriteria.iSortOrder = iSearchParams.iSortOrder;
    sortCriteria.iSecondarySortOrder = iSearchParams.iSecondarySortOrder;
    
    delete iSortResultSet;
    iSortResultSet = NULL;
    
    iSortResultSet = iSortingTable.SortL( sortCriteria, EFalse );
    //disable the automatic refresh
    iSortResultSet->SetAutoRefresh( EFalse );
    iSortResultSet->ToTopL();
    
    return ETrue;
    }    

// ==========================================================================
// FUNCTION: SearchL
// ==========================================================================
TBool CContainerStoreSearchHandler::SearchL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS( "SearchL" )
    
    TBool match = EFalse;
    TBool alreadySearched = EFalse;
    
    //check if we've already searched this message
    if ( !iIsNewSearch )
        {
        alreadySearched = iSearchResultTable.GetMessageL( aMessageId, match );
        __LOG_WRITE8_FORMAT2_DEBUG3( "Re-search: alreadySearch=%d, match=%d", alreadySearched, match )
        }
    
    //we need to get the properties if
    // 1) the message has never been searched (!alreadySearched), or
    // 2) the meeage has been searched and it matches the search result,
    // case 2) is used to populate the iPropertyBuf to be used by the
    // caller of this methos
    if ( !alreadySearched || match )
        {
        //get the properties first
        TContainerId parentId;
        iContainerStore.FastFetchPropertiesL( aMessageId, EFalse, parentId, iPropertyBuf );
        }

    //search the To, Cc, Bcc, Subject if we haven't searched this item before
    if ( !alreadySearched )
        {
        //get the "subject" and "from" the from fields
        TPropertiesDeserializer propDeserializer( iPropertyBuf );
        
        for ( TInt i = 0 ; i < iSearchStrings.Count() ; i++ )
            {
            TPtrC searchString = iSearchStrings[i]->Des();
        
            TBool moreProperties = propDeserializer.First();
            
            match = EFalse;
    
            while( moreProperties && !match )
                {
                if( propDeserializer.Name().Compare( KMsgStorePropertyTo ) == 0 ||
                    propDeserializer.Name().Compare( KMsgStorePropertyCc ) == 0 ||
                    propDeserializer.Name().Compare( KMsgStorePropertyBcc ) == 0 ||
                    propDeserializer.Name().Compare( KMsgStorePropertyFrom ) == 0 )
                    {
                    TPropertiesDeserializer addrDeserializer( propDeserializer.Value() );
                    
                    //search the display name
                    RBuf displayName;
                    CleanupClosePushL( displayName );
                    if ( addrDeserializer.Find( KMsgStorePropertyDisplayName ) )
                        {
                        TPtrC8ToRBuf16L( addrDeserializer.Value(), displayName );
                        }
                    
                    match = displayName.MatchC( searchString ) >= 0;
                    
                    if ( !match )
                        {
                        //search the email address
                        RBuf emailAddr;
                        CleanupClosePushL( emailAddr );
                        if ( addrDeserializer.Find( KMsgStorePropertyEmailAddress ) )
                            {
                            TPtrC8ToRBuf16L( addrDeserializer.Value(), emailAddr );
                            }
                        
                        match = emailAddr.MatchC( searchString ) >= 0;
                        
                        CleanupStack::PopAndDestroy( &emailAddr );
                        }
                    CleanupStack::PopAndDestroy( &displayName );
                    }
                else if ( propDeserializer.Name().Compare( KMsgStorePropertySubject ) == 0 )
                    {
                    //search the subject
                    RBuf subject;
                    CleanupClosePushL( subject );
                    
                    TPtrC8ToRBuf16L( propDeserializer.Value(), subject );
                    
                    match = subject.MatchC( searchString ) >= 0;
                    
                    CleanupStack::PopAndDestroy( &subject );
                    }
                
                moreProperties = propDeserializer.Next();
                }
            
            if ( !match )
                {
                //Find the first child of the message because that where the content is stored
                TBool isContentEncrypted;
                TMsgStoreId firstChildPartId = iContainerStore.FirstChildL( aMessageId, isContentEncrypted );
                //check if the first child part exists, if it does, than that is the body part
                // this is how intellisync supports plan text body.  The html body is sibling 
                // of the plain text body
                TInt bodyPartId = KMsgStoreInvalidId;
                TRAP_IGNORE( bodyPartId=iContainerStore.FirstChildL( firstChildPartId, isContentEncrypted ) );
                if ( bodyPartId == KMsgStoreInvalidId )
                    {
                    //this is no children of firstChildPartId, set the bodyPart to firstChildPartId
                    bodyPartId = firstChildPartId;
                    }
                
                TRAP_IGNORE( match = iContentManager.SearchContentL( bodyPartId, searchString, iSearchBuffer, isContentEncrypted ) );
                }
            
            if ( !match )
                {
                //the list of search strings are treated as "AND", so if there is any search string
                // that does not match, than we can decide that this msg does NOT match.
                // So, break out of the loop
                break;
                } 
            }  //end of for loop
        
        //add message to the already search table
        iSearchResultTable.AddMessageL( aMessageId, match );
        }
    
    return match;
    }

// ==========================================================================
// FUNCTION: CreateSearchPatternL
// ==========================================================================
void CContainerStoreSearchHandler::CreateSearchPatternL( RPointerArray<HBufC>&  aOriginalSearchString, 
                                                         RPointerArray<HBufC>& aSearchPatterns )
    {
    __LOG_ENTER_SUPPRESS("CreateSearchPatternL")
    
    const TChar KStarChar('*');
    
    aSearchPatterns.ResetAndDestroy();

    for ( int i = 0 ; i < aOriginalSearchString.Count() ; i++ )
        {
        HBufC* searchString = aOriginalSearchString[i];
        
        __LOG_BLOCK( TPtr searchStringPtr = searchString->Des(); )
        __LOG_WRITE_FORMAT1_INFO( "search string =%S", &searchStringPtr );
        
        HBufC* pattern = HBufC::NewL( searchString->Length() + 2 );
        aSearchPatterns.AppendL( pattern );
        TPtr seartStringPrt( pattern->Des() );
        
        //we are using MatchC(), so we need to add * to the start and end of the search pattern
        if ( searchString->Locate(KStarChar) != 0 )
            {
            //it does NOT start with "*", so add it
            seartStringPrt.Append( KStarChar );
            }
        seartStringPrt.Append( *searchString );
        if ( searchString->LocateReverse(KStarChar) != searchString->Length() - 1 )
            {
            //it does NOT start with "*", so add it
            seartStringPrt.Append( KStarChar );
            }
        }
    }

// ==========================================================================
// FUNCTION: SearchL
// ==========================================================================
void CContainerStoreSearchHandler::TPtrC8ToRBuf16L( const TPtrC8& aPtr8, RBuf& aBuf ) const
    {
    RBuf8 val8;
    CleanupClosePushL( val8 );
    
    val8.CreateL( aPtr8 );
    
    const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( val8.Ptr() );
    TPtrC16 val16( valuePtr16, val8.Length() / 2 );
    
    aBuf.CreateL( val16.Length() );
    aBuf.Copy( val16 );
    
    CleanupStack::PopAndDestroy( &val8 );
    }


