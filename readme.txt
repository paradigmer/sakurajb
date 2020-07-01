*How to build?
First, build idevicerestore, CBPatcher, iPwnder32, xpwn and iBoot32Patcher.

*How to use?
1, unzip ipsw
Unzip the 14G61 ipsw for iphone5.

2, patching bootloader
Apply a patch to the bootloader.

** iBSS **
cp path/to/iBSS.iphone5.RELEASE.dfu iBSS.iphone5.RELEASE.dfu
xpwntool iBSS.iphone5.RELEASE.dfu iBSS.iphone5.RELEASE.dec
bspatch iBSS.iphone5.RELEASE.dec pwnedibss iphone5_14G61.bundle/iBSS.iphone5.RELEASE.patch 
xpwntool pwnedibss path/to/iBSS.iphone5.RELEASE.dfu -t iBSS.iphone5.RELEASE.dfu

** iBEC **
cp path/to/iBEC.iphone5.RELEASE.dfu iBEC.iphone5.RELEASE.dfu
xpwntool iBEC.iphone5.RELEASE.dfu iBEC.iphone5.RELEASE.dec
bspatch iBEC.iphone5.RELEASE.dec pwnedibec iphone5_14G61.bundle/iBEC.iphone5.RELEASE.patch 
xpwntool pwnedibec path/to/iBEC.iphone5.RELEASE.dfu -t iBEC.iphone5.RELEASE.dfu

** Kernelcache **
cp path/to/kernelcache.release.iphone5 kernelcache.release.iphone5
xpwntool kernelcache.release.iphone5 kernelcache
CBPatcher kernelcache pwnkc 10.3.4 --nosb
xpwntool pwnkc path/to/kernelcache.release.iphone5 -t kernelcache.release.iphone5

** new iBoot **
!! This step is required if you use De Rebus Antiquis for iBoot exploit !!
cp path/to/iBoot.iphone5.RELEASE.img3 iBoot.iphone5.RELEASE.img3
xpwntool iBoot.iphone5.RELEASE.img3 iBoot.iphone5.RELEASE.dec
iBoot32Patcher iBoot.iphone5.RELEASE.dec pwnediboot --rsa --debug --local-boot --boot-partition9 --boot-ramdisk
xpwntool pwnediboot path/to/iBoot.iphone5.RELEASE.img3 -t iBoot.iphone5.RELEASE.img3
echo "0000010: 586F6269" | xxd -r - path/to/iBoot.iphone5.RELEASE.img3
echo "0000020: 586F6269" | xxd -r - path/to/iBoot.iphone5.RELEASE.img3

3, pwnding restore-ramdisk
cp -a path/to/058-75249-065.dmg 058-75249-065.dmg 
xpwntool path/to/058-75249-065.dmg ramdisk.dmg
hdiutil resize ramdisk.dmg -size 30m

hfsplus ramdisk.dmg mv usr/local/share/restore/options.n42.plist usr/local/share/restore/options.n42.plist_
hfsplus ramdisk.dmg add src/options.n42.plist usr/local/share/restore/options.n42.plist

!! Note: This ASR patch is incomplete 
!! If you really want to restore, use the iOS 9.3.5 patched ASR.
hfsplus ramdisk.dmg extract usr/sbin/asr
bspatch asr asr_patched iphone5_14G61.bundle/asr.patch
hfsplus ramdisk.dmg mv usr/sbin/asr usr/sbin/asr_orig
hfsplus ramdisk.dmg add asr_patched usr/sbin/asr
hfsplus ramdisk.dmg chmod 755 usr/sbin/asr

!! Next step is required if you use De Rebus Antiquis for iBoot exploit !!
hfsplus ramdisk.dmg untar src/binJX.tar
hfsplus ramdisk.dmg add exploit/ramdiskI_n42_iBoot-1940.10.58 ramdiskI.dmg
hfsplus ramdisk.dmg mv sbin/reboot sbin/reboot_
hfsplus ramdisk.dmg add exploit/exploit.sh sbin/reboot
hfsplus ramdisk.dmg chmod 755 sbin/reboot
hfsplus ramdisk.dmg add jb/sbpatcher32/rd_kc rd_kc 
hfsplus ramdisk.dmg chmod rd_kc

xpwntool ramdisk.dmg path/to/058-75249-065.dmg -t 058-75249-065.dmg

4, jailbreak filesystem
dmg extract 058-74968-065.dmg rootfs.dmg
hdiutil resize rootfs.dmg -size 2.75g

/* inject jb basesystem */
hfsplus rootfs.dmg untar jb/src/Cydia-10.tar
hfsplus rootfs.dmg add jb/src/launchctl bin/launchctl
hfsplus ramdisk.dmg chmod 755 bin/launchctl
hfsplus rootfs.dmg add jb/src/dirhelper usr/libexec/dirhelper
hfsplus ramdisk.dmg chmod 755 usr/libexec/dirhelper
hfsplus rootfs.dmg add jb/src/com.apple.springboard.plist private/var/mobile/Library/Preferences/com.apple.springboard.plist
hfsplus ramdisk.dmg mv private/etc/fstab private/etc/fstab_
hfsplus rootfs.dmg add jb/src/fstab private/etc/fstab 
hfsplus ramdisk.dmg chmod 755 private/etc/fstab

/* build&inject jailbreak */
/** extract xpcd_cache **/
hfsplus ramdisk.dmg extract System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib jb/xpcd/xpcd_cache.dylib
cd jb/xpcd
lipo -thin armv7s xpcd_cache.dylib -output xpcd_cache_armv7s.dylib

/** patch xpcd_cache **/
./xpcd_cache_patch xpcd_cache_armv7s.dylib xpcd_cache_armv7s_patched.dylib

/** build reloader **/
cd jb/reloader
xcrun -sdk iphoneos clang -arch armv7 -framework CoreFoundation reload.c -o reloader
codesign -f -s - -i com.apple.CrashHousekeeping reloader

/** build sbpatcher32 **/
cd jb/sbpatcher32
xcrun -sdk iphoneos clang sbpathcer32.c patchfinder.o -arch armv7 -framework CoreFoundation -o sbpatcher32
codesign -f -s - -i com.apple.rtbuddyd --entitlements tfp0.plist sbpatcher32

/** injecting jailbreak filesystem **/
hfsplus rootfs.dmg mv System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib_
hfsplus rootfs.dmg add jb/xpcd/xpcd_cache_armv7s_patched.dylib System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib
hfsplus rootfs.dmg chmod 755 System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib

hfsplus rootfs.dmg mv usr/libexec/CrashHousekeeping usr/libexec/CrashHousekeeping_
hfsplus rootfs.dmg add jb/reloader/reloader usr/libexec/CrashHousekeeping
hfsplus rootfs.dmg chmod 755 usr/libexec/CrashHousekeeping

hfsplus rootfs.dmg add jb/sbpatcher32/sbpatcher32 usr/libexec/rtbuddyd
hfsplus rootfs.dmg chmod 755 usr/libexec/rtbuddyd

/* moveing launchdaemons */
hfsplus rootfs.dmg mv System/Library/LaunchDaemons/com.apple.softwareupdateservicesd.plist private/var/tmp/com.apple.softwareupdateservicesd.plist
hfsplus rootfs.dmg mv System/Library/LaunchDaemons/com.apple.mobile.softwareupdated.plist private/var/tmp/com.apple.mobile.softwareupdated.plist
hfsplus rootfs.dmg mv System/Library/LaunchDaemons/com.apple.SpringBoard.plist Library/LaunchDaemons/com.apple.SpringBoard.plist
hfsplus rootfs.dmg mv System/Library/LaunchDaemons/com.apple.mobile.lockdown.plist Library/LaunchDaemons/com.apple.mobile.lockdown.plist
hfsplus rootfs.dmg mv System/Library/LaunchDaemons/com.apple.backboardd.plist Library/LaunchDaemons/com.apple.backboardd.plist

/* building dmg */
dmg build rootfs.dmg 058-74968-065.dmg

5, build ipsw

!! This step is required if you use De Rebus Antiquis for iBoot exploit !!
/* change buildmanifest */
Since ios 10.3, "Firmware/all_flash/*/manifest" no longer refers to firmware flashing to nand_firmware. But it can be controlled by BuildManifest.
Refer to the "manifest/BuildManifest.plist", in ipsw and modify BuildManifest.plist in ipsw to match the modified firmware.

"iBootX" is iOS 10 iBoot with the patch applied earlier.
If using DRA, img3 that flashes nand_firmware needs to be replaced with iOS 7 except iBootX and DeviceTree.

If all changes are applied, zip them.

6, restore cfw
/* Use ipwnder32 to enter pwned DFU mode. */
ipwnder32 -p
ipwnder32 -f path/to/ibss.pwned.dfu

/* Restore using idevicerestore. If applying DRA, impersonate iOS 10.3.4 blob with iOS 7 blob. */
idevicerestore -e -w cfw.ipsw
