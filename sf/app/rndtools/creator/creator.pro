TEMPLATE = app
TARGET = Creator

DEPENDPATH += .
DEPENDPATH += src engine/src
INCLUDEPATH += inc engine/inc

load(hb.prf)
symbian:CONFIG -= symbian_i18n

HEADERS += \
	inc/mainview.h \
	inc/notifications.h

SOURCES += main.cpp \
	src/mainview.cpp \
	src/notifications.cpp

symbian: {
        INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	HEADERS += engine/inc/creator.hrh \
			engine/inc/creator_calendar.h \
			engine/inc/creator_calendarbase.h \
			engine/inc/creator_calendarelement.h \
			engine/inc/creator_cmdscriptrun.h \
			engine/inc/creator_contactelement.h \
			engine/inc/creator_contactsetcache.h \
			engine/inc/creator_factory.h \
			engine/inc/creator_file.h \
			engine/inc/creator_fileelement.h \
			engine/inc/creator_landmark.h \
			engine/inc/creator_landmarkelement.h \
			engine/inc/creator_log.h \
			engine/inc/creator_logelement.h \
			engine/inc/creator_mailbox.h \
			engine/inc/creator_mailboxelement.h \
			engine/inc/creator_message.h \
			engine/inc/creator_messageelement.h \
			engine/inc/creator_modulebase.h \
			engine/inc/creator_note.h \
			engine/inc/creator_notepadwrapper.h \
			engine/inc/creator_phonebook.h \
			engine/inc/creator_phonebookwrapper.h \
			engine/inc/creator_phonebookapi.h \
			engine/inc/creator_phonebookbase.h \
			engine/inc/creator_randomdatafield.h \
			engine/inc/creator_randomdataparser.h \
			engine/inc/creator_scriptelement.h \
			engine/inc/creator_scriptelementfactory.h \
			engine/inc/creator_scriptentry.h \
			engine/inc/creator_scriptparser.h \
			engine/inc/creator_std.h \
			engine/inc/creator_traces.h \
			engine/inc/creator_wepsecuritysettings.h \
			engine/inc/randomdata.txt \
			inc/engine.h \
			inc/enginewrapper.h

	SOURCES += engine/src/creator_calendar.cpp \
			engine/src/creator_calendarelement.cpp \
			engine/src/creator_cmdscriptrun.cpp \
			engine/src/creator_contactelement.cpp \
			engine/src/creator_contactsetcache.cpp \
			engine/src/creator_factory.cpp \
			engine/src/creator_file.cpp \
			engine/src/creator_fileelement.cpp \
			engine/src/creator_landmark.cpp \
			engine/src/creator_landmarkelement.cpp \
			engine/src/creator_log.cpp \
			engine/src/creator_logelement.cpp \
			engine/src/creator_mailbox.cpp \
			engine/src/creator_mailboxelement.cpp \
			engine/src/creator_message.cpp \
			engine/src/creator_messageelement.cpp \
			engine/src/creator_note.cpp \
			engine/src/creator_notepadwrapper.cpp \
			engine/src/creator_phonebook.cpp \
			engine/src/creator_phonebookwrapper.cpp \
			engine/src/creator_phonebookapi.cpp \
			engine/src/creator_phonebookbase.cpp \
			engine/src/creator_randomdataparser.cpp \
			engine/src/creator_scriptelement.cpp \
			engine/src/creator_scriptelementfactory.cpp \
			engine/src/creator_scriptentry.cpp \
			engine/src/creator_scriptparser.cpp \
			src/engine.cpp \
			src/enginewrapper.cpp

	LIBS += -lestor \
			-lws32 \
			-lPlatformEnv \
			-limageconversion \
			-leikcore \
			-leuser \
			-lapparc \
			-lcone \ 
			-lcntmodel \
			-lmsgs \
			-llogwrap \
			-llogcli \
			-lefsrv \
			-lgsmu \
			-limcm \
			-lbafl \ 
			-leikcoctl \ 
			-leikctl \
			-lcommdb \
			-letext \ 
			-lcommonengine \ 
			-lcommonui \ 
			-lplatformenv \ 
			-lapgrfx \
			-lapmime \
			-lobexmtmutil \
			-lsmcm \
			-laknskins \
			-laknskinsrv \ 
			-lflogger \
			-letel \
			-letelmm \
			-lxmlframework \
			-lcharconv \ 
			-lsenutils \
			-lcalinterimapi \
			-leposlandmarks \
			-llbs \
			-lecom \
			-lcaf \
			-lcafutils \ 
			-ldrmrights \
			-ledbms \
			-lfbscli \
			-lbitmaptransforms \
			-lcmmanager \
			-lcmmanagerdatabase \
			-lnoteseditor \
			-lxqservice \
		    -lQtContacts \
			-lagendainterface 

	creatorDataBlock = \
			"SOURCEPATH ." \
			"START RESOURCE data/datacreator.rss" \
			"HEADER" \
			"TARGETPATH /resource/apps" \
			"END"

	MMP_RULES += creatorDataBlock
	MMP_RULES -= PAGED
  MMP_RULES *= UNPAGED

    BLD_INF_RULES.prj_exports += "./rom/creator.iby CORE_IBY_EXPORT_PATH(tools,creator.iby)
	BLD_INF_RULES.prj_exports += \
				"./files/backup_registration.xml    Z:/private/20011383/backup_registration.xml" \
				"./files/3GPP-70kB.dat              Z:/private/20011383/3GPP-70kB.3gpp" \
				"./files/AAC-100kB.dat              Z:/private/20011383/AAC-100kB.aac" \
				"./files/AMR-20kB.dat               Z:/private/20011383/AMR-20kB.amr" \
				"./files/BMP-25kB.dat               Z:/private/20011383/BMP-25kB.bmp" \
				"./files/Deck-1kB.dat               Z:/private/20011383/Deck-1kB.saveddeck" \
				"./files/DOC-20kB.dat               Z:/private/20011383/DOC-20kB.doc" \
				"./files/GIF-2kB.dat                Z:/private/20011383/GIF-2kB.gif" \
				"./files/HTML-20kB.dat              Z:/private/20011383/HTML-20kB.html" \
				"./files/JAD-1kB.dat                Z:/private/20011383/JAD-1kB.jad" \
				"./files/JAR-10kB.dat               Z:/private/20011383/JAR-10kB.jar" \
				"./files/JP2-65kB.dat               Z:/private/20011383/JP2-65kB.jp2" \
				"./files/JPEG-200kB.dat             Z:/private/20011383/JPEG-200kB.jpg" \
				"./files/JPEG-25kB.dat              Z:/private/20011383/JPEG-25kB.jpg" \
				"./files/JPEG-500kB.dat             Z:/private/20011383/JPEG-500kB.jpg" \
				"./files/MIDI-10kB.dat              Z:/private/20011383/MIDI-10kB.mid" \
				"./files/MP3-250kB.dat              Z:/private/20011383/MP3-250kB.mp3" \
				"./files/MP4-200kB.dat              Z:/private/20011383/MP4-200kB.mp4" \
				"./files/MXMF-40kB.dat              Z:/private/20011383/MXMF-40kB.mxmf" \
				"./files/PNG-15kB.dat               Z:/private/20011383/PNG-15kB.png" \
				"./files/PPT-40kB.dat               Z:/private/20011383/PPT-40kB.ppt" \
				"./files/RAM-1kB.dat                Z:/private/20011383/RAM-1kB.ram" \
				"./files/RM-95kB.dat                Z:/private/20011383/RM-95kB.rm" \
				"./files/RNG-1kB.dat                Z:/private/20011383/RNG-1kB.rng" \
				"./files/SVG-15kB.dat               Z:/private/20011383/SVG-15kB.svg" \
				"./files/SWF-15kB.dat               Z:/private/20011383/SWF-15kB.swf" \
				"./files/TIF-25kB.dat               Z:/private/20011383/TIF-25kB.tif" \
				"./files/TXT-10kB.dat               Z:/private/20011383/TXT-10kB.txt" \
				"./files/TXT-70kB.dat               Z:/private/20011383/TXT-70kB.txt" \
				"./files/VCF-1kB.dat                Z:/private/20011383/VCF-1kB.vcf" \
				"./files/VCS-1kB.dat                Z:/private/20011383/VCS-1kB.vcs" \
				"./files/WAV-20kB.dat               Z:/private/20011383/WAV-20kB.wav" \
				"./files/XLS-15kB.dat               Z:/private/20011383/XLS-15kB.xls" \
				"./files/SISX-10kB.dat              Z:/private/20011383/SISX-10kB.sisx" \
				"./files/WMA-50kB.dat               Z:/private/20011383/WMA-50kB.wma" \
				"./files/WMV-200kB.dat              Z:/private/20011383/WMV-200kB.wmv" \
				"./scripts/creator.xsd                   +/tools/s60rndtools/creator/creator.xsd" \
				"./scripts/browserEx.creatorxml          +/tools/s60rndtools/creator/browserEx.creatorxml" \
				"./scripts/calendarEx.creatorxml         +/tools/s60rndtools/creator/calendarEx.creatorxml" \
				"./scripts/connectionmethodEx.creatorxml +/tools/s60rndtools/creator/connectionmethodEx.creatorxml" \
				"./scripts/contactEx.creatorxml          +/tools/s60rndtools/creator/contactEx.creatorxml" \
				"./scripts/filesEx.creatorxml            +/tools/s60rndtools/creator/filesEx.creatorxml" \
				"./scripts/landmarkEx.creatorxml         +/tools/s60rndtools/creator/landmarkEx.creatorxml" \
				"./scripts/logEx.creatorxml              +/tools/s60rndtools/creator/logEx.creatorxml" \
				"./scripts/mailboxEx.creatorxml          +/tools/s60rndtools/creator/mailboxEx.creatorxml" \
				"./scripts/messagesEx.creatorxml         +/tools/s60rndtools/creator/messagesEx.creatorxml" \
				"./scripts/notepadEx.creatorxml          +/tools/s60rndtools/creator/notepadEx.creatorxml" 
    
    creator_buildstubsis_extension = \
        "$${LITERAL_HASH}ifdef MARM" \
        "    START EXTENSION app-services/buildstubsis" \
        "    OPTION SRCDIR sis" \
        "    OPTION SISNAME creator_stub" \
        "    END" \
        "$${LITERAL_HASH}endif" 
        
    BLD_INF_RULES.prj_extensions += creator_buildstubsis_extension
    
	RSS_RULES += "group_name = \"RnD Tools\"";
	TARGET.CAPABILITY = CAP_APPLICATION AllFiles

	TARGET.UID2 = 0x100039CE
	TARGET.UID3 = 0x20011383
	TARGET.SID = 0x20011383
	TARGET.VID = 0x101FB657 // Nokia
	TARGET.EPOCHEAPSIZE = 0x20000 0x1000000  // Min 128Kb, Max 16Mb

	ICON = icons/qgn_menu_creator.svg 

} else {
	error("Only Symbian supported!")
}
