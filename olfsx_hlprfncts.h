#ifndef OLFSX_HLPRHFNCTS_H
#define OLFSX_HLPRHFNCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <shadow.h>
#include <limits.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define ENV_VAR_WRITE_PROTECT_OFF 1
#define ENV_VAR_WRITE_PROTECT_ON  0

#define NPROCS get_nprocs() 
#define NPROCS_MINUS_1 NPROCS - 1 /* We don't wanna freeze the whole system when compiling something like GCC */

void exit_w_one_on_failed_xalloc(void* ptr);
void free_valid_ptr(void* ptr_to_free);
void clear();
char* get_self_exec_path(char* self_name_arg0);

char* get_self_exec_path(char* self_name_arg0);
uid_t get_uid_from_name(const char *name);
pid_t get_non_system_user_next_free_uid();
char* get_name_non_system_user_from_uid(uid_t uid);
char* convert_uid_gid_to_str(pid_t id);
u_int8_t string_is_empty(char* str);

void write_append_file(const char* pathname, const char* content);
void write_create_file(char* pathname, mode_t mode, char* content);
char* concat_etc_passwd_line(char** fields);

u_int32_t get_bytesize_str_array(char** str_array, u_int16_t array_size);
char* strcat_const_strs(char* dst, char* src, char* dst_buf);
char* strcat_heap_strs(char* str1, char* str2);
char* num_to_str(u_int32_t num);
char single_digit_to_char(u_int8_t single_digit_num);
char* convert_proc_num_to_str();
u_int8_t get_length_digits(u_int32_t num);

void set_makeflags_env_var();
void set_ninjajobs_env_var();

u_int32_t get_pseudo_size_null_terminated_self_aware_str_arr(char** array);
u_int32_t get_real_size_null_terminated_self_aware_str_arr(char** array);

u_int8_t check_last_char_str_is_single_digit(char* str);

u_int8_t check_blk_dev_valid(char** blk_devs, char* blk_dev, u_int32_t amount_blk_devs);
u_int8_t eval_yes_no_question(char* question);
u_int8_t check_str_contains_char(char findme, char* src_str);
char** split_str_at_char(char splitat, char* src_str);
void loop_till_blk_dev_valid(char** blk_devs, char* user_choice, char* question, u_int32_t amount_blk_devs);
void null_char_buffer(char buf[], u_int32_t size);
u_int8_t str_startswith(char* startswith, char* dst);
#endif
