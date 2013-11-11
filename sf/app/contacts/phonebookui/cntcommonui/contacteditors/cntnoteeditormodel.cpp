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
#include "cntnoteeditormodel.h"
#include "cntdetailmodelitem.h"
#include <qcontactnote.h>

CntNoteEditorModel::CntNoteEditorModel( QContact* aContact ) :
CntDetailEditorModel( aContact )
{
    HbDataFormModelItem* root = invisibleRootItem();
    foreach ( QContactNote note, mContact->details<QContactNote>() )
    {
        CntDetailModelItem* item = new CntDetailModelItem(note);
        appendDataFormItem( item, root );
        connect( item, SIGNAL(itemDataChanged(CntDetailModelItem*)), this, SLOT(emitDataChanged(CntDetailModelItem*)) );
    }
}

CntNoteEditorModel::~CntNoteEditorModel()
{
}

void CntNoteEditorModel::insertDetailField()
{
    QContactNote emptyNote;
    mNoteList.append( emptyNote );
    CntDetailModelItem* newNote = new CntDetailModelItem(emptyNote);
    appendDataFormItem( newNote, invisibleRootItem() );
    connect( newNote, SIGNAL(itemDataChanged(CntDetailModelItem*)), this, SLOT(emitDataChanged(CntDetailModelItem*)) );
    emitDataChanged(newNote);
}

void CntNoteEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
      
    int count( root->childCount() );
    for ( int i(0); i < count; i++ ) {
        CntDetailModelItem* detail = static_cast<CntDetailModelItem*>( root->childAt(i) );
        QContactNote note = detail->detail();
        
        if ( !mNoteList.contains(note) )
        {
            mContact->saveDetail( &note );
        }
        
        if ( note.note().isEmpty() )
        {
            mContact->removeDetail( &note );
        }
    }
}

QContactDetail CntNoteEditorModel::detail() const
{
    QListIterator<QContactNote> noteList(mContact->details<QContactNote>());
    noteList.toBack(); // go through backwards, so the newest item will be returned
    while ( noteList.hasPrevious() )
    {
        QContactNote note = noteList.previous();
        if ( !note.value(QContactNote::FieldNote).isEmpty() )
        {
            return note;
        }
    }
    return QContactNote(); // return empty address if none found
}

// End of File
