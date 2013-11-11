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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __MOBILITYTESTMTMAPI_H__
#define __MOBILITYTESTMTMAPI_H__

#ifndef __MOBILITY_TEST_FRAMEWORK

#define MOBILITY_TEST_CREATE(serviceId, mobilityManager)
#define MOBILITY_TEST_DELETE(mobilityManager)
#define MOBILITY_TEST_MTM_STATE(serviceId, state)

#else //__MOBILITY_TEST_FRAMEWORK

#include "timmobilitytestmtmstate.h"
#include "cimmobilitytestframework.h"

#define MOBILITY_TEST_CREATE(serviceId, mobilityManager) CImMobilityTestFramework::Create(serviceId, mobilityManager)
#define MOBILITY_TEST_DELETE(mobilityManager) CImMobilityTestFramework::Delete(mobilityManager)
#define MOBILITY_TEST_MTM_STATE(serviceId, state) CImMobilityTestFramework::MtmState(serviceId, state)

#endif //__MOBILITY_TEST_FRAMEWORK

#endif //__MOBILITYTESTMTMAPI_H__
