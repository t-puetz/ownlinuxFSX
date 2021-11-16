# ownlinuxFSX

My build of the new LFS 10 (based on SVN 2020707 multilib fork by Thomas). LFS 10 will have breacking changes. See README!

### Motivation

*Over 3 and a half years* I waited since my last successful build of Cross LFS x86_64 multilib. A few days ago it was announced in a changelog that for the upcoming LFS 10.0 will divide Chapter 5 into three parts. It will from now on build a cross-compilation capable toolchain to easier isolate the host toolchain from the to-be-build LFS system. This is my change to refresh my very own build of a multilib.

On said commit:

* LFS changelog: 2020-06-16
    *[bdubbs] - Split Chapter 5 into three separate chapters. Implement a new method of cross-building the LFS tool chain and other tools to simplify the method of isolating the new system from the original host. This will be the start of LFS-10.0.*

    Explanation on why it was decided to use cross compilation as the stanard in LFS from now on:

    From Chaper 5.ii Toolchain Technical Notes

    *The build process is based on the process of cross-compilation. Cross-compilation is normally used for building a compiler and its toolchain for a machine different from the one that is used for the build. This is not strictly needed for LFS, since the machine where the new system will run is the same as the one used for the build. But cross-compilation has the great advantage that anything that is cross-compiled cannot depend on the host environment.* 


### Why not use standard LFS ?

*Why multilib you ask? Why are you not content with the standard LFS?*

Because even in 2020, 17 years after the first 64-bit CPU the AMD Athlon64 was released, we still need multilib systems to use our system as a daily driver. Gaming needs it. Video, Sound and Music software needs it. Virtualization needs it and probably a lot of proprietary, closed software still needs it.

**A pure 64-bit system nowadays is as exotic as a daily driver nowadays as the use of IPv6still is in networking.**

### Will there be something different of YOUR build of this particular LFS fork? 

Yes!

* I like **bleeding edge and most of all modern software** so after I first established and noted down how to build this system, I will probably start experimenting with more up-to-date packages if those are available. I will especially **always use the newest kernel** and provide options for using release candidates of upcoming kernel versions. So this build will probably start with kernel 5.8rc5 or rc6.

* I will write/script the installation of this system in Python.

* I will support **UEFI only.**

* Since I think that AMD really took off since 2017 with their Ryzen CPU series and although they still do not have a chance against NVIDIA GPUs I think they are a company that actually respect the community way more than NVIDIA does (NVIDIA shits on providing open specifications so the nouveau team can actually build an alternative open source driver. Also NVIDIA messed up Wayland usage for all users of a NVIDIA GPU (As I love to repeat Linus Torvald famous words: **"Fuck you NVIDIA!"**), I will only test and develop this for pure AMD (AMD APU/CPU and/or AMD GPU) systems.
I know AMD has PSP, but apparently it is not as bad as Intel's Management Engine I guess since it does not have a full working network stack in Ring -3.
That said if collaborators join this projects and make commits to support **NVIDIA/Intel systems I will accept the pull requests.**

* I am thinking of making this a **Wayland** only build, but for the time being with xwayland support of course. Because otherwise gaming and video editing would be a PITA and defy my reasons why I am building a multilib system in the first place.

* I will **NOT SUPPORT SYSTEMD**. OpenRC and runit will be the only init systems I will support for now. I know there are GNU Shepard, sinit, finit, s6 and probably many more. But I know too little about them.
**Pull requests for systemD support will be denied.**

* **The standard DE for this distro will be sway**. Maybe I will support GNOME to have a floating WM. I will use the Artix implementation of GNOME since it managed to install a GNOME without it needing systemd. However if MATE or Cinnamon turn out to have good Wayland support (in the near future) I will use those.
Maintainers are welcome to add support for any other DE.

* I will probably preinstall **docker (or podman, don't know, yet) and K8s. MAYBE LXC/LXD.**

* Although I aim for full **ZFS** support this will not be on top of my list. I wannt to support ext4 first, then btrfs, then XFS+Stratis, then ZFS. Maybe for the keen people among us bcachefs would be nice to play with. LVM support and encrpytion will come also, but also way later.

* No swap support for now.

* I would like to get rid of Pulseaudio and use **Pipewire**, but although Pipewire took off in the last year and I plaid with it and wireplumber and I got the sound to work, it is stil lnot ready for end-user usage. So probably I will **use Pulseaudio until Pipewire has a good state** (which I hope will be in 2021)

* Also I will **not use Avahi** (need to check how I need to compile cupos so that it won't need it.)

* **elogind** will be the default session manager. I will accept pull requests for consolekit2 patches.

* If I use a display manager at all it will be lightdm. Otherwise I plan on just using PTY as login.

* If I find out what nowadays is the standalone version of gummiboot, which used to be goofiboot after gummiboot god sucked into the never ending software accumulating monster called systemd and was renamed systemd-boot I will probably ditch GRUB. But GRUB will be the default until then.

* Anything needed for using **BPF**, like bcc and bpftrace and python-bcc will be preinstalled and activated in the kernel. Also I will activate the bpfilter module. **bpfilter is the BPF based firewall that will finally get rid of iptables.
Until bpfilter has a good state I will preinstall nftables.**

* Wireguard will be preinstalled. It is time for OpenVPN to die. Wireguard is more modern, more lightweight (As Linus Towarlds stated (paraphrased): **Compared to the mess of OpenVPN/IPSec, WireGuard's code is a piece of art.**) and probably more secure and most importantly ridiculously easy to setup and configure.

* After this system is up and running I will make it installable first from an Artix Linux Live ISO which will come with everything needed to install ownlinuxFSX. When I figures out how to build my own ISO I will make the ISO based on ownlinuxFSX as well.

* An extreme ambition I have for the future is to offer the LLVM toolchain as an alternative to the GNU toolchain. **For now I will offer that the kernel can be compiled with both GCC and clang!**

* OpenDNS will be preconfigured if the user does not chose its home router as the DNS.

* Both iwconfig and wpa_supplicant (maybe also dialog-wifi) will be offered for WiFI configuration.

* The kernel will be based on the Artix Linux config of the same version. It will not be a minimalistic kernel by default. It will provide a lot of modules so it can run on many systems. If you want a customized kernel you have to edit the build scripts yourself.

