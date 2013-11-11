/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
*  Name     : BookmarkDumpMain.cpp
*  Part of  : Bookmark Engine - Bookmark Dump Utility
*
*  Description:
*      Entry point and main function for Bookmark Dump utility application.
*      
*
*/


// INCLUDE FILES

//#include "wrtsettings.h"
#include "bookmarks.h"
#include "xbel.h"

#ifdef Q_OS_SYMBIAN

#include <bacline.h>
#include <FavouritesDb.h>
#include <FavouritesLimits.h>
#include <FavouritesItemList.h>
#include "UnicodeFile.h"


// CONSTANTS
 
/// Formatting buffer size. Longest data is URL (plus format stuff).
LOCAL_D const TInt KFormatBufLen = KFavouritesMaxUrl + 20;

// ================= LOCAL FUNCTIONS =======================
static TInt MainImportL();

/**
* Main function. Process command line arguments and export bookmark data.
* We ignore errors in parsing.
*/
_LIT(KDataFilePath, "c:\\data\\bookmarks.txt"); 
int MainImport()
{
	TInt error = 0;
	TRAP(error, MainImportL());
	
	return (int)error;
}
TInt MainImportL()
    {
     
    TBuf<KFormatBufLen> buf;
    TInt num;
   
    RFs fs;
    User::LeaveIfError( fs.Connect() ); 
    //CleanupClosePushL<RFs>( fs );
    
    RUnicodeFile file;
    User::LeaveIfError( file.ReplaceL( fs, KDataFilePath, EFileWrite | EFileShareExclusive ) );
    //CleanupClosePushL<RUnicodeFile>( file );
        
    RFavouritesSession sess;
    User::LeaveIfError( sess.Connect() );
    //CleanupClosePushL<RFavouritesSession>( sess );
   
    RFavouritesDb db;
    User::LeaveIfError( db.Open( sess, KBrowserBookmarks ) );
    //CleanupClosePushL<RFavouritesDb>( db );
    
    User::LeaveIfError( db.Begin( /*aWrite=*/EFalse ) );
    //db.CleanupRollbackPushL();
    
    CFavouritesItemList* items = new (ELeave) CFavouritesItemList();
    //CleanupStack::PushL( items );
    
    const CFavouritesItem* item;
    User::LeaveIfError( db.GetAll( *items ) );
    buf.Format( _L("%d entries\r\n"), items->Count() );
   
    TInt counter = items->Count();
    
    User::LeaveIfError( file.Write( buf ) );
    for ( TInt i = 0; i < items->Count(); i++ )
        {
        User::LeaveIfError( file.Write( _L("\r\n") ) );
        item = items->At( i );
        // Uid.
        num = item->Uid();
        buf.Format( _L("Uid=(%d)"), num );
        User::LeaveIfError( file.Write( buf ) );
        // Fixed Uid info.
        if ( num == KFavouritesRootUid )
            {
            User::LeaveIfError( file.Write( _L(" (root folder)\r\n") ) );
            }
        else if ( num == KFavouritesHomepageUid )
            {
            User::LeaveIfError( file.Write( _L(" (homepage)\r\n") ) );
            }
        else if ( num == KFavouritesLastVisitedUid )
            {
            User::LeaveIfError( file.Write( _L(" (last visited)\r\n") ) );
            }
        else
            {
            User::LeaveIfError( file.Write( _L("\r\n") ) );
            }
        // Type.
        if ( item->Type() == CFavouritesItem::EFolder )
            {
            User::LeaveIfError( file.Write( _L("Type=Folder\r\n") ) );
            }
        else if ( item->Type() == CFavouritesItem::EItem )
            {
            User::LeaveIfError( file.Write( _L("Type=Item\r\n") ) );
            }
        else
            {
            User::LeaveIfError( file.Write( _L("Type=HUH???\r\n") ) );
            }
        TPtrC ptr;
        // Name
        ptr.Set( item->Name() );
        buf.Format( _L("Name=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Parent.
        buf.Format( _L("ParentFolder=(%d)\r\n"), item->ParentFolder() );
        User::LeaveIfError( file.Write( buf ) );
        // Url.
        ptr.Set( item->Url() );
        buf.Format( _L("Url=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Access Point.
        if ( item->WapAp().IsNull() )
            {
            User::LeaveIfError( file.Write( _L("WapAp=Null\r\n") ) );
            }
        else if ( item->WapAp().IsDefault() )
            {
            User::LeaveIfError( file.Write( _L("WapAp=Default\r\n") ) );
            }
        else
            {
            buf.Format( _L("WapAp=(%d)\r\n"), item->WapAp().ApId() );
            User::LeaveIfError( file.Write( buf ) );
            }
        // UserName.
        ptr.Set( item->UserName() );
        buf.Format( _L("UserName=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Password.
        ptr.Set( item->Password() );
        buf.Format( _L("Password=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Read-only.
        buf.Format( _L("ReadOnly=(%d)\r\n"), item->IsReadOnly() );
        User::LeaveIfError( file.Write( buf ) );
        // Factory item.
        buf.Format( _L("FactoryItem=(%d)\r\n"), item->IsFactoryItem() );
        User::LeaveIfError( file.Write( buf ) );
        // ContextId.
        buf.Format( _L("ContextId=(0x%x)\r\n"), item->ContextId() );
        User::LeaveIfError( file.Write( buf ) );
        // Modified.
        item->Modified().FormatL
            ( buf, _L("Modified=<%F%Y/%M/%D %H:%T:%S>\r\n") );
        User::LeaveIfError( file.Write( buf ) );
        // Preferred uid.
        if ( item->Type() == CFavouritesItem::EFolder )
            {
            User::LeaveIfError( db.PreferredUid( item->Uid(), num ) );
            buf.Format( _L("PreferredUid=(%d)\r\n"), num );
            User::LeaveIfError( file.Write( buf ) );
            }
        }
   
    //CleanupStack::PopAndDestroy();      // items
    User::LeaveIfError( db.Commit() );
   // CleanupStack::Pop();                // Pop the rollback
    //CleanupStack::PopAndDestroy( 5 );   // sess, db, file, fs, args
    	
    db.Close();
    
    file.Close();
    fs.Close();
    	
    return 0;
}

#endif