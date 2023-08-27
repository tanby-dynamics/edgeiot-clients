param (
    [string]$port = "COM4",
    
    [string]$target = "esp32"
)

Write-Host "Set the target"
idf.py set-target $target

Write-Host "Build the project"
idf.py build

Write-Host "Flash the firmware using the specified port"
idf.py -p $port flash


