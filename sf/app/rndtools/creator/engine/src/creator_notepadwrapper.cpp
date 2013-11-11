/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QDir>
#include "creator_notepadwrapper.h"

	/**
	* Constructor
	*/
CCreatorNotepadWrapper::CCreatorNotepadWrapper()
	{
	}
	
	/**
	* Destructor
	*/
CCreatorNotepadWrapper::~CCreatorNotepadWrapper()
	{
    
    iNotepadApi = NULL;
		
    if(iNotesEditorPluginLoader)
        {
	    iNotesEditorPluginLoader->unload();
        delete iNotesEditorPluginLoader;
        iNotesEditorPluginLoader = NULL;
        }
        
    if(iAgendaUtil)
        {
        delete iAgendaUtil;
        iAgendaUtil = NULL;
        }
	}
	
	/**
	* Two-Phased constructor
	*/
CCreatorNotepadWrapper* CCreatorNotepadWrapper::NewL()
	{
	CCreatorNotepadWrapper* self = CCreatorNotepadWrapper::NewLC();
	CleanupStack::Pop(self);
    return self;
	}

	/**
	* Two-Phased constructor
	*/
CCreatorNotepadWrapper* CCreatorNotepadWrapper::NewLC()
	{
	CCreatorNotepadWrapper* self = new (ELeave) CCreatorNotepadWrapper;
	CleanupStack::PushL(self);
	self->ConstructL();
    return self;
	}
	
	/**
	* ConstructL()
	*/

void CCreatorNotepadWrapper::ConstructL()
	{
	iAgendaUtil = new AgendaUtil();
	
	//	iNotepadApi = new NotesEditor(iAgendaUtil);
	
	// Load notes editor plugin.
    // Launch the notes editor using notes editor plugin api
    QDir dir(NOTES_EDITOR_PLUGIN_PATH);
    QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

    QT_TRYCATCH_LEAVING(
        // Create NotesEditor plugin loader object.
        iNotesEditorPluginLoader = new QPluginLoader(pluginName);

        // Load the plugin
        bool notesPluginLoaded = iNotesEditorPluginLoader->load();
        QObject *plugin = qobject_cast<QObject*> ( iNotesEditorPluginLoader->instance());

        iNotepadApi = qobject_cast<NotesEditorInterface*>(plugin);
        );

	}

TInt CCreatorNotepadWrapper::CreateNoteL( const TDesC& aText )
	{
	User::LeaveIfNull(iNotepadApi);

    QString textNote = QString::fromUtf16( aText.Ptr(),aText.Length());
    iNotepadApi->edit(textNote,iAgendaUtil);
    iNotepadApi->close(NotesEditorInterface::CloseWithSave, iAgendaUtil);
	
    return KErrNone;
    }
void CCreatorNotepadWrapper::DeleteAllL()
	{
	
	QList<AgendaEntry> ael;
    AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(AgendaUtil::IncludeNotes);

    ael = iAgendaUtil->fetchAllEntries(filter);
    for(int i=0 ; i<ael.count() ; i++)
    	{
        iAgendaUtil->deleteEntry(ael[i].id());
    	}
		
	}
	
