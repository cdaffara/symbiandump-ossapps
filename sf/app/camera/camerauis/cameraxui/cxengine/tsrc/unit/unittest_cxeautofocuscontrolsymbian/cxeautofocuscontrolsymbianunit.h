/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXEAUTOFOCUSCONTROLSYMBIANUNIT_H_
#define CXEAUTOFOCUSCONTROLSYMBIANUNIT_H_

#include "cxeautofocuscontrolsymbian.h"

/*
 * CxeAutoFocusControlSymbianUnit is inherited from
 * the class to be tested (CxeAutoFocusControlSymbian) so that
 * the tester class (UnitTestCxeAutoFocusControlSymbian) can be declared
 * as friend.
 */

class CxeCameraDevice;
class CxeSettings;

class CxeAutoFocusControlSymbianUnit: public CxeAutoFocusControlSymbian
{
public:
    CxeAutoFocusControlSymbianUnit(CxeCameraDevice &cameraDevice, CxeSettings &settings);
    virtual ~CxeAutoFocusControlSymbianUnit();

    friend class UnitTestCxeAutoFocusControlSymbian;
};

#endif /* CXEAUTOFOCUSCONTROLSYMBIANUNIT_H_ */
