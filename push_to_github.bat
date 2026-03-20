@echo off
echo Pushing optimizations to GitHub...

cd /d "%~dp0"

git init
git add -A
git commit -m "Performance optimizations: increased particle limit, vector pre-allocation"

git branch -M main
git remote add origin https://github.com/matthewvaishnav/out-of-orbit.git
git push -u origin main

echo Done!
pause
