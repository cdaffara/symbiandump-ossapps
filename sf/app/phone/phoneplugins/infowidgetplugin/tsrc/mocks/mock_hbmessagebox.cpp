/** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include "hbmessagebox.h"
#include <HbStylePrimitiveData>
#include <QGraphicsScene>

// NOTE! The following header include requires
//       INCLUDEPATH += /sf/mw/hb/include/hbcore/private
#include <hbstyleoptionmessagebox_p.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbMessageBox::HbMessageBox
// -----------------------------------------------------------------------------
//
HbMessageBox::HbMessageBox( 
        MessageBoxType type,
        QGraphicsItem * parent )
    //:
    //HbDialog( /*type, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbMessageBox::HbMessageBox
// -----------------------------------------------------------------------------
//
HbMessageBox::HbMessageBox( 
        const QString & text,
        MessageBoxType type,
        QGraphicsItem * parent )
    //:
    //HbDialog( /*text, type, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbMessageBox::~HbMessageBox
// -----------------------------------------------------------------------------
//
HbMessageBox::~HbMessageBox(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbMessageBox::setText
// -----------------------------------------------------------------------------
//
void HbMessageBox::setText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::text
// -----------------------------------------------------------------------------
//
QString HbMessageBox::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::setIcon
// -----------------------------------------------------------------------------
//
void HbMessageBox::setIcon( 
        const HbIcon & icon )
    {
    SMC_MOCK_METHOD1( void, const HbIcon &, icon )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::icon
// -----------------------------------------------------------------------------
//
HbIcon HbMessageBox::icon(  ) const
    {
    SMC_MOCK_METHOD0( HbIcon )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::setIconVisible
// -----------------------------------------------------------------------------
//
void HbMessageBox::setIconVisible( 
        bool visible )
    {
    SMC_MOCK_METHOD1( void, bool, visible )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::iconVisible
// -----------------------------------------------------------------------------
//
bool HbMessageBox::iconVisible(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::setStandardButtons
// -----------------------------------------------------------------------------
//
void HbMessageBox::setStandardButtons( 
        StandardButtons buttons )
    {
    //SMC_MOCK_METHOD1( void, StandardButtons, buttons )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::standardButtons
// -----------------------------------------------------------------------------
//
HbMessageBox::StandardButtons HbMessageBox::standardButtons(  ) const
    {
    SMC_MOCK_METHOD0( HbMessageBox::StandardButtons )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::primitive
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbMessageBox::primitive( 
        HbStyle::Primitive primitive ) const
    {
    SMC_MOCK_METHOD1( QGraphicsItem *, HbStyle::Primitive, primitive )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::question
// -----------------------------------------------------------------------------
//
void HbMessageBox::question( 
        const QString & questionText,
        QObject * receiver,
        const char * member,
        const QString & primaryButtonText,
        const QString & secondaryButtonText,
        QGraphicsWidget * headWidget,
        QGraphicsScene * scene,
        QGraphicsItem * parent )
    {
    SMC_MOCK_METHOD8( void, const QString &, questionText, 
        QObject *, receiver, 
        const char *, member, 
        const QString &, primaryButtonText, 
        const QString &, secondaryButtonText, 
        QGraphicsWidget *, headWidget, 
        QGraphicsScene *, scene, 
        QGraphicsItem *, parent )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::information
// -----------------------------------------------------------------------------
//
void HbMessageBox::information( 
        const QString & informationText,
        QObject * receiver,
        const char * member,
        QGraphicsWidget * headWidget,
        QGraphicsScene * scene,
        QGraphicsItem * parent )
    {
    SMC_MOCK_METHOD6( void, const QString &, informationText, 
        QObject *, receiver, 
        const char *, member, 
        QGraphicsWidget *, headWidget, 
        QGraphicsScene *, scene, 
        QGraphicsItem *, parent )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::warning
// -----------------------------------------------------------------------------
//
void HbMessageBox::warning( 
        const QString & warningText,
        QObject * receiver,
        const char * member,
        QGraphicsWidget * headWidget,
        QGraphicsScene * scene,
        QGraphicsItem * parent )
    {
    SMC_MOCK_METHOD6( void, const QString &, warningText, 
        QObject *, receiver, 
        const char *, member, 
        QGraphicsWidget *, headWidget, 
        QGraphicsScene *, scene, 
        QGraphicsItem *, parent )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::question
// -----------------------------------------------------------------------------
//
void HbMessageBox::question( 
        const QString & questionText,
        QObject * receiver,
        const char * member,
        HbMessageBox::StandardButtons buttons,
        QGraphicsWidget * headingWidget,
        QGraphicsScene * scene,
        QGraphicsItem * parent )
    {
    /*SMC_MOCK_METHOD7( void, const QString &, questionText, 
        QObject *, receiver, 
        const char *, member, 
        HbMessageBox::StandardButtons, buttons, 
        QGraphicsWidget *, headingWidget, 
        QGraphicsScene *, scene, 
        QGraphicsItem *, parent )*/
    }


// -----------------------------------------------------------------------------
// HbMessageBox::information
// -----------------------------------------------------------------------------
//
void HbMessageBox::information( 
        const QString & informationText,
        QObject * receiver,
        const char * member,
        HbMessageBox::StandardButtons buttons,
        QGraphicsWidget * headingWidget,
        QGraphicsScene * scene,
        QGraphicsItem * parent )
    {
    /*SMC_MOCK_METHOD7( void, const QString &, informationText, 
        QObject *, receiver, 
        const char *, member, 
        HbMessageBox::StandardButtons, buttons, 
        QGraphicsWidget *, headingWidget, 
        QGraphicsScene *, scene, 
        QGraphicsItem *, parent )*/
    }


// -----------------------------------------------------------------------------
// HbMessageBox::warning
// -----------------------------------------------------------------------------
//
void HbMessageBox::warning( 
        const QString & warningText,
        QObject * receiver,
        const char * member,
        HbMessageBox::StandardButtons buttons,
        QGraphicsWidget * headingWidget,
        QGraphicsScene * scene,
        QGraphicsItem * parent )
    {
    /*SMC_MOCK_METHOD7( void, const QString &, warningText, 
        QObject *, receiver, 
        const char *, member, 
        HbMessageBox::StandardButtons, buttons, 
        QGraphicsWidget *, headingWidget, 
        QGraphicsScene *, scene, 
        QGraphicsItem *, parent )*/
    }


// -----------------------------------------------------------------------------
// HbMessageBox::recreatePrimitives
// -----------------------------------------------------------------------------
//
void HbMessageBox::recreatePrimitives(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::updatePrimitives
// -----------------------------------------------------------------------------
//
void HbMessageBox::updatePrimitives(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::showEvent
// -----------------------------------------------------------------------------
//
void HbMessageBox::showEvent( 
        QShowEvent * event )
    {
    SMC_MOCK_METHOD1( void, QShowEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::HbMessageBox
// -----------------------------------------------------------------------------
//
HbMessageBox::HbMessageBox( 
        HbMessageBoxPrivate & dd,
        QGraphicsItem * parent )
    //:
    //HbDialog( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbMessageBox::initStyleOption
// -----------------------------------------------------------------------------
//
void HbMessageBox::initStyleOption( 
        HbStyleOptionMessageBox * option ) const
    {
    SMC_MOCK_METHOD1( void, HbStyleOptionMessageBox *, option )
    }


// -----------------------------------------------------------------------------
// HbMessageBox::initPrimitiveData
// -----------------------------------------------------------------------------
//
void HbMessageBox::initPrimitiveData( 
        HbStylePrimitiveData * primitiveData,
        const QGraphicsObject * primitive )
    {
    SMC_MOCK_METHOD2( void, HbStylePrimitiveData *, primitiveData, 
        const QGraphicsObject *, primitive )
    }


