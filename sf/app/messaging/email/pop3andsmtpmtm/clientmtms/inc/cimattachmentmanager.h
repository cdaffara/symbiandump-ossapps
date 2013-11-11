// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CIMATTACHMENTMANAGER_H
#define CIMATTACHMENTMANAGER_H
#include <mmsvattachmentmanager.h>
#include <miutmsg.h>

const TUid KUidMsvEmailAttachmentEntryId = {0x10205EA7};
const TUid KUidMsvEmailAttachmentEntryIndex = {0x10205EA8};

/**
Handles attachments  for CIMEmailMessage 
*/
class CImAttachmentManager : public CActive, public MMsvAttachmentManager 
	{
public:	
	static CImAttachmentManager* NewL(CImEmailMessage& aEmailMessage, CMsvEntry& aEntry);
	~CImAttachmentManager();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
	// from MMsvAttachmentManager
	virtual void AddAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	virtual void AddAttachmentL(RFile& aFileHandle, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	virtual void AddLinkedAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	virtual void AddEntryAsAttachmentL(TMsvId aEntryId, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	virtual void CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	virtual void RenameAttachmentL(TInt aIndex, const TDesC& aNewName, TRequestStatus& aStatus);
	virtual TInt AttachmentCount() const;
	virtual CMsvAttachment* GetAttachmentInfoL(TInt aIndex);
	virtual CMsvAttachment* GetAttachmentInfoL(TMsvAttachmentId aId);
	virtual void ModifyAttachmentInfoL(CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	virtual RFile GetAttachmentFileL(TInt aIndex);
	virtual RFile GetAttachmentFileL(TMsvAttachmentId aId);
	virtual RFile GetAttachmentFileForWriteL(TInt aIndex);
	virtual RFile GetAttachmentFileForWriteL(TMsvAttachmentId aId);
	virtual void RemoveAttachmentL(TInt aIndex, TRequestStatus& aStatus);
	virtual void RemoveAttachmentL(TMsvAttachmentId aId, TRequestStatus& aStatus);
	virtual void CancelRequest();
	
	// utility functions
	RPointerArray<CMsvAttachment>& AttachmentInfoSelection(); 
	void LoadL();
	void AppendAttachmentArrayL(TMsvId aAttachmentId);
	void DeleteAttachmentInArrayL(TMsvId aDeletedAttachmentId);
	TInt UniqueAttachmentId();
private:
	CImAttachmentManager(CImEmailMessage& aEmailMessage,CMsvEntry& aEntry);
	void ConstructL();

	TInt AttachmentIndexInMessage(CMsvAttachment& aAttachmentInfo);
	TInt AttachmentIndexInMessage(TMsvAttachmentId aId);
	void UpdateModifiedAttachmentInListL();
	void ChangeEntryDetailsL();
	void ModifyAttachmentInfoL();
private:
	enum TImAttachmentManagerStates
		{
		EModifyAttachmentInfo=0,
		EChangeEntryDetails
		};
		
	RPointerArray<CMsvAttachment>	iAttachmentInfoList;
	CMsvEntry&						iClientEntry;
	CImEmailMessage& 				iEmailMessage;	
	CMsvStore*						iStore;
	TRequestStatus*					iReportStatus;
	TInt							iAttachmentManagerState;
	CMsvOperation*					iMsvOperation;
	TInt							iUniqueId;
	CMsvEntry*						iEntry;
	};
#endif // CIMATTACHMENTMANAGER_H
