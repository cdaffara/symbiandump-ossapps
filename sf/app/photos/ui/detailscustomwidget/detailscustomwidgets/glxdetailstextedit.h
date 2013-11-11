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
 *  Description:   This is custom widget for details view in photos for displaying the description of image .
 *
 */

#ifndef GlXDETAILSDESCEDIT_H
#define GlXDETAILSDESCEDIT_H

#include <hblineedit.h>

#ifdef BUILD_DETAILSCUSTOM
#define MY_EXPORT Q_DECL_EXPORT
#else
#define MY_EXPORT Q_DECL_IMPORT
#endif

class MY_EXPORT GlxDetailsTextEdit: public HbLineEdit 
{
Q_OBJECT

public:
    GlxDetailsTextEdit(QGraphicsItem *parent = NULL);
    ~GlxDetailsTextEdit();
    void setItemText(const QString &text);
    void setTextItemReadOnly(bool value);

protected:
    void gestureEvent(QGestureEvent* event);

signals :
    void editorTapped();

private:
    //flag to set if the editor should emit signal or not for edit.
    bool mEditorReadOnly;

};

#endif // GlXDETAILSDESCEDIT_H
