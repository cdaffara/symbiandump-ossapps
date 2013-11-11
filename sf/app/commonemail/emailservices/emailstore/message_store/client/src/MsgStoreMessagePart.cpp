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
* Description:  Message store multipart message client support implementation.
*
*/



//<cmail>
#include "MsgStoreMessagePart.h"
#include "MsgStorePropertyKeys.h"
//</cmail>
#include "MsgStoreSessionContext.h"
#include "RMessageStoreSession.h"
#include "MessageStoreClientServer.h"
#include "MsgStoreMessage.h"

//_LIT( KTextPlain, "text/plain" );
//_LIT( KTextHTML, "text/html" );
_LIT(KFSMailContentTypeMultipartAlternative, "multipart/alternative");


// ========================
// CLASS: TPartsArray
// ========================
class TPartsArray : public MPropertiesArray
    {
    public:
    
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	    
        TPartsArray( CMsgStoreSessionContext& aContext, TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreMessagePart>& aParts );
        
        // inherited from MPropertiesArray              
        virtual void AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties );
        virtual void Reset();        
        
    private:
    
 	    // ==================
	    // PRIVATE ATTRIBUTES
	    // ==================
	
        CMsgStoreSessionContext&             iContext;
        TMsgStoreId                          iMailBoxId;
        RPointerArray<CMsgStoreMessagePart>& iParts;
        
    }; // end class TPartsArray

    
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
TPartsArray::TPartsArray( CMsgStoreSessionContext& aContext, TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreMessagePart>& aParts ) : 
    iContext( aContext ), iMailBoxId( aMailBoxId ), iParts( aParts ) 
    {
    } // end constructor
       
// ==========================================================================
// FUNCTION: AddElementL
// ==========================================================================
void TPartsArray::AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties )
    {
    CMsgStoreMessagePart *newPart = CMsgStoreMessagePart::NewL( iContext, aId, aParentId, iMailBoxId, aProperties );
    CleanupStack::PushL( newPart );
    
    iParts.AppendL( newPart );    
    
    CleanupStack::Pop( newPart );
    } // end AddElementL
    
// ==========================================================================
// FUNCTION: Reset
// ==========================================================================
void TPartsArray::Reset()
    {
    iParts.ResetAndDestroy();
    } // end Reset
    

/** Adds a child part to this part 

    /param aProperties the headers of the newly added part
    /note This method leaves with KErrNotSupported if it is NOT multi-part, i.e. this part
    already has content.
 */
EXPORT_C CMsgStoreMessagePart* CMsgStoreMessagePart::AddChildPartL( const CMsgStorePropertyContainer& aProperties,
                                                                    const TDesC& aContentFilename                 )
	{
	RBuf8 serializedProperties;
	CleanupClosePushL( serializedProperties );
	aProperties.SerializeL( serializedProperties );
	
	TInt partId = iContext.iSession.CreateContainerL( Id(), 
													  KMsgStoreInvalidId,   //do not check grand parent id
													  iMailBoxId,
													  EMsgStorePartBits,
													  serializedProperties,
													  ETrue,
                                                      aContentFilename );
	
	CMsgStoreMessagePart* part = CMsgStoreMessagePart::NewL( iContext, partId, Id(), iMailBoxId, serializedProperties );
	
	CleanupStack::PopAndDestroy( &serializedProperties );
	
	return part;
	}
		

/** Adds a child part to this part and set the content to the same content stored in the input file handle 

    /param aProperties the headers of the newly added part
            /param aFile file handle that contains the content.
    /note This method leaves with KErrNotSupported if it is NOT multi-part, i.e. this part
    already has content.
 */
EXPORT_C CMsgStoreMessagePart* CMsgStoreMessagePart::AddChildPartL( const CMsgStorePropertyContainer& aProperties,
                                                                    RFile& aFile )
	{
	RBuf8 serializedProperties;
	serializedProperties.CleanupClosePushL();
	aProperties.SerializeL( serializedProperties );
	
	TInt partId = iContext.iSession.CreateContainerL( Id(), 
													  KMsgStoreInvalidId,   //do not check grand parent id
													  iMailBoxId,
													  EMsgStorePartBits,
													  serializedProperties,
													  ETrue,
													  aFile );
	
	CMsgStoreMessagePart* part = CMsgStoreMessagePart::NewL( iContext, partId, Id(), iMailBoxId, serializedProperties );
	
	CleanupStack::PopAndDestroy( &serializedProperties );
	
	return part;
	}


/** Adds an existing part to the child 

    /param aPart already created child part
    /note This method leaves with KErrNotSupported if it is NOT multi-part, i.e. this part
    already has content.
 */

EXPORT_C CMsgStoreMessage* CMsgStoreMessagePart::AddChildMessageL( CMsgStorePropertyContainer& aProperties )
	{
	aProperties.AddOrUpdatePropertyL( KMsgStorePropertyIsEmbeddedMsg, ETrue );

	return static_cast<CMsgStoreMessage*>( AddChildPartL( aProperties ) );
	}


/** Copies an attachment from one message to another message.
	
	This function copies an an attachment, including the properties and content, from one
	message to another message.  This function will leave with KErrNotFound if the specified
	message is not found in the source folder or if the destination message is not found in
	the destination folder.

    KMsgStoreInvalidId may be passed for the destination folder ID if there is no desire to 
    check the source folder.
    
	ECapabilityWriteUserData is required to use this function.			
	
	\retval Returns the ID of the new message.

    \note Only works if the store is in an authenticated state, otherwise this function leaves
          with KErrNotReady.
*/		
EXPORT_C TMsgStoreId CMsgStoreMessagePart::CopyChildPartL( TMsgStoreId aPartId, TMsgStoreId aNewParentId )
	{
	iContext.VerifyTypeL( aPartId, EMsgStorePartBits );
	//new parent can be a message or a part
	iContext.VerifyTypeL( aNewParentId, EMsgStoreMessageBits, EMsgStorePartBits );
	
	return iContext.iSession.CopyContainerL( aPartId, 
	                                         Id(), 
	                                         KMsgStoreInvalidId,     //don't care
	                                         aNewParentId,
	                                         KMsgStoreInvalidId,     //don't care
	                                         iMailBoxId);	
	} // end CopyMessageL


/** Removes a child part from this part 

    /note This method leaves with KErrNotSupported if it is NOT multi-part.
 */
EXPORT_C void CMsgStoreMessagePart::RemoveChildPartL( TMsgStoreId aPartId, TBool aRecursive )
	{
	iContext.VerifyTypeL( aPartId, EMsgStorePartBits );
    
    TMsgStoreId parentId = ( aRecursive ? KMsgStoreInvalidId : Id() );
	
	iContext.iSession.DeleteContainerL( aPartId, parentId, KMsgStoreInvalidId, iMailBoxId );
	}

/** gets a child part fo this part 
    /note This method leaves with KErrNotSupported if it is NOT multi-part.
 */
EXPORT_C CMsgStoreMessagePart* CMsgStoreMessagePart::ChildPartL( TMsgStoreId aPartId, TBool aRecursive  )
	{
	iContext.VerifyTypeL( aPartId, EMsgStorePartBits );
	
	TMsgStoreId parentId = ( aRecursive ? KMsgStoreInvalidId : Id() );
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	
	iContext.iSession.ContainerPropertiesL( aPartId, parentId, KMsgStoreInvalidId, propertiesBuf );	
	
	if ( !aRecursive && parentId != Id() )
		{
		//the part may have been moved, so it is no longer a child of this part
		User::Leave( KErrNotFound );
		}
	
	CMsgStoreMessagePart* part = CMsgStoreMessagePart::NewL( iContext, aPartId, parentId, iMailBoxId, propertiesBuf );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	
	return part;
	}

/** list all the child part to this part 
    /note This method leaves with KErrNotSupported if it is NOT multi-part.
*/
EXPORT_C void CMsgStoreMessagePart::ChildPartsL( RPointerArray<CMsgStoreMessagePart>& aParts )
{
	
	TPartsArray partsArray( iContext, iMailBoxId, aParts );
	
	iContext.iSession.ChildrenPropertiesL( iId,               // aId
	                                       iParentId,         // aParentId
	                                       EMsgStorePartBits, // aContainerType
	                                       EFalse,            // aQuickProperties
	                                       EFalse,            // aRecursive	                                        	                                        
	                                       partsArray );	
	
	// make sure body come before attachments
	TInt count = aParts.Count(); 
	if ( count > 1 )
		{
		for ( TInt i = 0 ; i < count; i++ )
			{
			CMsgStoreMessagePart* part = aParts[i];
			TUint index = 0;
			if ( part->FindProperty( KMsgStorePropertyContentType, index ) )
				{
				const TDesC& contentType = part->PropertyValueDesL( index );
				if ( contentType == KFSMailContentTypeMultipartAlternative )
					{
						if( i > 0 )  
							{
							//body is not the first child, move it to the first place
							aParts.Remove(i);
							aParts.InsertL( part, 0 );
							}
						break;
					}
				}
			}
		}
}

EXPORT_C TBool CMsgStoreMessagePart::IsEmbeddedMessageL()
	{
	
	TUint index;
	TBool isEmbeddedMsg = EFalse;
	
	if ( FindProperty( KMsgStorePropertyIsEmbeddedMsg, index ) )
		{
		isEmbeddedMsg = PropertyValueBoolL( index );
		}
	
	return isEmbeddedMsg;
	
	}


CMsgStoreMessagePart* CMsgStoreMessagePart::NewL( CMsgStoreSessionContext& aSessionContext, 
												  TMsgStoreId              aId, 
												  TMsgStoreId              aParentId, 
												  TMsgStoreId              aMailBoxId,
												  const TDesC8&            aPropertiesBuf )
	{												  
	CMsgStoreMessagePart* self = new( ELeave ) CMsgStoreMessagePart( aSessionContext, aId, aParentId, aMailBoxId );
	CleanupStack::PushL( self );
	self->ConstructL( aPropertiesBuf );
	
	//check if the flag is embedded message exists
	if ( self->IsEmbeddedMessageL() )
		{
		CleanupStack::PopAndDestroy( self );
		//create an instance of embedded message
		self = CMsgStoreMessage::NewL( aSessionContext, aId, aParentId, KMsgStoreInvalidId, aMailBoxId, aPropertiesBuf );
		}
	else
		{
		CleanupStack::Pop( self );
		}
	
	return self;
	}

void CMsgStoreMessagePart::ConstructL( const TDesC8& aPropertiesBuf )
	{
	DeserializeL( aPropertiesBuf );
	}

CMsgStoreMessagePart::CMsgStoreMessagePart( CMsgStoreSessionContext& aSessionContext, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aMailBoxId )
: CMsgStorePropertyContainerWithContent( aSessionContext, aId, aParentId, aMailBoxId )
	{
	}
		

		
