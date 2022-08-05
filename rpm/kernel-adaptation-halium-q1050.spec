# Device details
%define device halium-qx1050

# Kernel target architecture
%define kernel_arch arm64

%define kcflags "KCFLAGS=-Wno-misleading-indentation -Wno-format -Wno-bool-operation -Wno-unused-variable -Wno-unused-result -Wno-pointer-to-int-cast -Wno-unused-value -Wno-sequence-point -Wno-return-type -Wno-implicit-int -Wno-bool-compare -Wno-maybe-uninitialized"

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
%define defconfig sfos-bengal-perf_defconfig

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

%define deviceinfo_dtb vendor/qcom/bengal.dtb
%define deviceinfo_dtbo vendor/qcom/bengal-idp-overlay.dtbo
%define deviceinfo_flash_pagesize 4096
%define deviceinfo_flash_offset_base 0x00000000
%define deviceinfo_flash_offset_kernel 0x00008000
%define deviceinfo_flash_offset_ramdisk 0x01000000
%define deviceinfo_flash_offset_second 0x00f00000
%define deviceinfo_flash_offset_tags 0x00000100
%define deviceinfo_flash_offset_dtb 0x01f00000
%define deviceinfo_kernel_cmdline console=ttyMSM0,115200n8 earlycon=msm_geni_serial,0x4a90000 androidboot.hardware=qcom androidboot.console=ttyMSM0 androidboot.memcg=1 lpm_levels.sleep_disabled=1 video=vfb:640x400,bpp=32,memsize=3072000 msm_rtb.filter=0x237 service_locator.enable=1 swiotlb=2048 loop.max_part=7 buildvariant=userdebug console=tty0
%define deviceinfo_bootimg_os_version 11
%define deviceinfo_bootimg_os_patch_level 2021-10-01
%define deviceinfo_bootimg_header_version 2
%define deviceinfo_bootimg_partition_size 100663296
%define deviceinfo_rootfs_image_sector_size 4096

%include kernel-adaptation-simplified/kernel-adaptation-simplified.inc
