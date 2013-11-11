// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "pbapfoldertree.h"
#include "pbapfolderbase.h"

#include "btaccesshostlog.h"
#include "pbapserver.h"


/*static*/ CVirtualFolders* CVirtualFolders::NewLC()
	{
	LOG_STATIC_FUNC
	CVirtualFolders* self = new (ELeave) CVirtualFolders;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

	
void CVirtualFolders::ConstructL()
	{
	LOG_FUNC
	}
		

CVirtualFolders::CVirtualFolders()
: iSubtrees(_FOFF(CVirtualFolders,iQueLink))
	{
	LOG_FUNC
	}

	
CVirtualFolders::~CVirtualFolders()
	{
	LOG_FUNC
	delete iItem;
	TSglQueIter<CVirtualFolders> iter(iSubtrees);
	CVirtualFolders* subtree=NULL;
	while ((subtree = iter++)!=NULL)
		{
		iSubtrees.Remove(*subtree);
		delete subtree;
		}
	}

/**
Place a folder object into the tree, takes ownership of aFolder
*/
void CVirtualFolders::PlaceFolderL(CFolderBase* aFolder)
	{
	LOG_FUNC
	if(iItem)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderAlreadyInPlace));
		User::Leave(KErrAlreadyExists);
		}
	iItem = aFolder;
	}
	
/**
Return a reference to the current folder object
*/
CFolderBase& CVirtualFolders::Folder() const
	{
	LOG_FUNC
	return *iItem;
	}
	
/**
Attach an entire subtree to the current tree
*/
void CVirtualFolders::AttachSubtree(CVirtualFolders* aSubtree)
	{
	LOG_FUNC

#ifdef _DEBUG // long hand __ASSERT_DEBUG for aSubtree already attached
	TSglQueIter<CVirtualFolders> iter(iSubtrees);
	CVirtualFolders* subtree = NULL;
	while ((subtree = iter++) != NULL)
		{
		if (subtree == aSubtree)
			{
			// aSubtree already exists
			Panic(EVirtualFolderSubtreeAlreadyExists);
			}
		}
#endif // _DEBUG

	iSubtrees.AddLast(*aSubtree);
	aSubtree->iParentFolder = this;
	}


CVirtualFolders* CVirtualFolders::ParentFolder() const
	{
	LOG_FUNC
	return iParentFolder;
	}

/**
Change to the nominated child folder object
@param aFolder the name of PBAP Folder to which to change
*/
CVirtualFolders* CVirtualFolders::NavigateFolder(const TDesC& aFolderName)
	{
	LOG_FUNC
	
	TSglQueIter<CVirtualFolders> iter(iSubtrees);
	
	CVirtualFolders* subtree = NULL;
	while ((subtree = iter++)!=NULL)
		{
		if (subtree->Folder().FolderName()==aFolderName)
			{
			return subtree;			
			}
		}
	return NULL;
	}
