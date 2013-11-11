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
#include "hbdataform.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbDataForm::HbDataForm
// -----------------------------------------------------------------------------
//
HbAbstractItemViewPrivate* temp(NULL);
HbAbstractItemContainer* temp2(NULL);

HbDataForm::HbDataForm( 
        HbDataFormPrivate & /*dd*/,
        HbAbstractItemContainer * /*container*/,
        QGraphicsItem * parent ) : HbAbstractItemView( *temp, temp2, 0, parent)
    {
    
    }


// -----------------------------------------------------------------------------
// HbDataForm::Q_PROPERTY
// -----------------------------------------------------------------------------
//
/*
void HbDataForm::Q_PROPERTY( 
        QString heading READ heading WRITE setHeading ) Q_PROPERTY ( QString description READ description WRITE setDescription ):HbDataForm ( QGraphicsItem * parent )
    {
    SMC_MOCK_METHOD1( void, QString heading READ heading WRITE setHeading ) Q_PROPERTY ( QString description READ description WRITE setDescription ):HbDataForm ( QGraphicsItem *, parent )
    }
*/

// -----------------------------------------------------------------------------
// HbDataForm::~HbDataForm
// -----------------------------------------------------------------------------
//
HbDataForm::~HbDataForm(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDataForm::type
// -----------------------------------------------------------------------------
//
/*
int HbDataForm::type(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }*/


// -----------------------------------------------------------------------------
// HbDataForm::scrollTo
// -----------------------------------------------------------------------------
//
void HbDataForm::scrollTo( 
        const QModelIndex & index,
        ScrollHint hint )
    {
  //  SMC_MOCK_METHOD2( void, const QModelIndex &, index, 
    //    ScrollHint, hint )
    }

// -----------------------------------------------------------------------------
// HbDataForm::setExpanded
// -----------------------------------------------------------------------------
//
void HbDataForm::setExpanded( 
        const QModelIndex & index,
        bool expanded )
    {
 //   SMC_MOCK_METHOD2( void, const QModelIndex &, index, 
   //     bool, expanded )
    }


// -----------------------------------------------------------------------------
// HbDataForm::isExpanded
// -----------------------------------------------------------------------------
//
bool HbDataForm::isExpanded( 
        const QModelIndex & index ) const
    {
  //  SMC_MOCK_METHOD1( bool, const QModelIndex &, index )
    }


// -----------------------------------------------------------------------------
// HbDataForm::setHeading
// -----------------------------------------------------------------------------
//
void HbDataForm::setHeading( 
        const QString & heading )
    {
    SMC_MOCK_METHOD1( void, const QString &, heading )
    }


// -----------------------------------------------------------------------------
// HbDataForm::heading
// -----------------------------------------------------------------------------
//
QString HbDataForm::heading(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbDataForm::setDescription
// -----------------------------------------------------------------------------
//
void HbDataForm::setDescription( 
        const QString & description )
    {
    SMC_MOCK_METHOD1( void, const QString &, description )
    }


// -----------------------------------------------------------------------------
// HbDataForm::description
// -----------------------------------------------------------------------------
//
QString HbDataForm::description(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbDataForm::primitive
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbDataForm::primitive( 
        HbStyle::Primitive primitive ) const
    {
 //   SMC_MOCK_METHOD1( QGraphicsItem *, HbStyle::Primitive, primitive )
    }


// -----------------------------------------------------------------------------
// HbDataForm::setModel
// -----------------------------------------------------------------------------
//
void HbDataForm::setModel( 
        QAbstractItemModel * model,
        HbAbstractViewItem * prototype )
    {
//    SMC_MOCK_METHOD2( void, QAbstractItemModel *, model, 
  //      HbAbstractViewItem *, prototype )
    }


// -----------------------------------------------------------------------------
// HbDataForm::addConnection
// -----------------------------------------------------------------------------
//
void HbDataForm::addConnection( 
        HbDataFormModelItem * item,
        const char * signal,
        QObject * receiver,
        const char * slot )
    {
 /*   SMC_MOCK_METHOD4( void, HbDataFormModelItem *, item, 
        const char *, signal, 
        QObject *, receiver, 
        const char *, slot )
   */ }


// -----------------------------------------------------------------------------
// HbDataForm::removeConnection
// -----------------------------------------------------------------------------
//
void HbDataForm::removeConnection( 
        HbDataFormModelItem * item,
        const char * signal,
        QObject * receiver,
        const char * slot )
    {
 /*   SMC_MOCK_METHOD4( void, HbDataFormModelItem *, item, 
        const char *, signal, 
        QObject *, receiver, 
        const char *, slot )
   */ }


// -----------------------------------------------------------------------------
// HbDataForm::removeAllConnection
// -----------------------------------------------------------------------------
//
void HbDataForm::removeAllConnection(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbDataForm::removeAllConnection
// -----------------------------------------------------------------------------
//
void HbDataForm::removeAllConnection( 
        HbDataFormModelItem * item )
    {
//    SMC_MOCK_METHOD1( void, HbDataFormModelItem *, item )
    }


// -----------------------------------------------------------------------------
// HbDataForm::itemShown
// -----------------------------------------------------------------------------
//
void HbDataForm::itemShown( 
        const QModelIndex & index )
    {
   // SMC_MOCK_METHOD1( void, const QModelIndex &, index )
    }


// -----------------------------------------------------------------------------
// HbDataForm::rowsInserted
// -----------------------------------------------------------------------------
//
void HbDataForm::rowsInserted( 
        const QModelIndex & parent,
        int start,
        int end )
    {
//    SMC_MOCK_METHOD3( void, const QModelIndex &, parent, 
  //      int, start, 
    //    int, end )
    }


// -----------------------------------------------------------------------------
// HbDataForm::rowsAboutToBeRemoved
// -----------------------------------------------------------------------------
//
void HbDataForm::rowsAboutToBeRemoved( 
        const QModelIndex & index,
        int start,
        int end )
    {
//    SMC_MOCK_METHOD3( void, const QModelIndex &, index, 
  //      int, start, 
    //    int, end )
    }


// -----------------------------------------------------------------------------
// HbDataForm::dataChanged
// -----------------------------------------------------------------------------
//
void HbDataForm::dataChanged( 
        const QModelIndex & topLeft,
        const QModelIndex & bottomRight )
    {
  //  SMC_MOCK_METHOD2( void, const QModelIndex &, topLeft, 
    //    const QModelIndex &, bottomRight )
    }


// -----------------------------------------------------------------------------
// HbDataForm::initStyleOption
// -----------------------------------------------------------------------------
//
void HbDataForm::initStyleOption( 
        HbStyleOptionDataForm * option )
    {
 //   SMC_MOCK_METHOD1( void, HbStyleOptionDataForm *, option )
    }


