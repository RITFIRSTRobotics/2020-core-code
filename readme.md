# 2020-core-code

Constants, header files, and declarations shared between repositories.

## Usage

When cloning a repository that uses the core code, you must first:
```
git submodule init
git submodule update
cd core
```
The repository will then have the version of core that the submodule points to (may not be the most recent).
If you want to update the version of core to the current revision, run:
```
cd core
git checkout master
git pull origin master
cd ..
git add core
git commit -m "up core to most recent version"
git push
```
