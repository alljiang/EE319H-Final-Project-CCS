# invoke SourceDir generated makefile for MasterController.pem4f
MasterController.pem4f: .libraries,MasterController.pem4f
.libraries,MasterController.pem4f: package/cfg/MasterController_pem4f.xdl
	$(MAKE) -f C:\Users\Allen\workspace_CCS\MasterController/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Allen\workspace_CCS\MasterController/src/makefile.libs clean

