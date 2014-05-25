# Cyclone V Avalon Interrupts Example

A demonstration of how to send an interrupt from the Cyclone V FPGA to the
HPS and handle the IRQ in Linux.

Includes Verilog description which triggers interrupt when a switch or
push button changes state, as well as Linux kernel sysfs driver and userspace
command-line program.

## Setup Instructions

## Programming the FPGA

1. Go into the fpga directory.
2. Open interrupt\_example.qpf in quartus.
3. Open the soc\_system.qsys file in Qsys and generate the system.
4. Compile the SOF file and program it onto the FPGA.

## Loading the Kernel Driver

1. Go into the software/kernel directory and run `make`.
2. Copy the fpga\_uinput.ko file to the SDCard.
3. On the HPS, run `echo 1 > /sys/class/fpga-bridge/lwhps2fpga/enable`.
4. Run `insmod fpga\_uinput.ko`.
5. Make sure that the file "/sys/bus/platform/drivers/fpga\_uinput/fpga\_uinput"

## Running the userspace program

1. Go into the software/userspace directory and run `make`.
2. Copy the `readstate` binary to the SD card.
3. Run the executable on the HPS.
4. Start flipping switches and pushing buttons. The program should print
   out information when an input changes state.
