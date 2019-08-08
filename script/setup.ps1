mkdir build
Push-Location build
cmake ..

mkdir sample
Push-Location sample
cmake ..\..\sample

Pop-Location
Pop-Location

mkdir bin
