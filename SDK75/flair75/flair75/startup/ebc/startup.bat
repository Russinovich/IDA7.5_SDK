@echo off
sigmake -s -f0 -a0 -o0 "-nStartups of PE files for EBC" pe* pe
zipsig	pe
