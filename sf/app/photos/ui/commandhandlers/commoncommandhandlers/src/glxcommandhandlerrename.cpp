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

//Includes Qt/Symbian
#include <QFile>
#include <caf/manager.h>

//Includes Orbit
#include <hblabel.h>
#include <hbmessagebox.h>

//User Includes
#include <mglxmedialist.h>
#include <glxcommondialogs.h>
#include <mpxcollectionpath.h>
#include <glxcommandfactory.h>
#include <glxattributecontext.h>
#include <glxcommandhandlers.hrh>
#include <glxattributeretriever.h>
#include "glxlocalisationstrings.h"
#include <glxfetchcontextremover.h>
#include <glxcommandhandlerrename.h>   

GlxCommandHandlerRename::GlxCommandHandlerRename()
    {
    }

GlxCommandHandlerRename::~GlxCommandHandlerRename()
    {
    }

CMPXCommand* GlxCommandHandlerRename::CreateCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& aConsume) const
    {
    Q_UNUSED(aCommandId);
    Q_UNUSED(aConsume);
    
    bool ok = false;
    CMPXCommand* command = NULL;
    CMPXCollectionPath* path = NULL;
    QString mainPane = GetName(aMediaList);
    GlxTextInputDialog* dlg = new GlxTextInputDialog();

    if (aCommandId == EGlxCmdRenameFile)
        {
        mMediaTitle = dlg->getText(GLX_DIALOG_NAME_PROMPT, mainPane, &ok);
        }
    else
        {
        mMediaTitle = dlg->getText(GLX_DIALOG_ALBUM_PROMPT, mainPane, &ok);
        }
    delete dlg;

    if(ok == true)
        {
        TPtrC16 newMediaItemTitleDes =
                (reinterpret_cast<const TUint16*> (mMediaTitle.utf16()));
        HBufC* newMediaItemTitle = newMediaItemTitleDes.Alloc();
        CleanupStack::PushL(newMediaItemTitle);

        if (aCommandId == EGlxCmdRenameFile)
            {
            TDesC& aTitleText = *newMediaItemTitle;
            TInt index = aMediaList.FocusIndex();
            const TGlxMedia& media = aMediaList.Item(index);
            TParsePtrC parsePtr(media.Uri());

            //Constructs the File Name with complete Path
            TFileName destinationFileName;
            destinationFileName.Append(parsePtr.DriveAndPath());
            destinationFileName.Append(aTitleText);
            destinationFileName.Append(parsePtr.Ext());

            HBufC* modifiedName = destinationFileName.AllocLC();
            QString fileName = QString::fromUtf16(modifiedName->Ptr(),
                    modifiedName->Length());

            ContentAccess::CManager *manager =
                    ContentAccess::CManager::NewL();
            CleanupStack::PushL(manager);

            QFile filePath(fileName);
            bool isFileExist = filePath.exists();

            if (!isFileExist)
                {
                //rename the media
                TInt error = manager->RenameFile(media.Uri(), *modifiedName);

                if (error == KErrNone)
                    {
                    path = aMediaList.PathLC(
                            NGlxListDefs::EPathFocusOrSelection);
                    command = TGlxCommandFactory::RenameCommandLC(
                            *newMediaItemTitle, *path);
                    CleanupStack::Pop(command);
                    CleanupStack::PopAndDestroy(path);
                    }
                }
            else
                {
                //The Name of the file name already exists, display an message .
                HandleErrorL(KErrAlreadyExists);
                }
            CleanupStack::PopAndDestroy(manager);
            CleanupStack::PopAndDestroy(modifiedName);
            CleanupStack::PopAndDestroy(newMediaItemTitle);
            }
        else
            {
            //Rename of Album
            CMPXCollectionPath* path = aMediaList.PathLC(
                    NGlxListDefs::EPathFocusOrSelection);
            command = TGlxCommandFactory::RenameCommandLC(*newMediaItemTitle,
                    *path);
            CleanupStack::Pop(command);
            CleanupStack::PopAndDestroy(path);
            CleanupStack::PopAndDestroy(newMediaItemTitle);
            }
        }
    return command;
    }

QString GlxCommandHandlerRename::CompletionTextL() const
    {
	//return command completion text
	return QString();
    }

QString GlxCommandHandlerRename::GetName(MGlxMediaList& aMediaList) const
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
	context->AddAttributeL( KMPXMediaGeneralTitle );
	
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
            const TDesC& titleDesc = media->ValueText(KMPXMediaGeneralTitle);
            title = QString::fromUtf16(titleDesc.Ptr(),
                    titleDesc.Length());
			}
		}
    CleanupStack::PopAndDestroy( context );

    return title;
    }

void GlxCommandHandlerRename::HandleErrorL(TInt aErrorCode) const
	{
    if(aErrorCode == KErrAlreadyExists)
        {
        QString stringToDisplay = hbTrId(GLX_NAME_ALREADY_EXIST).arg(
                mMediaTitle);
        HbMessageBox::warning(stringToDisplay);
        }
	else{
		GlxMpxCommandHandler::HandleErrorL(aErrorCode);
		}	
	}

