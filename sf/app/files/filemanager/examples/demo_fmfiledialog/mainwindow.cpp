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
*     
*
*/

#include "mainwindow.h"
#include "fmfiledialog.h"
#include "hbmessagebox.h"

#define getDir "getExistingDirectory"
#define getFile "getOpenFileName"
#define saveFile "getSaveFileName"

#define getDir_SetDir "getExistingDirectory_setDir"
#define getFile_SetDir "getOpenFileName_setDir"
#define saveFile_SetDir "getSaveFileName_setDir"

#define getDir_SetFilter "getExistingDirectory_setFilter"
#define getDir_SetFilter_AllDirs "getExistingDirectory_setFilter_AllDirs"
#define getFile_SetFilter "getOpenFileName_setFilter"
#define saveFile_SetFilter "getSaveFileName_setFilter"

TestMainWindow::TestMainWindow()
{
}
TestMainWindow::~TestMainWindow()
{
}

void TestMainWindow::init()
{
    HbView *mainView = new HbView;
    addView( mainView );

    HbListWidget *listWidget = new HbListWidget( mainView );
    mainView->setWidget( listWidget );

    listWidget->addItem( getDir );
    listWidget->addItem( getFile );
    listWidget->addItem( saveFile );
    listWidget->addItem( " " );
    listWidget->addItem( getDir_SetDir );
    listWidget->addItem( getFile_SetDir );
    listWidget->addItem( saveFile_SetDir );
    listWidget->addItem( " " );
    listWidget->addItem( getDir_SetFilter );
    listWidget->addItem( getDir_SetFilter_AllDirs );
    listWidget->addItem( getFile_SetFilter );
    listWidget->addItem( saveFile_SetFilter );
    listWidget->addItem( " " );
    listWidget->addItem( "placeHolderA" );
    listWidget->addItem( "placeHolderB" );
    listWidget->addItem( "placeHolderC" );
    listWidget->addItem( "placeHolderD" );
    listWidget->addItem( "placeHolderE" );
    listWidget->addItem( "placeHolderF" );
    listWidget->addItem( "placeHolderG" );
    listWidget->addItem( "placeHolderH" );
    listWidget->addItem( "placeHolderI" );
    listWidget->addItem( "placeHolderJ" );
    listWidget->addItem( "placeHolderK" );
    listWidget->addItem( "placeHolderL" );
    listWidget->addItem( "placeHolderM" );
    listWidget->addItem( "placeHolderN" );
    listWidget->addItem( "placeHolderO" );
    listWidget->addItem( "placeHolderP" );
    listWidget->addItem( "placeHolderQ" );
    listWidget->addItem( "placeHolderR" );
    listWidget->addItem( "placeHolderS" );
    listWidget->addItem( "placeHolderT" );
    listWidget->addItem( "placeHolderU" );
    listWidget->addItem( "placeHolderV" );
    listWidget->addItem( "placeHolderW" );
    listWidget->addItem( "placeHolderX" );
    listWidget->addItem( "placeHolderY" );
    listWidget->addItem( "placeHolderZ" );

    connect(listWidget, SIGNAL(activated(HbListWidgetItem*)), this,SLOT(activatedListItem(HbListWidgetItem*)), Qt::QueuedConnection );
}

void TestMainWindow::activatedListItem(HbListWidgetItem *item)
{
    if( item->text().compare( getDir ) == 0 ) {
        getExistingDirectory();
    } else if( item->text().compare( getFile ) == 0 ) {
        getOpenFileName();
    } else if( item->text().compare( saveFile ) == 0 ) {
       getSaveFileName();
    } else if( item->text().compare( getDir_SetDir ) == 0 ) {
        getExistingDirectory_SetDir();
    } else if( item->text().compare( getFile_SetDir ) == 0 ) {
       getOpenFileName_SetDir();
    } else if( item->text().compare( saveFile_SetDir ) == 0 ) {
        getSaveFileName_SetDir();
    } else if( item->text().compare( getDir_SetFilter ) == 0 ) {
        getExistingDirectory_SetFilter();
    } else if( item->text().compare( getDir_SetFilter_AllDirs ) == 0 ) {
        getExistingDirectory_SetFilter_AllDirs();
    } else if( item->text().compare( getFile_SetFilter ) == 0 ) {
        getOpenFileName_SetFilter();
    } else if( item->text().compare( saveFile_SetFilter ) == 0 ) {
       getSaveFileName_SetFilter();
    }
}

void TestMainWindow::getExistingDirectory()
{
    // getExistingDirectory with default parameter.
    QString path = FmFileDialog::getExistingDirectory();
    HbMessageBox::information( path );
}
void TestMainWindow::getOpenFileName()
{
    // getOpenFileName with default parameter.
    QString path = FmFileDialog::getOpenFileName();
    HbMessageBox::information( path );
}
void TestMainWindow::getSaveFileName()
{
    // getSaveFileName with default parameter.
    QString path = FmFileDialog::getSaveFileName();
    HbMessageBox::information( path );
}

void TestMainWindow::getExistingDirectory_SetDir()
{
    // getExistingDirectory and set header & default Directory
    QString path = FmFileDialog::getExistingDirectory( 0, "Look in:", "C:/Data/Images" );
    HbMessageBox::information( path );
}
void TestMainWindow::getOpenFileName_SetDir()
{
    // getOpenFileName and set header & default Directory
    QString path = FmFileDialog::getOpenFileName( 0, "Look in:", "C:/Data/Images" );
    HbMessageBox::information( path );
}
void TestMainWindow::getSaveFileName_SetDir()
{
    // getSaveFileName and set header & default Directory
    QString path = FmFileDialog::getSaveFileName( 0, "Look in:", "C:/Data/Images" );
    HbMessageBox::information( path );
}

void TestMainWindow::getExistingDirectory_SetFilter()
{
    // getExistingDirectory and set folder&file filter
    QStringList filterList;
    filterList<<"*data*"<<"*image*"<<"*sound*";
    QString path = FmFileDialog::getExistingDirectory( 0, "Look in:", "", filterList );
    HbMessageBox::information( path );
}

void TestMainWindow::getExistingDirectory_SetFilter_AllDirs()
{
    // getExistingDirectory and set folder&file filter
    QStringList filterList;
    filterList<<"*data*"<<"*image*"<<"*sound*";
    QString path = FmFileDialog::getExistingDirectory( 0, "Look in:", "", filterList, FmFileDialog::DisplayAllDirs );
    HbMessageBox::information( path );
}

void TestMainWindow::getOpenFileName_SetFilter()
{
    // getOpenFileName and set file filter
    //Set DisplayAllDirs to display all folders, otherwise folders will also be filted.
    QStringList filterList;
    filterList<<"*.jpg"<<"*.gif"<<"*.png";
    QString path = FmFileDialog::getOpenFileName( 0, "Look in:", "", filterList, FmFileDialog::DisplayAllDirs );
    HbMessageBox::information( path );
}
void TestMainWindow::getSaveFileName_SetFilter()
{
    // getSaveFileName and set file filter
    //Set DisplayAllDirs to display all folders, otherwise folders will also be filted.
    QStringList filterList;
    filterList<<"*.jpg"<<"*.gif"<<"*.png";
    QString path = FmFileDialog::getSaveFileName( 0, "Look in:", "", filterList, FmFileDialog::DisplayAllDirs );
    HbMessageBox::information( path );
}
