/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HLPLCH_INTERNAL_H
#define __HLPLCH_INTERNAL_H

/** Maximum length of command line to launch application. 
@internalComponent
@released
*/
const TInt KMaxCmdLineLength = 512;

/** resource file name
@internalComponent
@released
*/
_LIT(KHelpLauncherResource, "z:\\resource\\apps\\lch.rsc");

class CHlpCmdLine : public CBase
/**
@internalComponent
@released
*/
	{
public:
	static CHlpCmdLine* NewL(CArrayFix<TCoeHelpContext>* aContextList);
	static CHlpCmdLine* NewLC(CArrayFix<TCoeHelpContext>* aContextList);
	~CHlpCmdLine();
public:
	TPtr8 CmdLineL();
private:
	void BuildCmdLineL();
	void AddContextL(TCoeHelpContext& aContext);
private:
	void ConstructL();
	CHlpCmdLine(CArrayFix<TCoeHelpContext>* aContextList);
private:
	CArrayFix<TCoeHelpContext>* iContextList;
	CBufFlat* iCmdLine;
	};
	
#endif //__HLPLCH_INTERNAL_H
