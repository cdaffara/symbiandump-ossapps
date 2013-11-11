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
 *     The source file of the query sdialog utility of file manager
 */

#include "fmdlgutils.h"
#include "fmcombinedquery.h"
#include "fmtimequery.h"
#include "fmsingletextquery.h"
#include "fmmultitextquery.h"
#include "fmutils.h"
#include "fmviewmanager.h"
#include "fmmessagebox.h"
#include "fmdrivequery.h"

#include <QString>
#include <QStringList>
#include <QMap>

#include <hblabel.h>
#include <hbaction.h>

/*
    Private Constructor
*/
FmDlgUtils::FmDlgUtils()
{
}

/*!
    Destructor
*/
FmDlgUtils::~FmDlgUtils()
{
}

/*!
    Shows single select setting query dialog
    \a title used for title text
    \a textList used for text list for available settings
    \a selectedIndex used for storing selected index
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    return true if selection made, otherwise false
*/
bool FmDlgUtils::showSingleSettingQuery(
        const QString &title,
        const QStringList &textList,
        int &selectedIndex, const QString &associatedDrives )
{
   bool ret( false );

    int i = 0;
    QMap< int, QString > items;   // Maps list item with corresponding text in viewer
    for( QStringList::const_iterator it = textList.begin(); it!= textList.end(); ++it ){
        items.insert( i, *it );
        ++i;
    }
    bool ok ( false );                // SK return (out parameter)

    FmCombinedQuery *cQuery=new FmCombinedQuery;
    cQuery->setSelectMode( FmCombinedQuery::SingleSelectMode );
    cQuery->setHeadingWidget( new HbLabel( title ) );
    cQuery->setListItems( items );

    if( selectedIndex != -1 ){
	cQuery->setMarkedListItem( selectedIndex );
    }

    QString sk1 ( hbTrId( "txt_common_button_ok" ) );
    QString sk2 ( hbTrId("txt_common_button_cancel" ) );

    if( !sk1.isEmpty() ) {
        HbAction *primary = new HbAction( sk1 );
        cQuery->setPrimaryAction( primary );
    }

    if( !sk2.isEmpty() ) {
        HbAction *secondary = new HbAction( sk2 );
        cQuery->setSecondaryAction( secondary );
    }

    //HbAction* action = cQuery->exec();
	HbAction* action = executeDialog( cQuery, associatedDrives );
    if( action == cQuery->primaryAction() ){
        ok = true;
    }
    else{
        ok = false;
    }
    if( ok ){
        //get selected key list.
        QList<int> selectedKeyList = cQuery->selectedKeys();

        // Get new selection and notify changes
        int selectionResult( 0 );
        int i = 0;
        for ( QMap< int, QString >::const_iterator it = items.begin(); it != items.end(); ++it ){
            // if one item has been selected( judge if the key is in the selected key list)
            if ( selectedKeyList.indexOf( it.key() ) >= 0 ) {
                selectionResult = i;
				break;
            }
            ++i;
        }
        if ( selectionResult != selectedIndex ){
            selectedIndex = selectionResult;
            ret = true;
        }
    }
    delete cQuery;
    return ret;
}

/*!
    Shows multi select setting query dialog
    \a title used for title text
    \a textList used for text list for available settings
    \a selection used for storing selected indexes as bitmask
    \a dominantIndex used for dominant index for select all behaviour
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    return true if selection made, otherwise false
*/
bool FmDlgUtils::showMultiSettingQuery(
        const QString &title,
        const QStringList &textList,
        quint32 &selection,
        int dominantIndex, const QString &associatedDrives )
{
    bool ret( false );

    int i = 0;
    QMap<int, QString> items;   // Maps list item with corresponding text in viewer
    for( QStringList::const_iterator it = textList.begin(); it!= textList.end(); ++it){
        items.insert( i, *it );
        ++i;
    }
    bool ok ( false );                // SK return (out parameter)

    FmCombinedQuery *cQuery=new FmCombinedQuery;
    cQuery->setSelectMode( FmCombinedQuery::MultiSelectMode );
    cQuery->setHeadingWidget( new HbLabel( title ) );
    cQuery->setListItems( items );
	cQuery->setDominantIndex( dominantIndex );

    // Create selection array
    int count( textList.count() );
    for ( int i( 0 ); i < count; ++i ){
        if ( selection & ( 1 << i ) ){
            cQuery->setMarkedListItem( i );
        }
    }


    QString sk1 ( hbTrId( "txt_common_button_ok" ) );
    QString sk2 ( hbTrId( "txt_common_button_cancel" ) );

    if( !sk1.isEmpty() ) {
        HbAction *primary = new HbAction( sk1 );
        cQuery->setPrimaryAction( primary );
    }

    if( !sk2.isEmpty() ) {
        HbAction *secondary = new HbAction( sk2 );
        cQuery->setSecondaryAction( secondary );
    }

    HbAction* action = executeDialog( cQuery, associatedDrives );
    if( action == cQuery->primaryAction() ){
        ok = true;
    }
    else{
        ok = false;
    }
    if( ok )
    {
        //get selected key list.
        QList<int> selectedKeyList = cQuery->selectedKeys();

        // Get new selection and notify changes
        quint32 selectionResult( 0 );
        int i = 0;
        for ( QMap< int, QString >::const_iterator it = items.begin(); it != items.end(); ++it ){
            // if one item has been selected( judge if the key is in the selected key list)
            if ( selectedKeyList.indexOf( it.key() ) >= 0 ) {
                selectionResult |= 1 << i;
            }
            ++i;
        }
        if ( selectionResult != selection ){
            selection = selectionResult;
            ret = true;
        }
    }
    delete cQuery;
    return ret;
}

/*!
    Shows time setting query dialog
    \a title used for title text
    \a time used for selected time
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    return true if selection made, otherwise false
*/
bool FmDlgUtils::showTimeSettingQuery(
        const QString &title, QTime &time, const QString &associatedDrives )
{
    bool ret( false );
    bool ok ( false );                // SK return (out parameter)

    FmTimeQuery *cQuery=new FmTimeQuery;
    cQuery->setHeadingWidget( new HbLabel( title ) );
    cQuery->setTime( time );

    QString sk1 ( hbTrId( "txt_common_button_ok" ) );
    QString sk2 ( hbTrId( "txt_common_button_cancel" ) );

    if( !sk1.isEmpty() ) {
        HbAction *primary = new HbAction( sk1 );
        cQuery->setPrimaryAction( primary );
    }

    if( !sk2.isEmpty() ) {
        HbAction *secondary = new HbAction( sk2 );
        cQuery->setSecondaryAction( secondary );
    }

	HbAction* action = executeDialog( cQuery, associatedDrives );
    if( action == cQuery->primaryAction() ){
        ok = true;
    }
    else{
        ok = false;
    }
    if( ok )
    {
        QTime resultTime = cQuery->getTime();
        if ( resultTime != time ){
            time = resultTime;
            ret = true;
        }
    }
    delete cQuery;
    return ret;
}

/*!
    Shows text query dialog
    \a title used for title text
    \a text used for receiving user input text
    \a validRegExpStringList used for set valid regExp string list
    \a maxLength used for setting max length of input text
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    Set \a isReturnFalseWhenNoTextChanged true if need make return value as true while no text changed
    return true if text change is made, otherwise false
*/
bool FmDlgUtils::showTextQuery(
        const QString& title, QString& text, QStringList validRegExpStringList,
		int maxLength, const QString &associatedDrives, bool isReturnFalseWhenNoTextChanged )
{
    bool ret( false );
    bool ok ( true );                // SK return (out parameter)

    FmSingleTextQuery *cQuery = new FmSingleTextQuery();
    if( maxLength != -1 ){
        cQuery->setLineEditMaxLength( maxLength );
    }
    cQuery->setHeadingWidget( new HbLabel( title ) );

	cQuery->setRegExpStringList( validRegExpStringList );

    QString sk1 ( hbTrId ( "txt_common_button_ok" ) );
    QString sk2 ( hbTrId ( "txt_common_button_cancel" ) );

    if( !sk1.isEmpty() ) {
        HbAction *primary = new HbAction( sk1 );
        cQuery->setPrimaryAction( primary );
    }

    if( !sk2.isEmpty() ) {
        HbAction *secondary = new HbAction( sk2 );
        cQuery->setSecondaryAction( secondary );
    }
    cQuery->setLineEditText( text );
    cQuery->checkActions();
    
	HbAction* action = executeDialog( cQuery, associatedDrives );
    if( action == cQuery->primaryAction() ){
        ok = true;
    }
    else{
        ok = false;
    }

    if( ok ){
        QString newName = cQuery->getLineEditText();
        if ( newName == text ){
            if( isReturnFalseWhenNoTextChanged ) {
                ret = false;
            }
            else {
                ret = true;
            }
        } else {
            text = newName;
            ret = true;
        }
    }
    delete cQuery;
    return ret;
}

/*!
    Shows single-line password query dialog
    \a title used for title text
    \a pwd used for receiving user input password
    \a maxLength used for setting max length of input password
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    return true if password text is inputted, otherwise false
*/
bool FmDlgUtils::showSinglePasswordQuery( const QString &title, QString &pwd, int maxLength, const QString &associatedDrives )
{
    bool ret( false );

    FmSingleTextQuery *cQuery = new FmSingleTextQuery( HbLineEdit::Password );
    if( maxLength != -1 ){
        cQuery->setLineEditMaxLength( maxLength );
    }
    cQuery->setHeadingWidget( new HbLabel( title ) );
    
    // validate that length is not zero. space is acceptable character.
    cQuery->setRegExpStringList( QStringList( Regex_ValidUnZeroLength ) );
    QString sk1 ( hbTrId("txt_common_button_ok" ) );
    QString sk2 ( hbTrId("txt_common_button_cancel" ) );

    if( !sk1.isEmpty() ) {
        HbAction *primary = new HbAction( sk1 );
        cQuery->setPrimaryAction( primary );
    }

    if( !sk2.isEmpty() ) {
        HbAction *secondary = new HbAction( sk2 );
        cQuery->setSecondaryAction( secondary );
    }
    cQuery->checkActions();

    HbAction* action = executeDialog( cQuery, associatedDrives );
    if( action == cQuery->primaryAction() ){
        ret = true;
        pwd = cQuery->getLineEditText();
    }
    else{
        ret = false;
    }

    delete cQuery;
    return ret;
}

/*!
    Shows mulit-line password query dialog. If two password is not equal, primary action is dimmed
    \a firstLabel used for first line title text
    \a secondLabel used for second line title text
    \a pwd used for receiving user input password
    \a maxLength used for setting max length of input password
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    return true if password text is inputted, otherwise false
*/
bool FmDlgUtils::showMultiPasswordQuery(     
   const QString &firstLabel, const QString &secondLabel, QString &pwd, int maxLength, const QString &associatedDrives )
{
    bool ret( false );

    FmMultiTextQuery *cQuery = new FmMultiTextQuery( HbLineEdit::Password );
    if( maxLength != -1 ){
        cQuery->setLineEditMaxLength( maxLength );
    }
    cQuery->setFirstLabelText( firstLabel );
    cQuery->setSecondLabelText( secondLabel );

    QString sk1 = ( hbTrId( "txt_common_button_ok" ) );
    QString sk2 = ( hbTrId( "txt_common_button_cancel" ) );

    if( !sk1.isEmpty() ) {
        HbAction *primary = new HbAction( sk1 );
        cQuery->setPrimaryAction( primary );
    }

    if( !sk2.isEmpty() ) {
        HbAction *secondary = new HbAction( sk2 );
        cQuery->setSecondaryAction( secondary );
    }
    cQuery->checkActions();
    
	HbAction* action = executeDialog( cQuery, associatedDrives );
    if( action == cQuery->primaryAction() ){
        QString firstPwd = cQuery->getFirstEditText();
        QString secondPwd = cQuery->getSecondEditText();
        if ( firstPwd == secondPwd ){
            pwd = secondPwd;
            ret = true;
        }
    }
    else{
        ret = false;
    }

    delete cQuery;
    return ret;
}

/*!
    This is a convenience function for showing a question dialog with \a questionText
    Default buttons are Yes and No. 
*/
bool FmDlgUtils::question( const QString &questionText,
    HbMessageBox::StandardButtons buttons )
{
    FmMessageBox msgBox;
    return msgBox.question( questionText, buttons );
}

/*!
    This is a convenience function for showing an information dialog with \a informationText
    Default button is OK, default behavior is not blocking ui..
*/
void FmDlgUtils::information( const QString &informationText,
    HbMessageBox::StandardButtons buttons, bool isBlocking )
{
    FmMessageBox msgBox;
    return msgBox.information( informationText, buttons, isBlocking );
}

/*!
    This is a convenience function for showing an warning dialog with \a warningText
    Default button is OK, default behavior is blocking ui.
*/
void FmDlgUtils::warning( const QString &warningText,
    HbMessageBox::StandardButtons buttons, bool isBlocking )
{
    FmMessageBox msgBox;
    return msgBox.warning( warningText, buttons, isBlocking );
}

/*!
    Shows query dialog for select backup drive
    \a title used for title text
    only backup target drives will be shown.
    return selected drive, and empty string for cancel.
*/
QString FmDlgUtils::showBackupDriveQuery( const QString& title )
{
    QString ret;

    FmDriveQuery *cQuery = new FmDriveQuery();
    cQuery->setHeadingWidget( new HbLabel( title ) );
    
    QString sk = ( hbTrId( "txt_common_button_cancel" ) );
    
    HbAction *primary = new HbAction( sk );
    cQuery->setPrimaryAction( primary );

    executeDialog( cQuery, QString() );
    ret = cQuery->selectedDrive();
    
    delete cQuery;
    return ret;
}

/*
    Private function used to popup \a dialog 
    If want popup dialog in class FmDlgUtils, please call this function.
    Dialog will be closed as canceled while drive in \a associatedDrives is removed
    return selected action.
*/
HbAction *FmDlgUtils::executeDialog( FmDialog *dialog, const QString &associatedDrives )
{
    for( int i = 0; i < associatedDrives.length(); i++ ) {
        QString drive( associatedDrives[i] + QString( ":/" ) );
        if( !FmUtils::isDriveAvailable( drive ) ) {
            FM_LOG( "executeDialog return 0_ " + associatedDrives );
            return 0;
        }
    }
    
    FmDlgCloseUnit dlgCloseUnit( dialog );
    dlgCloseUnit.addAssociatedDrives( associatedDrives );

    FmViewManager::viewManager()->addDlgCloseUnit( &dlgCloseUnit );
    HbAction* action = dialog->exec();
    FmViewManager::viewManager()->removeDlgCloseUnit( &dlgCloseUnit );
    return action;
}
