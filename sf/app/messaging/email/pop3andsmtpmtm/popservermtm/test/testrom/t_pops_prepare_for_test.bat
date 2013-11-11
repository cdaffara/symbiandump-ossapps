rem Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem T_SMTS_PREPARE_FOR_TEST.BAT
rem 
rem

REM NOTE! Do not use tabs here!
md c:\MsgTest
md c:\MsgTest\pops
md c:\MsgTest\pops\scripts
md c:\logs
md c:\logs\email

copy z:\System\MsgTest\msgtest\pops\connect.out c:\msgtest\pops\connect.out
copy z:\System\MsgTest\msgtest\pops\connect.scr c:\msgtest\pops\connect.scr
copy z:\System\MsgTest\msgtest\pops\connect_and_quit.out c:\msgtest\pops\connect_and_quit.out
copy z:\System\MsgTest\msgtest\pops\connect_and_quit1.out c:\msgtest\pops\connect_and_quit1.out
copy z:\System\MsgTest\msgtest\pops\delete_all.out c:\msgtest\pops\delete_all.out
copy z:\System\MsgTest\msgtest\pops\download_all1.out c:\msgtest\pops\download_all1.out
copy z:\System\MsgTest\msgtest\pops\download_all1.scr c:\msgtest\pops\download_all1.scr
copy z:\System\MsgTest\msgtest\pops\download_all10.out c:\msgtest\pops\download_all10.out
copy z:\System\MsgTest\msgtest\pops\download_all10.scr c:\msgtest\pops\download_all10.scr
copy z:\System\MsgTest\msgtest\pops\download_all2.out c:\msgtest\pops\download_all2.out
copy z:\System\MsgTest\msgtest\pops\download_all2.scr c:\msgtest\pops\download_all2.scr
copy z:\System\MsgTest\msgtest\pops\download_all3.out c:\msgtest\pops\download_all3.out
copy z:\System\MsgTest\msgtest\pops\download_all3.scr c:\msgtest\pops\download_all3.scr
copy z:\System\MsgTest\msgtest\pops\download_all4.out c:\msgtest\pops\download_all4.out
copy z:\System\MsgTest\msgtest\pops\download_all4.scr c:\msgtest\pops\download_all4.scr
copy z:\System\MsgTest\msgtest\pops\download_all5.out c:\msgtest\pops\download_all5.out
copy z:\System\MsgTest\msgtest\pops\download_all5.scr c:\msgtest\pops\download_all5.scr
copy z:\System\MsgTest\msgtest\pops\download_all6.out c:\msgtest\pops\download_all6.out
copy z:\System\MsgTest\msgtest\pops\download_all6.scr c:\msgtest\pops\download_all6.scr
copy z:\System\MsgTest\msgtest\pops\download_all7.out c:\msgtest\pops\download_all7.out
copy z:\System\MsgTest\msgtest\pops\download_all7.scr c:\msgtest\pops\download_all7.scr
copy z:\System\MsgTest\msgtest\pops\download_all8.out c:\msgtest\pops\download_all8.out
copy z:\System\MsgTest\msgtest\pops\download_all8.scr c:\msgtest\pops\download_all8.scr
copy z:\System\MsgTest\msgtest\pops\download_all9.out c:\msgtest\pops\download_all9.out
copy z:\System\MsgTest\msgtest\pops\download_all9.scr c:\msgtest\pops\download_all9.scr
copy z:\System\MsgTest\msgtest\pops\missing_fields_download.out c:\msgtest\pops\missing_fields_download.out
copy z:\System\MsgTest\msgtest\pops\missing_fields_download.scr c:\msgtest\pops\missing_fields_download.scr
copy z:\System\MsgTest\msgtest\pops\missing_fields_sync.scr c:\msgtest\pops\missing_fields_sync.scr
copy z:\System\MsgTest\msgtest\pops\online_local_copy.dmp c:\msgtest\pops\online_local_copy.dmp
copy z:\System\MsgTest\msgtest\pops\online_local_move.dmp c:\msgtest\pops\online_local_move.dmp
copy z:\System\MsgTest\msgtest\pops\online_populate.dmp c:\msgtest\pops\online_populate.dmp
copy z:\System\MsgTest\msgtest\pops\poplog.scr c:\msgtest\pops\poplog.scr
copy z:\System\MsgTest\msgtest\pops\popprog.scr c:\msgtest\pops\popprog.scr
copy z:\System\MsgTest\msgtest\pops\populate_twice.scr c:\msgtest\pops\populate_twice.scr
copy z:\System\MsgTest\msgtest\pops\second_connect.dmp c:\msgtest\pops\second_connect.dmp
copy z:\System\MsgTest\msgtest\pops\tpops5_connect_copy.out c:\msgtest\pops\tpops5_connect_copy.out
copy z:\System\MsgTest\msgtest\pops\tpops5_connect_copy.scr c:\msgtest\pops\tpops5_connect_copy.scr
copy z:\System\MsgTest\msgtest\pops\tpops5_connect_quit.out c:\msgtest\pops\tpops5_connect_quit.out
copy z:\System\MsgTest\msgtest\pops\tpops5_connect_quit.scr c:\msgtest\pops\tpops5_connect_quit.scr
copy z:\System\MsgTest\msgtest\pops\tpops5_connect_quit1.out c:\msgtest\pops\tpops5_connect_quit1.out

copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_02_18.out c:\msgtest\pops\t_pops_synchronise_02_18.out			
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_02_18.scr c:\msgtest\pops\t_pops_synchronise_02_18.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_04.out c:\msgtest\pops\t_pops_synchronise_04.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_04.scr c:\msgtest\pops\t_pops_synchronise_04.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_05.out c:\msgtest\pops\t_pops_synchronise_05.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_05.scr c:\msgtest\pops\t_pops_synchronise_05.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_06.out c:\msgtest\pops\t_pops_synchronise_06.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_06.scr c:\msgtest\pops\t_pops_synchronise_06.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_07.out c:\msgtest\pops\t_pops_synchronise_07.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_07.scr c:\msgtest\pops\t_pops_synchronise_07.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_08.out c:\msgtest\pops\t_pops_synchronise_08.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_08.scr c:\msgtest\pops\t_pops_synchronise_08.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_09.out c:\msgtest\pops\t_pops_synchronise_09.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_09.scr c:\msgtest\pops\t_pops_synchronise_09.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_10.out c:\msgtest\pops\t_pops_synchronise_10.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_10.scr c:\msgtest\pops\t_pops_synchronise_10.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_11.out c:\msgtest\pops\t_pops_synchronise_11.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_11.scr c:\msgtest\pops\t_pops_synchronise_11.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_12.out c:\msgtest\pops\t_pops_synchronise_12.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_12.scr c:\msgtest\pops\t_pops_synchronise_12.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_13.out c:\msgtest\pops\t_pops_synchronise_13.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_13.scr c:\msgtest\pops\t_pops_synchronise_13.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_14.out c:\msgtest\pops\t_pops_synchronise_14.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_14.scr c:\msgtest\pops\t_pops_synchronise_14.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_15.out c:\msgtest\pops\t_pops_synchronise_15.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_15.scr c:\msgtest\pops\t_pops_synchronise_15.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_19.out c:\msgtest\pops\t_pops_synchronise_19.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_19.scr c:\msgtest\pops\t_pops_synchronise_19.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_21.out c:\msgtest\pops\t_pops_synchronise_21.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_21.scr c:\msgtest\pops\t_pops_synchronise_21.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_22.out c:\msgtest\pops\t_pops_synchronise_22.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_22.scr c:\msgtest\pops\t_pops_synchronise_22.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_23.out c:\msgtest\pops\t_pops_synchronise_23.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_23.scr c:\msgtest\pops\t_pops_synchronise_23.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_24.out c:\msgtest\pops\t_pops_synchronise_24.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_24.scr c:\msgtest\pops\t_pops_synchronise_24.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_25.out c:\msgtest\pops\t_pops_synchronise_25.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_25.scr c:\msgtest\pops\t_pops_synchronise_25.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_27.out c:\msgtest\pops\t_pops_synchronise_27.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_27.scr c:\msgtest\pops\t_pops_synchronise_27.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_28.out c:\msgtest\pops\t_pops_synchronise_28.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_28.scr c:\msgtest\pops\t_pops_synchronise_28.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_30.out c:\msgtest\pops\t_pops_synchronise_30.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_30.scr c:\msgtest\pops\t_pops_synchronise_30.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_31.out c:\msgtest\pops\t_pops_synchronise_31.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_31.scr c:\msgtest\pops\t_pops_synchronise_31.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_32.out c:\msgtest\pops\t_pops_synchronise_32.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_32.scr c:\msgtest\pops\t_pops_synchronise_32.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_33.out c:\msgtest\pops\t_pops_synchronise_33.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_33.scr c:\msgtest\pops\t_pops_synchronise_33.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_34.out c:\msgtest\pops\t_pops_synchronise_34.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_34.scr c:\msgtest\pops\t_pops_synchronise_34.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_35.out c:\msgtest\pops\t_pops_synchronise_35.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_35.scr c:\msgtest\pops\t_pops_synchronise_35.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_36.out c:\msgtest\pops\t_pops_synchronise_36.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_36.scr c:\msgtest\pops\t_pops_synchronise_36.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_37.out c:\msgtest\pops\t_pops_synchronise_37.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_37.scr c:\msgtest\pops\t_pops_synchronise_37.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_38.out c:\msgtest\pops\t_pops_synchronise_38.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_38.scr c:\msgtest\pops\t_pops_synchronise_38.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_39.out c:\msgtest\pops\t_pops_synchronise_39.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_39.scr c:\msgtest\pops\t_pops_synchronise_39.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_40.out c:\msgtest\pops\t_pops_synchronise_40.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_40.scr c:\msgtest\pops\t_pops_synchronise_40.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_41.out c:\msgtest\pops\t_pops_synchronise_41.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_41.scr c:\msgtest\pops\t_pops_synchronise_41.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_42.out c:\msgtest\pops\t_pops_synchronise_42.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_42.scr c:\msgtest\pops\t_pops_synchronise_42.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_45_61_62.out c:\msgtest\pops\t_pops_synchronise_45_61_62.out     
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_45_61_62.scr c:\msgtest\pops\t_pops_synchronise_45_61_62.scr     
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_47_48.out c:\msgtest\pops\t_pops_synchronise_47_48.out        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_47_48.scr c:\msgtest\pops\t_pops_synchronise_47_48.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_49_50.out c:\msgtest\pops\t_pops_synchronise_49_50.out        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_49_50.scr c:\msgtest\pops\t_pops_synchronise_49_50.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_51.out c:\msgtest\pops\t_pops_synchronise_51.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_51.scr c:\msgtest\pops\t_pops_synchronise_51.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_52.out c:\msgtest\pops\t_pops_synchronise_52.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_52.scr c:\msgtest\pops\t_pops_synchronise_52.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_54.out c:\msgtest\pops\t_pops_synchronise_54.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_54.scr c:\msgtest\pops\t_pops_synchronise_54.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_55.out c:\msgtest\pops\t_pops_synchronise_55.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_55.scr c:\msgtest\pops\t_pops_synchronise_55.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_56.out c:\msgtest\pops\t_pops_synchronise_56.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_56.scr c:\msgtest\pops\t_pops_synchronise_56.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_57.out c:\msgtest\pops\t_pops_synchronise_57.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_57.scr c:\msgtest\pops\t_pops_synchronise_57.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_58.out c:\msgtest\pops\t_pops_synchronise_58.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_58.scr c:\msgtest\pops\t_pops_synchronise_58.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_64.out c:\msgtest\pops\t_pops_synchronise_64.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_64.scr c:\msgtest\pops\t_pops_synchronise_64.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_65_66.out c:\msgtest\pops\t_pops_synchronise_65_66.out        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_65_66.scr c:\msgtest\pops\t_pops_synchronise_65_66.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_67_68.out c:\msgtest\pops\t_pops_synchronise_67_68.out        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_67_68.scr c:\msgtest\pops\t_pops_synchronise_67_68.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_70.out c:\msgtest\pops\t_pops_synchronise_70.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_70.scr c:\msgtest\pops\t_pops_synchronise_70.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_71.out c:\msgtest\pops\t_pops_synchronise_71.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_71.scr c:\msgtest\pops\t_pops_synchronise_71.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_73to75.out c:\msgtest\pops\t_pops_synchronise_73to75.out       
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_73to75.scr c:\msgtest\pops\t_pops_synchronise_73to75.scr       
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_76.out c:\msgtest\pops\t_pops_synchronise_76.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_76.scr c:\msgtest\pops\t_pops_synchronise_76.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_77.out c:\msgtest\pops\t_pops_synchronise_77.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_77.scr c:\msgtest\pops\t_pops_synchronise_77.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_78.out c:\msgtest\pops\t_pops_synchronise_78.out           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_78.scr c:\msgtest\pops\t_pops_synchronise_78.scr           
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_79_80.out c:\msgtest\pops\t_pops_synchronise_79_80.out        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_79_80.scr c:\msgtest\pops\t_pops_synchronise_79_80.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_empty.out c:\msgtest\pops\t_pops_synchronise_empty.out        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_empty.scr c:\msgtest\pops\t_pops_synchronise_empty.scr        
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_no_synch.out c:\msgtest\pops\t_pops_synchronise_no_synch.out     
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_no_synch.scr c:\msgtest\pops\t_pops_synchronise_no_synch.scr     
copy z:\System\MsgTest\msgtest\pops\t_pops_synchronise_uid_refresh.out c:\msgtest\pops\t_pops_synchronise_uid_refresh.out  
attrib c:\msgtest\pops\*.* -r

copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsCannotConnect.scr c:\msgtest\pops\scripts\t_popTlsCannotConnect.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsCantLogin.scr c:\msgtest\pops\scripts\t_popTlsCantLogin.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsInvalidPassword.scr c:\msgtest\pops\scripts\t_popTlsInvalidPassword.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsInvalidUser.scr c:\msgtest\pops\scripts\t_popTlsInvalidUser.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsNotSupported.scr c:\msgtest\pops\scripts\t_popTlsNotSupported.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsOk.scr c:\msgtest\pops\scripts\t_popTlsOk.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsStopResponding1.scr c:\msgtest\pops\scripts\t_popTlsStopResponding1.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsStopResponding2.scr c:\msgtest\pops\scripts\t_popTlsStopResponding2.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popTlsStopResponding3.scr c:\msgtest\pops\scripts\t_popTlsStopResponding3.scr
copy z:\System\MsgTest\msgtest\pops\scripts\t_popApopTlsOk.scr c:\msgtest\pops\scripts\t_popApopTlsOk.scr
attrib c:\msgtest\pops\scripts\*.* -r
