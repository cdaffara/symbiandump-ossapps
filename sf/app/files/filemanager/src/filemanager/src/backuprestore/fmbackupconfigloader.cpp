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
 * 
 * Description:
 *     The source file of the back up config loader of file manager
 */

#include "fmbackupconfigloader.h"
#include "fmbkupengine.h"
#include "fmbkupcommon.h"
#include "fmutils.h"

#include <QFile>
#include <QByteArray>
#include <QCoreApplication>

FmBackupConfigLoader::FmBackupConfigLoader(void)
{
    
}

FmBackupConfigLoader::~FmBackupConfigLoader(void)
{
    /*
	if( mProgressDialog )
		delete mProgressDialog;
		*/
}

void FmBackupConfigLoader::loadData()
{
    if( mDrivesAndOperationList.count() != 0 || mBackupCategoryList.count() != 0 ) {
        return;
    }

    QString path = QCoreApplication::applicationDirPath ();
    path = FmUtils::getBurConfigPath( path );
    QFile file( path );
    if( !file.exists() ) {
        Q_ASSERT( false );
        return;
    }
    if( !file.open( QIODevice::ReadOnly ) ) {
        Q_ASSERT( false );
        return;
    }
    QByteArray data = file.readAll();
    xml.clear();	
    xml.addData(data);
    file.close();
    parseXml();

//    mBackupSettings->load();
}

QList<FmBkupDrivesAndOperation* > FmBackupConfigLoader::driversAndOperationList()
{
    loadData();
    return mDrivesAndOperationList; 
}
QList<FmBkupBackupCategory*> FmBackupConfigLoader::backupCategoryList()
{
    loadData();
    return mBackupCategoryList;
}
    
void FmBackupConfigLoader::parseXml()
{
    while(!xml.atEnd()){
        xml.readNext();
        if( xml.isStartElement() ){ 
            currentTag = xml.name().toString();
            if( xml.name() == "drivesAndOperation" ){
                parseDrivesAndOperation();
            }
            else if( xml.name() == "backupCategory" ){
                parseBackupCategory();
            }
        } 
    }
    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        qDebug("XML ERROR: %d :%s"), xml.lineNumber() ,xml.errorString().toAscii().data();
    }
}


void FmBackupConfigLoader::parseDrivesAndOperation()
{
    FmBkupDrivesAndOperation* driversAndOperation = 
        new FmBkupDrivesAndOperation;

    while( !( xml.isEndElement() && xml.name() == "drivesAndOperation" ) )
    {
        xml.readNext();
        currentTag = xml.name().toString();
        if( xml.name() == "drive" )
        {
            driveString = xml.readElementText();
        }
        else if( xml.name() == "type" )
        {
            typeString = xml.readElementText();
        }
    }

    bool ok = false;
    bool everyParamOk = true;

    unsigned int driveInt = 
        ConfigStringConverter::drivesAndOperations_DriveToUInt( &ok, driveString );
    if( !ok ){
        everyParamOk = false;
    }

    unsigned int typeInt =
        ConfigStringConverter::drivesAndOperations_TypeToUInt( &ok, typeString );
    if( !ok ){
        everyParamOk = false;
    }

    if( everyParamOk ){
        driversAndOperation->setDrvCategories( driveInt );
        driversAndOperation->setOwnerDataType( typeInt );
        mDrivesAndOperationList.append(driversAndOperation);
    } 
    else{
        delete driversAndOperation;
    }

    QString debugString;
    debugString = debugString + "drive = " + driveString;
    debugString = debugString + "\n type = " + typeString;
    //QMessageBox::notifyation( NULL, "drive&Type = ", debugString, QMessageBox::Ok, QMessageBox::NoButton );

    driveString.clear();
    typeString.clear();
}

void FmBackupConfigLoader::parseBackupCategory()
{
    FmBkupBackupCategory* backupCategory = 
        new FmBkupBackupCategory();

    while( !( xml.isEndElement() && xml.name() == "backupCategory" ) )
    {
        xml.readNext();
        currentTag = xml.name().toString();
        if( xml.name() == "category" )
        {
            categoryString = xml.readElementText();
        }
        else if( xml.name() == "archive_name" )
        {
            archive_nameString = xml.readElementText();
        }
        else if( xml.name() == "special_flags" )
        {
            special_flagsString = xml.readElementText();
        }
        else if( xml.name() == "exclude_special_flags" )
        {
           exclude_special_flagsString = xml.readElementText();
        }
        else if( xml.name() == "uids" ){
            QString tempString = xml.readElementText();
            if( tempString.length()>0 ){
                /// debug-----------
                uidsString += '|';
                uidsString += tempString;
                /// debug-----------

                bool ok = false;
                unsigned int value = tempString.toUInt( &ok, 16); 
                if( ok ){
                    backupCategory->addUids( value );
                }
            }
        }
        else if( xml.name() == "exclude_uids" ){
            QString tempString = xml.readElementText();
            if( tempString.length()>0 ){
                /// debug-----------
                exclude_uidsString += '|';
                exclude_uidsString += tempString;
                /// debug-----------

                bool ok = false;
                unsigned int value = tempString.toUInt( &ok, 16); 
                if( ok ){
                    backupCategory->addExclude_uids( value );
                }
            }
        }
    }

    
    bool ok = false;
    bool everyParamOk = true;

    unsigned int categoryInt =
        ConfigStringConverter::backupCategory_CategoryToUInt( &ok, categoryString );
    if( !ok ){
        everyParamOk = false;
    }

    if( archive_nameString.length() <= 0 ){
        everyParamOk = false;
    }
    
    unsigned int special_flagsInt =
        ConfigStringConverter::backupCategory_Special_flagsToUInt( &ok, special_flagsString );
    if( !ok ){
        everyParamOk = false;
    }

    unsigned int exclude_special_flagsInt =
        ConfigStringConverter::backupCategory_Special_flagsToUInt( &ok, exclude_special_flagsString );
    if( !ok ){
        everyParamOk = false;
    }

    if( everyParamOk ){
        backupCategory->setCategory( categoryInt );
        backupCategory->setArchive_name( archive_nameString );
        backupCategory->setSpecial_flags( special_flagsInt );
        backupCategory->setExclude_special_flags( exclude_special_flagsInt );
        mBackupCategoryList.append( backupCategory );
    } 
    else{
        delete backupCategory;
    }

    QString debugString;
    debugString = debugString + "category = " + categoryString;
    debugString = debugString + "\n archive_name = " + archive_nameString;
    debugString = debugString + "\n special_flags = " + special_flagsString;
    debugString = debugString + "\n exclude_special_flags = " + special_flagsString;
    debugString = debugString + "\n exclude_special_flags = " + exclude_special_flagsString;
    debugString = debugString + "\n uids = " + uidsString;
    debugString = debugString + "\n exclude_uids = " + exclude_uidsString;
    //QMessageBox::notifyation( NULL, "category = ", debugString, QMessageBox::Ok, QMessageBox::NoButton );

    categoryString.clear();
    archive_nameString.clear();
    special_flagsString.clear();
    exclude_special_flagsString.clear();
    uidsString.clear();
    exclude_uidsString.clear();
}

