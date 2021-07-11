@echo on
:: Script to pull down local packages (vs. published packages)
::
:: Usage: ci_local_packages.bat 
::
:: Environment Variables:
::    PKG_BRANCH=<branch>

pushd %WORKSPACE%
::git clone --depth 1 --single-branch --branch master https://github.com/johnttaylor/arduino.git
::orc.py mount -o arduino
::git clone --depth 1 --single-branch --branch master https://github.com/johnttaylor/colony.bsp.renesas.rx.git
::orc.py mount -o colony.bsp.renesas.rx
git clone --depth 1 --single-branch --branch origin/dev_john https://github.com/johnttaylor/nqbp.git
orc.py mount -o nqbp
git clone --depth 1 --single-branch --branch origin/dev_john https://github.com/johnttaylor/colony.bsp.git
orc.py mount -o colony.bsp
git clone --depth 1 --single-branch --branch master https://github.com/johnttaylor/catch.git
orc.py mount -o catch
::git clone --depth 1 --single-branch --branch master https://github.com/johnttaylor/freertos.git
::orc.py mount -o freertos
popd
