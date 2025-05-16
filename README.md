# Minecraft-Injector-java-
The DLL is injected into Minecraft by the injector using manual mapping By allocating memory in the target process putting the DLL into that memory and changing the process entry point to the DLL's loader AND IT DOES NOT USE CREATEREMOTETHREAD. In order to reroute function calls to the custom functions of the injected DLL it also hooks the Import Address Table (IAT)

# Injection methods used
- Manual Mapping
- Import Address Table (IAT) Hooking
