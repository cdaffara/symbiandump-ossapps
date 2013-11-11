del *.sis
makesis -d\ podcatcher_udeb.pkg
signsis podcatcher_udeb.sis podcatcher_udeb_signed.sis podcast.cer podcast.key

makesis -d\ podcatcher_urel.pkg
signsis podcatcher_urel.sis podcatcher_urel_signed.sis podcast.cer podcast.key