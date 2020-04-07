# invoke SourceDir generated makefile for SlaveController.pem4f
SlaveController.pem4f: .libraries,SlaveController.pem4f
.libraries,SlaveController.pem4f: package/cfg/SlaveController_pem4f.xdl
	$(MAKE) -f C:\Users\Allen\Documents\GitHub\EE319H-Final-Project-CCS\SlaveController/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Allen\Documents\GitHub\EE319H-Final-Project-CCS\SlaveController/src/makefile.libs clean

