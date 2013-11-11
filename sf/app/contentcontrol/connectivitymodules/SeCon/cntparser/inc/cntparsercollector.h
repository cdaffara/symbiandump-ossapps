/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCntParserCollector header
*
*/



#ifndef __CNTPARSERCOLLECTOR_H__
#define __CNTPARSERCOLLECTOR_H__

#include <f32file.h>
#include <s32file.h>

class CCntParserServer;
class CContactDatabase;
class CContactIdArray;
class CCntFilter;
class CVersitTlsData;

class CCntParserCollector : public CActive
{
	public:

	CCntParserCollector();
	static CCntParserCollector* NewL(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase);
	static CCntParserCollector* NewLC(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase);
	virtual ~CCntParserCollector();
	void ConstructL(CCntParserServer* aServer,CContactDatabase* aCurrentDatabase);
	TInt CreateIrMCL2FileL(const TDesC& aFileName,TInt aMaxNumberOfContacts,TBool aForced);
	void PrepareParserL();
	void ParseCntDatabaseL();
	void DoCancel();

	TInt CurrentItem();
	TInt Count();
	TInt ContactsSaved();

	protected:
	void RunL();
	TInt RunError(TInt aError);
	void ExternalizeCurrentContactL(CContactDatabase* aContactsDb,TInt aCurrentItem,TBool ExternalizeEmpty);
	void CloseResources();
	void SaveTotalCountL();
	TInt ReadTotalCountL() const;
	TBool ContactCountChanged();

	private:
	CContactDatabase*	iContactsDb;	//Caller has the ownership
	TInt iCurrentItem;
	TInt iCount;						//Contacts in CDB
	TInt iMaxNumberOfContacts;			//Max contacts to be parsed (MAXIMUM RECORDS in info.log)
	RFileWriteStream iWriter;
	CContactIdArray* iIds;				//List of ID numbers of contacts
	CCntParserServer* iServer;			//Caller has the ownership
	CCntFilter* iFilter;
	CVersitTlsData* iVersitTLS;			//For speed optimization
	TBool iCancel;
	TInt iContactsSaved;
	TInt iTotalCount;
	
	RFs iFsSession;
};

#endif