# Lima-camera-spectrumoneccd
SpectrumOneCCD detector Lima plugin

Win64 plugin working with NI GPIB/ENET boards. Tested with GPIB/ENET 100, and drivers 488.2 version 17.6 (later versions don't support GPIB/ENET 100, only GPIB/ENET 1000).

## Getting the 488.2 library to build the plugin

After installing drivers 488.2 from National Instruments, the library is located here:
`C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C`

Binaries for win64 msvc are here:
`C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\lib64\msvc`
DISCLAIMER: It seems like maven doesn't detect .obj files properly. `ni4882.obj` is then to be renamed to `ni4882.lib`

Includes are here:
`C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\include`
