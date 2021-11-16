#include <malloc.h>
#include "olfsx_hlprfncts.h"
#include "olfsx_ashostroot.h"
#include "olfsx_ashostlfs.h"

void chown_file_lfs_lfs(const char* pathname) {
    seteuid(0);
    setegid(0);

    int res = chown(pathname, get_uid_from_name("lfs"), get_uid_from_name("lfs"));

    if(res == -1) {
        perror("Error while calling chown()");
    } else {
        printf("Chowned path %s to user %d and group %d\n", pathname, get_uid_from_name("lfs"), get_uid_from_name("lfs") );
    }
    
}

char* concat_mkfs_cmd(char* device, char* mountpoint)
{
    char* pre_mount_cmd = "";
    char* pre_mount_cmd2 = "";
    char* pre_mount_cmd3 = "";
    char* mount_cmd = "";

    pre_mount_cmd = strcat_heap_strs("mount ", device);
    pre_mount_cmd2 = strcat_heap_strs(pre_mount_cmd, " ");
    pre_mount_cmd3 = strcat_heap_strs(pre_mount_cmd2, getenv("LFS"));
    mount_cmd = strcat_heap_strs(pre_mount_cmd3, mountpoint);

    free_valid_ptr(pre_mount_cmd);
    free_valid_ptr(pre_mount_cmd2);
    free_valid_ptr(pre_mount_cmd3);

    return mount_cmd;
}

char* concat_etc_passwd_line(char** fields)
{
    u_int32_t sum_size_strings_in_array = get_bytesize_str_array(fields, 7);

    /** We need to add 6 bytes for the colon field separators
    But we need to subtract 6 bytes from the first 6 strings/elements of the array 
    Because otherwise we would have intermittent delimiters in our new string
    We DO WANT the space for the last delimiter though. **/
    u_int32_t bufsize = (sum_size_strings_in_array + 6 - 6 + 1);
    char* content = malloc(bufsize * sizeof(char));
    exit_w_one_on_failed_xalloc(content);
    null_char_buffer(content, bufsize);
    char* start_ptr = content;

    for(u_int8_t i = 0; i < 7; i++) {
        u_int16_t str_size_element = strlen(fields[i]);
        for(u_int16_t j = 0; j < str_size_element; j++) {
            *content = fields[i][j];
            content++;
        }
    }
    content = start_ptr;

    return content;
}

void set_lfs_shadow_passwd()
{
    /* Password is Lfslfs */
    const char* lfs_shadow_record = "lfs:$6$JWktulJsLjqbo9P2$RwlhChfkhXjYJad6P6gblC1POpiFmw4BALymL6DVr/V7OBYhVs2Ro6/AokhVWnzLzIfWWlPJsQuGsPV6SrGC8/:18470::::::\n";
    write_append_file("/etc/shadow", lfs_shadow_record);
}

void create_lfs_group()
{
    pid_t gid = get_non_system_user_next_free_uid();
    gid++; /*Super stupid workaround because get_non_system_user_next_free_uid() is not working*/
    char* gid_str = convert_uid_gid_to_str(gid);
    char* precursor_lfs_group_record = "lfs:x:";

    /* We need to append gid_str and : to the string */
    /* Lets first concatenate gid_str and : so we get something like 1001: */
    char* gid_str_w_colon = strcat_heap_strs(gid_str, ":\n");
    /* Now lets create the final group db record by concatenating that */
    /* to precursor_lfs_group_record so we get something like lfs:x:1001: */
    char* lfs_group_record = strcat_heap_strs(precursor_lfs_group_record, gid_str_w_colon);

    write_append_file("/etc/group", lfs_group_record);
    free_valid_ptr(gid_str_w_colon);
    free_valid_ptr(lfs_group_record);
}

void create_lfs_user()
{
    if(get_uid_from_name("lfs") == -1) {
        pid_t new_user_uid = get_non_system_user_next_free_uid();
        new_user_uid++; /*Super stupid workaround because get_non_system_user_next_free_uid() is not working*/

        /* Convert new user's uid and gid to string because we will pass string array */
        /* to a function that will generate the line for us that will then be written to /etc/passwd */
        char* uid_str = convert_uid_gid_to_str(new_user_uid);
        char* uid_str_w_colon = strcat_heap_strs(uid_str, ":");
        char** fields = malloc(7 * sizeof(char*));
        exit_w_one_on_failed_xalloc(fields);

        fields[0] = "lfs:";
        fields[1] = "x:";
        fields[2] = uid_str_w_colon;
        fields[3] = uid_str_w_colon;
        fields[4] = "lfs_unprivileged_user:";
        fields[5] = "/home/lfs:";
        fields[6] = "/bin/bash\n";

        char* lfs_etc_passwd_entry = concat_etc_passwd_line(fields);
        write_append_file("/etc/passwd", lfs_etc_passwd_entry);

        free_valid_ptr(uid_str_w_colon);

        if(mkdir("/home/lfs", 0700) != 0) {
            perror("Creating lfs home directory failed");
        }
        chown_file_lfs_lfs("/home/lfs");
        free_valid_ptr(fields);
    } else {
        char* q = "A user with the name \"lfs\" already exists. I will continue using this one.\nDo you want to continue? [Y/N]: ";
        u_int8_t user_wants_to_continue_anyways = eval_yes_no_question(q);
        if(!user_wants_to_continue_anyways) {
            printf("You chose No...will exit now with 1!\n");
            exit(1);
        }
    }
}

void become_lfs_user(char** environ)
{
    /** When a privileged process executes setuid() with a nonzero argument, then the
    real user ID, effective user ID, and saved set-user-ID are all set to the value spec-
    ified in the uid argument. This is a one-way trip, in that once a privileged process
    has changed its identifiers in this way, it loses all privileges and therefore can not
    subsequently use setuid() to reset the identifiers back to 0. If this is not desired,
    then either seteuid() or setreuid() **/

    /* I just assume that the gid of lfs will be the same as the uid of lfs */
    seteuid(get_uid_from_name("lfs"));
    setegid(get_uid_from_name("lfs"));

    /* Both together maybe overkill but oh well... */
    clearenv();
    environ = NULL;

    set_env_vars_lfs_user();

    chdir("/home/lfs");
}

char** get_all_blk_devs(u_int8_t just_print)
{
    DIR *dir = NULL;
    struct dirent* dirptr;
    u_int16_t blk_dev_cntr = 0;
    u_int16_t i = 0;
    u_int32_t init_size_str_arr = (1 * sizeof(char*)); /* 1 char pointer (8 bytes on x86_64 64-bit) */
    char** block_devs = malloc(init_size_str_arr);
    exit_w_one_on_failed_xalloc(block_devs);

    if((dir=opendir("/dev")) == NULL) {
        printf("Error opening /dev. Are you root?\n");
        exit(1);
    }

    while((dirptr=readdir(dir)) != NULL) {
        struct stat attr;
        chdir("/dev");
        stat(dirptr->d_name, &attr);

        if(S_ISBLK(attr.st_mode)) {
            blk_dev_cntr++;
            u_int32_t new_size_realloc = (blk_dev_cntr * sizeof(char*));
            block_devs = realloc(block_devs, new_size_realloc);
            exit_w_one_on_failed_xalloc(block_devs);

            if(just_print)
                printf("Found hard drive/partition: %s\n", dirptr->d_name);
        }
    }

    if(just_print)
        return block_devs;

    closedir(dir);

    dirptr = NULL;

    if((dir=opendir("/dev")) == NULL) {
        printf("Error opening /dev. Are you root?\n");
        exit(1);
    }

    while((dirptr=readdir(dir)) != NULL) {
        struct stat attr;
        //chdir("/dev");
        stat(dirptr->d_name, &attr);

        if(S_ISBLK(attr.st_mode)) {
            char* dir_name_abs_path = strcat_heap_strs("/dev/", dirptr->d_name);
            block_devs[i] = dir_name_abs_path;
            i++;
        }
    }

    closedir(dir);

    /* Factor of two because we will add the size string AND NULL */
    u_int32_t new_size_realloc = ((i * sizeof(char*)) + (2 * sizeof(char*)));
    block_devs = realloc(block_devs, new_size_realloc);
    exit_w_one_on_failed_xalloc(block_devs);

    char* arr_size_str = malloc((get_length_digits(i) + 1) * sizeof(char));
    exit_w_one_on_failed_xalloc(arr_size_str);

    arr_size_str = num_to_str(i);
    block_devs[i] = arr_size_str;
    block_devs[i + 1] = NULL;

    return block_devs;
}

void lsblk()
{
    printf("\nYour machine's block devices:\n");
    if(system("lsblk") == -1)
        if(system("blkid") == -1)
            if(system("fdisk -l") == -1)
                get_all_blk_devs(1);
}

u_int8_t ask_user_partitions_or_not()
{
    char* q = "Do you want partitions or install LFS to a single drive/partition?\n Y(es) = 'Partitions' / N(o) = Single drive/partition [Y/N]: ";
    return eval_yes_no_question(q);
}

char** ask_user_what_partitions(char** detected_blk_devs, u_int32_t amount_blk_devs, u_int8_t user_wants_extra_partitions)
{
    char** chosen_partitions = malloc(256 * sizeof(char*));
    exit_w_one_on_failed_xalloc(chosen_partitions);
    u_int8_t size_char_buf_blk_dev_paths = 32;

    printf("\nnNo matter whether you chose to outsource part of LFS to different partitions or not: \n");
    printf("You must choose which partitions will be your EFI boot partition (ESP) and your ROOT (/) partition.\n");
    printf("You can choose the one you current host system already uses if it has enough space left or you can choose a new one: \n\n");

    char* question_esp_part = "What partition do you want to be LFS' EFI (/boot/efi) partition?: ";
    printf("%s ",  question_esp_part);
    char* user_choice_esp = malloc(size_char_buf_blk_dev_paths * sizeof(char));
    exit_w_one_on_failed_xalloc(user_choice_esp);
    null_char_buffer(user_choice_esp, size_char_buf_blk_dev_paths);
    scanf(" %s", user_choice_esp);
    printf("For ESP you chose the mountpoint: %s\n", user_choice_esp);
    
    loop_till_blk_dev_valid(detected_blk_devs, user_choice_esp, question_esp_part, amount_blk_devs);
    chosen_partitions[0] = strcat_heap_strs("/boot/efi:", user_choice_esp);
    free_valid_ptr(user_choice_esp);

    char* question_root_part = "What partition do you want to be LFS' ROOT (/) partition?: ";
    printf("%s ",  question_root_part);
    char* user_choice_root = malloc(size_char_buf_blk_dev_paths * sizeof(char));
    null_char_buffer(user_choice_root, size_char_buf_blk_dev_paths);
    scanf(" %s", user_choice_root);
    printf("For ROOT you chose the mountpoint: %s\n", user_choice_root);

    loop_till_blk_dev_valid(detected_blk_devs, user_choice_root, question_root_part, amount_blk_devs);
    chosen_partitions[1] = strcat_heap_strs("/:", user_choice_root);
    free_valid_ptr(user_choice_root);

    if(!user_wants_extra_partitions) {
        chosen_partitions[2] = NULL;
        return chosen_partitions;
    }
 
    char** pp_val_partitions_arr = malloc(6 * sizeof(char*));
    exit_w_one_on_failed_xalloc(pp_val_partitions_arr);
    char** startpos = pp_val_partitions_arr;
    pp_val_partitions_arr[0] = malloc(6 * sizeof(char));
    exit_w_one_on_failed_xalloc(pp_val_partitions_arr[0]);
    pp_val_partitions_arr[0] = "/home"; 
    pp_val_partitions_arr[1] = malloc(5 * sizeof(char));
    exit_w_one_on_failed_xalloc(pp_val_partitions_arr[1]);
    pp_val_partitions_arr[1] = "/var";
    pp_val_partitions_arr[2] = malloc(11 * sizeof(char));
    exit_w_one_on_failed_xalloc(pp_val_partitions_arr[2]);
    pp_val_partitions_arr[2] = "/usr/local";
    pp_val_partitions_arr[3] = malloc(5 * sizeof(char));
    exit_w_one_on_failed_xalloc(pp_val_partitions_arr[3]);
    pp_val_partitions_arr[3] = "/opt";
    pp_val_partitions_arr[4] = malloc(5 * sizeof(char));
    exit_w_one_on_failed_xalloc(pp_val_partitions_arr[4]);
    pp_val_partitions_arr[4] = "/srv";

    char* q1 = strcat_heap_strs("Do you want a partition for", " ");
    u_int8_t loop_counter = 1;
    u_int16_t partitions_wanted_counter = 2;
    char* q2 = NULL;
    char* q3 = NULL;

    for(u_int8_t i = 0; i < 5; i++) {
        q2 = strcat_heap_strs(q1, *pp_val_partitions_arr);
        q3 = strcat_heap_strs(q2, "? [Y/N]: ");

        u_int8_t user_wants_this_partition = eval_yes_no_question(q3);

        if(user_wants_this_partition) {
            char* question_this_part = strcat_heap_strs("\nWhat drive/partition do you want to use for ", pp_val_partitions_arr[i]);
            question_this_part = strcat_heap_strs(question_this_part, "?: ");

            char* user_choice = malloc(size_char_buf_blk_dev_paths * sizeof(char));
            exit_w_one_on_failed_xalloc(user_choice);
            null_char_buffer(user_choice, size_char_buf_blk_dev_paths);

            printf("%s ",  question_this_part);
            scanf(" %s", user_choice);

            loop_till_blk_dev_valid(detected_blk_devs, user_choice, question_this_part, amount_blk_devs);

            char* part_path_w_colon = strcat_heap_strs(*pp_val_partitions_arr, ":");
            chosen_partitions[loop_counter + 1] = strcat_heap_strs(part_path_w_colon, user_choice);

            partitions_wanted_counter++;
            pp_val_partitions_arr++;

            free_valid_ptr(user_choice);
            free_valid_ptr(question_this_part);
        } else {
            loop_counter--;
            break;
        }
        free_valid_ptr(q3);
        loop_counter++;
    }
    pp_val_partitions_arr = startpos;

    free_valid_ptr(q1);
    free_valid_ptr(q2);
    free_valid_ptr(q3);
    
    free_valid_ptr(pp_val_partitions_arr);

    
    chosen_partitions[partitions_wanted_counter] = NULL;

    return chosen_partitions;
}


void ask_user_format_then_format(char** chosen_partitions)
{   
    clear();
    printf("\nNow you can choose whether or not you want to format one or more of the partitions you just chose for your soon-to-be LFS system! :) \n\n");
    char* q2 = NULL;
    char* q3 = NULL;
    char* q4 = NULL;
    char* q5 = NULL;
    char** startpos = chosen_partitions;
    char* q1 = strcat_heap_strs("\nDo you want to format", " ");

    while(*chosen_partitions != NULL) {
        char** mountpoint_part_arr = split_str_at_char(':', *chosen_partitions);

        if(atoi(mountpoint_part_arr[0]) != -1) {
            q2 = strcat_heap_strs(q1,  mountpoint_part_arr[1]);
            q3 = strcat_heap_strs(q2, " (");
            q4 = strcat_heap_strs(q3, mountpoint_part_arr[0]);
            q5 = strcat_heap_strs(q4, ")? [Y/N] ");
            u_int8_t user_wants_to_format = eval_yes_no_question(q5);
            
            if(user_wants_to_format) {
                char* mkfs_cmd = "";
                if(str_startswith("/boot/efi", mountpoint_part_arr[0]))
                    mkfs_cmd = strcat_heap_strs("mkfs.vfat -F32 ", mountpoint_part_arr[1]);
                else
                    mkfs_cmd = strcat_heap_strs("mkfs.ext4 -q ", mountpoint_part_arr[1]);
                    
                system(mkfs_cmd);
                free_valid_ptr(mountpoint_part_arr[1]);
                free_valid_ptr(mountpoint_part_arr[0]);
                free_valid_ptr(mountpoint_part_arr);
            }
            free_valid_ptr(q2);
            free_valid_ptr(q3);
            free_valid_ptr(q4);
            free_valid_ptr(q5);
        }
        chosen_partitions++;
    }
    free_valid_ptr(q1);
    chosen_partitions = startpos;
}

void set_lfs_env_var()
{
    setenv("LFS", "/mnt/lfs", ENV_VAR_WRITE_PROTECT_OFF);
}

void create_lfs_root_folder() {
    clear();
    char* lfs_root = getenv("LFS");
    
    if(mkdir(lfs_root, 0700) != 0)
        perror(strcat_heap_strs("Error creating directory %s ", lfs_root));
    else
        printf("Created directory %s!\n", lfs_root);
}

char** create_essential_lfs_folders()
{
    char* boot = strcat_heap_strs(getenv("LFS"), "/boot");
    char* boot_efi = strcat_heap_strs(getenv("LFS"), "/boot/efi");
    char* bin = strcat_heap_strs(getenv("LFS"), "/bin");
    char* sbin = strcat_heap_strs(getenv("LFS"), "/sbin");
    char* usr = strcat_heap_strs(getenv("LFS"), "/usr");
    char* etc = strcat_heap_strs(getenv("LFS"), "/etc");
    char* lib = strcat_heap_strs(getenv("LFS"), "/lib");
    char* home = strcat_heap_strs(getenv("LFS"), "/home");
    char* var = strcat_heap_strs(getenv("LFS"), "/var");
    char* opt = strcat_heap_strs(getenv("LFS"), "/opt");
    char* tools = strcat_heap_strs(getenv("LFS"), "/tools");
    char* lib64 = strcat_heap_strs(getenv("LFS"), "/lib64");
    char* sources = strcat_heap_strs(getenv("LFS"), "/sources");
    
    char** folders = malloc(13 * sizeof(char*));
    exit_w_one_on_failed_xalloc(folders);
    
    folders[0] = boot;
    folders[1] = boot_efi;
    folders[2] = bin;
    folders[3] = sbin;
    folders[4] = usr;
    folders[5] = etc;
    folders[6] = lib;
    folders[7] = home;
    folders[8] = var;
    folders[9] = opt;
    folders[10]= tools;
    folders[11] = lib64;
    folders[12] = sources;
    
    for(u_int8_t i = 0; i < 13; i++) {
        if(mkdir(folders[i], 0700) != 0) {
            perror(strcat_heap_strs("Error creating directory %s ", folders[i]));
        }  else {
            printf("Created directory %s!\n", folders[i]);
        }
    }
    return folders;
}

char** mount_partitions(char** chosen_partitions)
{
    char** startpos = chosen_partitions;
    char* mount_cmd = "";
    
    /** 
    *   Mounting root first is mandatory. First element is /boot/efi:<device> though 
    *   So let's manually increment the chose_partitions pointer
    *   Then we mount the rest via a loop
    *   In the loop we then skip the second element of course 
    **/
    char** mountpoint_part_arr = split_str_at_char(':', *(chosen_partitions + 1));
    mount_cmd = concat_mkfs_cmd(mountpoint_part_arr[1], mountpoint_part_arr[0]);
    printf("Mounting device %s to mount point %s...\n", mountpoint_part_arr[1], strcat_heap_strs(getenv("LFS"), mountpoint_part_arr[0]));
    printf("Executing: %s\n", mount_cmd);
    system(mount_cmd);
    printf("Done! :)\n");
    free_valid_ptr(mount_cmd);   
    char** lfs_fs_structure =  create_essential_lfs_folders();
    
    char* last_mountpoint_mounted = "";

    while(*chosen_partitions != NULL) {
        if(strcmp(last_mountpoint_mounted, "/boot/efi") == 0)
            /* Skipping root because already mounted! */
            chosen_partitions++;
            
        char** mountpoint_part_arr = split_str_at_char(':', *chosen_partitions);

        if(atoi(mountpoint_part_arr[0]) != -1) {
            mount_cmd = concat_mkfs_cmd(mountpoint_part_arr[1], mountpoint_part_arr[0]);
            printf("Mounting device %s to mount point %s...\n", mountpoint_part_arr[1], strcat_heap_strs(getenv("LFS"), mountpoint_part_arr[0]));
            printf("Executing: %s\n", mount_cmd);
            system(mount_cmd);
            printf("Done! :)\n");
            free_valid_ptr(mount_cmd);
            last_mountpoint_mounted = mountpoint_part_arr[0];
        }
        chosen_partitions++;
    }
    chosen_partitions = startpos;
    return lfs_fs_structure;
}

void download_packages_and_patches(char* path_wget_list) 
{
    system("cp -av sources/* /mnt/lfs/sources/");
    system("cp -av helper_scripts /mnt/lfs/");
    char* wget_cmd_pre1 = strcat_heap_strs("wget --no-clobber --input-file=", path_wget_list);
    char* wget_cmd_pre2 = strcat_heap_strs(wget_cmd_pre1, " --continue --directory-prefix=");
    char* wget_cmd = strcat_heap_strs(wget_cmd_pre2, "/mnt/lfs/sources");
    
    printf("Will download packages and patches now...\n");
    printf("Running: %s\n", wget_cmd);

    system(wget_cmd);
    
    printf("All packages and patches successfully downloaded to /mnt/lfs/sources!\n");
    
    free_valid_ptr(wget_cmd);
    free_valid_ptr(wget_cmd_pre2);
    free_valid_ptr(wget_cmd_pre1);
}
