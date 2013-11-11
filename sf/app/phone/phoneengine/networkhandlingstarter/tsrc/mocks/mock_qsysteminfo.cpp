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
#include <qsysteminfo.h>
#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>

// ============================ MEMBER FUNCTIONS ===============================

QTM_BEGIN_NAMESPACE


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QSystemStorageInfo::QSystemStorageInfo
// -----------------------------------------------------------------------------
//
QSystemStorageInfo::QSystemStorageInfo( QObject * parent )
    :
    QObject(parent)
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemStorageInfo::~QSystemStorageInfo
// -----------------------------------------------------------------------------
//
QSystemStorageInfo::~QSystemStorageInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemStorageInfo::totalDiskSpace
// -----------------------------------------------------------------------------
//
Q_INVOKABLE qlonglong QSystemStorageInfo::totalDiskSpace( 
        const QString & driveVolume )
    {
    SMC_MOCK_METHOD1( Q_INVOKABLE qlonglong, const QString &, driveVolume )
    }


// -----------------------------------------------------------------------------
// QSystemStorageInfo::availableDiskSpace
// -----------------------------------------------------------------------------
//
Q_INVOKABLE qlonglong QSystemStorageInfo::availableDiskSpace( 
        const QString & driveVolume )
    {
    SMC_MOCK_METHOD1( Q_INVOKABLE qlonglong, const QString &, driveVolume )
    }


// -----------------------------------------------------------------------------
// QSystemStorageInfo::logicalDrives
// -----------------------------------------------------------------------------
//
QStringList QSystemStorageInfo::logicalDrives(  )
    {
    SMC_MOCK_METHOD0( QStringList )
    }


// -----------------------------------------------------------------------------
// QSystemStorageInfo::typeForDrive
// -----------------------------------------------------------------------------
//
Q_INVOKABLE QSystemStorageInfo::DriveType QSystemStorageInfo::typeForDrive( 
        const QString & driveVolume )
    {
    SMC_MOCK_METHOD1( Q_INVOKABLE QSystemStorageInfo::DriveType, const QString &, driveVolume )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::QSystemDeviceInfo
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::QSystemDeviceInfo ( QObject * parent )
    : 
    QObject(parent)
    {
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::~QSystemDeviceInfo
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::~QSystemDeviceInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::inputMethodType
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfo::inputMethodType(  )
    {
    SMC_MOCK_METHOD0( QSystemDeviceInfo::InputMethodFlags )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::imei
// -----------------------------------------------------------------------------
//
QString QSystemDeviceInfo::imei(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::imsi
// -----------------------------------------------------------------------------
//
QString QSystemDeviceInfo::imsi(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::manufacturer
// -----------------------------------------------------------------------------
//
QString QSystemDeviceInfo::manufacturer(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::model
// -----------------------------------------------------------------------------
//
QString QSystemDeviceInfo::model(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::productName
// -----------------------------------------------------------------------------
//
QString QSystemDeviceInfo::productName(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::batteryLevel
// -----------------------------------------------------------------------------
//
int QSystemDeviceInfo::batteryLevel(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::batteryStatus
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::BatteryStatus QSystemDeviceInfo::batteryStatus(  )
    {
    SMC_MOCK_METHOD0( QSystemDeviceInfo::BatteryStatus )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::isDeviceLocked
// -----------------------------------------------------------------------------
//
bool QSystemDeviceInfo::isDeviceLocked(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::simStatus
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::SimStatus QSystemDeviceInfo::simStatus(  )
    {
    SMC_MOCK_METHOD0( QSystemDeviceInfo::SimStatus )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::currentProfile
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::Profile QSystemDeviceInfo::currentProfile(  )
    {
    SMC_MOCK_METHOD0( QSystemDeviceInfo::Profile )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::currentPowerState
// -----------------------------------------------------------------------------
//
QSystemDeviceInfo::PowerState QSystemDeviceInfo::currentPowerState(  )
    {
    SMC_MOCK_METHOD0( QSystemDeviceInfo::PowerState )
    }


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::connectNotify
// -----------------------------------------------------------------------------
//
void QSystemDeviceInfo::connectNotify(const char *signal)
{
    SMC_MOCK_METHOD1( void, const char *, signal )
}


// -----------------------------------------------------------------------------
// QSystemDeviceInfo::disconnectNotify
// -----------------------------------------------------------------------------
//
void QSystemDeviceInfo::disconnectNotify(const char *signal)
{
    SMC_MOCK_METHOD1( void, const char *, signal )
}


// -----------------------------------------------------------------------------
// QSystemInfo::QSystemInfo
// -----------------------------------------------------------------------------
//
QSystemInfo::QSystemInfo( QObject * parent )
    : 
    QObject(parent)
    {
    }


// -----------------------------------------------------------------------------
// QSystemInfo::~QSystemInfo
// -----------------------------------------------------------------------------
//
QSystemInfo::~QSystemInfo()
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemInfo::currentLanguage
// -----------------------------------------------------------------------------
//
QString QSystemInfo::currentLanguage()
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemInfo::availableLanguages
// -----------------------------------------------------------------------------
//
QStringList QSystemInfo::availableLanguages(  )
    {
    SMC_MOCK_METHOD0( QStringList )
    }


// -----------------------------------------------------------------------------
// QSystemInfo::currentCountryCode
// -----------------------------------------------------------------------------
//
QString QSystemInfo::currentCountryCode(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemInfo::version
// -----------------------------------------------------------------------------
//
QString QSystemInfo::version( 
        QSystemInfo::Version type,
        const QString & parameter )
    {
    SMC_MOCK_METHOD2( QString, QSystemInfo::Version, type, 
        const QString &, parameter )
    }


// -----------------------------------------------------------------------------
// QSystemInfo::hasFeatureSupported
// -----------------------------------------------------------------------------
//
bool QSystemInfo::hasFeatureSupported( 
        QSystemInfo::Feature feature )
    {
    SMC_MOCK_METHOD1( bool, QSystemInfo::Feature, feature )
    }


// -----------------------------------------------------------------------------
// QSystemInfo::connectNotify
// -----------------------------------------------------------------------------
//
void QSystemInfo::connectNotify(const char *signal)
{
    SMC_MOCK_METHOD1( void, const char *, signal )
}


// -----------------------------------------------------------------------------
// QSystemInfo::disconnectNotify
// -----------------------------------------------------------------------------
//
void QSystemInfo::disconnectNotify(const char *signal)
{
    SMC_MOCK_METHOD1( void, const char *, signal )
}


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::QSystemNetworkInfo
// -----------------------------------------------------------------------------
//
QSystemNetworkInfo::QSystemNetworkInfo( QObject * parent )
    : 
    QObject(parent)
    {
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::~QSystemNetworkInfo
// -----------------------------------------------------------------------------
//
QSystemNetworkInfo::~QSystemNetworkInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::networkStatus
// -----------------------------------------------------------------------------
//
Q_INVOKABLE QSystemNetworkInfo::NetworkStatus QSystemNetworkInfo::networkStatus( 
        QSystemNetworkInfo::NetworkMode mode )
    {
    SMC_MOCK_METHOD1( Q_INVOKABLE QSystemNetworkInfo::NetworkStatus, 
        QSystemNetworkInfo::NetworkMode, mode )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::networkSignalStrength
// -----------------------------------------------------------------------------
//
Q_INVOKABLE int QSystemNetworkInfo::networkSignalStrength( 
    QSystemNetworkInfo::NetworkMode mode )
    {
    SMC_MOCK_METHOD1( Q_INVOKABLE int, 
        QSystemNetworkInfo::NetworkMode, mode )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::macAddress
// -----------------------------------------------------------------------------
//
QString QSystemNetworkInfo::macAddress( 
    QSystemNetworkInfo::NetworkMode mode )
    {
    SMC_MOCK_METHOD1( QString, QSystemNetworkInfo::NetworkMode, mode )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::currentMode
// -----------------------------------------------------------------------------
//
QSystemNetworkInfo::NetworkMode QSystemNetworkInfo::currentMode(  )
    {
    SMC_MOCK_METHOD0( QSystemNetworkInfo::NetworkMode )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::cellId
// -----------------------------------------------------------------------------
//
int QSystemNetworkInfo::cellId(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::locationAreaCode
// -----------------------------------------------------------------------------
//
int QSystemNetworkInfo::locationAreaCode(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::currentMobileCountryCode
// -----------------------------------------------------------------------------
//
QString QSystemNetworkInfo::currentMobileCountryCode(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::currentMobileNetworkCode
// -----------------------------------------------------------------------------
//
QString QSystemNetworkInfo::currentMobileNetworkCode(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::homeMobileCountryCode
// -----------------------------------------------------------------------------
//
QString QSystemNetworkInfo::homeMobileCountryCode(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::homeMobileNetworkCode
// -----------------------------------------------------------------------------
//
QString QSystemNetworkInfo::homeMobileNetworkCode(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::networkName
// -----------------------------------------------------------------------------
//
Q_INVOKABLE QString QSystemNetworkInfo::networkName( 
    QSystemNetworkInfo::NetworkMode mode )
    {
    SMC_MOCK_METHOD1( Q_INVOKABLE QString, QSystemNetworkInfo::NetworkMode, mode )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::interfaceForMode
// -----------------------------------------------------------------------------
//
QNetworkInterface QSystemNetworkInfo::interfaceForMode( 
    QSystemNetworkInfo::NetworkMode mode )
    {
    SMC_MOCK_METHOD1( QNetworkInterface, QSystemNetworkInfo::NetworkMode, mode )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::connectNotify
// -----------------------------------------------------------------------------
//
void QSystemNetworkInfo::connectNotify( 
        const char * signal )
    {
    SMC_MOCK_METHOD1( void, const char *, signal )
    }


// -----------------------------------------------------------------------------
// QSystemNetworkInfo::disconnectNotify
// -----------------------------------------------------------------------------
//
void QSystemNetworkInfo::disconnectNotify( 
        const char * signal )
    {
    SMC_MOCK_METHOD1( void, const char *, signal )
    }


// -----------------------------------------------------------------------------
// QSystemScreenSaver::QSystemScreenSaver
// -----------------------------------------------------------------------------
//
QSystemScreenSaver::QSystemScreenSaver ( QObject * parent )
    :
    QObject(parent)
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemScreenSaver::~QSystemScreenSaver
// -----------------------------------------------------------------------------
//
QSystemScreenSaver::~QSystemScreenSaver(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemScreenSaver::screenSaverInhibited
// -----------------------------------------------------------------------------
//
bool QSystemScreenSaver::screenSaverInhibited(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QSystemScreenSaver::setScreenSaverInhibit
// -----------------------------------------------------------------------------
//
Q_INVOKABLE bool QSystemScreenSaver::setScreenSaverInhibit(  )
    {
    SMC_MOCK_METHOD0( Q_INVOKABLE bool )
    }


// -----------------------------------------------------------------------------
// QSystemDisplayInfo::QSystemDisplayInfo
// -----------------------------------------------------------------------------
//
QSystemDisplayInfo::QSystemDisplayInfo( 
        QObject * parent )
    :
    QObject( parent )
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemDisplayInfo::~QSystemDisplayInfo
// -----------------------------------------------------------------------------
//
QSystemDisplayInfo::~QSystemDisplayInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QSystemDisplayInfo::displayBrightness
// -----------------------------------------------------------------------------
//
int QSystemDisplayInfo::displayBrightness( 
        int screen )
    {
    SMC_MOCK_METHOD1( int, int, screen )
    }


// -----------------------------------------------------------------------------
// QSystemDisplayInfo::colorDepth
// -----------------------------------------------------------------------------
//
int QSystemDisplayInfo::colorDepth( 
        int screen )
    {
    SMC_MOCK_METHOD1( int, int, screen )
    }

#include "moc_qsysteminfo.cpp"

QTM_END_NAMESPACE
