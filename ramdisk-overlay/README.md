# ramdisk-overlay

Files here are appended to the boot ramdisk by kernel-adaptation-simplified
(`find . | cpio -o -H newc | <compressor> >> boot-ramdisk.img`), so they
override the base ramdisk on extraction (last cpio member wins).

## Contents

- `scripts/local-premount/abm-guest-guard` — safety guard for the
  SailfishOS-as-second-OS (ABM / Volla Boot Manager) case. It runs from
  `mountroot()` before the userdata partition is selected, mounted or formatted.
  A guest boot entry carries `systempart=`/`datapart=` on the kernel cmdline;
  the guard refuses to continue (halium `panic`, never returning to mountroot)
  when the entry is misconfigured in a way that would make the guest touch the
  host's internal userdata:
    - `replaceme` still on the cmdline (the install script never finalized the
      entry),
    - `datapart=` missing or not a block device,
    - `datapart=` resolving to the host's internal `userdata` partition.

  Without it, `mountroot()` would auto-detect the internal userdata, mount it,
  format it if it looks wiped, and SailfishOS first-boot home encryption would
  create its LUKS `home.img` there, destroying the primary OS.

- `scripts/local-premount/ORDER` — runs `abm-guest-guard` before the stock
  `resume` premount script. Matches the stock initramfs-tools ORDER otherwise.

The guard is purely additive and device-agnostic (it keys off the cmdline
only), so the same overlay applies to every SD-backed Volla SFOS multiboot
device and survives base-ramdisk bumps. A normal guest boot (valid `datapart=`
that is not the host userdata) and a standalone boot (no `systempart=`) are
unaffected.
