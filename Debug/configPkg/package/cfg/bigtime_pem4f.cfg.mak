# invoke SourceDir generated makefile for bigtime.pem4f
bigtime.pem4f: .libraries,bigtime.pem4f
.libraries,bigtime.pem4f: package/cfg/bigtime_pem4f.xdl
	$(MAKE) -f C:\Users\Allen\Documents\GitHub\EE319H-Final-Project-CCS/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Allen\Documents\GitHub\EE319H-Final-Project-CCS/src/makefile.libs clean

