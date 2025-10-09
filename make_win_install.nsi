!define APPNAME "ttcp"
!define APPDIR "$PROGRAMFILES\ttcp"

OutFile "bin/ttcp-setup.exe"
InstallDir ${APPDIR}
RequestExecutionLevel admin

Section "Install"
  SetOutPath $INSTDIR
  File "bin/ttcp.exe"
  WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" "$INSTDIR;$PATH"
SectionEnd
