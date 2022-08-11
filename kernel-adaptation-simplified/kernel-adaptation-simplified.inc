%if 0%{!?rpm_device:1}
%define rpm_device %{device}
%endif

%if 0%{!?kernel_arch:1}
%define kernel_arch arm
%endif

# The prjconf should have an ExportFilter like this (mer/sailfish has this):
#   ExportFilter: \.armv7hl\.rpm$ armv8el
# We lie about our architecture and allows OBS to cross-publish this 486 cross-built spec to the armv7hl repos
%if 0%{?device_target_cpu:1}
%define _target_cpu %{device_target_cpu}
%else
%define _target_cpu %{_arch}
%endif

%if 0%{!?defconfig:1}
%define defconfig defconfig
%endif

%if 0%{!?source_directory:1}
%define source_directory linux/
%endif

%if 0%{!?build_Image:1}
%define build_Image 1
%endif

%if 0%{!?build_uImage:1}
%define build_uImage 0
%endif

%if 0%{!?build_zImage:1}
%define build_zImage 0
%endif

%if 0%{!?build_modules:1}
%define build_modules 1
%endif

%define _tmpdir tmp

Name:           kernel-adaptation-%{rpm_device}
Summary:        Kernel adaptation for %{rpm_device}
Group:          System
License:        GPLv2
Version:        2.6
Release:        1
Provides:       kernel
Source:         %{name}-%{version}.tar.bz2
Requires(pre):  kmod >= 9
BuildRequires:  pkgconfig(ncurses)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  mer-kernel-checks
#BuildRequires:  kmod >= 9
#BuildRequires:  perl
#BuildRequires:  fdupes
BuildRequires:  flex
BuildRequires:  bison
BuildRequires:  bc
BuildRequires:  python
BuildRequires:  python3-base
BuildRequires:  openssl
#!BuildIgnore:  rpmlint-mini

%if 0%{?crossbuild:1}
BuildRequires:  cross-%{crossbuild}-gcc
BuildRequires:  cross-%{crossbuild}-binutils
BuildRequires:  cross-%{crossbuild}-as
%endif

%description
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build

rm -rf %{_tmpdir}
mkdir -p %{_tmpdir}

export ARCH=%{kernel_arch}
export LOCALVERSION="-%{device}"

%if 0%{?crossbuild:1}
export CROSS_COMPILE=%{crossbuild}-meego-linux-gnu-
# FIXME? Shouldn't be needed
export PATH=/opt/cross/bin/:$PATH
%endif

defconfig_name=$(basename %{defconfig})
[ -e "./%{defconfig}" ] && cp -f ./%{defconfig} %{source_directory}/arch/%{kernel_arch}/configs/$defconfig_name

%if 0%{?apply_patches:1}
#Apply any patches
echo "***** Applying patches *****"
sh patches/apply-patches.sh %{source_directory}
%endif

echo "***** Cleaning *****"
make -C %{source_directory} O=out clean

echo "***** Making defconfig *****"
make -C %{source_directory} O=out defconfig KBUILD_DEFCONFIG=$defconfig_name

%if 0%{?build_Image:1}
echo "***** Building Image *****"
make -C %{source_directory} -j$(nproc) O=out %{kcflags}
%endif

%if 0%{?build_modules:1}
echo "***** Building Modules *****"
make -C %{source_directory} -j$(nproc) O=out %{kcflags} modules
%endif

%if 0%{?build_dtboimage:1}
echo "***** Building DTBO Image *****"
python2 kernel-adaptation-simplified/mkdtboimg.py create linux/out/dtbo.img %{source_directory}/out/arch/%{kernel_arch}/boot/dts/%{deviceinfo_dtbo}
%endif

ls -lh %{source_directory}/out/arch/%{kernel_arch}/boot/*Image*

echo "***** Building Boot Image *****"
KERNEL=%{source_directory}/out/arch/%{kernel_arch}/boot/Image.gz
DTB=%{source_directory}/out/arch/%{kernel_arch}/boot/dts/%{deviceinfo_dtb}
EXTRA_ARGS=""
RAMDISK=kernel-adaptation-simplified/halium-boot-ramdisk.img
OUT=%{source_directory}/out/arch/%{kernel_arch}/boot/boot.img

if [ "%{deviceinfo_bootimg_header_version}" -le 2 ]; then
    EXTRA_ARGS+=" --base %{deviceinfo_flash_offset_base} --kernel_offset %{deviceinfo_flash_offset_kernel} --ramdisk_offset %{deviceinfo_flash_offset_ramdisk} --second_offset %{deviceinfo_flash_offset_second} --tags_offset %{deviceinfo_flash_offset_tags} --pagesize %{deviceinfo_flash_pagesize}"
fi

if [ "%{deviceinfo_bootimg_header_version}" -eq 2 ]; then
    EXTRA_ARGS+=" --dtb $DTB --dtb_offset %{deviceinfo_flash_offset_dtb}"
fi

python ./kernel-adaptation-simplified/mkbootimg.py --kernel "$KERNEL" --ramdisk "$RAMDISK" --cmdline "%{deviceinfo_kernel_cmdline}" --header_version %{deviceinfo_bootimg_header_version} -o "$OUT" --os_version %{deviceinfo_bootimg_os_version} --os_patch_level %{deviceinfo_bootimg_os_patch_level} $EXTRA_ARGS

if [ -n "%{deviceinfo_bootimg_partition_size}" ]; then
    EXTRA_ARGS=""
    [ -f "$HERE/rsa4096_boot.pem" ] && EXTRA_ARGS=" --key $HERE/rsa4096_boot.pem --algorithm SHA256_RSA4096"
    python2 "./kernel-adaptation-simplified/avbtool" add_hash_footer --image "$OUT" --partition_name boot --partition_size %{deviceinfo_bootimg_partition_size} $EXTRA_ARGS

    if [ -n "$deviceinfo_bootimg_append_vbmeta" ] && $deviceinfo_bootimg_append_vbmeta; then
        python2 "./kernel-adaptation-simplified/avbtool" append_vbmeta_image --image "$OUT" --partition_size "%{deviceinfo_bootimg_partition_size}" --vbmeta_image "$TMPDOWN/vbmeta.img"
    fi
fi

%install
echo "***** Installing *****"
mkdir -p %{buildroot}/boot

%if %{build_Image}
echo "***** Installing Image *****"
cp %{source_directory}/out/arch/%{kernel_arch}/boot/*Image* %{buildroot}/boot/
cp %{source_directory}/out/arch/%{kernel_arch}/boot/dts/%{deviceinfo_dtb} %{buildroot}/boot/ || true
cp %{source_directory}/out/arch/%{kernel_arch}/boot/boot.img %{buildroot}/boot/ || true
%endif

%if 0%{?devicetrees:1}
for dtb in %{devicetrees}; do
	mkdir -p %{buildroot}/boot/$(basename $(dirname $dtb))
	cp linux/out/arch/%{kernel_arch}/boot/dts/$dtb %{buildroot}/boot/$dtb
	cp %{buildroot}/boot/*/*.dtb %{buildroot}/boot/
done
%endif

%if 0%{?build_dtboimage:1}
cp %{source_directory}/out/dtbo.img %{buildroot}/boot/ || true
%endif

%if %{build_modules}
    mkdir -p %{buildroot}/lib/modules
    make -C %{source_directory} O=out modules_install INSTALL_MOD_PATH=%{buildroot}
%endif

# Build filelist
find %{buildroot} ! \( -type d \) | sed -e s,"%{buildroot}",,g> %{_tmpdir}/kernel-adaptation.files

%post
# This oneshot is enabled only during package upgrades, as initial
# installation is done when we flash device.
if [ $1 -ne 1 ] ; then
  add-preinit-oneshot /var/lib/platform-updates/flash-bootimg.sh || :
fi

%files -f %{_tmpdir}/kernel-adaptation.files
