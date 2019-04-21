@echo off
echo:args %1 %2 %3 > log.txt
:: Script to access the git username/password via Environment variables
echo:username=%GITHUB_USERNAME%
echo:password=%GITHUB_PASSWORD%
