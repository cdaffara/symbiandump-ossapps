// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Parsing and Processing of script commands, to test the
// SMIL DOM classes.
// 
//


#ifndef __T_SCRIPTRUNNER_H
#define __T_SCRIPTRUNNER_H

#include "t_TestScript.h"
#include "t_LogFileWriter.h"
#include <badesca.h>
#include <MDXMLSMIL.h>  // The SMIL DOM classes we are testing

// Enum used to choose an element-specific Id operation.
typedef enum
	{
	KIdSetOp,
	KIdGetOp,
	KIdRemoveOp
	} TIdOp;

//
//
// CScriptRunner
//

class CScriptRunner : public CBase
	{
public:
	static CScriptRunner* NewL();
	~CScriptRunner();

	void ParseL(const TDesC& aCommand);

private:
	CScriptRunner();
	void ConstructL();

	void ProcessL();
	void IdAttTestL( TIdOp aOp );

private:
	HBufC*				iCurrentCommand;
	CDesCArrayFlat*		iArgumentList;
	CTestDataWriter*	iWriter;
	CMDXMLElement*		iCurrentElement; // Just a pointer, nothing is owned
	CMDXMLSMILDocument*	iDOMtreeRoot;
	};


#endif
