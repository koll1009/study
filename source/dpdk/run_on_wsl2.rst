RUN DPDK ON WSL2
================

WSL2
----
The Windows Subsystem for Linux 2 (WSL2) is a new version of the Windows Subsystem for Linux (WSL).
It's a compatibility layer that enables users to run Linux binaries natively on Windows OS.

The main difference between WSL1 and WSL2 is the architecture.
In WSL2, Microsoft has introduced a real Linux kernel with full system call compatibility.
This means that it's no longer translating Linux system calls into Windows system calls,
but instead runs a full-fledged Linux kernel in a lightweight virtual machine.
This makes WSL2 significantly faster and more compatible than WSL1.

However, WSL2 is not a full Linux distribution. It's designed to provide a Linux-compatible kernel interface,
but it does not include a graphical user interface (GUI) or support for Linux kernel modules out of the box.
Moreover, it doesn't support all features of a typical Linux kernel,
and there are some limitations in terms of interacting with the Windows host and with the system hardware.



UIO
---

UIO, or Userspace I/O, is a framework in the Linux kernel that allows peripheral device drivers to be
handled in userspace. This can be useful for a variety of reasons:

#. It can reduce the complexity of writing a device driver, as it doesn't require knowledge
   of kernel space programming.

#. It allows for the use of libraries and tools that are only available in userspace.

#. It can aid in debugging and development, as a crash in the driver won't bring down the entire system.

UIO works by mapping the device's memory regions into userspace and using interrupts to
communicate with the device. The actual device driver in userspace can then access
the device's memory directly, without having to go through the kernel.

The UIO framework itself is quite simple. The kernel-side component mainly handles
the setup and teardown of the device mappings, and the delivery of interrupts to userspace.
The actual driver, including all the device-specific logic, is implemented in userspace.

This approach can be useful for high-performance applications like networking or graphics,
where bypassing the kernel can reduce latency and increase throughput.
However, it's not suitable for all types of devices, and it requires the device to
be designed in a way that makes it safe for userspace to directly access its memory.

uio_hv_generic
--------------

on a Linux system, DPDK can use UIO or VFIO drivers to bypass the kernel.
However, WSL2 does not currently support VFIO, and the standard UIO drivers like uio_pci_generic
are not available because WSL2 does not provide direct access to PCI devices.

The uio_hv_generic driver is a special case. It is a UIO driver for devices on the VMBus in Hyper-V.
When running Linux in a Hyper-V virtual machine, devices like network adapters show up on the VMBus,
and uio_hv_generic can be used to access them from a user space application.
icrosoft has enabled this driver in the WSL2 kernel configuration, which is why you can use it
to run DPDK applications in WSL2.

Build WSL2 kernel
^^^^^^^^^^^^^^^^^

uio_hv_generic.ko is a built-in dirvers on WSL2 and disabled by default,
It's supposed to be enabled manully using below command:

.. code-block:: bash

   #!/bin/bash
   make menuconfig

Then you can build kernel referring to this article: `Build WSL2 kernel <https://alexkaouris.medium.com/run-your-own-kernel-with-wsl2-21e3143e014e>`_

NOTICE
------

If you want to debug the DPDK examples using remote-wsl, it's supposed to set up the network in bridge mode.
Otherwise, when you subsitude the driver by uio_hv_generic, the remote-wsl will disconnect.