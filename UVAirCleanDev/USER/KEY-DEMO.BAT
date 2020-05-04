SET PATH=D:\Keil_v5\ARM\ARMCC\Bin;C:\Python27\;C:\Python27\Scripts;d:\Program Files\Espressif\ESP-IDF Tools\mconf-idf;d:\Program Files\Espressif\ESP-IDF Tools\tools\bin;C:\Program Files\Espressif\ESP-IDF Tools\mconf-idf;C:\Program Files\Espressif\ESP-IDF Tools\tools\bin;C:\Program Files\Espressif\ESP-IDF Tools\mconf-idf;C:\Program Files\Espressif\ESP-IDF Tools\tools\bin;C:\Program Files\Espressif\ESP-IDF Tools\mconf-idf;C:\Program Files\Espressif\ESP-IDF Tools\tools\bin;C:\Program Files\Espressif\ESP-IDF Tools\mconf-idf;C:\Program Files\Espressif\ESP-IDF Tools\tools\bin;C:\Program Files\Espressif\ESP-IDF Tools\mconf-idf;C:\Program Files\Espressif\ESP-IDF Tools\tools\bin;d:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\bin;d:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\lib;d:\MentorGraphics\9.5PADS\MGC_HOME.ixn\bin;d:\MentorGraphics\9.5PADS\MGC_HOME.ixn\lib;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\IDM Computer Solutions\UltraEdit;C:\Program Files\CMake\bin;C:\Program Files\Git\cmd;C:\Program Files\dotnet\;C:\Users\Kevin\AppData\Local\Programs\Python\Python37\Scripts\;C:\Users\Kevin\AppData\Local\Programs\Python\Python37\;C:\Users\Kevin\AppData\Local\Microsoft\WindowsApps;D:\altera\13.1\modelsim_ase\win32aloem;D:\altera\13.1\modelsim_ae\win32aloem;d:\MentorGraphics\9.5PADS\SDD_HOME\CAMCAD
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=KEY-DEMO
SET CPU_CLOCK=0x044AA200
"D:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\output\startup_stm32f10x_hd._ia"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\main.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\led.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\key.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\pwm_out.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\adc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\timer3.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_gpio.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_rcc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\misc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_tim.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_adc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_dma.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_exti.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_flash.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\core_cm3.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\system_stm32f10x.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\rtx_conf_cm.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\Output\UvAirCleanDev.lnp"
"D:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" "..\Output\UvAirCleanDev.axf" --i32combined --output "..\Output\UvAirCleanDev.hex"
