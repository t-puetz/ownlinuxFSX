#ifndef OLFSX_ASHOSTLFS_H
#define OLFSX_ASHOSTLFS_H

#include "olfsx_hlprfncts.h"

typedef struct meta_build_info {
    /* cx_compile_toolchain_lfs, cx_compile_temp_tools_lfs, build_add_tools_final_system, final_system */
    char* lfs_stage;
    /* Some packages must be compiled in two stages, lets keep track of those */
    char* package_meta_name;
    char* package_name;
    char* package_version;
    /** Meson/ninja, make w/o autotools and w/o configure, CMake
     *  Hardcoded and valid build system identifier values are:
     *  make-configure-no-autotools, make-configure-w-autotools,
     *  meson-ninja, cmake, perl, python, unknown, and none
    **/
    char* build_system; 
    /* Actual tool that will do/supervise the build process: make, meson etc. */
    /* Need this member because install tool may differ from build tool like Meson and Ninja */
    char* build_tool; 
    char* install_tool; 
    char* configure_tool;
}meta_build_info;

typedef struct build_path {
    meta_build_info* meta_data;
    char**           pre_configure;
    char**           configure;
    char**           post_configure;
    char**           pre_build; 
    char**           build;
    char**           post_build;
    char**           pre_install;
    char**           install;
    char**           post_install; 
    u_int8_t         commands_pre_configure;
    u_int8_t         commands_configure;
    u_int8_t         commands_post_configure;
    u_int8_t         commands_pre_build;
    u_int8_t         commands_build;
    u_int8_t         commands_post_build;
    u_int8_t         commands_pre_install;
    u_int8_t         commands_install;
    u_int8_t         commands_post_install;
}build_path;

typedef struct compile_cx_toolchain_as_lfs {
    
}compile_cx_toolchain_as_lfs;

void set_env_vars_lfs_user();
void write_lfs_bashrc();
void write_lfs_bash_profile();
void fork_as_lfs_user(char** environ, char** lfs_fs_folders);
meta_build_info* get_binutils_compile_cx_toolchain_meta_data();
build_path* get_binutils_compile_cx_toolchain_build_path(meta_build_info* binutils_compile_cx_toolchain_meta_data);
void do_binutils_compile_cx_toolchain_install();
void do_compile_cx_toolchain();

#endif
