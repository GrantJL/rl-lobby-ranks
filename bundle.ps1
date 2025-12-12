<#
.SYNOPSIS
Bundle plugin submission
.DESCRIPTION
Bundle plugin submission
#>

param
(
	[Parameter(Mandatory=$true)]
	[string]$VersionString,
	[string]$Repo="."
)

$InformationPreference = 'Continue'

$BuildDir="$Repo\build"
$BundleDir="$BuildDir\bundle"
$Name="rl-lobby-ranks-$VersionString"

$BundleTo="$BundleDir\$Name"

Remove-Item -Path "$BundleTo" `
		-Force `
		-Recurse `
		-ProgressAction:SilentlyContinue `
		-InformationAction:SilentlyContinue `
		-WarningAction:SilentlyContinue `
		-ErrorAction:SilentlyContinue
New-Item -ItemType "Directory" -Path "$BundleTo" -Force | Out-Null

$BundleToSrc="$BundleTo\source"
robocopy "$Repo" "$BundleToSrc" *.sln README.md

robocopy "$Repo\lobby-ranks" "$BundleToSrc\lobby-ranks" *.cc LICENSE *.h *.cpp *.inl *. *.cpp *.h *.props *.rc *.vcxproj *.vcxproj.filters *.ps1 *.aps README.md

robocopy "$Repo\lobby-ranks\fmt" "$BundleToSrc\lobby-ranks\fmt" *.cc *.cpp *.h  *.inl LICENSE /s

robocopy "$Repo\lobby-ranks\IMGUI" "$BundleToSrc\lobby-ranks\IMGUI" *.cc *.cpp *.h  *.inl LICENSE /s

robocopy "$Repo\lobby-ranks\jsoncpp" "$BundleToSrc\lobby-ranks\jsoncpp" LICENSE
robocopy "$Repo\lobby-ranks\jsoncpp\include" "$BundleToSrc\lobby-ranks\jsoncpp\include" *.h /s
robocopy "$Repo\lobby-ranks\jsoncpp\src\lib_json" "$BundleToSrc\lobby-ranks\jsoncpp\src\lib_json" *.cc *.cpp *.h  *.inl LICENSE /s

$BundleToPlugin="$BundleTo\plugins"
robocopy "$Repo\plugins" "$BundleToPlugin" "lobby-ranks.dll"

$ZipDir="$Repo\dist"
$ZipOut = "$ZipDir\$Name.zip"
$ZipOut = Join-Path (Resolve-Path .) $ZipOut

try {
	Push-Location "$BundleDir"
	Remove-Item -Path "$ZipOut" `
		-Force `
		-Recurse `
		-ProgressAction:SilentlyContinue `
		-InformationAction:SilentlyContinue `
		-WarningAction:SilentlyContinue `
		-ErrorAction:SilentlyContinue
	7z a "$ZipOut" "$name/" -r
}
finally {
	Pop-Location
}

Remove-Item -Path "$BundleDir" `
		-Force `
		-Recurse `
		-ProgressAction:SilentlyContinue `
		-InformationAction:SilentlyContinue `
		-WarningAction:SilentlyContinue `
		-ErrorAction:SilentlyContinue
