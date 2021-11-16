#ifndef OLFSX_ASHOSTROOT_H
#define OLFSX_ASHOSTROOT_H

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

void chown_file_lfs_lfs(const char* pathname);
char* concat_mkfs_cmd(char* device, char* mountpoint);
char* concat_etc_passwd_line(char** fields);
void set_lfs_shadow_passwd();
void create_lfs_group();
void create_lfs_user();
void become_lfs_user(char** environ);
char** get_all_blk_devs(u_int8_t just_print);
void lsblk();
u_int8_t ask_user_partitions_or_not();
char** ask_user_what_partitions(char** detected_blk_devs, u_int32_t amount_blk_devs, u_int8_t user_wants_extra_partitions);
void ask_user_format_then_format(char** chosen_partitions);
void set_lfs_env_var();
char** create_essential_lfs_folders();
void create_lfs_root_folder();
char** mount_partitions(char** chosen_partitions);
void download_packages_and_patches(char* path_wget_list);

#endif
