# invoke SourceDir generated makefile for bigtime.pem4f
bigtime.pem4f: .libraries,bigtime.pem4f
.libraries,bigtime.pem4f: package/cfg/bigtime_pem4f.xdl
	$(MAKE) -f C:\Users\Allen\workspace_CCS\EE319H_Final_Project/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Allen\workspace_CCS\EE319H_Final_Project/src/makefile.libs clean

