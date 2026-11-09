@echo off
echo Building AI8051U-SI24R1 with Keil C51 (8-bit) ...
if exist Objects\AI8051U-SI24R1.Hex del Objects\AI8051U-SI24R1.Hex
"%LOCALAPPDATA%\Keil_v5_C51\UV4\UV4.exe" -b "AI8051U-SI24R1.uvproj" -j0 -o build_uvision.log
if exist Objects\AI8051U-SI24R1.Hex (
  for %%A in (Objects\AI8051U-SI24R1.Hex) do echo BUILD OK - HEX size %%~zA bytes
) else (
  echo BUILD FAILED - see build_uvision.log
)
pause
