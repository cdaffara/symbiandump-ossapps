// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CTESTSMTPMOBILITYSETTINGS_H__
#define __CTESTSMTPMOBILITYSETTINGS_H__

#include <test/tefunit.h>

class CImSmtpMobilitySettings;

/**
@internalTechnology
@prototype
*/
class CTestSmtpMobilitySettings : public CActiveTestFixture
	{
public:
	CTestSmtpMobilitySettings();
	~CTestSmtpMobilitySettings();
	
	virtual void SetupL();

	void SetValuesL(CImSmtpMobilitySettings& aSettings);
	void CheckSettingsEmpty(const CImSmtpMobilitySettings& aSettings);
	void CheckSettingsSet(const CImSmtpMobilitySettings& aSettings);

	void TestCreateEmptyL();
	void TestSetValuesL();
	void TestResetL();
	void TestCopyL();
	void TestSmtpSettingsL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	CImSmtpMobilitySettings* iSettings;
	};

#endif __CTESTSMTPMOBILITYSETTINGS_H__
