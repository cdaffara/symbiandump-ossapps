rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description: 
rem

@echo off
set CODE_TEST_LIB=NTmvc_TargetLibMD.lib CtSymbianNativeMem.lib
set AMC_TARGET=x86-symbian-native

REM ########################################################
REM ### Initialize instrumenting (x: is the subst drive)
REM instctsymb x:\ 9



REM ########################################################
REM ### Default settings (performance analysis, statement coverage)
REM set CTDRIVER_ARGS=-CTsize_t=unsigned -CTtag-allocator -CTv -CTno-tag-includes

REM ########################################################
REM ### Calendar Application - Coverage
REM set CTDRIVER_ARGS=-CTsize_t=unsigned -CTtag-allocator -CTsymbian -CTno-tag-includes -CTtag-level=DC -CTonly-tag-files=CalenActive.cpp,CalenApp.cpp,CalenAppUi.cpp,CalenContainer.cpp,CalenCycleState.cpp,CalenDayListBoxModel.cpp,CalenDayOnlyEventCont.cpp,CalenDayOnlyEventLBData.cpp,CalenDayOnlyEventLBItemDrawer.cpp,CalenDayOnlyEventLBView.cpp,CalenDayOnlyEventListbox.cpp,CalenDayView.cpp,CalenDefaultVariant.cpp,CalenDeleteUi.cpp,CalenDoc.cpp,CalenDrawUtils.cpp,CalenEditUi.cpp,CalenEnv.cpp,CalenFileListSettingItem.cpp,CalenFocusState.cpp,CalenIcons.cpp,CalenInstanceId.cpp,CalenMonthCellListBoxData.cpp,CalenMonthCellListBoxItemDrawer.cpp,CalenMonthCont.cpp,CalenMonthData.cpp,CalenMonthGrid.cpp,CalenMonthView.cpp,CalenPopup.cpp,CalenPopupController.cpp,CalenPopupData.cpp,CalenPopupEntry.cpp,CalenQueries.cpp,CalenSetting.cpp,CalenSettingViewDlg.cpp,CalenSliderSettingItem.cpp,CalenState.cpp,CalenTitlePane.cpp,CalenVariantHandler.cpp,CalenView.cpp,CalenWeekCont.cpp,CalenWeekHour.cpp,CalenWeekLBData.cpp,CalenWeekLBItemDrawer.cpp,CalenWeekLBView.cpp,CalenWeekListbox.cpp,CalenWeekListBoxLayout.cpp,CalenWeekView.cpp,ToDoContainer.cpp,ToDoController.cpp,ToDoView.cpp,CalenDBChangeNotifier.cpp,CalenEcomWatcher.cpp,CalenGlobalData.cpp,CalenSend.cpp,CalenVCalExport.cpp,CalenViewObserver.cpp,CalenEcomWatcher.cpp,CalenInterimUtils.cpp,CalenInterimUtils2.cpp,CalenInterimUtils2Impl.cpp,CalenAgnEventHandler.cpp,CalenCmdLineParser.cpp,CalenCommonUI.cpp,CalenDateUtils.cpp,CalenDebug.cpp,CalenUtils.cpp,CalenAsyncCommands.cpp,CalenEngine.cpp,CalenMdl.cpp,CalenNoteDataUtil.cpp,CalenAlarmFields.cpp,CalenAnnivForm.cpp,CalenDayForm.cpp,calenDefaultEditors.cpp,CalenDefaultEditorsProxy.cpp,CalenDescription.cpp,CalenDescriptionField.cpp,CalenEditorData.cpp,CalenEditorDataHandler.cpp,CalenEntryUpdater.cpp,CalenFormBase.cpp,CalenMeetingForm.cpp,CalenRepeatFields.cpp,CalenSynchField.cpp

REM ########################################################
REM ### Calendar Application - Performance
REM set CTDRIVER_ARGS=-CTsize_t=unsigned -CTtag-allocator -CTsymbian -CTno-tag-includes -CTtag-level=1 -CTonly-tag-files=CalenActive.cpp,CalenApp.cpp,CalenAppUi.cpp,CalenContainer.cpp,CalenCycleState.cpp,CalenDayListBoxModel.cpp,CalenDayOnlyEventCont.cpp,CalenDayOnlyEventLBData.cpp,CalenDayOnlyEventLBItemDrawer.cpp,CalenDayOnlyEventLBView.cpp,CalenDayOnlyEventListbox.cpp,CalenDayView.cpp,CalenDefaultVariant.cpp,CalenDeleteUi.cpp,CalenDoc.cpp,CalenDrawUtils.cpp,CalenEditUi.cpp,CalenEnv.cpp,CalenFileListSettingItem.cpp,CalenFocusState.cpp,CalenIcons.cpp,CalenInstanceId.cpp,CalenMonthCellListBoxData.cpp,CalenMonthCellListBoxItemDrawer.cpp,CalenMonthCont.cpp,CalenMonthData.cpp,CalenMonthGrid.cpp,CalenMonthView.cpp,CalenPopup.cpp,CalenPopupController.cpp,CalenPopupData.cpp,CalenPopupEntry.cpp,CalenQueries.cpp,CalenSetting.cpp,CalenSettingViewDlg.cpp,CalenSliderSettingItem.cpp,CalenState.cpp,CalenTitlePane.cpp,CalenVariantHandler.cpp,CalenView.cpp,CalenWeekCont.cpp,CalenWeekHour.cpp,CalenWeekLBData.cpp,CalenWeekLBItemDrawer.cpp,CalenWeekLBView.cpp,CalenWeekListbox.cpp,CalenWeekListBoxLayout.cpp,CalenWeekView.cpp,ToDoContainer.cpp,ToDoController.cpp,ToDoView.cpp,CalenDBChangeNotifier.cpp,CalenEcomWatcher.cpp,CalenGlobalData.cpp,CalenSend.cpp,CalenVCalExport.cpp,CalenViewObserver.cpp,CalenEcomWatcher.cpp,CalenInterimUtils.cpp,CalenInterimUtils2.cpp,CalenInterimUtils2Impl.cpp,CalenAgnEventHandler.cpp,CalenCmdLineParser.cpp,CalenCommonUI.cpp,CalenDateUtils.cpp,CalenDebug.cpp,CalenUtils.cpp,CalenAsyncCommands.cpp,CalenEngine.cpp,CalenMdl.cpp,CalenNoteDataUtil.cpp,CalenAlarmFields.cpp,CalenAnnivForm.cpp,CalenDayForm.cpp,calenDefaultEditors.cpp,CalenDefaultEditorsProxy.cpp,CalenDescription.cpp,CalenDescriptionField.cpp,CalenEditorData.cpp,CalenEditorDataHandler.cpp,CalenEntryUpdater.cpp,CalenFormBase.cpp,CalenMeetingForm.cpp,CalenRepeatFields.cpp,CalenSynchField.cpp

REM ########################################################
REM ### AgnVersit2, Versit2
REM set CTDRIVER_ARGS=-CTsize_t=unsigned -CTtag-allocator -CTsymbian -CTno-tag-includes -CTtag-level=DC -CTonly-tag-files=AgnExtIntLib.cpp,AgnVersit2.cpp,AgnVersit2Exporter.cpp,AgnVersit2Importer.cpp,AgnVersit2StringProvider.cpp,ICal.cpp,ICalAlarm.cpp,ICalBase.cpp,ICalComponent.cpp,ICalContentLineReader.cpp,ICalContentLineWriter.cpp,ICalEvent.cpp,ICalFreeBusy.cpp,ICalJournal.cpp,ICalParser.cpp,ICalProperty.cpp,ICalPropertyParam.cpp,ICalRuleSegment.cpp,ICalTimeZone.cpp,ICalTimeZoneInterval.cpp,ICalTodo.cpp,ICalUtil.cpp,ICalValue.cpp

REM ########################################################
REM ### AgnVersit2
set CTDRIVER_ARGS=-CTsize_t=unsigned -CTtag-allocator -CTsymbian -CTno-tag-includes -CTtag-level=DC -CTonly-tag-files=AgnExtIntLib.cpp,AgnVersit2.cpp,AgnVersit2Exporter.cpp,AgnVersit2Importer.cpp,AgnVersit2StringProvider.cpp

REM ########################################################
REM ### Versit2
REM set CTDRIVER_ARGS=-CTsize_t=unsigned -CTtag-allocator -CTsymbian -CTno-tag-includes -CTtag-level=DC -CTonly-tag-files=ICal.cpp,ICalAlarm.cpp,ICalBase.cpp,ICalComponent.cpp,ICalContentLineReader.cpp,ICalContentLineWriter.cpp,ICalEvent.cpp,ICalFreeBusy.cpp,ICalJournal.cpp,ICalParser.cpp,ICalProperty.cpp,ICalPropertyParam.cpp,ICalRuleSegment.cpp,ICalTimeZone.cpp,ICalTimeZoneInterval.cpp,ICalTodo.cpp,ICalUtil.cpp,ICalValue.cpp

REM ########################################################
REM ### Stop instumenting
REM set CTDRIVER_ARGS=-CTag-level=0

@echo on
