Set-Location build
msbuild vehdbg.sln /p:configuration=Release

Copy-Item lib\Release\libvehdbg.lib ..\bin\libvehdbg.lib

Set-Location sample
msbuild sample.sln /p:configuration=Release

Copy-Item branchtracer\Release\branchtracer.dll ..\..\bin\branchtracer.dll
Copy-Item dllinjector\Release\dllinjector.exe ..\..\bin\dllinjector.exe
Copy-Item readme_sample\Release\readme_sample.dll ..\..\bin\readme_sample.dll

Set-Location ..\..
