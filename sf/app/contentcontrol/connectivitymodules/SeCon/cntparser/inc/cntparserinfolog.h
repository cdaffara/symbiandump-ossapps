/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCntParserInfoLog header.
*    This class is intended to create the INFO.LOG file for RANSI car phone when
*    downloading contacts using IrMC L2. The implementation contains only mandatory
*    needs and some extension possibilities. This class is RANSI specific and
*    should _NOT_ be used by anyone else without a deeper knowledge.
*
*/


#ifndef _CNTPARSERINFOLOG_H_
#define _CNTPARSERINFOLOG_H_


class CCntParserInfoLog : public CBase
{
	public:
	CCntParserInfoLog();
	virtual ~CCntParserInfoLog();
	IMPORT_C static CCntParserInfoLog* NewL();
	void ConstructL();

	IMPORT_C void SaveToFileL(const TDesC& aFileName) const;

	IMPORT_C void SetTotalRecords(TInt aTotalRecords);
	IMPORT_C void SetMaximumRecords(TInt aMaximumRecords);

	IMPORT_C TInt TotalRecords() const;
	IMPORT_C TInt MaximumRecords() const;

	private:

	TInt	iTotalRecords;
	TInt	iLastUsedIndex;
	TInt	iMaximumRecords;
};

#endif // _CNTPARSERINFOLOG_H_
