#include <sys/wait.h>
#include "olfsx_hlprfncts.h"
#include "olfsx_ashostroot.h"
#include "olfsx_ashostlfs.h"

int main(int argc, char** argv, char** environ)
{
    pid_t pid = 0;

    if(geteuid()) {
         printf("You must run this script as root!\n");
         exit(1);
    }
    
    
    char* basedir_plus_exec_name = get_self_exec_path(argv[0]);
    char* program_dir_only = getcwd(NULL, 0);
    
    printf("Working directory: %s\n", program_dir_only);
    printf("Full path of this installer: %s\n", basedir_plus_exec_name);

    /* Before we fork we can do anything that is needed to be done by root as
       described in section II of the LFS book. Let's get started. Clear the screen... */
    clear();
    
    /* TODO: Actually act according to Chapter 2.2 "Host System Requirements" here */
    //
    //
    //
    
    /* Do some stuff to prepare for Chapters 2.4 and 2.5 "Creating File System on the Parition " here */
    
    /* Get all block devices in the system by name and also  count their number*/
    char** arr_blk_devs = get_all_blk_devs(0);
    u_int32_t amount_blk_devs = get_pseudo_size_null_terminated_self_aware_str_arr(arr_blk_devs);
    /* The user should also see on the scvreen what options he/she got */
    lsblk();
    /* Ask the user whether or not he/she wants to install LFS on a single disk/partition or on multiple partitions */
    u_int8_t user_wants_partitions_or_not = ask_user_partitions_or_not();
    /* Get the paritions chosen by the user */
    char** partitions_wanted_map = ask_user_what_partitions(arr_blk_devs, amount_blk_devs, user_wants_partitions_or_not);
    /* Ask user if said paritions should be formatted (just is supported ext4 for now) */
    ask_user_format_then_format(partitions_wanted_map);
    
    /* Chaptger 2.7 "Setting the $LFS Variable "*/
    set_lfs_env_var();
    
    /* Execute Chapter 2.7 "Mounting the New Parition" */
    create_lfs_root_folder();
    /* This obvious function also runs create_essential_lfs_folders() */
    /* Which already fullfills Chapter 4.2 "Creating a limited directory layout in LFS filesystem"  and a little more*/
    char**  freshly_creatd_lfs_fs_structure = mount_partitions(partitions_wanted_map);
    
    /*  Execute Chapter 3.1 "Introduction: Packages and Patches by downloading from the wget-list */
    chdir(program_dir_only);
    char* path_to_wget_list = strcat_heap_strs(program_dir_only, "/wget-list");
    download_packages_and_patches(path_to_wget_list);
    free(path_to_wget_list);
    
    /* Fullfill Chapter 4.3 "Adding the LFS User" */
    create_lfs_group();
    create_lfs_user();
    /* Also for Chapter 4.3 we set the Password to Lfslfs for the lfs user  by copying the corresponding SHA512 hash to /etc/shadow */
    /* I do not know how to set a password in system's programming without actually reimplimenting passwd (same goes for useradd) */
    set_lfs_shadow_passwd();

    /* TODO: Why did I get the lfs' uid HERE? Goldfish memory incoming.... */
    //uid_t uid_lfs_user = get_uid_from_name("lfs");

    switch(pid = fork()) {
        case -1:
        printf("Error while forking!\n");
        exit(1);
        case 0:
        printf("C1 process running.\n");
        become_lfs_user(environ);
        fork_as_lfs_user(environ, freshly_creatd_lfs_fs_structure);
        free_valid_ptr(arr_blk_devs);
        free_valid_ptr(partitions_wanted_map);
        free_valid_ptr(program_dir_only);
        break;
        default:
        if(waitpid(pid, NULL, 0)) {
             printf("I am P1 waiting for C1!\n");
        }
        break;
    }
    return 0;
}

