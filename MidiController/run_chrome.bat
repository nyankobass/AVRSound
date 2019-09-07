REM Chromeへのpath
SET CHROME=C:\Program Files (x86)\Google\Chrome\Application\chrome.exe
SET DIR=%~dp0

start "" "%CHROME%" "%DIR%/index.html"
