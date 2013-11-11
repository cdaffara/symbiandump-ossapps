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
#include <qevent.h>

#include <QUrl>
#include <QWidget>
#include <QGesture>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QWheelEvent::QWheelEvent
// -----------------------------------------------------------------------------
//
QWheelEvent::QWheelEvent( 
        const QPoint & pos,
        int delta,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers,
        Qt::Orientation orient )
    :QInputEvent(Wheel, modifiers)
    //QInputEvent( /*pos, delta, buttons, modifiers, orient*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QWheelEvent::QWheelEvent
// -----------------------------------------------------------------------------
//
QWheelEvent::QWheelEvent( 
        const QPoint & pos,
        const QPoint & globalPos,
        int delta,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers,
        Qt::Orientation orient )
    : QInputEvent(Wheel)
    //QInputEvent( /*pos, globalPos, delta, buttons, modifiers, orient*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QWheelEvent::~QWheelEvent
// -----------------------------------------------------------------------------
//
QWheelEvent::~QWheelEvent(  )
    {
    
    }

// -----------------------------------------------------------------------------
// QUpdateLaterEvent::QUpdateLaterEvent
// -----------------------------------------------------------------------------
//
QUpdateLaterEvent::QUpdateLaterEvent( 
        const QRegion & paintRegion )
    :QEvent(UpdateLater)
    //QEvent( /*paintRegion*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QUpdateLaterEvent::~QUpdateLaterEvent
// -----------------------------------------------------------------------------
//
QUpdateLaterEvent::~QUpdateLaterEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QGestureEvent::QGestureEvent
// -----------------------------------------------------------------------------
//
QGestureEvent::QGestureEvent( 
        const QList<QGesture *> & gestures )
    :QEvent(QEvent::Gesture)
    //QEvent( /*gestures*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGestureEvent::~QGestureEvent
// -----------------------------------------------------------------------------
//
QGestureEvent::~QGestureEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QGestureEvent::gestures
// -----------------------------------------------------------------------------
//
QList <QGesture * > QGestureEvent::gestures(  ) const
    {
    SMC_MOCK_METHOD0( QList <QGesture * > )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::gesture
// -----------------------------------------------------------------------------
//
QGesture * QGestureEvent::gesture( 
        Qt::GestureType type ) const
    {
    SMC_MOCK_METHOD1( QGesture *, Qt::GestureType, type )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::activeGestures
// -----------------------------------------------------------------------------
//
QList <QGesture * > QGestureEvent::activeGestures(  ) const
    {
    SMC_MOCK_METHOD0( QList <QGesture * > )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::canceledGestures
// -----------------------------------------------------------------------------
//
QList <QGesture * > QGestureEvent::canceledGestures(  ) const
    {
    SMC_MOCK_METHOD0( QList <QGesture * > )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::setAccepted
// -----------------------------------------------------------------------------
//
void QGestureEvent::setAccepted( 
        QGesture *gesture, bool value)
    {
    SMC_MOCK_METHOD2( void, QGesture *, gesture, bool, value )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::accept
// -----------------------------------------------------------------------------
//
void QGestureEvent::accept( 
        QGesture *gesture)
    {
    SMC_MOCK_METHOD1( void, QGesture *, gesture )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::ignore
// -----------------------------------------------------------------------------
//
void QGestureEvent::ignore( 
        QGesture *gesture )
    {
    SMC_MOCK_METHOD1( void, QGesture *, gesture )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::isAccepted
// -----------------------------------------------------------------------------
//
bool QGestureEvent::isAccepted( 
        QGesture *gesture ) const
    {
    SMC_MOCK_METHOD1( bool, QGesture *, gesture )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::setAccepted
// -----------------------------------------------------------------------------
//
void QGestureEvent::setAccepted( 
        Qt::GestureType type, bool value)
    {
    SMC_MOCK_METHOD2( void, Qt::GestureType, type, bool, value )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::accept
// -----------------------------------------------------------------------------
//
void QGestureEvent::accept( 
        Qt::GestureType type )
    {
    SMC_MOCK_METHOD1( void, Qt::GestureType, type )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::ignore
// -----------------------------------------------------------------------------
//
void QGestureEvent::ignore( 
        Qt::GestureType type )
    {
    SMC_MOCK_METHOD1( void, Qt::GestureType, type )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::isAccepted
// -----------------------------------------------------------------------------
//
bool QGestureEvent::isAccepted( 
        Qt::GestureType type ) const
    {
    SMC_MOCK_METHOD1( bool, Qt::GestureType, type )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::setWidget
// -----------------------------------------------------------------------------
//
void QGestureEvent::setWidget( 
        QWidget * widget )
    {
    SMC_MOCK_METHOD1( void, QWidget *, widget )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::widget
// -----------------------------------------------------------------------------
//
QWidget * QGestureEvent::widget(  ) const
    {
    SMC_MOCK_METHOD0( QWidget * )
    }


// -----------------------------------------------------------------------------
// QGestureEvent::mapToGraphicsScene
// -----------------------------------------------------------------------------
//
QPointF QGestureEvent::mapToGraphicsScene( 
        const QPointF & gesturePoint ) const
    {
    SMC_MOCK_METHOD1( QPointF, const QPointF &, gesturePoint )
    }


// -----------------------------------------------------------------------------
// QContextMenuEvent::QContextMenuEvent
// -----------------------------------------------------------------------------
//
QContextMenuEvent::QContextMenuEvent( 
        Reason reason,
        const QPoint & pos,
        const QPoint & globalPos,
        Qt::KeyboardModifiers modifiers )
    :QInputEvent(ContextMenu, modifiers)
    //QInputEvent( /*reason, pos, globalPos, modifiers*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QContextMenuEvent::QContextMenuEvent
// -----------------------------------------------------------------------------
//
QContextMenuEvent::QContextMenuEvent( 
        Reason reason,
        const QPoint & pos,
        const QPoint & globalPos )
    :QInputEvent(ContextMenu)
    //QInputEvent( /*reason, pos, globalPos*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QContextMenuEvent::QContextMenuEvent
// -----------------------------------------------------------------------------
//
QContextMenuEvent::QContextMenuEvent( 
        Reason reason,
        const QPoint & pos )
    :QInputEvent(ContextMenu)
    //QInputEvent( /*reason, pos*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QContextMenuEvent::~QContextMenuEvent
// -----------------------------------------------------------------------------
//
QContextMenuEvent::~QContextMenuEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QStatusTipEvent::QStatusTipEvent
// -----------------------------------------------------------------------------
//
QStatusTipEvent::QStatusTipEvent( 
        const QString & tip )
    :QEvent(StatusTip)
    //QEvent( /*tip*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QStatusTipEvent::~QStatusTipEvent
// -----------------------------------------------------------------------------
//
QStatusTipEvent::~QStatusTipEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QMoveEvent::QMoveEvent
// -----------------------------------------------------------------------------
//
QMoveEvent::QMoveEvent( 
        const QPoint & pos,
        const QPoint & oldPos )
    :QEvent(Move)
    //QEvent( /*pos, oldPos*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QMoveEvent::~QMoveEvent
// -----------------------------------------------------------------------------
//
QMoveEvent::~QMoveEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragEnterEvent::QDragEnterEvent
// -----------------------------------------------------------------------------
//
QDragEnterEvent::QDragEnterEvent( 
        const QPoint & pos,
        Qt::DropActions actions,
        const QMimeData * data,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers )
    :QDragMoveEvent(pos, actions, data, buttons, modifiers, DragEnter)
    //QDragMoveEvent( /*pos, actions, data, buttons, modifiers*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragEnterEvent::~QDragEnterEvent
// -----------------------------------------------------------------------------
//
QDragEnterEvent::~QDragEnterEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QIconDragEvent::QIconDragEvent
// -----------------------------------------------------------------------------
//
QIconDragEvent::QIconDragEvent(  )
    :QEvent(IconDrag)
    {
    
    }


// -----------------------------------------------------------------------------
// QIconDragEvent::~QIconDragEvent
// -----------------------------------------------------------------------------
//
QIconDragEvent::~QIconDragEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QFileOpenEvent::QFileOpenEvent
// -----------------------------------------------------------------------------
//
QFileOpenEvent::QFileOpenEvent( 
        const QString & file )
    :QEvent(FileOpen)
    //QEvent( /*file*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QFileOpenEvent::QFileOpenEvent
// -----------------------------------------------------------------------------
//
QFileOpenEvent::QFileOpenEvent( 
        const QUrl & url )
    :QEvent(FileOpen)
    //QEvent( /*url*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QFileOpenEvent::~QFileOpenEvent
// -----------------------------------------------------------------------------
//
QFileOpenEvent::~QFileOpenEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QFileOpenEvent::url
// -----------------------------------------------------------------------------
//
QUrl QFileOpenEvent::url(  ) const
    {
    SMC_MOCK_METHOD0( QUrl )
    }


// -----------------------------------------------------------------------------
// QHelpEvent::QHelpEvent
// -----------------------------------------------------------------------------
//
QHelpEvent::QHelpEvent( 
        Type type,
        const QPoint & pos,
        const QPoint & globalPos )
    :QEvent(type)
    //QEvent( /*type, pos, globalPos*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QHelpEvent::~QHelpEvent
// -----------------------------------------------------------------------------
//
QHelpEvent::~QHelpEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QToolBarChangeEvent::QToolBarChangeEvent
// -----------------------------------------------------------------------------
//
QToolBarChangeEvent::QToolBarChangeEvent( 
        bool t )
    :QEvent(ToolBarChange)
    //QEvent( /*t*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QToolBarChangeEvent::~QToolBarChangeEvent
// -----------------------------------------------------------------------------
//
QToolBarChangeEvent::~QToolBarChangeEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragMoveEvent::QDragMoveEvent
// -----------------------------------------------------------------------------
//
QDragMoveEvent::QDragMoveEvent( 
        const QPoint & pos,
        Qt::DropActions actions,
        const QMimeData * data,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers,
        Type type )
    :QDropEvent(pos, actions, data, buttons, modifiers, type)
    //QDropEvent( /*pos, actions, data, buttons, modifiers, type*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragMoveEvent::~QDragMoveEvent
// -----------------------------------------------------------------------------
//
QDragMoveEvent::~QDragMoveEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QPaintEvent::QPaintEvent
// -----------------------------------------------------------------------------
//
QPaintEvent::QPaintEvent( 
        const QRegion & paintRegion )
    :QEvent(Paint)
    //QEvent( /*paintRegion*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QPaintEvent::QPaintEvent
// -----------------------------------------------------------------------------
//
QPaintEvent::QPaintEvent( 
        const QRect & paintRect )
    :QEvent(Paint)
    //QEvent( /*paintRect*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QPaintEvent::~QPaintEvent
// -----------------------------------------------------------------------------
//
QPaintEvent::~QPaintEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QKeyEvent::QKeyEvent
// -----------------------------------------------------------------------------
//
QKeyEvent::QKeyEvent( 
        Type type,
        int key,
        Qt::KeyboardModifiers modifiers,
        const QString & text,
        bool autorep,
        ushort count )
    :QInputEvent(type, modifiers)
    //QInputEvent( /*type, key, modifiers, text, autorep, count*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QKeyEvent::~QKeyEvent
// -----------------------------------------------------------------------------
//
QKeyEvent::~QKeyEvent(  )
    {
    
    }

// -----------------------------------------------------------------------------
// QKeyEvent::matches
// -----------------------------------------------------------------------------
//
bool QKeyEvent::matches( 
        QKeySequence::StandardKey key ) const
    {
    SMC_MOCK_METHOD1( bool, QKeySequence::StandardKey, key )
    }


// -----------------------------------------------------------------------------
// QKeyEvent::modifiers
// -----------------------------------------------------------------------------
//
Qt::KeyboardModifiers QKeyEvent::modifiers(  ) const
    {
    SMC_MOCK_METHOD0( Qt::KeyboardModifiers )
    }


// -----------------------------------------------------------------------------
// QKeyEvent::createExtendedKeyEvent
// -----------------------------------------------------------------------------
//
QKeyEvent * QKeyEvent::createExtendedKeyEvent( 
        Type type,
        int key,
        Qt::KeyboardModifiers modifiers,
        quint32 nativeScanCode,
        quint32 nativeVirtualKey,
        quint32 nativeModifiers,
        const QString & text,
        bool autorep,
        ushort count )
    {
    /*SMC_MOCK_METHOD9( QKeyEvent *, Type, type, 
        int, key, 
        Qt::KeyboardModifiers, modifiers, 
        quint32, nativeScanCode, 
        quint32, nativeVirtualKey, 
        quint32, nativeModifiers, 
        const QString &, text, 
        bool, autorep, 
        ushort, count )*/
    }


// -----------------------------------------------------------------------------
// QKeyEvent::nativeScanCode
// -----------------------------------------------------------------------------
//
quint32 QKeyEvent::nativeScanCode(  ) const
    {
    SMC_MOCK_METHOD0( quint32 )
    }


// -----------------------------------------------------------------------------
// QKeyEvent::nativeVirtualKey
// -----------------------------------------------------------------------------
//
quint32 QKeyEvent::nativeVirtualKey(  ) const
    {
    SMC_MOCK_METHOD0( quint32 )
    }


// -----------------------------------------------------------------------------
// QKeyEvent::nativeModifiers
// -----------------------------------------------------------------------------
//
quint32 QKeyEvent::nativeModifiers(  ) const
    {
    SMC_MOCK_METHOD0( quint32 )
    }


// -----------------------------------------------------------------------------
// QClipboardEvent::QClipboardEvent
// -----------------------------------------------------------------------------
//
QClipboardEvent::QClipboardEvent( 
        QEventPrivate * data )
    :QEvent(QEvent::Clipboard)
    //QEvent( /*data*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QClipboardEvent::~QClipboardEvent
// -----------------------------------------------------------------------------
//
QClipboardEvent::~QClipboardEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QHideEvent::QHideEvent
// -----------------------------------------------------------------------------
//
QHideEvent::QHideEvent(  )
    :QEvent(Hide)
    //QEvent( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QHideEvent::~QHideEvent
// -----------------------------------------------------------------------------
//
QHideEvent::~QHideEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QInputMethodEvent::QInputMethodEvent
// -----------------------------------------------------------------------------
//
QInputMethodEvent::QInputMethodEvent(  )
    :QEvent(QEvent::InputMethod)
    //QEvent( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QInputMethodEvent::QInputMethodEvent
// -----------------------------------------------------------------------------
//
QInputMethodEvent::QInputMethodEvent( 
        const QString & preeditText,
        const QList<Attribute> & attributes )
    :QEvent(QEvent::InputMethod)
    //QEvent( /*preeditText, attributes*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QInputMethodEvent::setCommitString
// -----------------------------------------------------------------------------
//
void QInputMethodEvent::setCommitString( 
        const QString & commitString,
        int replaceFrom,
        int replaceLength )
    {
    SMC_MOCK_METHOD3( void, const QString &, commitString, 
        int, replaceFrom, 
        int, replaceLength )
    }


// -----------------------------------------------------------------------------
// QInputMethodEvent::QInputMethodEvent
// -----------------------------------------------------------------------------
//
QInputMethodEvent::QInputMethodEvent( 
        const QInputMethodEvent & other )
    :QEvent(QEvent::InputMethod)
    //QEvent( /*other*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QTabletEvent::QTabletEvent
// -----------------------------------------------------------------------------
//
QTabletEvent::QTabletEvent( 
        Type t,
        const QPoint & pos,
        const QPoint & globalPos,
        const QPointF & hiResGlobalPos,
        int device,
        int pointerType,
        qreal pressure,
        int xTilt,
        int yTilt,
        qreal tangentialPressure,
        qreal rotation,
        int z,
        Qt::KeyboardModifiers keyState,
        qint64 uniqueID )
    :QInputEvent(t, keyState)
    //QInputEvent( /*t, pos, globalPos, hiResGlobalPos, device, pointerType, pressure, xTilt, yTilt, tangentialPressure, rotation, z, keyState, uniqueID*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QTabletEvent::~QTabletEvent
// -----------------------------------------------------------------------------
//
QTabletEvent::~QTabletEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QDropEvent::QDropEvent
// -----------------------------------------------------------------------------
//
QDropEvent::QDropEvent( 
        const QPoint & pos,
        Qt::DropActions actions,
        const QMimeData * data,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers,
        Type type )
    :QEvent(type)
    //QEvent( /*pos, actions, data, buttons, modifiers, type*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QDropEvent::~QDropEvent
// -----------------------------------------------------------------------------
//
QDropEvent::~QDropEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QDropEvent::setDropAction
// -----------------------------------------------------------------------------
//
void QDropEvent::setDropAction( 
        Qt::DropAction act )
    {
    SMC_MOCK_METHOD1( void, Qt::DropAction, act )
    }


// -----------------------------------------------------------------------------
// QDropEvent::source
// -----------------------------------------------------------------------------
//
QWidget * QDropEvent::source(  ) const
    {
    SMC_MOCK_METHOD0( QWidget * )
    }


// -----------------------------------------------------------------------------
// QDropEvent::format
// -----------------------------------------------------------------------------
//
const char * QDropEvent::format( 
        int n ) const
    {
    SMC_MOCK_METHOD1( const char *, int, n )
    }


// -----------------------------------------------------------------------------
// QDropEvent::encodedData
// -----------------------------------------------------------------------------
//
QByteArray QDropEvent::encodedData( 
        const char * ch) const
    {
    SMC_MOCK_METHOD1( QByteArray, const char *, ch )
    }


// -----------------------------------------------------------------------------
// QDropEvent::provides
// -----------------------------------------------------------------------------
//
bool QDropEvent::provides( 
        const char * ch) const
    {
    SMC_MOCK_METHOD1( bool, const char *, ch )
    }


// -----------------------------------------------------------------------------
// QHoverEvent::QHoverEvent
// -----------------------------------------------------------------------------
//
QHoverEvent::QHoverEvent( 
        Type type,
        const QPoint & pos,
        const QPoint & oldPos )
    :QEvent(type)
    //QEvent( /*type, pos, oldPos*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QHoverEvent::~QHoverEvent
// -----------------------------------------------------------------------------
//
QHoverEvent::~QHoverEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QActionEvent::QActionEvent
// -----------------------------------------------------------------------------
//
QActionEvent::QActionEvent( 
        int type,
        QAction * action,
        QAction * before )
    :QEvent(static_cast<QEvent::Type>(type))
    //QEvent( /*type, action, before*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QActionEvent::~QActionEvent
// -----------------------------------------------------------------------------
//
QActionEvent::~QActionEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragLeaveEvent::QDragLeaveEvent
// -----------------------------------------------------------------------------
//
QDragLeaveEvent::QDragLeaveEvent(  )
    :QEvent(DragLeave)
    //QEvent( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragLeaveEvent::~QDragLeaveEvent
// -----------------------------------------------------------------------------
//
QDragLeaveEvent::~QDragLeaveEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QResizeEvent::QResizeEvent
// -----------------------------------------------------------------------------
//
QResizeEvent::QResizeEvent( 
        const QSize & size,
        const QSize & oldSize )
    :QEvent(Resize)
    //QEvent( /*size, oldSize*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QResizeEvent::~QResizeEvent
// -----------------------------------------------------------------------------
//
QResizeEvent::~QResizeEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragResponseEvent::QDragResponseEvent
// -----------------------------------------------------------------------------
//
QDragResponseEvent::QDragResponseEvent( 
        bool accepted )
    :QEvent(DragResponse)
    //QEvent( /*accepted*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QDragResponseEvent::~QDragResponseEvent
// -----------------------------------------------------------------------------
//
QDragResponseEvent::~QDragResponseEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QInputEvent::QInputEvent
// -----------------------------------------------------------------------------
//
QInputEvent::QInputEvent( 
        Type type,
        Qt::KeyboardModifiers modifiers )
    :QEvent(type)
    //QEvent( /*type, modifiers*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QInputEvent::~QInputEvent
// -----------------------------------------------------------------------------
//
QInputEvent::~QInputEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QWhatsThisClickedEvent::QWhatsThisClickedEvent
// -----------------------------------------------------------------------------
//
QWhatsThisClickedEvent::QWhatsThisClickedEvent( 
        const QString & href )
    :QEvent(WhatsThisClicked)
    //QEvent( /*href*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QWhatsThisClickedEvent::~QWhatsThisClickedEvent
// -----------------------------------------------------------------------------
//
QWhatsThisClickedEvent::~QWhatsThisClickedEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QShortcutEvent::QShortcutEvent
// -----------------------------------------------------------------------------
//
QShortcutEvent::QShortcutEvent( 
        const QKeySequence & key,
        int id,
        bool ambiguous )
    :QEvent(Shortcut)
    //QEvent( /*key, id, ambiguous*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QShortcutEvent::~QShortcutEvent
// -----------------------------------------------------------------------------
//
QShortcutEvent::~QShortcutEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QMouseEvent::QMouseEvent
// -----------------------------------------------------------------------------
//
QMouseEvent::QMouseEvent( 
        Type type,
        const QPoint & pos,
        Qt::MouseButton button,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers )
    :QInputEvent(type, modifiers)
    //QInputEvent( /*type, pos, button, buttons, modifiers*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QMouseEvent::QMouseEvent
// -----------------------------------------------------------------------------
//
QMouseEvent::QMouseEvent( 
        Type type,
        const QPoint & pos,
        const QPoint & globalPos,
        Qt::MouseButton button,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers )
    :QInputEvent(type, modifiers)
    //QInputEvent( /*type, pos, globalPos, button, buttons, modifiers*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QMouseEvent::~QMouseEvent
// -----------------------------------------------------------------------------
//
QMouseEvent::~QMouseEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QMouseEvent::createExtendedMouseEvent
// -----------------------------------------------------------------------------
//
QMouseEvent * QMouseEvent::createExtendedMouseEvent( 
        Type type,
        const QPointF & pos,
        const QPoint & globalPos,
        Qt::MouseButton button,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers )
    {
    /*SMC_MOCK_METHOD6( QMouseEvent *, Type, type, 
        const QPointF &, pos, 
        const QPoint &, globalPos, 
        Qt::MouseButton, button, 
        Qt::MouseButtons, buttons, 
        Qt::KeyboardModifiers, modifiers )*/
    }


// -----------------------------------------------------------------------------
// QMouseEvent::posF
// -----------------------------------------------------------------------------
//
QPointF QMouseEvent::posF(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QFocusEvent::QFocusEvent
// -----------------------------------------------------------------------------
//
QFocusEvent::QFocusEvent( 
        Type type,
        Qt::FocusReason reason )
    :QEvent(type)
    //QEvent( /*type, reason*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QFocusEvent::~QFocusEvent
// -----------------------------------------------------------------------------
//
QFocusEvent::~QFocusEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QFocusEvent::reason
// -----------------------------------------------------------------------------
//
Qt::FocusReason QFocusEvent::reason(  )
    {
    SMC_MOCK_METHOD0( Qt::FocusReason )
    }


// -----------------------------------------------------------------------------
// QFocusEvent::reason
// -----------------------------------------------------------------------------
//
Qt::FocusReason QFocusEvent::reason(  ) const
    {
    SMC_MOCK_METHOD0( Qt::FocusReason )
    }


// -----------------------------------------------------------------------------
// QTouchEvent::QTouchEvent
// -----------------------------------------------------------------------------
//
QTouchEvent::QTouchEvent( 
        QEvent::Type eventType,
        QTouchEvent::DeviceType deviceType,
        Qt::KeyboardModifiers modifiers,
        Qt::TouchPointStates touchPointStates,
        const QList<QTouchEvent::TouchPoint> & touchPoints )
    :QInputEvent(eventType, modifiers)
    //QInputEvent( /*eventType, deviceType, modifiers, touchPointStates, touchPoints*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QTouchEvent::~QTouchEvent
// -----------------------------------------------------------------------------
//
QTouchEvent::~QTouchEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QCloseEvent::QCloseEvent
// -----------------------------------------------------------------------------
//
QCloseEvent::QCloseEvent(  )
    :QEvent(Close)
    //QEvent( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QCloseEvent::~QCloseEvent
// -----------------------------------------------------------------------------
//
QCloseEvent::~QCloseEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QWindowStateChangeEvent::QWindowStateChangeEvent
// -----------------------------------------------------------------------------
//
QWindowStateChangeEvent::QWindowStateChangeEvent( 
        Qt::WindowStates aOldState )
    :QEvent(WindowStateChange)
    //QEvent( /*aOldState*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QWindowStateChangeEvent::QWindowStateChangeEvent
// -----------------------------------------------------------------------------
//
QWindowStateChangeEvent::QWindowStateChangeEvent( 
        Qt::WindowStates aOldState,
        bool isOverride )
    :QEvent(WindowStateChange)
    //QEvent( /*aOldState, isOverride*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QWindowStateChangeEvent::~QWindowStateChangeEvent
// -----------------------------------------------------------------------------
//
QWindowStateChangeEvent::~QWindowStateChangeEvent(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QWindowStateChangeEvent::isOverride
// -----------------------------------------------------------------------------
//
bool QWindowStateChangeEvent::isOverride(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QShowEvent::QShowEvent
// -----------------------------------------------------------------------------
//
QShowEvent::QShowEvent(  )
    :QEvent(Show)
    //QEvent( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QShowEvent::~QShowEvent
// -----------------------------------------------------------------------------
//
QShowEvent::~QShowEvent(  )
    {
    
    }


