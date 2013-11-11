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
* Description: filemanager file icon provider source file 
*
*/

#include "fmfileiconprovider.h"
#include "fmfiletyperecognizer.h"
#include "fmutils.h"

#include <QDir>
#include <QFileInfo>

#include "hbicon.h"

//#define mmcIcon         ":image/qgn_prop_mmc_memc.svg"
//#define mmcNoneIcon     ":image/qgn_prop_fmgr_mmc_no_sub.svg"
//#define mmcLockedIcon   ":image/qgn_prop_mmc_locked.svg"
//#define phoneMemoryIcon ":image/qgn_prop_phone_memc.svg"
//#define usbMemoryIcon   ":image/qgn_prop_usb_memc.svg"
//#define massMemoryIcon  ":image/qgn_prop_fmgr_ms.svg"

#define phoneMemoryIcon "qtg_large_mobile"
#define massMemoryIcon  "qtg_large_mass_storage"
#define mmcIcon         "qtg_large_mmc"
#define mmcNoneIcon     "qtg_large_mmc_removed"
#define usbMemoryIcon   "qtg_large_usb_memory"

//TODO: UI_Update: mmcl locked icon have not provided in icon spec
// Just use original custom-defined icon
#define mmcLockedIcon   ":image/qgn_prop_mmc_locked.svg"


#define folderIcon      "qtg_large_folder"

#define imageIcon       "qtg_large_photos" 
#define videoIcon       "qtg_large_video" 
#define toneIcon        "qtg_large_tone"
#define playlistIcon    "qtg_large_playlist" 
#define textIcon        "qtg_large_text" 
#define sisxIcon        "qtg_large_sisx" 
#define javaIcon        "qtg_large_java" 
#define flashIcon       "qtg_large_flash" 
#define widgetIcon      "qtg_large_widget" 
#define weblinkIcon     "qtg_large_web_link" 
#define queryIcon       "qtg_large_query"

FmFileIconProvider::FmFileIconProvider()
{
    mFileTypeRecognizer = new FmFileTypeRecognizer();
}

FmFileIconProvider::~FmFileIconProvider()
{
    delete mFileTypeRecognizer;
}
        
QIcon FmFileIconProvider::icon(const QFileInfo &info) const
{
    QIcon retIcon;
    
    QString filePath( info.absoluteFilePath() );
    FmFileTypeRecognizer::FileType fileType = mFileTypeRecognizer->getType( filePath );
    switch( fileType )
        {
        case FmFileTypeRecognizer::FileTypeDrive:
            {
            FmDriverInfo driveInfo = FmUtils::queryDriverInfo( filePath );
            switch ( driveInfo.driveType() )
                {
                case FmDriverInfo::EDriveTypeMassStorage:
                    retIcon = HbIcon( massMemoryIcon ).qicon();
                    break;
                case FmDriverInfo::EDriveTypeUsbMemory:
                    retIcon = HbIcon( usbMemoryIcon ).qicon();
                    break;
                case FmDriverInfo::EDriveTypeMemoryCard:
                    if( !( driveInfo.driveState() & FmDriverInfo::EDriveNotPresent ) ) {
                        retIcon = HbIcon( mmcIcon ).qicon();
                    } else {
                        retIcon = HbIcon( mmcNoneIcon ).qicon();    
                    }
                    break;
                case FmDriverInfo::EDriveTypePhoneMemory:
                    retIcon = HbIcon( phoneMemoryIcon ).qicon();
                    break;
                case FmDriverInfo::EDriveTypeNotExist: // drive not exist, let QFileIconProvider to judge icon
                default:  // filemanger do not provide icon for other drive types, let QFileIconProvider to judge icon
                    break;
               }
            break;
            }
        case FmFileTypeRecognizer::FileTypeFolder:
            {
            retIcon = HbIcon( folderIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeImage:
            {
            retIcon = HbIcon( imageIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeVideo:
            {
            retIcon = HbIcon( videoIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeTone:
            {
            retIcon = HbIcon( toneIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypePlaylist:
            {
            retIcon = HbIcon( playlistIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeText:
            {
            retIcon = HbIcon( textIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeSisx:
            {
            retIcon = HbIcon( sisxIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeJava:
            {
            retIcon = HbIcon( javaIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeFlash:
            {
            retIcon = HbIcon( flashIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeWidget:
            {
            retIcon = HbIcon( widgetIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeWebLink:
            {
            retIcon = HbIcon( weblinkIcon ).qicon();
            break;
            }
        case FmFileTypeRecognizer::FileTypeUnKnown:
            {
            // currently filemanger icon doc is not unified with icon name.
            // for example, qtg_small_unknown  qtg_large_query both means for unknown type
			// but the two icon name is not the same.
            retIcon = HbIcon( queryIcon ).qicon();
            break;
            }
        }
    
    // if cannot get icon, return icon from QFileIconProvider
    if( retIcon.isNull() ) {
        retIcon = QFileIconProvider::icon( info );
    }
    return retIcon;
}
