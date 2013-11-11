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
* Description:  Main.
*
*/

#include <QTranslator>
#include <QMessageBox>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include "hshomescreen.h"
#include "hstest_global.h"

namespace
{
    const char TR_FILE_PATH[] = "resource/qt/translations";
}

#ifdef Q_OS_SYMBIAN

#include <QFile>
#include <QDir>
#include <f32file.h>
#include <eikenv.h>
#include <e32debug.h>
#include <apgwgnam.h>

#include "hsrecoverymanager.h"

void loadTranslationFilesOnSymbian(QTranslator &commonTranslator, 
                                    QTranslator &hsTranslator, 
                                    QTranslator &alTranslator);
void copyWallpapersFromRom();
void copyHsDatabaseFileFromRom();
void createPrivateFolder();
void copyWallpaperOriginalsFromRomToPhotos();

/*!
    \fn setHomescreenAsSystemAppL(CEikonEnv* eikonEnv)
    \ingroup group_hsapplication
    \internal
    \brief TODO
*/
void setHomescreenAsSystemAppL(CEikonEnv* eikonEnv)
{
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC(eikonEnv->WsSession());
    wgName->SetSystem(ETrue); // Allow only application with PowerManagement cap to shut us down
    wgName->SetCaptionL(_L("hsapplication"));
    wgName->SetAppUid(TUid::Uid(0x20022F35));
    wgName->SetWindowGroupName(eikonEnv->RootWin());
    CleanupStack::PopAndDestroy();
}

/*!
    \fn myMessageOutput(QtMsgType type, const char *msg)
    \ingroup group_hsapplication
    \internal    
    \brief TODO
*/
void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {

    case QtDebugMsg:
        RDebug::Printf("[HS] Debug: %s\n", msg);
        break;

    case QtWarningMsg:
        RDebug::Printf("[HS] Warning: %s\n", msg);
        break;

    case QtCriticalMsg:
        RDebug::Printf("[HS] Critical: %s\n", msg);
        break;

    case QtFatalMsg:
        RDebug::Printf("[HS] Fatal: %s\n", msg);
        abort();
    }
}

#endif //Q_OS_SYMBIAN

/*!
    \fn main(int argc, char *argv[])
    \ingroup group_hsapplication
    \internal    
    \brief TODO
*/
int main(int argc, char *argv[])
{
#ifdef Q_OS_SYMBIAN
     qInstallMsgHandler(myMessageOutput);
#endif

    HSTEST_FUNC_ENTRY("HS::main()");

    HbApplication app(argc, argv, Hb::NoSplash);

#ifdef Q_OS_SYMBIAN
    QString currentPath = QString(argv[0]).left(2);
    QDir::setCurrent(currentPath);
    QString debugMsg = "HS::main() - HsApplication's current dir set to: " + currentPath;
    HSDEBUG(debugMsg);
#endif //Q_OS_SYMBIAN

    // add translator for homescreen and application library
    QTranslator hsTranslator;
    QTranslator alTranslator;
    QTranslator commonTranslator;
#ifdef Q_OS_SYMBIAN
    // TRAP is must here, otherwise it crashes
    TRAP_IGNORE( loadTranslationFilesOnSymbian(commonTranslator, hsTranslator, alTranslator) );
#else
    QString locale = QLocale::system().name();
    QString hsTrFile = QString("homescreen_") + locale;
    hsTranslator.load(hsTrFile, QString(TR_FILE_PATH) );
    QString alTrFile = QString("hsapplibrary_") + locale;
    alTranslator.load(alTrFile, QString(TR_FILE_PATH) );
#endif //Q_OS_SYMBIAN
    qApp->installTranslator(&commonTranslator);
    qApp->installTranslator(&hsTranslator);
    qApp->installTranslator(&alTranslator);
    
#ifdef Q_OS_SYMBIAN
    copyHsDatabaseFileFromRom();
    HsRecoveryManager recoveryManager;
    recoveryManager.execute();
    copyWallpapersFromRom();
#endif
    
    HbMainWindow window;
    window.setRenderHint(QPainter::SmoothPixmapTransform);
    window.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        
    HsHomeScreen hs;
    QObject::connect(&app, SIGNAL(aboutToQuit()) ,&hs, SLOT(stop()));
    QObject::connect(&hs, SIGNAL(exit()), &app, SLOT(quit()),Qt::QueuedConnection);
    hs.start();

#ifdef Q_OS_SYMBIAN
    CEikonEnv * env = CEikonEnv::Static();
    TRAP_IGNORE(setHomescreenAsSystemAppL(env));
#endif //Q_OS_SYMBIAN

    HSTEST("HS::main() call app.exec");
    try {
        app.exec();
    } catch(...) {
        HSTEST("HS::main() app.exec() catch");
    }
    return 0;
}

#ifdef Q_OS_SYMBIAN

/*!
    \fn createPrivateFolder()
    \ingroup group_hsapplication
    \internal    
    \brief Private folder creator
    Creates private folder to c: drive if not exists yet.
*/
void createPrivateFolder()
{
    QDir dir("c:/private/20022f35/");
    if(!dir.exists()) {
        HSTEST("HS::main() - c:/private/20022f35/ does not exist.");
        RFs fsSession;
        if(fsSession.Connect() != KErrNone){
            HSTEST("HS::main() - Homescreen private dir creation failed!");
        }
        fsSession.CreatePrivatePath(EDriveC);
        HSTEST("HS::main() - c:/private/20022f35/ created.");
        fsSession.Close();
        if(!dir.exists()) {
            HSTEST("HS::main() - c:/private/20022f35/ creation failed.");
        }
    }
    HSTEST("HS::main() - c:/private/20022f35/ exists.");
}


/*!
    \fn copyHsDatabaseFileFromRom()
    \ingroup group_hsapplication
    \internal    
    \brief Database copier
    Copies homescreen database from rom if database does not exist on c: drive
*/
void copyHsDatabaseFileFromRom()
{
    QFile file("c:/private/20022f35/homescreen.db");
    //If DB file does not exist copy default DB from z: (ROM)
    if(!file.exists()) {
        HSTEST("HS::main() - homescreen.db not in c:");
        file.setFileName("z:/private/20022f35/homescreen.db");
        if(!file.exists()) {
            qWarning() << "HS::main() - homescreen.db not in ROM!";
        } else {
            HSTEST("HS::main() - homescreen.db found from z:");
            createPrivateFolder();
            //Copy DB from z: to c:
            if (!file.copy("c:/private/20022f35/homescreen.db")) {
                HSTEST("HS::main() - homescreen.db copy from ROM to c: failed!");
            }
            HSTEST("HS::main() - homescreen.db copied from ROM to c:!");
            file.setFileName("c:/private/20022f35/homescreen.db");
            if(!file.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
                qWarning() << "HS::main() - homescreen.db ReadWrite permission settings on c: failed!";
            }
            HSTEST("HS::main() - homescreen.db permission set to ReadWrite!");
        }
        // Copy wallpaper originals to C drive user data to be visible in Photos
        copyWallpaperOriginalsFromRomToPhotos();
    }
#ifdef __WINS__
    else if(!file.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
        HSTEST("HS::main() - homescreen.db ReadWrite permission settings on emulator's c: failed!");
    }
#endif
    
}

/*!
    \fn copyWallpapersFromRom()
    \ingroup group_hsapplication
    \internal    
    \brief Wallpaper copier
    Copies homescreen wallpapers from rom if wallpaper directory
    does not exist on c: drive
*/
void copyWallpapersFromRom()
{
    {
        QDir dir("c:/private/20022f35/wallpapers/scene");
        if(!dir.exists()) {
            dir.mkpath("c:/private/20022f35/wallpapers/scene");
            QDir dir2("z:/private/20022f35/wallpapers/scene");
            dir2.setFilter(QDir::Files);
            QStringList files = dir2.entryList();
            foreach(QString file, files)
            {
                QString targetFile("c:/private/20022f35/wallpapers/scene/" + file);
                if ( QFile::copy(dir2.absoluteFilePath(file), targetFile) ) {
                    qDebug() << "image copied" << dir2.absoluteFilePath(file);
                } 
                QFile createdFile(targetFile);
                if(!createdFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
                    qDebug() << "read write permission set failed for file" << targetFile;
                }
            }
        }
    }
    {
        QDir dir("c:/private/20022f35/wallpapers/page");
        if(!dir.exists()) {
            dir.mkpath("c:/private/20022f35/wallpapers/page");
            QDir dir2("z:/private/20022f35/wallpapers/page");
            dir2.setFilter(QDir::Files);
            QStringList files = dir2.entryList();
            foreach(QString file, files)
            {
                QString targetFile("c:/private/20022f35/wallpapers/page/" + file);
                if ( QFile::copy(dir2.absoluteFilePath(file), targetFile) ) {
                    qDebug() << "image copied" << dir2.absoluteFilePath(file);
                } 
                QFile createdFile(targetFile);
                if(!createdFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
                    qDebug() << "read write permission set failed for file" << targetFile;
                }
            }
        }
    }
}

/*!
    \fn copyWallpaperOriginalFromRomToPhotos()
    \ingroup group_hsapplication
    \internal    
    \brief Wallpaper original copier
    Copies homescreen wallpaper originals from rom to C for Photos
    does not exist on c: drive
*/
void copyWallpaperOriginalsFromRomToPhotos()
{
    QString targetPath("e:/data/images/wallpapers");
    QDir dir(targetPath);
    if(!dir.exists()) {
        dir.mkpath(targetPath);    
        QDir dir2("z:/private/20022f35/wallpapers/originals");
        dir2.setFilter(QDir::Files);
        QStringList files = dir2.entryList();
        foreach(QString file, files) {
            QString targetFile(targetPath + "/" + file);
            if ( QFile::copy(dir2.absoluteFilePath(file), targetFile) ) {
                qDebug() << "Wallpaper original copied from " << dir2.absoluteFilePath(file) << "to " << targetFile;
            } 
            QFile createdFile(targetFile);
            if(!createdFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
                qDebug() << "Read write permission set failed for wallpaper original" << targetFile;
            }
        }
    }
}

/*!
    \fn loadTranslationFilesOnSymbian(QTranslator &commonTranslator, 
                                      QTranslator &hsTranslator, 
                                      QTranslator &alTranslator)
    \ingroup group_hsapplication
    \internal    
    \brief translation loader
    Load translation files.
*/
void loadTranslationFilesOnSymbian(QTranslator &commonTranslator, 
                                   QTranslator &hsTranslator, 
                                   QTranslator &alTranslator)
{
    QString locale = QLocale::system().name();
    bool commonLoaded(false);
    QString commonTrFile = QString("common_") + locale;
    commonLoaded = commonTranslator.load(commonTrFile, QString("z:/") + TR_FILE_PATH);
    if (!commonLoaded) {
        commonTranslator.load(commonTrFile, QString("c:/") + TR_FILE_PATH);
    }
    bool hsLoaded(false);
    QString hsTrFile = QString("homescreen_") + locale;
    hsLoaded = hsTranslator.load(hsTrFile, QString("z:/") + TR_FILE_PATH);
    if (!hsLoaded) {
        hsTranslator.load(hsTrFile, QString("c:/") + TR_FILE_PATH);
    }

    bool alLoaded(false);
    QString alTrFile = QString("hsapplibrary_") + locale;
    alLoaded = alTranslator.load(alTrFile, QString("z:/") + TR_FILE_PATH);
    if (!alLoaded) {
        alTranslator.load(alTrFile, QString("c:/") + TR_FILE_PATH);
    }
    
}

#endif
