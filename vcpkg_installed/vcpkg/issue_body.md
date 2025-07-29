Package: zlib:x64-windows@1.3.1

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.43.34810.0
-    vcpkg-tool version: 2024-12-09-1005b78fa1bf1dde1a20c2734cba4ea61ca94d9a
    vcpkg-readonly: true
    vcpkg-scripts version: 0c4cf19224a049cf82f4521e29e39f7bd680440c

**To Reproduce**

`vcpkg install `

**Failure logs**

```
Downloading madler-zlib-v1.3.1.tar.gz
Successfully downloaded madler-zlib-v1.3.1.tar.gz.
-- Extracting source C:/Users/ADIPR/AppData/Local/vcpkg/downloads/madler-zlib-v1.3.1.tar.gz
-- Applying patch 0001-Prevent-invalid-inclusions-when-HAVE_-is-set-to-0.patch
-- Applying patch 0002-build-static-or-shared-not-both.patch
-- Applying patch 0003-android-and-mingw-fixes.patch
-- Using source at D:/Projects/C++/lazygit/vcpkg_installed/vcpkg/blds/zlib/src/v1.3.1-2e5db616bf.clean
-- Configuring x64-windows
-- Building x64-windows-dbg
CMake Error at scripts/cmake/vcpkg_execute_build_process.cmake:134 (message):
    Command failed: C:/msys64/mingw64/bin/cmake.exe --build . --config Debug --target install -- -v -j9
    Working Directory: D:/Projects/C++/lazygit/vcpkg_installed/vcpkg/blds/zlib/x64-windows-dbg
    See logs for more information:
      D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\install-x64-windows-dbg-out.log

Call Stack (most recent call first):
  D:/Projects/C++/lazygit/vcpkg_installed/x64-windows/share/vcpkg-cmake/vcpkg_cmake_build.cmake:74 (vcpkg_execute_build_process)
  D:/Projects/C++/lazygit/vcpkg_installed/x64-windows/share/vcpkg-cmake/vcpkg_cmake_install.cmake:16 (vcpkg_cmake_build)
  C:/Users/ADIPR/AppData/Local/vcpkg/registries/git-trees/3f05e04b9aededb96786a911a16193cdb711f0c9/portfile.cmake:26 (vcpkg_cmake_install)
  scripts/ports.cmake:196 (include)



```

<details><summary>D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\install-x64-windows-dbg-out.log</summary>

```
Change Dir: 'D:/Projects/C++/lazygit/vcpkg_installed/vcpkg/blds/zlib/x64-windows-dbg'

Run Build Command(s): C:/Users/ADIPR/AppData/Local/vcpkg/downloads/tools/ninja/1.10.2-windows/ninja.exe -v -v -j9 install
[1/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\adler32.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\adler32.c
[2/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\crc32.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\crc32.c
[3/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\inffast.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\inffast.c
[4/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\compress.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\compress.c
[5/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\deflate.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\deflate.c
[6/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\gzclose.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\gzclose.c
[7/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\inflate.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\inflate.c
[8/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\gzlib.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\gzlib.c
[9/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\gzwrite.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\gzwrite.c
[10/18] RC D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\win32\zlib1.rc CMakeFiles\zlib.dir\win32\zlib1.rc.res.d CMakeFiles\zlib.dir\win32\zlib1.rc.res "Note: including file: " "D:/Softwares/VSC/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64/cl.exe" C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\rc.exe -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -I D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -I D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg -c65001 /DWIN32 -D_DEBUG /fo CMakeFiles\zlib.dir\win32\zlib1.rc.res D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\win32\zlib1.rc
FAILED: CMakeFiles/zlib.dir/win32/zlib1.rc.res 
RC D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\win32\zlib1.rc CMakeFiles\zlib.dir\win32\zlib1.rc.res.d CMakeFiles\zlib.dir\win32\zlib1.rc.res "Note: including file: " "D:/Softwares/VSC/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64/cl.exe" C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\rc.exe -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -I D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -I D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg -c65001 /DWIN32 -D_DEBUG /fo CMakeFiles\zlib.dir\win32\zlib1.rc.res D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\win32\zlib1.rc
Microsoft (R) Windows (R) Resource Compiler Version 10.0.10011.16384
Copyright (C) Microsoft Corporation.  All rights reserved.

fatal error RC1107: invalid usage; use RC /? for Help
[11/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\uncompr.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\uncompr.c
[12/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\inftrees.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\inftrees.c
[13/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\infback.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\infback.c
[14/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\gzread.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\gzread.c
[15/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\zutil.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\zutil.c
[16/18] D:\Softwares\VSC\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe   -DNO_FSEEKO -DZLIB_DLL -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean -ID:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\x64-windows-dbg /nologo /DWIN32 /D_WINDOWS /utf-8 /MP  /D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\zlib.dir\trees.c.obj /FdCMakeFiles\zlib.dir\ /FS -c D:\Projects\C++\lazygit\vcpkg_installed\vcpkg\blds\zlib\src\v1.3.1-2e5db616bf.clean\trees.c
ninja: build stopped: subcommand failed.
```
</details>

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "name": "lazygit",
  "version": "0.1.0",
  "builtin-baseline": "f4ea42fa5c2b993cf2b75725331616999e2e34d1",
  "dependencies": [
    {
      "name": "curl",
      "version>=": "8.14.1"
    },
    {
      "name": "curlcpp",
      "version>=": "3.1"
    }
  ]
}

```
</details>
