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

#ifndef __CREATORNOTEPADWRAPPER_H__
#define __CREATORNOTEPADWRAPPER_H__

//#include "engine.h"
//#include "creator_modulebase.h"


#include <e32base.h>
#include <QPluginLoader>
#include <NotesEditorInterface>
#include <AgendaUtil>
#include <AgendaEntry>


class CCreatorNotepadWrapper : public CBase
	{
	public:
		static CCreatorNotepadWrapper* NewL();
		static CCreatorNotepadWrapper* NewLC();
		~CCreatorNotepadWrapper();
		
	private:
		CCreatorNotepadWrapper();
		void ConstructL();
		
	public:
		TInt CreateNoteL( const TDesC& aText );
		void DeleteAllL();
		
	private:
		AgendaUtil *iAgendaUtil;
		NotesEditorInterface *iNotepadApi;	//QT Notes api
		QPluginLoader* iNotesEditorPluginLoader; // for loading QT Notes api
	};

#endif // __CREATORNOTEPADWRAPPER_H__
