https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

Instructions to build and flash:
Make a build folder in the src directory (yes, it should be outside, yes, I will fix)
cd into this build folder
run "cmake .." (This only needs to be run once, in theory)
run "make" (This needs to be run each time you want to flash, this rebuilds the project)
run "openocd -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg -c "program titan.elf verify reset exit"" (note that you need a quote on the end, so the double quote at the end of that line is intentional).  
