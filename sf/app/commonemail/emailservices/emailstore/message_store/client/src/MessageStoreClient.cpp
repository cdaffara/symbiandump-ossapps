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
* Description:  Message store client session to the server.
*
*/



// ========
// INCLUDES
// ========

#include <e32svr.h>
#include <s32mem.h>
//#include <MessageStoreClient.h>
//<cmail>
#include "MsgStoreTypes.h"
#include "DebugLogMacros.h"
//</cmail>

#include "MessageStoreClientServer.h"
#include "emailstoreuids.hrh"
#include "PropertiesSerializer.h"
#include "MsgStoreFolder.h"
#include "MsgStoreObserver.h"
#include "MsgStorePropertyContainerWithContent.h"
#include "MsgStoreSessionContext.h"

// =========
// CONSTANTS
// =========

//const TUint KSearchBufferLength = 30;


/** This class defines the message store search client API.

    This class is used to notify the search client of the completion of an asynchronous search of the
    message store.
*/
class MMsgStoreSearchClient
    {
    public:

        virtual void MatchFound( TMsgStoreId aMessageId ) = 0;
    
        virtual void SearchCompleted() = 0;
    
    }; // end class MMsgStoreSearchClient


/** This class defines the message store quick property client API.

    This class is used to notify the client when a quick property is ready to be processed, or
    to notify the client something has changed, restart building the list
*/
class MMsgStoreQuickPropertyClient
    {
    public:

        virtual void ProcessQuickProperty( const CMsgStorePropertyContainer& aContainer ) = 0;
    
        virtual void Reset() = 0;
    
    }; // end class MMsgStoreSearchClient


    
// ===============================
// CLASS: TPropertyContainerProxy
// ===============================
class TPropertyContainerProxy : public MPropertiesArray
    {
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
        
        TPropertyContainerProxy( MMsgStoreQuickPropertyClient& aClient );
        
        // inherited from MPropertiesArray              
        virtual void AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties );
        virtual void Reset();        
        
    private:
    
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
    
        MMsgStoreQuickPropertyClient& iClient;
        
    }; // end class TPropertyContainerProxy

    
// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ----------------
// CMsgStoreSession
// ----------------

#if 0
// ==========================================================================
// FUNCTION: CopyAttachmentL
// ==========================================================================
EXPORT_C TMsgStoreId CMsgStoreSession::CopyAttachmentL( TMsgStoreId aAttachmentId,
                                                        TMsgStoreId aSourceMessageId, 
                                                        TMsgStoreId aSourceFolderId, 
                                                        TMsgStoreId aDestinationMessageId, 
                                                        TMsgStoreId aDestinationFolderId )
    {
    iContext->VerifyTypeL( aAttachmentId, EMsgStoreAttachmentBits );
    iContext->VerifyTypeL( aSourceMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aSourceFolderId, EMsgStoreFolderBits );
    iContext->VerifyTypeL( aDestinationMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aDestinationFolderId, EMsgStoreFolderBits );
    
    return iContext->iSession.CopyContainerL( aAttachmentId, 
                                              aSourceMessageId, 
                                              aSourceFolderId,
                                              aDestinationMessageId,
                                              aDestinationFolderId );   
    } // end CopyAttachmentL

// ==========================================================================
// FUNCTION: QuickMessagePropertiesL
// ==========================================================================
EXPORT_C void CMsgStoreSession::QuickMessagePropertiesL( TMsgStoreId aFolderId, RPointerArray<CMsgStorePropertyContainer>& aQuickProperties )
    {
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );
    
    TPropertyContainersArray containersArray( aQuickProperties );
    
    iContext->iSession.ChildrenPropertiesL( aFolderId,            // aId
                                            KMsgStoreInvalidId,   // aParentId (do not need to check parent ID)
                                            EMsgStoreMessageBits, // aContainerType
                                            ETrue,                // aQuickProperties
                                            EFalse,               // aRecursive                                         
                                            containersArray );
    } // end QuickMessagePropertiesL
    
// ==========================================================================
// FUNCTION: QuickMessagePropertiesL overloaded
// ==========================================================================
EXPORT_C void CMsgStoreSession::QuickMessagePropertiesL( TMsgStoreId aFolderId, MMsgStoreQuickPropertyClient& aClient )
    {
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );
    
    TPropertyContainerProxy containerProxy( aClient );
    
    iContext->iSession.ChildrenPropertiesL( aFolderId,            // aId
                                            KMsgStoreInvalidId,   // aParentId (do not need to check parent ID)
                                            EMsgStoreMessageBits, // aContainerType
                                            ETrue,                // aQuickProperties
                                            EFalse,               // aRecursive                                         
                                            containerProxy );
    } // end QuickMessagePropertiesL

// ==========================================================================
// FUNCTION: AttachmentsL
// ==========================================================================
EXPORT_C void CMsgStoreSession::AttachmentsL( TMsgStoreId                         aMessageId,
                                              TMsgStoreId                         aFolderId,
                                              RPointerArray<CMsgStoreAttachment>& aAttachmentsList )
    {
    iContext->VerifyTypeL( aMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );

//  TAttachmentsArray attachmentsArray( *iContext, aAttachmentsList );
    
    iContext->iSession.ChildrenPropertiesL( aMessageId,              // aId
                                            aFolderId,               // aParentId
                                            EMsgStoreAttachmentBits, // aContainerType
                                            EFalse,                  // aQuickProperties
                                            EFalse,                  // aRecursive                                                                                      
                                            attachmentsArray ); 
    } // end AttachmentsL

// ==========================================================================
// FUNCTION: AttachmentL
// ==========================================================================
EXPORT_C CMsgStoreAttachment* CMsgStoreSession::AttachmentL( TMsgStoreId aAttachmentId,
                                                             TMsgStoreId aMessageId,
                                                             TMsgStoreId aFolderId )
    {
    iContext->VerifyTypeL( aAttachmentId, EMsgStoreAttachmentBits );
    iContext->VerifyTypeL( aMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );
    
    RBuf8 propertiesBuf;
    CleanupClosePushL( propertiesBuf );
    
    iContext->iSession.ContainerPropertiesL( aAttachmentId, aMessageId, aFolderId, propertiesBuf ); 

    CMsgStoreAttachment* attachment = CMsgStoreAttachment::NewL( *iContext, aAttachmentId, aMessageId, propertiesBuf );
    
    CleanupStack::PopAndDestroy( &propertiesBuf );
    
    return attachment;
    } // end AttachmentL

// ==========================================================================
// FUNCTION: AddAttachmentL
// ==========================================================================
EXPORT_C CMsgStoreAttachment* CMsgStoreSession::AddAttachmentL( TMsgStoreId                       aMessageId,
                                                                TMsgStoreId                       aFolderId,
                                                                const TDesC&                      aFilename, 
                                                                const CMsgStorePropertyContainer& aProperties )
    {
    iContext->VerifyTypeL( aMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );
    
    if( aFilename.Length() == 0 )
        {
        __LOG_STATIC_ENTER( "msg", "AddAttachmentL" )
        __LOG_WRITE_ERROR( "zero length file name" )
        __LOG_STATIC_EXIT
        User::Leave( KErrArgument );
        } // end if
    
    RBuf8 serializedProperties;
    CleanupClosePushL( serializedProperties );
    aProperties.SerializeL( serializedProperties );
    
    TInt id = iContext->iSession.CreateContainerL( aMessageId, 
                                                   aFolderId,
                                                   EMsgStoreAttachmentBits,
                                                   serializedProperties,
                                                   ETrue,
                                                   aFilename );
    
    CMsgStoreAttachment* attachment = CMsgStoreAttachment::NewL( *iContext, id, aMessageId, serializedProperties );
    
    CleanupStack::PopAndDestroy( &serializedProperties );
    
    return attachment;
    } // end AddAttachmentL

// ==========================================================================
// FUNCTION: AddAttachmentL
// ==========================================================================
EXPORT_C CMsgStoreAttachment* CMsgStoreSession::AddAttachmentL( TMsgStoreId                       aMessageId,
                                                                TMsgStoreId                       aFolderId,
                                                                const CMsgStorePropertyContainer& aProperties )
    {
    iContext->VerifyTypeL( aMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );
    
    RBuf8 serializedProperties;
    CleanupClosePushL( serializedProperties );
    aProperties.SerializeL( serializedProperties );
    
    TInt id = iContext->iSession.CreateContainerL( aMessageId, 
                                                   aFolderId,
                                                   EMsgStoreAttachmentBits,
                                                   serializedProperties );
    
    CMsgStoreAttachment* attachment = CMsgStoreAttachment::NewL( *iContext, id, aMessageId, serializedProperties );
    
    CleanupStack::PopAndDestroy( &serializedProperties );
    
    return attachment;
    } // end AddAttachmentL

// ==========================================================================
// FUNCTION: RemoveAttachmentL
// ==========================================================================
EXPORT_C void CMsgStoreSession::RemoveAttachmentL( TMsgStoreId aAttachmentId,
                                                   TMsgStoreId aMessageId,
                                                   TMsgStoreId aFolderId )
    {
    iContext->VerifyTypeL( aMessageId, EMsgStoreMessageBits );
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );
    iContext->VerifyTypeL( aAttachmentId, EMsgStoreAttachmentBits );
    
    iContext->iSession.DeleteContainerL( aAttachmentId, aMessageId, aFolderId );
    } // end RemoveAttachmentL
        
// ==========================================================================
// FUNCTION: SearchL
// ==========================================================================
EXPORT_C void CMsgStoreSession::SearchL( TMsgStoreId            aFolderId, 
                                         TMsgStoreSearchType    aSearchType, 
                                         const TDesC&           aSearchString,
                                         MMsgStoreSearchClient& aSearchClient )
    {
    iContext->VerifyTypeL( aFolderId, EMsgStoreFolderBits );

    if( aSearchString.Length() == 0 )
        {
        __LOG_STATIC_ENTER( "msg", "SearchL" )
        __LOG_WRITE_ERROR( "zero length search string" )
        __LOG_STATIC_EXIT
        User::Leave( KErrArgument );
        } // end if
    
    if( iContext->iSearchHandler )
        {
        __LOG_STATIC_ENTER( "msg", "SearchL" )
        __LOG_WRITE_ERROR( "search already in progress" )
        __LOG_STATIC_EXIT
        User::Leave( KErrInUse );
        } // end if        
    
    iContext->iSearchHandler = CMsgStoreSearchHandler::NewL( iContext->iSession, aFolderId, aSearchType, aSearchString, *this );
    iContext->iSearchClient  = &aSearchClient;
    
    } // end SearchL
                                           
// ==========================================================================
// FUNCTION: CancelSearch
// ==========================================================================
EXPORT_C void CMsgStoreSession::CancelSearch()
    {
    delete iContext->iSearchHandler;
    iContext->iSearchHandler = NULL;
    iContext->iSearchClient  = NULL;
    } // end CancelSearch
    
// ==========================================================================
// FUNCTION: MatchFound
// ==========================================================================
void CMsgStoreSession::MatchFound( TMsgStoreId aMessageId )
    {
    iContext->iSearchClient->MatchFound( aMessageId );
    } // end MatchFound
    
// ==========================================================================
// FUNCTION: SearchCompleted
// ==========================================================================
void CMsgStoreSession::SearchCompleted()
    {
    delete iContext->iSearchHandler;
    iContext->iSearchHandler = NULL;
    
    iContext->iSearchClient->SearchCompleted();
    iContext->iSearchClient = NULL;
    } // end SearchCompleted
    
#endif

// ----------------------
// CMsgStoreSearchHandler
// ----------------------
#if 0
// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreSearchHandler* CMsgStoreSearchHandler::NewL( RMessageStoreSession&  aSession,
                                                      TMsgStoreId            aFolderId, 
                                                      TMsgStoreSearchType    aSearchType, 
                                                      const TDesC&           aSearchString,
                                                      MMsgStoreSearchClient& aSearchClient )
    {
    CMsgStoreSearchHandler* self = new(ELeave) CMsgStoreSearchHandler( aSession, aSearchClient );
    CleanupStack::PushL( self );
    self->ConstructL( aFolderId, aSearchType, aSearchString );
    CleanupStack::Pop( self );
    return self;
    } // end NewL
    
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreSearchHandler::CMsgStoreSearchHandler( RMessageStoreSession& aSession,
                                                MMsgStoreSearchClient& aSearchClient ) :
    CActive( EPriorityStandard ),
    iSession( aSession ),
    iSearchClient( aSearchClient )
    {
    __LOG_CONSTRUCT( "msg", "CMsgStoreSearchHandler" ) 
    
    CActiveScheduler::Add(this);    
    } // end constructor
                               
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreSearchHandler::ConstructL( TMsgStoreId           aFolderId, 
                                         TMsgStoreSearchType   aSearchType, 
                                         const TDesC&          aSearchString )    
    {
    iMatchBuffer[0].CreateL( sizeof(TMsgStoreId) * KSearchBufferLength );
    iMatchBuffer[1].CreateL( sizeof(TMsgStoreId) * KSearchBufferLength );
    
    iSession.PrepareSearchL( aFolderId, aSearchType, aSearchString );
    
    iSession.GetMatchesL( iStatus, iMatchBuffer[iMatchBufferIndex] );
    SetActive();
    } // end ConstructL

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreSearchHandler::~CMsgStoreSearchHandler()
    {
    Cancel();
    
    iMatchBuffer[0].Close();
    iMatchBuffer[1].Close();
    
    if( iThisObjectHasBeenDeleted )
        {
        *iThisObjectHasBeenDeleted = ETrue;
        } // end if
        
    __LOG_DESTRUCT
    } // end destructor
    
// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CMsgStoreSearchHandler::RunL()
    {
    __LOG_ENTER( "RunL" )
    
    RBuf8& currentMatchBuffer = iMatchBuffer[iMatchBufferIndex];
    
    if( iStatus == KErrNone && currentMatchBuffer.Length() > 0 )
        {
        // Switch to the other match buffer for the next call to the session.  Make this call before
        // calling the search client with matches, just in case the client cancels the search during
        // the callback.  This will cause DoCancel to be called during this object's destruction, which
        // will cancel the server-side search.
        iMatchBufferIndex = (iMatchBufferIndex + 1) % 2;
        iSession.GetMatchesL( iStatus, iMatchBuffer[iMatchBufferIndex] );
        SetActive();                                      

        // Client's may cancel the search during callbacks.  This flag mechanism is used to detect that case,
        // and avoid using any heap variables that may no longer be valid.
        TBool thisObjectHasBeenDeleted = EFalse;
        iThisObjectHasBeenDeleted = &thisObjectHasBeenDeleted;
            
        TBool atEndOfIds = (currentMatchBuffer.Length() == 0);
        TUint offset = 0;

        while( !thisObjectHasBeenDeleted && !atEndOfIds )
            {
            const TMsgStoreId& currentId = *reinterpret_cast<const TMsgStoreId *>( currentMatchBuffer.Ptr() + offset );
            
            __LOG_WRITE8_FORMAT1_INFO( "match found (%i)", currentId )  
              
            offset += sizeof(TMsgStoreId);                    
            if( offset >= currentMatchBuffer.Length() )
                {
                atEndOfIds = ETrue;                
                } // end if
                
            // Note that the client may cancel the search during this callback.                    
            iSearchClient.MatchFound( currentId );                    
                                
            } // end while                          

        if( !thisObjectHasBeenDeleted )
            {
            iThisObjectHasBeenDeleted = NULL;
            } // end if
        }
    else
        {
        __LOG_WRITE8_FORMAT1_INFO( "Search completed, iStatus=%d", iStatus.Int() );
        
        iSearchClient.SearchCompleted();
        
        } // end if         

    // __LOG_EXIT was removed because it crashes in cases where thisObjectHasBeenDeleted is true        
    
    } // end RunL
    
// ==========================================================================
// FUNCTION: RunError
// ==========================================================================
TInt CMsgStoreSearchHandler::RunError( TInt aError )
    {
    __LOG_ENTER_SUPPRESS( "RunError" )
    
    __LOG_WRITE8_FORMAT1_ERROR( "aError=%i", aError )
    
    // Something unexpected failed.  Notify the client that search is complete.
    iSearchClient.SearchCompleted();
    
    return KErrNone;
    
    } // end RunError

// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CMsgStoreSearchHandler::DoCancel()
    {
    iSession.CancelSearch();
    } // end 
#endif
        
// ------------------------        
// TPropertyContainerProxy
// ------------------------        

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
TPropertyContainerProxy::TPropertyContainerProxy( MMsgStoreQuickPropertyClient& aClient ) : 
    iClient( aClient ) 
    {
    } // end constructor
       
// ==========================================================================
// FUNCTION: AddElementL
// ==========================================================================
void TPropertyContainerProxy::AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties )
    {
    CMsgStorePropertyContainer *newContainer = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( newContainer );

    newContainer->SetIds( aId, aParentId );
    newContainer->DeserializeL( aProperties );

    iClient.ProcessQuickProperty( *newContainer );    
    
    CleanupStack::PopAndDestroy( newContainer );
    } // end AddElementL
    
// ==========================================================================
// FUNCTION: Reset
// ==========================================================================
void TPropertyContainerProxy::Reset()
    {
    iClient.Reset();
    } // end Reset
        
