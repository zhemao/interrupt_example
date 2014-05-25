# Cyclone V Avalon Interrupts Example

A demonstration of how to send an interrupt from the Cyclone V FPGA to the
HPS and handle the IRQ in Linux.

Includes Verilog description which triggers interrupt when a switch or
push button changes state, as well as Linux kernel sysfs driver and userspace
command-line program.
