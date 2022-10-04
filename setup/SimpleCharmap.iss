; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Simple Charmap"
#define MyAppVersion "1.0"
#define MyAppPublisher "jager012"
#define MyAppURL "https://github.com/jager012"
#define MyAppExeName "SimpleCharmap.exe"
#define MyAppInternalName "SimpleCharmap"

[Setup]
AppId={{958938DA-BD4F-4407-87F3-466428D22844}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=license.rtf
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputBaseFilename=SimpleCharmapSetup
SetupIconFile=..\assets\icon.ico
UninstallDisplayIcon={app}\SimpleCharmap.exe
Compression=lzma
SolidCompression=yes
WizardStyle=modern
LanguageDetectionMethod=uilanguage
ShowLanguageDialog=auto

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"

[CustomMessages]
en.InstallVC=Install Microsoft Visual C++ 2015-2022 Redistributable
en.RemoveAppSettings=Do you want to remove the application's settings?
it.InstallVC=Installa Microsoft Visual C++ 2015-2022 Redistributable
it.RemoveAppSettings=Rimuovere le impostazioni dell'applicazione?


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\codepoints.dat"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\groups.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\release\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\release\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\release\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: ignoreversion deleteafterinstall; Check: VCRedistNotInstalled

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Description: {cm:InstallVC}; Flags: waituntilterminated postinstall; Check: VCRedistNotInstalled

[Code]
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
  begin
    if(MsgBox(ExpandConstant('{cm:RemoveAppSettings}'), mbConfirmation, MB_YESNO) = IDYES) then
    begin
      RegDeleteKeyIncludingSubkeys(HKCU, 'SOFTWARE\jager012\{#MyAppInternalName}');
      RegDeleteKeyIfEmpty(HKCU, 'SOFTWARE\jager012');
      RegDeleteKeyIncludingSubkeys(HKLM, 'SOFTWARE\jager012\{#MyAppInternalName}');
      RegDeleteKeyIfEmpty(HKLM, 'SOFTWARE\jager012');
    end
  end;
end;

function VCRedistNotInstalled: Boolean;
begin
  Result := false;
  if not RegKeyExists(HKLM, 'SOFTWARE\Classes\Installer\Dependencies\Microsoft.VS.VC_RuntimeAdditionalVSU_amd64,v14') or
     not RegKeyExists(HKLM, 'SOFTWARE\Classes\Installer\Dependencies\Microsoft.VS.VC_RuntimeMinimumVSU_amd64,v14') or
     not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\DevDiv\VC\Servicing\14.0\RuntimeAdditional') or
     not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\DevDiv\VC\Servicing\14.0\RuntimeMinimum') or
     not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64') or 
     not RegKeyExists(HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\X64') then
     Result := true;
end;