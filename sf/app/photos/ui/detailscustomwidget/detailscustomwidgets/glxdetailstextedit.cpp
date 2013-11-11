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

#include <QDebug>
#include <QEvent>
#include <QInputContext>
#include <QGraphicsObject>

#include <hbeditorinterface>
#include <hbtapgesture.h>
#include <hbapplication.h>

#include "glxdetailstextedit.h"

//--------------------------------------------------------------------------------------------------------------------------------------------
//gestureEvent
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsTextEdit::gestureEvent(QGestureEvent* event)
{
    HbLineEdit::gestureEvent(event);
    HbTapGesture *tap = qobject_cast<HbTapGesture *> (event->gesture(Qt::TapGesture));
    if (tap && (tap->state() == Qt::GestureFinished || tap->state() == Qt::GestureUpdated)) {
        QInputContext *ic = qApp->inputContext();
        if (ic) {
            QEvent *openEvent = new QEvent(QEvent::CloseSoftwareInputPanel);
            ic->filterEvent(openEvent);
            delete openEvent;

            if (!mEditorReadOnly && tap->state() == Qt::GestureFinished) {
                //Only emit the signal in case of editor is not ready only
                emit editorTapped();
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsTextEdit
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsTextEdit::GlxDetailsTextEdit(QGraphicsItem *parent) :
    HbLineEdit(parent), mEditorReadOnly(0)
{
    setAlignment(Qt::AlignLeft);
    clearContextMenuFlag(Hb::ShowTextContextMenuOnLongPress);
    grabGesture(Qt::TapGesture);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsTextEdit
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsTextEdit::~GlxDetailsTextEdit()
{
    mEditorReadOnly = FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemText
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsTextEdit::setItemText(const QString &text)
{
    setPlainText(text);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setTextItemReadOnly
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsTextEdit::setTextItemReadOnly(bool value)
{
    mEditorReadOnly = value;
}
