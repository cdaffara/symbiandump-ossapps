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

#ifndef PBAPFOLDERTREE_H
#define PBAPFOLDERTREE_H

#include <e32base.h>
#include "pbaprequest.h"

class CFolderBase;


class CVirtualFolders : public CBase
	{
public:
	static CVirtualFolders* NewLC();
	~CVirtualFolders();

	void PlaceFolderL(CFolderBase* aFolder);	// aFolder owned by tree
	void AttachSubtree(CVirtualFolders* aSubtree);

	CVirtualFolders* ParentFolder() const;	
	CVirtualFolders* NavigateFolder(const TDesC& aFolderName);
	CFolderBase& Folder() const;
	
private:
	CVirtualFolders();
	void ConstructL();
	
private:
	CFolderBase* iItem;	// the actual object at this node in the tree
	CVirtualFolders* iParentFolder;	// the tree that brought us here
	TSglQue<CVirtualFolders> iSubtrees;
	TSglQueLink iQueLink;
	};

#endif //PBAPFOLDERTREE_H
