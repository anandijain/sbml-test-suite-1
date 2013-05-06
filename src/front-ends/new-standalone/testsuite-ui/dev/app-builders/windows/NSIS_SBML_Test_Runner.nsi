; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "${NAME}"
!define PRODUCT_VERSION "${VERSION}"
!define PRODUCT_PUBLISHER "SBML Team"
!define PRODUCT_WEB_SITE "${WEBSITE}"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${DIST_DIR}\..\src\data\application-icons\windows\Uninstall_SBML_Test_Runner_icon.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Start the ${PRODUCT_NAME}"
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

!include "x64.nsh"

!define PRODUCT_RUN32 "SBML Test Runner - Windows x86.exe"
!define PRODUCT_RUN64 "SBML Test Runner - Windows x64.exe"
!define SM_FOLDER "SBML Test Runner"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${DIST_DIR}\Setup_SBMLTestRunner-${PRODUCT_VERSION}-Windows.exe"
InstallDir "$PROGRAMFILES\SBML\SBML Test Runner"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try

  File "${DIST_DIR}\SBML Test Runner - Windows x86.exe"
  File "${DIST_DIR}\SBML Test Runner - Windows x64.exe"
  File "${DIST_DIR}\..\..\COPYING.txt"
  File "${DIST_DIR}\..\..\LICENSE.txt"
  File "${DIST_DIR}\..\..\README.txt"

  CreateDirectory "$SMPROGRAMS\${SM_FOLDER}"

  ${If} ${RunningX64}
    CreateShortCut "$SMPROGRAMS\${SM_FOLDER}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_RUN64}"
    CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_RUN64}"
  ${Else}
    CreateShortCut "$SMPROGRAMS\${SM_FOLDER}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_RUN32}"
    CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_RUN32}"
  ${EndIf}
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\SBML Test Suite website.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${SM_FOLDER}\SBML Test Suite website\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\${SM_FOLDER}\Uninstall.lnk" "$INSTDIR\Uninstall ${PRODUCT_NAME}.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\Uninstall ${PRODUCT_NAME}.exe"
  ${If} ${RunningX64}
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${PRODUCT_RUN64}"
  ${Else}
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${PRODUCT_RUN32}"
  ${EndIf}
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall ${PRODUCT_NAME}.exe"
  ${If} ${RunningX64}
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_RUN64}"
  ${Else}
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_RUN32}"
  ${EndIf}
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function LaunchLink
  ${If} ${RunningX64}
    ExecShell "" "$INSTDIR\${PRODUCT_RUN64}"
  ${Else}
    ExecShell "" "$INSTDIR\${PRODUCT_RUN32}"
  ${EndIf}
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\SBML Test Suite website.url"
  Delete "$INSTDIR\Uninstall ${PRODUCT_NAME}.exe"
  Delete "$INSTDIR\${PRODUCT_RUN64}"
  Delete "$INSTDIR\${PRODUCT_RUN32}"
  Delete "$INSTDIR\COPYING.txt"
  Delete "$INSTDIR\LICENSE.txt"
  Delete "$INSTDIR\README.txt"

  Delete "$SMPROGRAMS\${SM_FOLDER}\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${SM_FOLDER}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${SM_FOLDER}\SBML Test Suite website.lnk"

  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"

  RMDir /r "$SMPROGRAMS\${SM_FOLDER}"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
