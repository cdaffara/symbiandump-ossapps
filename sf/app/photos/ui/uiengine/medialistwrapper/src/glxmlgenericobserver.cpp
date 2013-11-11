/*
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
*/


/*glxmlgenericobserver.cpp*/

// INCLUDE FILES

//Gallery Headers
#include <glxtracer.h>                                 // For TRACER
#include <glxlog.h>                                     // For Glx Logging
#include <glxerrormanager.h>                        //?    // For CGlxErrormanager
#include <glxuistd.h>
#include <mglxmedialist.h>                              // CGlxMedialist
#include <glxthumbnailattributeinfo.h>              //?   // KGlxMediaIdThumbnail
#include <glxdrmutility.h>                              // DRM utility class to provide DRM-related functionality
#include <mpxmediadrmdefs.h>                            // KMPXMediaDrmProtected


#include "glxmlgenericobserver.h"
#include "glxmlwrapper_p.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMLGenericObserver* CGlxMLGenericObserver::NewL(
        MGlxMediaList& aMediaList,GlxMLWrapperPrivate* aMLWrapperPrivate)
    {
    TRACER("CGlxMLGenericObserver::NewLC()");
    CGlxMLGenericObserver* self = 
            new (ELeave) CGlxMLGenericObserver(aMediaList , aMLWrapperPrivate);
    //CleanupStack::PushL(self); //To:Do remove the comment later
    self->ConstructL();
    //CleanupStack::Pop(self);
    return self;
    }
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMLGenericObserver::CGlxMLGenericObserver(MGlxMediaList& aMediaList,
        GlxMLWrapperPrivate* aMLWrapperPrivate  ) :
    iMediaList(aMediaList), iMLWrapperPrivate(aMLWrapperPrivate)
    {
    TRACER("CGlxMLGenericObserver::CGlxMLGenericObserver");
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::ConstructL()
    {
    TRACER("CGlxMLGenericObserver::ConstructL");
    iMediaList.AddMediaListObserverL(this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMLGenericObserver::~CGlxMLGenericObserver()
    {
    TRACER("CGlxMLGenericObserver::~CGlxMLGenericObserver");
    iMediaList.RemoveMediaListObserver( this );
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxMLGenericObserver::HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, 
     MGlxMediaList* aList )
    {
    TRACER("CGlxMLGenericObserver::HandleItemAddedL()");
    GLX_DEBUG3("CGlxMLGenericObserver::HandleItemAddedL() aStartIndex(%d),"
            " aEndIndex(%d)", aStartIndex, aEndIndex);

    //Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleItemAddedL( aStartIndex,aEndIndex,aList );
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleItemRemovedL( TInt aStartIndex, 
        TInt aEndIndex, MGlxMediaList* aList )
    {
    TRACER("CGlxMLGenericObserver::HandleItemRemovedL()");
	 //Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleItemRemovedL( aStartIndex,aEndIndex,aList );
    }

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleAttributesAvailableL( TInt aItemIndex, 
    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList )
    {
    TRACER("CGlxMLGenericObserver::HandleAttributesAvailableL()");
	 //Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleAttributesAvailableL(aItemIndex,aAttributes,aList);
    }
    
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleFocusChangedL( NGlxListDefs::
    TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, 
    MGlxMediaList* aList )
    {
    TRACER("CGlxMLGenericObserver::HandleFocusChangedL()");
	//Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleFocusChangedL(aType,aNewIndex,aOldIndex,aList );
    }

// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleItemSelectedL(TInt /*aIndex*/, 
    TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMLGenericObserver::HandleItemSelectedL");
	//Forward call to the MLWrapperPrivate
   // iMLWrapperPrivate->HandleFocusChangedL((int)aIndex,(int)aSelected,aList );
    }

// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
 void CGlxMLGenericObserver::HandleMessageL( const CMPXMessage& /*aMessage*/, 
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMLGenericObserver::HandleMessageL()");
	//Forward call to the MLWrapperPrivate
   // iMLWrapperPrivate->HandleFocusChangedL(aMessage,aList );
    }
  
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxMLGenericObserver::HandleError( TInt aError ) 
    {
    TRACER("CGlxMLGenericObserver::HandleError()");
    //Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleError(aError);
    }

// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
    TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMLGenericObserver::HandleCommandCompleteL()");
	//Forward call to the MLWrapperPrivate
    //todo remove comment iMLWrapperPrivate->HandleCommandCompleteL(aCommandResult,aError,aList);
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleMediaL( TInt aListIndex, MGlxMediaList* aList )
    {
    TRACER("CGlxMLGenericObserver::HandleMediaL()");
	//Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleMediaL(aListIndex,aList);

    }
  
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxMLGenericObserver::HandleItemModifiedL( const RArray<TInt>& aItemIndexes,
    MGlxMediaList* aList)
    {
    TRACER("CGlxMLGenericObserver::HandleItemModifiedL()");
	//Forward call to the MLWrapperPrivate
    iMLWrapperPrivate->HandleItemModifiedL(aItemIndexes, aList);
    }
                  
void CGlxMLGenericObserver::HandlePopulatedL(MGlxMediaList* aList)
    {
    TRACER("CGlxMLGenericObserver::HandlePopulatedL()");
    iMLWrapperPrivate->HandlePopulatedL(aList);
    }
     

