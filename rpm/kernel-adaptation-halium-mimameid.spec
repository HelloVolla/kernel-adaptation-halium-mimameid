# Device details
%define device halium-mimameid

# Kernel target architecture
%define kernel_arch arm64

%define kcflags "KCFLAGS=-Wno-misleading-indentation -Wno-format -Wno-bool-operation -Wno-unused-variable -Wno-unused-result -Wno-pointer-to-int-cast -Wno-unused-value -Wno-sequence-point -Wno-return-type -Wno-implicit-int -Wno-bool-compare -Wno-maybe-uninitialized -Wno-duplicate-decl-specifier -Wno-memset-elt-size -Wno-switch-unreachable -Wno-sizeof-pointer-memaccess -Wno-enum-compare -Wno-tautological-compare -Wno-unused-function -Wno-parentheses"

#Compiler to use
##define compiler CC=clang
##define compileropts CLANG_TRIPLE=aarch64-linux-gnu-
%define compiler %{nil}
%define compileropts %{nil}

# Crossbuild toolchain to use
%define crossbuild aarch64

# RPM target architecture, remove to leave it unaffected
# You should have a good reason to change the target architecture
# (like building on aarch64 targeting an armv7hl repository)
%define device_target_cpu aarch64

# Defconfig to pick-up
%define defconfig sfos-gs5_defconfig

# Linux kernel source directory
%define source_directory linux/

# Build modules
%define build_modules 1

# Build Image
%define build_Image 1

# Apply Patches
%define apply_patches 1

##define build_dtboimg 1

# Build and pick-up the following devicetrees
##define devicetrees

#Device Info

%define deviceinfo_dtb mediatek/mt6768.dtb
%define deviceinfo_flash_pagesize 2048
%define deviceinfo_flash_offset_base 0x40000000
%define deviceinfo_flash_offset_kernel 0x00080000
%define deviceinfo_flash_offset_ramdisk 0x07c80000
%define deviceinfo_flash_offset_second 0x00e88000
%define deviceinfo_flash_offset_tags 0x0bc80000
%define deviceinfo_flash_offset_dtb 0x0bc80000
%define deviceinfo_kernel_cmdline bootopt=64S3,32N2,64N2 systempart=/dev/mapper/system
%define deviceinfo_bootimg_os_version 11
%define deviceinfo_bootimg_os_patch_level 2021-11-01
%define deviceinfo_bootimg_header_version 2
%define deviceinfo_bootimg_partition_size 33554432
%define deviceinfo_rootfs_image_sector_size 4096
%define deviceinfo_bootimg_qcdt false

%include kernel-adaptation-simplified/kernel-adaptation-simplified.inc
