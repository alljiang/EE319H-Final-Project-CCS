# invoke SourceDir generated makefile for SlaveController.pem4f
SlaveController.pem4f: .libraries,SlaveController.pem4f
.libraries,SlaveController.pem4f: package/cfg/SlaveController_pem4f.xdl
	$(MAKE) -f C:\Users\Allen\workspace_CCS\SlaveController/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Allen\workspace_CCS\SlaveController/src/makefile.libs clean

