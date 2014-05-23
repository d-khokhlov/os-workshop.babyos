@echo off

pushd
cd /d %~dp0

if not exist images (
    mkdir images
)

bfi -t=144 -l=BabyOS -f=images\babyos.ima scripts\%1 ..\targets
start "" "C:\Program Files\Oracle\VirtualBox\VirtualBox.exe" --startvm "64150db7-c92e-4290-b528-e39675aa0d44"

popd