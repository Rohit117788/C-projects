Write-Host "Compiling C++ GUI Login System..." -ForegroundColor Green

if (Get-Command cl -ErrorAction SilentlyContinue) {
    Write-Host "Using Visual Studio Compiler..." -ForegroundColor Yellow
    cl /EHsc main.cpp user.cpp admin.cpp email_sender.cpp /link user32.lib comctl32.lib wininet.lib /out:LoginSystem.exe
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Compilation successful!" -ForegroundColor Green
        $run = Read-Host "Run the application? (y/n)"
        if ($run -eq 'y' -or $run -eq 'Y') {
            .\LoginSystem.exe
        }
    } else {
        Write-Host "Compilation failed!" -ForegroundColor Red
    }
} else {
    Write-Host "Trying MinGW..." -ForegroundColor Yellow
    
    if (Get-Command g++ -ErrorAction SilentlyContinue) {
        Write-Host "Using MinGW Compiler..." -ForegroundColor Yellow
        g++ -o LoginSystem.exe main.cpp user.cpp admin.cpp email_sender.cpp -luser32 -lcomctl32 -lgdi32 -lwininet
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Compilation successful!" -ForegroundColor Green
            $run = Read-Host "Run the application? (y/n)"
            if ($run -eq 'y' -or $run -eq 'Y') {
                .\LoginSystem.exe
            }
        } else {
            Write-Host "Compilation failed!" -ForegroundColor Red
        }
    } else {
        Write-Host "No C++ compiler found!" -ForegroundColor Red
        Write-Host "Install Visual Studio or MinGW" -ForegroundColor Yellow
    }
} 