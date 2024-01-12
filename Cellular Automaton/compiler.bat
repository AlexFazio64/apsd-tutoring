@echo off
gcc %1 -I %MSMPI_INC% -L %MSMPI_LIB64% -lmsmpi -o %2