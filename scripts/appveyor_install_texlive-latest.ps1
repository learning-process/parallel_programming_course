$ErrorActionPreference = "Stop"

$scriptRoot = (Resolve-Path $(If ($PSScriptRoot) { $PSScriptRoot } Else { "." })).Path
. "${scriptRoot}\install_texlive_windows.ps1" `
    -Repository 'http://mirrors.rit.edu/CTAN/systems/texlive/tlnet' `
    -ProfilePath "${scriptRoot}\texlive2019-win.profile"
