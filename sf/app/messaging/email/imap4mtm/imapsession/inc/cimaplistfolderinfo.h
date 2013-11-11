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

#ifndef __CIMAPLISTFOLDERINFO_H__
#define __CIMAPLISTFOLDERINFO_H__

#include <e32base.h>

/**
Stores the information about a single folder that is 
returned by the IMAP LIST command.
@internalComponent
@prototype
*/
class CImapListFolderInfo : public CBase
	{
public:
	/** 
	Represents each of the four "name attributes" as described in sectio 7.2.2 of RFC 3501
	Note that additional attributes, such as those described in 
	RFC 3348 (\\HasChildren and \\HasNoChildren) are not supported.
	*/
	enum TIMapListFlags {	
   					/**
   					Represents the \\Noinferiors attribute.
   					It is not possible for any child levels of hierarchy to exist
					under this name; no child levels exist now and none can be
					created in the future.
   					*/
   					ENoinferiors	= 0x1,
   					/**
   					Represents the \\Noselect attribute.
   					It is not possible to use this name as a selectable mailbox.
   					*/
					ENoselect		= 0x2,
					/**
					Represents the \\Marked attribute.
					The mailbox has been marked "interesting" by the server; the
					mailbox probably contains messages that have been added since
					the last time the mailbox was selected.
					*/
					EMarked			= 0x4,
					/**
					Represents the \\Unmarked attribute.
					The mailbox does not contain any additional messages since the
					last time the mailbox was selected.
					*/
					EUnmarked		= 0x8
  					};

	static CImapListFolderInfo* NewL();
	~CImapListFolderInfo();

	void SetFolderNameL(HBufC* aFolderName);
	IMPORT_C TPtrC FolderName() const;
   
	void SetFlag(TIMapListFlags aFlagId, TBool aValue);
	IMPORT_C TBool QueryFlag(TIMapListFlags aFlagId);
	
	static TInt CompareByFolderName(const CImapListFolderInfo& aFirst, const CImapListFolderInfo& aSecond);

private:
	CImapListFolderInfo();

public: 
	/**
	Each individual folder on an IMAP server can have its own hierarchy seperator.
	Typically, all mail folders might use '\' or '/' as their hierarchy seperator,
	but news folders on the same IMAP server would use '.'
	See the example in section 6.3.8 of RFC 3501
	*/
	TChar iHierarchySeperator; 
	
	/**
	Bitwise combinaton of TIMapListFlags.
	*/
	TUint8 iFlags;

private:
	/**
	The name of the folder
	*/
	HBufC* iFolderName;
    };

typedef RPointerArray<CImapListFolderInfo> RArrayImapListFolderInfo;

		 
#endif // __CIMAPLISTFOLDERINFO_H__
