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
#include <QAction>

#include <QIcon>
#include <QActionGroup>
#include <QMenu>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        QObject * parent )
    {
    
    }


// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        const QString & text,
        QObject * parent )
    {
    
    }


// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        const QIcon & icon,
        const QString & text,
        QObject * parent )
    {
    
    }


#ifdef QT3_SUPPORT
// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        QObject * parent,
        const char * name )
    {
    
    }


// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        const QString & text,
        const QKeySequence & shortcut,
        QObject * parent,
        const char * name )
    {
    
    }


// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        const QIcon & icon,
        const QString & text,
        const QKeySequence & shortcut,
        QObject * parent,
        const char * name )
    {
    
    }
#endif

// -----------------------------------------------------------------------------
// QAction::~QAction
// -----------------------------------------------------------------------------
//
QAction::~QAction(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QAction::setActionGroup
// -----------------------------------------------------------------------------
//
void QAction::setActionGroup( 
        QActionGroup * group )
    {
    SMC_MOCK_METHOD1( void, QActionGroup *, group )
    }


// -----------------------------------------------------------------------------
// QAction::actionGroup
// -----------------------------------------------------------------------------
//
QActionGroup * QAction::actionGroup(  ) const
    {
    SMC_MOCK_METHOD0( QActionGroup * )
    }


// -----------------------------------------------------------------------------
// QAction::setIcon
// -----------------------------------------------------------------------------
//
void QAction::setIcon( 
        const QIcon & icon )
    {
    SMC_MOCK_METHOD1( void, const QIcon &, icon )
    }


// -----------------------------------------------------------------------------
// QAction::icon
// -----------------------------------------------------------------------------
//
QIcon QAction::icon(  ) const
    {
    SMC_MOCK_METHOD0( QIcon )
    }


// -----------------------------------------------------------------------------
// QAction::setText
// -----------------------------------------------------------------------------
//
void QAction::setText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// QAction::text
// -----------------------------------------------------------------------------
//
QString QAction::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QAction::setIconText
// -----------------------------------------------------------------------------
//
void QAction::setIconText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// QAction::iconText
// -----------------------------------------------------------------------------
//
QString QAction::iconText(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QAction::setToolTip
// -----------------------------------------------------------------------------
//
void QAction::setToolTip( 
        const QString & tip )
    {
    SMC_MOCK_METHOD1( void, const QString &, tip )
    }


// -----------------------------------------------------------------------------
// QAction::toolTip
// -----------------------------------------------------------------------------
//
QString QAction::toolTip(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QAction::setStatusTip
// -----------------------------------------------------------------------------
//
void QAction::setStatusTip( 
        const QString & statusTip )
    {
    SMC_MOCK_METHOD1( void, const QString &, statusTip )
    }


// -----------------------------------------------------------------------------
// QAction::statusTip
// -----------------------------------------------------------------------------
//
QString QAction::statusTip(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QAction::setWhatsThis
// -----------------------------------------------------------------------------
//
void QAction::setWhatsThis( 
        const QString & what )
    {
    SMC_MOCK_METHOD1( void, const QString &, what )
    }


// -----------------------------------------------------------------------------
// QAction::whatsThis
// -----------------------------------------------------------------------------
//
QString QAction::whatsThis(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QAction::setPriority
// -----------------------------------------------------------------------------
//
void QAction::setPriority( 
        QAction::Priority priority )
    {
    SMC_MOCK_METHOD1( void, Priority, priority )
    }


// -----------------------------------------------------------------------------
// QAction::priority
// -----------------------------------------------------------------------------
//
QAction::Priority QAction::priority(  ) const
    {
    SMC_MOCK_METHOD0( Priority )
    }


// -----------------------------------------------------------------------------
// QAction::menu
// -----------------------------------------------------------------------------
//
QMenu * QAction::menu(  ) const
    {
    SMC_MOCK_METHOD0( QMenu * )
    }


// -----------------------------------------------------------------------------
// QAction::setMenu
// -----------------------------------------------------------------------------
//
void QAction::setMenu( 
        QMenu * menu )
    {
    SMC_MOCK_METHOD1( void, QMenu *, menu )
    }


// -----------------------------------------------------------------------------
// QAction::setSeparator
// -----------------------------------------------------------------------------
//
void QAction::setSeparator( 
        bool b )
    {
    SMC_MOCK_METHOD1( void, bool, b )
    }


// -----------------------------------------------------------------------------
// QAction::isSeparator
// -----------------------------------------------------------------------------
//
bool QAction::isSeparator(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::setShortcut
// -----------------------------------------------------------------------------
//
void QAction::setShortcut( 
        const QKeySequence & shortcut )
    {
    SMC_MOCK_METHOD1( void, const QKeySequence &, shortcut )
    }


// -----------------------------------------------------------------------------
// QAction::shortcut
// -----------------------------------------------------------------------------
//
QKeySequence QAction::shortcut(  ) const
    {
    SMC_MOCK_METHOD0( QKeySequence )
    }


// -----------------------------------------------------------------------------
// QAction::setShortcuts
// -----------------------------------------------------------------------------
//
void QAction::setShortcuts( 
        const QList<QKeySequence> & shortcuts )
    {
 //   SMC_MOCK_METHOD1( void, const QList<QKeySequence> &, shortcuts )
    }


// -----------------------------------------------------------------------------
// QAction::setShortcuts
// -----------------------------------------------------------------------------
//
void QAction::setShortcuts( 
        QKeySequence:: StandardKey )
    {
//    SMC_MOCK_METHOD1( void, QKeySequence::, StandardKey )
    }


// -----------------------------------------------------------------------------
// QAction::shortcuts
// -----------------------------------------------------------------------------
//
QList <QKeySequence > QAction::shortcuts(  ) const
    {
//    SMC_MOCK_METHOD0( QList <QKeySequence > )
    }


// -----------------------------------------------------------------------------
// QAction::setShortcutContext
// -----------------------------------------------------------------------------
//
void QAction::setShortcutContext( 
        Qt::ShortcutContext context )
    {
    //SMC_MOCK_METHOD1( void, Qt::ShortcutContext, context )
    }


// -----------------------------------------------------------------------------
// QAction::shortcutContext
// -----------------------------------------------------------------------------
//
Qt::ShortcutContext QAction::shortcutContext(  ) const
    {
    SMC_MOCK_METHOD0( Qt::ShortcutContext )
    }


// -----------------------------------------------------------------------------
// QAction::setAutoRepeat
// -----------------------------------------------------------------------------
//
void QAction::setAutoRepeat( bool )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::autoRepeat
// -----------------------------------------------------------------------------
//
bool QAction::autoRepeat(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::setFont
// -----------------------------------------------------------------------------
//
void QAction::setFont( 
        const QFont & font )
    {
    SMC_MOCK_METHOD1( void, const QFont &, font )
    }


// -----------------------------------------------------------------------------
// QAction::font
// -----------------------------------------------------------------------------
//
QFont QAction::font(  ) const
    {
    SMC_MOCK_METHOD0( QFont )
    }


// -----------------------------------------------------------------------------
// QAction::setCheckable
// -----------------------------------------------------------------------------
//
void QAction::setCheckable( bool  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::isCheckable
// -----------------------------------------------------------------------------
//
bool QAction::isCheckable(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::data
// -----------------------------------------------------------------------------
//
QVariant QAction::data(  ) const
    {
    SMC_MOCK_METHOD0( QVariant )
    }


// -----------------------------------------------------------------------------
// QAction::setData
// -----------------------------------------------------------------------------
//
void QAction::setData( 
        const QVariant & var )
    {
    SMC_MOCK_METHOD1( void, const QVariant &, var )
    }


// -----------------------------------------------------------------------------
// QAction::isChecked
// -----------------------------------------------------------------------------
//
bool QAction::isChecked(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::isEnabled
// -----------------------------------------------------------------------------
//
bool QAction::isEnabled(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::isVisible
// -----------------------------------------------------------------------------
//
bool QAction::isVisible(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::activate
// -----------------------------------------------------------------------------
//
void QAction::activate( 
        ActionEvent event )
    {
    SMC_MOCK_METHOD1( void, ActionEvent, event )
    }


// -----------------------------------------------------------------------------
// QAction::showStatusText
// -----------------------------------------------------------------------------
//
bool QAction::showStatusText( 
        QWidget * widget )
    {
    SMC_MOCK_METHOD1( bool, QWidget *, widget )
    }


// -----------------------------------------------------------------------------
// QAction::setMenuRole
// -----------------------------------------------------------------------------
//
void QAction::setMenuRole( 
        QAction::MenuRole menuRole )
    {
    SMC_MOCK_METHOD1( void, MenuRole, menuRole )
    }


// -----------------------------------------------------------------------------
// QAction::menuRole
// -----------------------------------------------------------------------------
//
QAction::MenuRole QAction::menuRole(  ) const
    {
    SMC_MOCK_METHOD0( MenuRole )
    }


// -----------------------------------------------------------------------------
// QAction::setSoftKeyRole
// -----------------------------------------------------------------------------
//
void QAction::setSoftKeyRole( 
        QAction::SoftKeyRole softKeyRole )
    {
    SMC_MOCK_METHOD1( void, SoftKeyRole, softKeyRole )
    }


// -----------------------------------------------------------------------------
// QAction::softKeyRole
// -----------------------------------------------------------------------------
//
QAction::SoftKeyRole QAction::softKeyRole(  ) const
    {
    
    SMC_MOCK_METHOD0( SoftKeyRole )
    }


// -----------------------------------------------------------------------------
// QAction::setIconVisibleInMenu
// -----------------------------------------------------------------------------
//
void QAction::setIconVisibleInMenu( 
        bool visible )
    {
    SMC_MOCK_METHOD1( void, bool, visible )
    }


// -----------------------------------------------------------------------------
// QAction::isIconVisibleInMenu
// -----------------------------------------------------------------------------
//
bool QAction::isIconVisibleInMenu(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QAction::parentWidget
// -----------------------------------------------------------------------------
//
QWidget * QAction::parentWidget(  ) const
    {
 //   SMC_MOCK_METHOD0( QWidget * )
    }


// -----------------------------------------------------------------------------
// QAction::associatedWidgets
// -----------------------------------------------------------------------------
//
QList <QWidget * > QAction::associatedWidgets(  ) const
    {
 //   SMC_MOCK_METHOD0( QList <QWidget * > )
    }


// -----------------------------------------------------------------------------
// QAction::associatedGraphicsWidgets
// -----------------------------------------------------------------------------
//
QList <QGraphicsWidget * > QAction::associatedGraphicsWidgets(  ) const
    {
 //   SMC_MOCK_METHOD0( QList <QGraphicsWidget * > )
    }

// -----------------------------------------------------------------------------
// QAction::setChecked
// -----------------------------------------------------------------------------
//
void QAction::setChecked( bool )
    {
    //SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::toggle
// -----------------------------------------------------------------------------
//
void QAction::toggle(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::setEnabled
// -----------------------------------------------------------------------------
//
void QAction::setEnabled( bool )
    {
   // SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::setVisible
// -----------------------------------------------------------------------------
//
void QAction::setVisible( bool  )
    {
  //  SMC_MOCK_METHOD0( void )
    }

/*
// -----------------------------------------------------------------------------
// QAction::changed
// -----------------------------------------------------------------------------
//
void QAction::changed(  )
    {
 //   SMC_MOCK_METHOD0( Q_SIGNALS : void )
    }


// -----------------------------------------------------------------------------
// QAction::triggered
// -----------------------------------------------------------------------------
//
void QAction::triggered( 
        bool checked )
    {
    SMC_MOCK_METHOD1( void, bool, checked )
    }


// -----------------------------------------------------------------------------
// QAction::hovered
// -----------------------------------------------------------------------------
//
void QAction::hovered(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::toggled
// -----------------------------------------------------------------------------
//
void QAction::toggled(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QAction::activated
// -----------------------------------------------------------------------------
//
void QAction::activated( 
         int )
    {
 //   SMC_MOCK_METHOD1( QT_MOC_COMPAT void, , int )
    }
*/

// -----------------------------------------------------------------------------
// QAction::event
// -----------------------------------------------------------------------------
//
bool QAction::event( 
        QEvent * )
    {
//    SMC_MOCK_METHOD1( bool, QEvent, * )
    }


// -----------------------------------------------------------------------------
// QAction::QAction
// -----------------------------------------------------------------------------
//
QAction::QAction( 
        QActionPrivate & dd,
        QObject * parent )
    //:
    //QObject( /*dd, parent*/ )
    {
    
    }


