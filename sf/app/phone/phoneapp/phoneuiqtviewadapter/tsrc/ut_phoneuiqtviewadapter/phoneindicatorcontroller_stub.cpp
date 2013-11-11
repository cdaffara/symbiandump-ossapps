/*
 * phoneindicatorcontroller_stub.cpp
 *
 *  Created on: May 4, 2010
 *      Author: maposio
 */

#include "phoneindicatorcontroller.h"

bool m_setActiveCallCalled;
bool m_clearActiveCallCalled;
bool m_enableCallIndCalled;
bool m_disableCallIndCalled;

PhoneIndicatorController::PhoneIndicatorController(
    PhoneVisibilityHandler& visibilityHandler, QObject *parent)
    :
    QObject(parent)
{
    m_setActiveCallCalled = false;
    m_clearActiveCallCalled = false;
    m_enableCallIndCalled = false;
    m_disableCallIndCalled = false;
}

PhoneIndicatorController::~PhoneIndicatorController()
{
// TODO Auto-generated destructor stub
}


void PhoneIndicatorController::setActiveCallData()
{
    m_setActiveCallCalled = true;
}

void PhoneIndicatorController::clearActiveCallData()
{
    m_clearActiveCallCalled = true;
}

void PhoneIndicatorController::enableActiveCallIndicator()
{
    m_enableCallIndCalled = true;
}

void PhoneIndicatorController::disableActiveCallIndicator()
{
    m_disableCallIndCalled = true;
}
