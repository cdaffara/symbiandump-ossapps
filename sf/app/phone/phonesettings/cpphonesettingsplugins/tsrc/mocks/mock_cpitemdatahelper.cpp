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
#include <QList>
#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include "cpitemdatahelper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpItemDataHelper::CpItemDataHelper
// -----------------------------------------------------------------------------
//
CpItemDataHelper::CpItemDataHelper( 
        HbDataForm * /*form*/ )
    //:
    //QObject( /*form*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::~CpItemDataHelper
// -----------------------------------------------------------------------------
//
CpItemDataHelper::~CpItemDataHelper(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::bindToForm
// -----------------------------------------------------------------------------
//
void CpItemDataHelper::bindToForm( 
        HbDataForm * form )
    {
    SMC_MOCK_METHOD1( void, HbDataForm *, form )
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::addItemPrototype
// -----------------------------------------------------------------------------
//
void CpItemDataHelper::addItemPrototype( 
        HbAbstractViewItem * prototype )
    {
    SMC_MOCK_METHOD1( void, HbAbstractViewItem *, prototype )
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::addConnection
// -----------------------------------------------------------------------------
//
void CpItemDataHelper::addConnection( 
        HbDataFormModelItem * item,
        const char * signal,
        QObject * receiver,
        const char * method )
    {
    SMC_MOCK_METHOD4( void, HbDataFormModelItem *, item, 
        const char *, signal, 
        QObject *, receiver, 
        const char *, method )
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::removeConnection
// -----------------------------------------------------------------------------
//
void CpItemDataHelper::removeConnection( 
        HbDataFormModelItem * item,
        const char * signal,
        QObject * receiver,
        const char * method )
    {
    SMC_MOCK_METHOD4( void, HbDataFormModelItem *, item, 
        const char *, signal, 
        QObject *, receiver, 
        const char *, method )
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::connectToForm
// -----------------------------------------------------------------------------
//
void CpItemDataHelper::connectToForm( 
        const char * signal,
        QObject * receiver,
        const char * method )
    {
    SMC_MOCK_METHOD3( void, const char *, signal, 
        QObject *, receiver, 
        const char *, method )
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::disconnectFromForm
// -----------------------------------------------------------------------------
//
void CpItemDataHelper::disconnectFromForm( 
        const char * signal,
        QObject * receiver,
        const char * method )
    {
    SMC_MOCK_METHOD3( void, const char *, signal, 
        QObject *, receiver, 
        const char *, method )
    }


// -----------------------------------------------------------------------------
// CpItemDataHelper::widgetFromModelIndex
// -----------------------------------------------------------------------------
//
HbWidget * CpItemDataHelper::widgetFromModelIndex( 
        const QModelIndex & index )
    {
    SMC_MOCK_METHOD1( HbWidget *, const QModelIndex &, index )
    }


