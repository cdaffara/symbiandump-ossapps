/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <smcmockclassincludes.h>
#include <QDebug>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <HbCheckBox>
#include <QTranslator>
#include <QLocale>
#include <QApplication>
#include <QTimer>
#include <cpitemdatahelper.h>
#include <psetwrapper.h>
#include "cpbarringplugingroup.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpBarringPluginGroup::CpBarringPluginGroup
// -----------------------------------------------------------------------------
//
CpBarringPluginGroup::CpBarringPluginGroup( 
        CpItemDataHelper & helper )
    :CpSettingFormItemData(HbDataFormModelItem::GroupItem, hbTrId("Barring"),0),
    m_helper(helper)
    {
    
    }


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::~CpBarringPluginGroup
// -----------------------------------------------------------------------------
//
CpBarringPluginGroup::~CpBarringPluginGroup(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::itemShown
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::itemShown( 
        const QModelIndex & item )
    {
    SMC_MOCK_METHOD1( void, const QModelIndex &, item )
    }


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::barringStatusRequestCompleted
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::barringStatusRequestCompleted(
    int result,
    const QList<unsigned char> & basicServiceGroupIds,
    PSetCallBarringWrapper::BarringStatus status)
{
    SMC_MOCK_METHOD3( void, 
        int, result,
        const QList<unsigned char> &, basicServiceGroupIds, 
        PSetCallBarringWrapper::BarringStatus, status)
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::enableBarringRequestCompleted
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::enableBarringRequestCompleted(
    int result,
    PSetCallBarringWrapper::BarringType barringType,
    PSetCallBarringWrapper::BarringStatus barringStatus, 
    bool plural)
{
    SMC_MOCK_METHOD4( void, 
        int, result,
        PSetCallBarringWrapper::BarringType, barringType, 
        PSetCallBarringWrapper::BarringStatus, barringStatus,
        bool, plural)
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::disableBarringRequestCompleted
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::disableBarringRequestCompleted(
    int result,
    PSetCallBarringWrapper::BarringType barringType,
    PSetCallBarringWrapper::BarringStatus barringStatus, 
    bool plural)
{
    SMC_MOCK_METHOD4( void, 
        int, result,
        PSetCallBarringWrapper::BarringType, barringType, 
        PSetCallBarringWrapper::BarringStatus, barringStatus,
        bool, plural)
}

// -----------------------------------------------------------------------------
// CpBarringPluginGroup::barringPasswordChangeRequestCompleted
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::barringPasswordChangeRequestCompleted(int result)
{
    SMC_MOCK_METHOD1( void, int, result)
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::changeBarringStateRequested
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::changeBarringStateRequested(int checkState)
{
    SMC_MOCK_METHOD1( void, int, checkState)
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::changeBarringPasswordRequested
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::changeBarringPasswordRequested()
{
    SMC_MOCK_METHOD0( void ) 
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::processBarringStatusRequestQueue
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::processBarringStatusRequestQueue()
{
    SMC_MOCK_METHOD0( void )    
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::completeBarringStateChangeRequestHandling
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::completeBarringStateChangeRequestHandling(
        QString barringPassword,
        bool okPressed)
{
    SMC_MOCK_METHOD2( void, QString, barringPassword, bool, okPressed)
}


// -----------------------------------------------------------------------------
// CpBarringPluginGroup::changeBarringPasswordPhasesHandling
// -----------------------------------------------------------------------------
//
void CpBarringPluginGroup::changeBarringPasswordPhasesHandling(
        QString barringPassword,
        bool okPressed)
{
    SMC_MOCK_METHOD2( void, QString, barringPassword, bool, okPressed)
}


