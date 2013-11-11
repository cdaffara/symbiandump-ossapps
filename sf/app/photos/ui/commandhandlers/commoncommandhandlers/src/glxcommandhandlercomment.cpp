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

#include <hblabel.h>

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlercomment.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxfetchcontextremover.h>
#include <glxcommondialogs.h>
#include "glxlocalisationstrings.h"

GlxCommandHandlerComment::GlxCommandHandlerComment()
    {
    }

GlxCommandHandlerComment::~GlxCommandHandlerComment()
    {
    }

CMPXCommand* GlxCommandHandlerComment::CreateCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& aConsume) const
    {
    Q_UNUSED(aCommandId);
    Q_UNUSED(aConsume);
    
    CMPXCommand* command = NULL;
    QString mainPane = GetName(aMediaList);
    QString title(GLX_DIALOG_DESCRIPTION);
    QString mediaTitle = NULL;
    bool ok = false;    
    GlxTextInputDialog* dlg = new GlxTextInputDialog(false);
    mediaTitle = dlg->getText(title, mainPane, &ok);
    delete dlg;

    if(ok == true)
        {
        TPtrC16 newMediaItemTitleDes
            = (reinterpret_cast<const TUint16*> (mediaTitle.utf16()));

        HBufC* newMediaItemTitle = newMediaItemTitleDes.Alloc();
        CleanupStack::PushL(newMediaItemTitle);

        CMPXCollectionPath* path = aMediaList.PathLC(
                NGlxListDefs::EPathFocusOrSelection);
        command = 
            TGlxCommandFactory::SetDescriptionCommandLC(*newMediaItemTitle,*path);
        CleanupStack::Pop(command);
        CleanupStack::PopAndDestroy(path);
        CleanupStack::PopAndDestroy(newMediaItemTitle);
        }
    return command;
    }

QString GlxCommandHandlerComment::CompletionTextL() const
    {
	//return command completion text
	return QString();
    }

QString GlxCommandHandlerComment::GetName(MGlxMediaList& aMediaList) const
    {
        // Create an iterator to retrieve the relevant attribute
    TGlxSelectionIterator iterator;
    // only want the title of one item
	iterator.SetRange( 1 );
    CGlxAttributeContext* context = new( ELeave )
        CGlxAttributeContext( &iterator );
	CleanupStack::PushL( context );
        
	// Want to read the title attribute    
    // Configure the context
	context->AddAttributeL( KMPXMediaGeneralComment );
	
	// Add the context to the media list
	aMediaList.AddContextL( context, KGlxFetchContextPriorityBlocking );

    // TGlxContextRemover will remove the context when it goes out of scope
    // Used here to avoid a trap and still have safe cleanup    
    TGlxFetchContextRemover contextRemover( context, aMediaList );
    // put to cleanupstack as cleanupstack is emptied before stack objects
    // are deleted
    CleanupClosePushL( contextRemover );
	// Yuck - a leave and a return code!
	// EFalse => don't show a progress dialog
    TInt err = GlxAttributeRetriever::RetrieveL( *context, aMediaList, EFalse );
    // Using a distinct error value as "LeaveIfError( FuncL() );" looks bad
    User::LeaveIfError( err );
    // context off the list
    CleanupStack::PopAndDestroy( &contextRemover );

    // Now get the title of the relevant item: if *an* item is selected use its
    // title, otherwise use the item with focus
	// Get the index of the item to rename
    TInt index = KErrNotFound;
    QString title = NULL;
	// first see if there's a selected item...
    if ( aMediaList.SelectionCount() == 1 )
    	{
        // Find the index of the selected item
    	index = aMediaList.Count();
    	do
            {
            --index;
            }
        while ( !aMediaList.IsSelected( index ) && index > 0 );
    	}
    else
    	{
    	// Use the index of the item with focus
       	index = aMediaList.FocusIndex();
    	}

	if ( index != KErrNotFound )
		{
        // use iterator to get the right item
        iterator.SetToFirst( &aMediaList );
        const CGlxMedia* media = aMediaList.Item( iterator++ ).Properties();
		if( media )
			{
			// found the item's media properties, extract the title
            const TDesC& titleDesc = media->ValueText(KMPXMediaGeneralComment);
            title = QString::fromUtf16(titleDesc.Ptr(),
                    titleDesc.Length());
			}
		}
    CleanupStack::PopAndDestroy( context );

    return title;
    }


