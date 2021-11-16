#include "olfsx_ashostlfs.h"
#include "olfsx_ashostroot.h"
#include <sys/wait.h>

void set_env_vars_lfs_user()
{
    setenv("USER", "lfs", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("HOME", "/home/lfs", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("TERM", "xterm-256color", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("PS1", "\\u:\\w\\$ ", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("LFS",  "/mnt/lfs", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("LC_ALL", "POSIX", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("LFS_TGT", "x86_64-lfs-linux-gnu", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("LFS_TGT32", "i686-lfs-linux-gnu", ENV_VAR_WRITE_PROTECT_OFF);
    setenv("PATH", "/mnt/lfs/tools/bin/bin:/usr/bin", ENV_VAR_WRITE_PROTECT_OFF);

    set_makeflags_env_var();
    set_ninjajobs_env_var();
}

void write_lfs_bashrc() {
    char* content = "set +h\numask 022\nLFS=/mnt/lfs\nLC_ALL=POSIX\nLFS_TGT=x86_64-lfs-linux-gnu\nLFS_TGT32=i686-fs-linux-gnu\nPATH=/usr/bin\n[ ! -L /bin ] && PATH=/bin:$PATH\nPATH=$LFS/tools/bin:$PATH\nexport LFS LC_ALL LFS_TGT LFS_TGT32 PATH\n";
    mode_t mode = 0644;
    char* pathname = "/home/lfs/.bashrc";

    write_create_file(pathname, mode, content);
    chown_file_lfs_lfs(pathname);
}

void write_lfs_bash_profile() {
    char* content = "exec env -i HOME=$HOME TERM=$TERM PS1=\'\\u:\\w\\$ \' /bin/bash\n";
    mode_t mode = 0644;
    char* pathname = "/home/lfs/.bash_profile";
   
    write_create_file(pathname, mode, content);
    chown_file_lfs_lfs(pathname);
}

void chown_lfs_root_subfolders_lfs_lfs(char** root_lfs_subfolders)
{
    for(u_int8_t i = 0; i < 13; i++) {
        chown_file_lfs_lfs(root_lfs_subfolders[i]);
    }

    u_int8_t reverse_index = 0;
    for(u_int8_t i = 0; i < 13; i++) {
        if(i >= 0 && i < 13)
            reverse_index = ((i - 13) * -1) - 1;
        else if(i == 13)
            reverse_index = 0;

        free_valid_ptr(root_lfs_subfolders[reverse_index]);
    }
}

meta_build_info* get_binutils_compile_cx_toolchain_meta_data() 
{
    meta_build_info* binutils_meta = malloc(1 * sizeof(meta_build_info)); 
    exit_w_one_on_failed_xalloc(binutils_meta);
    
    binutils_meta->lfs_stage         = "compile_cx_toolchain";
    binutils_meta->package_meta_name = "binutils_cx_toolchain_stg_one";
    binutils_meta->package_name      = "binutils";
    binutils_meta->package_version   = "do-something-here-to-get-version-dynamically"; //TODO
    binutils_meta->build_system      = "make-configure-no-autotools";
    binutils_meta->configure_tool    = "configure";
    binutils_meta->build_tool        = "make";
    binutils_meta->install_tool      = "make";
    
    return binutils_meta;
}

build_path* get_binutils_compile_cx_toolchain_build_path(meta_build_info* binutils_compile_cx_toolchain_meta_data) 
{
    build_path*  binutils_build_path = malloc(1 * sizeof(build_path));
    exit_w_one_on_failed_xalloc(binutils_build_path);
    
    binutils_build_path->commands_pre_configure     = 1;
    binutils_build_path->commands_configure         = 1;
    binutils_build_path->commands_build             = 1;
    binutils_build_path->commands_install           = 1;
    binutils_build_path->commands_post_install      = 1;
    
    binutils_build_path->meta_data      = binutils_compile_cx_toolchain_meta_data;
    
    binutils_build_path->pre_configure  = malloc(binutils_build_path->commands_pre_configure * sizeof(char*));
    exit_w_one_on_failed_xalloc(binutils_build_path->pre_configure);
    char** binutils_pre_configure       = binutils_build_path->pre_configure;
    binutils_pre_configure[0]           = "mkdir binutils && tar xvf binutils*.tar.* -C binutils --strip-components 1 && mkdir -pv binutils/build";
    
    binutils_build_path->configure      = malloc(binutils_build_path->commands_configure * sizeof(char*));
    exit_w_one_on_failed_xalloc(binutils_build_path->configure);
    char** binutils_configure           = binutils_build_path->configure;
    binutils_configure[0]               = "cd /mnt/lfs/sources/binutils/build && ../configure --prefix=$LFS/tools --with-sysroot=$LFS --target=$LFS_TGT --disable-nls --disable-werror";
    
    binutils_build_path->build          = malloc(binutils_build_path->commands_build * sizeof(char*));
    exit_w_one_on_failed_xalloc(binutils_build_path->build);
    char** binutils_build               = binutils_build_path->build;
    binutils_build[0]                   = "cd /mnt/lfs/sources/binutils/build && make";
    
    binutils_build_path->install        = malloc(binutils_build_path->commands_install * sizeof(char*));
    exit_w_one_on_failed_xalloc(binutils_build_path->install);
    char** binutils_install             = binutils_build_path->install;
    binutils_install[0]                 = "cd /mnt/lfs/sources/binutils/build && make install";
    
    binutils_build_path->post_install   = malloc(binutils_build_path->commands_post_install * sizeof(char*));
    exit_w_one_on_failed_xalloc(binutils_build_path->post_install);
    char** binutils_post_install        = binutils_build_path->post_install;
    binutils_post_install[0]            = "find . -maxdepth 1 -type d -name \"binutils\" -exec rm -rf \"{}\" \\;";
    
    return binutils_build_path;
}

void do_binutils_compile_cx_toolchain_install()
{
    meta_build_info* binutils_meta = get_binutils_compile_cx_toolchain_meta_data();
    build_path*      binutils_path = get_binutils_compile_cx_toolchain_build_path(binutils_meta);
    
    char** binutils_pre_configure  = binutils_path->pre_configure;
    char** binutils_configure      = binutils_path->configure;
    char** binutils_build          = binutils_path->build;
    char** binutils_install        = binutils_path->install;
    char** binutils_post_install   = binutils_path->post_install;

    for(u_int8_t i = 0; i < binutils_path->commands_pre_configure; i++) {
        system(binutils_pre_configure[i]);
    }

    for(u_int8_t i = 0; i < binutils_path->commands_configure; i++) {
        system(binutils_configure[i]);
    }

    for(u_int8_t i = 0; i < binutils_path->commands_build; i++) {
        system(binutils_build[i]);
    }

    for(u_int8_t i = 0; i < binutils_path->commands_install; i++) {
        system(binutils_install[i]);
    }
   
    printf("Deleting /mnt/lfs/sources/binutils and returning to /mnt/lfs/sources...\n");
    
    for(u_int8_t i = 0; i < binutils_path->commands_post_install; i++) {
        system(binutils_post_install[i]);
    }

    free_valid_ptr(binutils_pre_configure);
    free_valid_ptr(binutils_configure);
    free_valid_ptr(binutils_build);
    free_valid_ptr(binutils_install);
    free_valid_ptr(binutils_post_install);
    free_valid_ptr(binutils_path);
    free_valid_ptr(binutils_meta);
}


meta_build_info* get_gcc_compile_cx_toolchain_meta_data() 
{
    meta_build_info* gcc_meta = malloc(1 * sizeof(meta_build_info)); 
    exit_w_one_on_failed_xalloc(gcc_meta);
    
    gcc_meta->lfs_stage         = "compile_cx_toolchain";
    gcc_meta->package_meta_name = "gcc_cx_toolchain_stg_one";
    gcc_meta->package_name      = "binutils";
    gcc_meta->package_version   = "do-something-here-to-get-version-dynamically"; //TODO
    gcc_meta->build_system      = "make-configure-no-autotools";
    gcc_meta->configure_tool    = "configure";
    gcc_meta->build_tool        = "make";
    gcc_meta->install_tool      = "make";
    
    return gcc_meta;
}

build_path* get_gcc_compile_cx_toolchain_build_path(meta_build_info* gcc_compile_cx_toolchain_meta_data) 
{
    build_path*  gcc_build_path         = malloc(1 * sizeof(build_path));
    exit_w_one_on_failed_xalloc(gcc_build_path);
    
    gcc_build_path->commands_pre_configure     = 4;
    gcc_build_path->commands_configure         = 1;
    gcc_build_path->commands_build             = 1;
    gcc_build_path->commands_install           = 1;
    gcc_build_path->commands_post_install      = 2;
    
    gcc_build_path->meta_data      = gcc_compile_cx_toolchain_meta_data;
    
    gcc_build_path->pre_configure  = malloc(gcc_build_path->commands_pre_configure * sizeof(char*));
    exit_w_one_on_failed_xalloc(gcc_build_path->pre_configure);
    char** gcc_pre_configure       = gcc_build_path->pre_configure;
    gcc_pre_configure[0]           = "mkdir gcc && tar xf gcc*.tar.* -C gcc --strip-components 1";
    gcc_pre_configure[1]           = "cd gcc && tar -xf ../mpfr-4.1.0.tar.xz && mv -v mpfr-4.1.0 mpfr && tar -xf ../gmp-6.2.0.tar.xz && mv -v gcc-6.2.0 gmp && tar -xf ../mpc-1.2.0.tar.gz && mv -v mpc-1.2.0 mpc";
    gcc_pre_configure[2]           = "cd gcc && case $(uname -m) in \n\tx86_64)\n\tsed -e '/m64=/s/lib64/lib/' -i.orig gcc/config/i386/t-linux64\n;;\nesac";
    gcc_pre_configure[3]           = "cd gcc && mkdir build";
    
    gcc_build_path->configure      = malloc(gcc_build_path->commands_configure * sizeof(char*));
    exit_w_one_on_failed_xalloc(gcc_build_path->configure);
    char** gcc_configure           = gcc_build_path->configure;
    gcc_configure[0]               = "cd /mnt/lfs/sources/gcc/build && ../configure --target=$LFS_TGT --prefix=$LFS/tools --with-glibc-version=2.11 --with-sysroot=$LFS --with-newlib --without-headers --enable-initfini-array --disable-nls --disable-shared --disable-multilib --disable-decimal-float --disable-threads --disable-libatomic --disable-libgomp --disable-libquadmath --disable-libssp --disable-libvtv --disable-libstdcxx --enable-languages=c,c++";
    
    gcc_build_path->build          = malloc(gcc_build_path->commands_build * sizeof(char*));
    exit_w_one_on_failed_xalloc(gcc_build_path->build);
    char** gcc_build               = gcc_build_path->build;
    gcc_build[0]                   = "cd /mnt/lfs/sources/gcc/build && make";
    
    gcc_build_path->install        = malloc(gcc_build_path->commands_install * sizeof(char*));
    exit_w_one_on_failed_xalloc(gcc_build_path->install);
    char** gcc_install             = gcc_build_path->install;
    gcc_install[0]                 = "cd /mnt/lfs/sources/gcc/build && make install";
    
    gcc_build_path->post_install   = malloc(gcc_build_path->commands_post_install * sizeof(char*));
    exit_w_one_on_failed_xalloc(gcc_build_path->post_install);
    char** gcc_post_install        = gcc_build_path->post_install;
    char* first_command            = "/mnt/lfs/helper_scripts/001_cx_toolchain_gcc_stg_one_create_limits_header.sh";
    gcc_post_install[0]            = first_command;
    gcc_post_install[1]            = "find . -maxdepth 1 -type d -name \"gcc\" -exec rm -rf \"{}\" \\;";
    
    return gcc_build_path;
}


void do_gcc_compile_cx_toolchain_install()
{
    meta_build_info* gcc_meta = get_gcc_compile_cx_toolchain_meta_data();
    build_path*      gcc_path = get_gcc_compile_cx_toolchain_build_path(gcc_meta);
    
    char** gcc_pre_configure  = gcc_path->pre_configure;
    char** gcc_configure      = gcc_path->configure;
    char** gcc_build          = gcc_path->build;
    char** gcc_install        = gcc_path->install;
    char** gcc_post_install   = gcc_path->post_install;
    
    for(u_int8_t i = 0; i < gcc_path->commands_pre_configure; i++) {
        system(gcc_pre_configure[i]);
    }

    for(u_int8_t i = 0; i < gcc_path->commands_configure; i++) {
        system(gcc_configure[i]);
    }

    for(u_int8_t i = 0; i < gcc_path->commands_build; i++) {
        system(gcc_build[i]);
    }

    for(u_int8_t i = 0; i < gcc_path->commands_install; i++) {
        system(gcc_install[i]);
    }

    printf("Deleting /mnt/lfs/sources/gcc and returning to /mnt/lfs/sources...\n");

    for(u_int8_t i = 0; i < gcc_path->commands_post_install; i++) {
        system(gcc_post_install[i]);
    }

    free_valid_ptr(gcc_pre_configure);
    free_valid_ptr(gcc_configure);
    free_valid_ptr(gcc_build);
    free_valid_ptr(gcc_install);
    free_valid_ptr(gcc_post_install);
    free_valid_ptr(gcc_path);
    free_valid_ptr(gcc_meta);
}

void do_compile_cx_toolchain()
{
    chdir("/mnt/lfs/sources");

    /* Cross Binutils */
    do_binutils_compile_cx_toolchain_install();    
    do_gcc_compile_cx_toolchain_install();    
}

void fork_as_lfs_user(char** environ, char** lfs_fs_folders)
{
    printf("Forking as LFS user: EUID: %u\n", geteuid());

    if(geteuid() == get_uid_from_name("lfs")) {
        switch(fork()) {
            case -1:
            printf("Error while forking!\n");
            exit(1);
            case 0:
            printf("C2 process running\n");
            /* Chown the whole lfs filesystem for lfs:lfs - Chapter 4.2 */
            chown_lfs_root_subfolders_lfs_lfs(lfs_fs_folders);
            
            /* Chapter 4.4 "Setting Up the Environment */
            write_lfs_bashrc();
            write_lfs_bash_profile();
            
            /* Let's start with Chapter 5 - "Compiling a Cross-Toolchain" */
            setuid(get_uid_from_name("lfs"));
            setgid(get_uid_from_name("lfs"));
            do_compile_cx_toolchain();      
            
            break;
            default:
            if(waitpid(getpid(), NULL, 0)) {
                printf("C1 (now P2) waiting for C2!\n");
            }
            break;
        }
    } else {
        printf("fork_as_lfs_user() may only be called in the context of lfs user (EUID = %d)\n", get_uid_from_name("lfs"));
        exit(1);
    }
}

