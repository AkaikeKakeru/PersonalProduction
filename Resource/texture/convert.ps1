$files = Get-Item *.png

foreach($f in $files) {
    Start-Process -FilePath TextureConverter.exe -ArgumentList $f, "-ml", "3", -Wait
}

pause
