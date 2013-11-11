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
#include <QObject>
#include <QVariant>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QObjectUserData::~QObjectUserData
// -----------------------------------------------------------------------------
//
QObjectUserData::~QObjectUserData(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QObjectData::~QObjectData
// -----------------------------------------------------------------------------
//
QObjectData::~QObjectData(  )
    {
    
    }

#ifdef QT3_SUPPORT
// -----------------------------------------------------------------------------
// QObject::QObject
// -----------------------------------------------------------------------------
//
QT3_SUPPORT_CONSTRUCTOR QObject::QObject( 
        QObject * parent,
        const char * name )
    {
    
    }
#endif

// -----------------------------------------------------------------------------
// QObject::~QObject
// -----------------------------------------------------------------------------
//
QObject::~QObject(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QObject::event
// -----------------------------------------------------------------------------
//
bool QObject::event( 
        QEvent * )
    {
 //   SMC_MOCK_METHOD1( bool, QEvent, * )
    }


// -----------------------------------------------------------------------------
// QObject::eventFilter
// -----------------------------------------------------------------------------
//
bool QObject::eventFilter( 
        QObject *,
        QEvent * )
    {
 //   SMC_MOCK_METHOD2( bool, QObject, *, 
//        QEvent, * )
    }

#ifdef qdoc
// -----------------------------------------------------------------------------
// QObject::tr
// -----------------------------------------------------------------------------
//
QString QObject::tr( 
        const char * sourceText,
        const char * comment,
        int n )
    {
    SMC_MOCK_METHOD3( QString, const char *, sourceText, 
        const char *, comment, 
        int, n )
    }


// -----------------------------------------------------------------------------
// QObject::trUtf8
// -----------------------------------------------------------------------------
//
QString QObject::trUtf8( 
        const char * sourceText,
        const char * comment,
        int n )
    {
    SMC_MOCK_METHOD3( QString, const char *, sourceText, 
        const char *, comment, 
        int, n )
    }


// -----------------------------------------------------------------------------
// QObject::metaObject
// -----------------------------------------------------------------------------
//
const QMetaObject * QObject::metaObject(  ) const
    {
 //   SMC_MOCK_METHOD0( const QMetaObject * )
    }

#endif
#ifdef QT_NO_TRANSLATION
// -----------------------------------------------------------------------------
// QObject::tr
// -----------------------------------------------------------------------------
//
QString QObject::tr( 
        const char * sourceText,
        const char * )
    {
    SMC_MOCK_METHOD2( QString, const char *, sourceText, 
        const char, * )
    }


// -----------------------------------------------------------------------------
// QObject::tr
// -----------------------------------------------------------------------------
//
QString QObject::tr( 
        const char * sourceText,
        const char * )
    {
    SMC_MOCK_METHOD2( QString, const char *, sourceText, 
        const char, * )
    }
#ifndef QT_NO_TEXTCODEC

// -----------------------------------------------------------------------------
// QObject::trUtf8
// -----------------------------------------------------------------------------
//
QString QObject::trUtf8( 
        const char * sourceText,
        const char * )
    {
    SMC_MOCK_METHOD2( QString, const char *, sourceText, 
        const char, * )
    }


// -----------------------------------------------------------------------------
// QObject::trUtf8
// -----------------------------------------------------------------------------
//
QString QObject::trUtf8( 
        const char * sourceText,
        const char * )
    {
    SMC_MOCK_METHOD2( QString, const char *, sourceText, 
        const char, * )
    }
#endif
#endif //QT_NO_TRANSLATION

// -----------------------------------------------------------------------------
// QObject::objectName
// -----------------------------------------------------------------------------
//
QString QObject::objectName(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QObject::setObjectName
// -----------------------------------------------------------------------------
//
void QObject::setObjectName( 
        const QString & name )
    {
    SMC_MOCK_METHOD1( void, const QString &, name )
    }


// -----------------------------------------------------------------------------
// QObject::blockSignals
// -----------------------------------------------------------------------------
//
bool QObject::blockSignals( 
        bool b )
    {
    SMC_MOCK_METHOD1( bool, bool, b )
    }


// -----------------------------------------------------------------------------
// QObject::thread
// -----------------------------------------------------------------------------
//
QThread * QObject::thread(  ) const
    {
//    SMC_MOCK_METHOD0( QThread * )
    }


// -----------------------------------------------------------------------------
// QObject::moveToThread
// -----------------------------------------------------------------------------
//
void QObject::moveToThread( 
        QThread * thread )
    {
 //   SMC_MOCK_METHOD1( void, QThread *, thread )
    }


// -----------------------------------------------------------------------------
// QObject::startTimer
// -----------------------------------------------------------------------------
//
int QObject::startTimer( 
        int interval )
    {
    SMC_MOCK_METHOD1( int, int, interval )
    }


// -----------------------------------------------------------------------------
// QObject::killTimer
// -----------------------------------------------------------------------------
//
void QObject::killTimer( 
        int id )
    {
    SMC_MOCK_METHOD1( void, int, id )
    }

#ifdef QT3_SUPPORT
// -----------------------------------------------------------------------------
// QObject::child
// -----------------------------------------------------------------------------
//
QT3_SUPPORT QObject * QObject::child( 
        const char * objName,
        const char * inheritsClass,
        bool recursiveSearch ) const
    {
    SMC_MOCK_METHOD3( QT3_SUPPORT QObject *, const char *, objName, 
        const char *, inheritsClass, 
        bool, recursiveSearch )
    }


// -----------------------------------------------------------------------------
// QObject::queryList
// -----------------------------------------------------------------------------
//
QT3_SUPPORT QObjectList QObject::queryList( 
        const char * inheritsClass,
        const char * objName,
        bool regexpMatch,
        bool recursiveSearch ) const
    {
    SMC_MOCK_METHOD4( QT3_SUPPORT QObjectList, const char *, inheritsClass, 
        const char *, objName, 
        bool, regexpMatch, 
        bool, recursiveSearch )
    }
#endif

// -----------------------------------------------------------------------------
// QObject::setParent
// -----------------------------------------------------------------------------
//
void QObject::setParent( 
        QObject * )
    {
 //   SMC_MOCK_METHOD1( void, QObject, * )
    }


// -----------------------------------------------------------------------------
// QObject::installEventFilter
// -----------------------------------------------------------------------------
//
void QObject::installEventFilter( 
        QObject * )
    {
 //   SMC_MOCK_METHOD1( void, QObject, * )
    }


// -----------------------------------------------------------------------------
// QObject::removeEventFilter
// -----------------------------------------------------------------------------
//
void QObject::removeEventFilter( 
        QObject * )
    {
 //   SMC_MOCK_METHOD1( void, QObject, * )
    }


// -----------------------------------------------------------------------------
// QObject::connect
// -----------------------------------------------------------------------------
//
bool QObject::connect( 
        const QObject * sender,
        const char * signal,
        const QObject * receiver,
        const char * member,
        Qt:: ConnectionType )
    {
//    SMC_MOCK_METHOD5( bool, const QObject *, sender, 
//        const char *, signal, 
//        const QObject *, receiver, 
//        const char *, member, 
//        Qt::, ConnectionType )
    }


// -----------------------------------------------------------------------------
// QObject::disconnect
// -----------------------------------------------------------------------------
//
bool QObject::disconnect( 
        const QObject * sender,
        const char * signal,
        const QObject * receiver,
        const char * member )
    {
//    SMC_MOCK_METHOD4( bool, const QObject *, sender, 
//        const char *, signal, 
//        const QObject *, receiver, 
//        const char *, member )
    }


// -----------------------------------------------------------------------------
// QObject::dumpObjectTree
// -----------------------------------------------------------------------------
//
void QObject::dumpObjectTree(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QObject::dumpObjectInfo
// -----------------------------------------------------------------------------
//
void QObject::dumpObjectInfo(  )
    {
    SMC_MOCK_METHOD0( void )
    }

#ifndef QT_NO_PROPERTIES
// -----------------------------------------------------------------------------
// QObject::setProperty
// -----------------------------------------------------------------------------
//
bool QObject::setProperty( 
        const char * name,
        const QVariant & value )
    {
//    SMC_MOCK_METHOD2( bool, const char *, name, 
//        const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// QObject::property
// -----------------------------------------------------------------------------
//
QVariant QObject::property( 
        const char * name ) const
    {
//    SMC_MOCK_METHOD1( QVariant, const char *, name )
    }


// -----------------------------------------------------------------------------
// QObject::dynamicPropertyNames
// -----------------------------------------------------------------------------
//
QList <QByteArray > QObject::dynamicPropertyNames(  ) const
    {
//    SMC_MOCK_METHOD0( QList <QByteArray > )
    }
#endif // QT_NO_PROPERTIES
#ifndef QT_NO_USERDATA
// -----------------------------------------------------------------------------
// QObject::registerUserData
// -----------------------------------------------------------------------------
//
uint QObject::registerUserData(  )
    {
    SMC_MOCK_METHOD0( uint )
    }


// -----------------------------------------------------------------------------
// QObject::setUserData
// -----------------------------------------------------------------------------
//
void QObject::setUserData( 
        uint id,
        QObjectUserData * data )
    {
//    SMC_MOCK_METHOD2( void, uint, id, 
//        QObjectUserData *, data )
    }


// -----------------------------------------------------------------------------
// QObject::userData
// -----------------------------------------------------------------------------
//
QObjectUserData * QObject::userData( 
        uint id ) const
    {
//    SMC_MOCK_METHOD1( QObjectUserData *, uint, id )
    }
#endif // QT_NO_USERDATA

// -----------------------------------------------------------------------------
// QObject::destroyed
// -----------------------------------------------------------------------------
//
/*
Q_SIGNALS : void QObject::destroyed( 
        QObject * )
    {
    SMC_MOCK_METHOD1( Q_SIGNALS : void, QObject, * )
    }*/


// -----------------------------------------------------------------------------
// QObject::deleteLater
// -----------------------------------------------------------------------------
//
void QObject::deleteLater(  )
    {
  //  SMC_MOCK_METHOD0( Q_SLOTS : void )
    }


// -----------------------------------------------------------------------------
// QObject::sender
// -----------------------------------------------------------------------------
//
QObject * QObject::sender(  ) const
    {
 //   SMC_MOCK_METHOD0( QObject * )
    }


// -----------------------------------------------------------------------------
// QObject::receivers
// -----------------------------------------------------------------------------
//
int QObject::receivers( 
        const char * signal ) const
    {
    SMC_MOCK_METHOD1( int, const char *, signal )
    }


// -----------------------------------------------------------------------------
// QObject::timerEvent
// -----------------------------------------------------------------------------
//
void QObject::timerEvent( 
        QTimerEvent * )
    {
 //   SMC_MOCK_METHOD1( void, QTimerEvent, * )
    }


// -----------------------------------------------------------------------------
// QObject::childEvent
// -----------------------------------------------------------------------------
//
void QObject::childEvent( 
        QChildEvent * )
    {
 //   SMC_MOCK_METHOD1( void, QChildEvent, * )
    }


// -----------------------------------------------------------------------------
// QObject::customEvent
// -----------------------------------------------------------------------------
//
void QObject::customEvent( 
        QEvent * )
    {
 //   SMC_MOCK_METHOD1( void, QEvent, * )
    }


// -----------------------------------------------------------------------------
// QObject::connectNotify
// -----------------------------------------------------------------------------
//
void QObject::connectNotify( 
        const char * signal )
    {
 //   SMC_MOCK_METHOD1( void, const char *, signal )
    }


// -----------------------------------------------------------------------------
// QObject::disconnectNotify
// -----------------------------------------------------------------------------
//
void QObject::disconnectNotify( 
        const char * signal )
    {
  //  SMC_MOCK_METHOD1( void, const char *, signal )
    }


// -----------------------------------------------------------------------------
// QObject::QObject
// -----------------------------------------------------------------------------
//
QObject::QObject( 
        QObjectPrivate & dd,
        QObject * parent )
    {
    
    }


