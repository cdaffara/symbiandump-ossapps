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
#include <QDebug>
#include <smcmockclassincludes.h>
#include <qserviceplugininterface.h>


QTM_BEGIN_NAMESPACE

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QServicePluginInterface::QServicePluginInterface
// -----------------------------------------------------------------------------
//
QServicePluginInterface::QServicePluginInterface(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QServicePluginInterface::~QServicePluginInterface
// -----------------------------------------------------------------------------
//
QServicePluginInterface::~QServicePluginInterface(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QServicePluginInterface::installService
// -----------------------------------------------------------------------------
//
void QServicePluginInterface::installService(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QServicePluginInterface::uninstallService
// -----------------------------------------------------------------------------
//
void QServicePluginInterface::uninstallService(  )
    {
    SMC_MOCK_METHOD0( void )
    }

QTM_END_NAMESPACE