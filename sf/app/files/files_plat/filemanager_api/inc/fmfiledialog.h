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
*     The header file of the file dialog
*
*/

#ifndef FMFILEDIALOG_H
#define FMFILEDIALOG_H

#include "fmglobal.h"


#include <hbdialog.h>

class QString;
class QStringList;
class FmFileDialogPrivate;

class FMFILEDIALOG_EXPORT FmFileDialog : public HbDialog
{
Q_OBJECT
public:

    enum Option
    {
        /* 
         * Open dir as dialog's working directory and prevent user up to parent directory
         */
        PreventDirUp    = 0x00000001,
        /* 
         * Ignore filter for dir, list all directory. Set this when need only filter filename.
         */
        DisplayAllDirs  = 0x00000002
    };
    Q_DECLARE_FLAGS(Options, Option)

    /**
     * Static function that will return an existing directory selected by the user
     * @param parent            Parent widget
     * @param title             Title for dialog
     * @param dir               Dialog's working directory is set to dir
     * @param nameFilters       used for filter folder
     * @param Options           Options
     * @return Directory path that user selected. 
     *         user can select cancel to close dialog and empty string will be returned
     */
    static QString getExistingDirectory( HbWidget *parent = 0,
                                        const QString &title = QString(),
                                        const QString &dir = QString(),
                                        const QStringList &nameFilters = QStringList(),
                                        Options options = 0 );

    /**
     * Static function that returns an existing file selected by the use
     * @param parent            Parent widget
     * @param title             Title for dialog
     * @param dir               Dialog's working directory is set to dir
     * @param nameFilters       used for filter file/folder, such as "*.jpg", "*.txt"
     *                          Please set options with "DisplayAllDirs" to avoid folder be filtered when only filter filename.
     * @param Options           Options
     * @return File path that user selected. It is an esisting file.
     *         user can select cancel to close dialog and empty string will be returned
     */
    static QString getOpenFileName( HbWidget *parent = 0,
                                    const QString &title = QString(),
                                    const QString &dir = QString(),
                                    const QStringList &nameFilters = QStringList(),
                                    Options options = 0 );
    /**
     * Static function that will return a file name selected by the user. The file does not have to exist.
     * @param parent            Parent widget
     * @param title             Title for dialog
     * @param dir               Dialog's working directory is set to dir
     * @param nameFilters       used for filter file/folder, such as "*.jpg", "*.txt"
     *                          Please set options with "DisplayAllDirs" to avoid folder be filtered when only filter filename.
     * @param Options           Options
     * @return File path that user selected or inputted in file name line edit.
     *         This file does not have to exist.
     *         user can select cancel to close dialog and empty string will be returned
     */
    static QString getSaveFileName( HbWidget *parent = 0,
                                    const QString &title = QString(),
                                    const QString &dir = QString(),
                                    const QStringList &nameFilters = QStringList(),
                                    Options options = 0 );

private slots:
    void dialogClosed(HbAction *action);
    
private:
    explicit FmFileDialog( QGraphicsItem *parent );
    ~FmFileDialog();

    bool exec();
    HbAction *primaryAction() const;
    void setPrimaryAction( HbAction *action );

    HbAction *secondaryAction() const;
    void setSecondaryAction( HbAction *action );
    
    FmFileDialogPrivate *d_ptr;
    
    Q_DISABLE_COPY( FmFileDialog)
    Q_DECLARE_PRIVATE_D(d_ptr,FmFileDialog)
    Q_PRIVATE_SLOT(d_func(),void _q_handleUpButton())
    Q_PRIVATE_SLOT(d_func(),void _q_handleTextChanged(QString))
    Q_PRIVATE_SLOT(d_func(),void _q_handlePathChanged(QString))
    Q_PRIVATE_SLOT(d_func(),void _q_handleFileActivated(QString))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FmFileDialog::Options)

#endif // FMFILEDIALOG_H
