#include <malloc.h>
#include "olfsx_hlprfncts.h"
#include <math.h>

void exit_w_one_on_failed_xalloc(void* ptr)
{
    if(ptr == NULL) {
        printf("Malloc or realloc failed!\n");
        exit(1);
    }
}

void free_valid_ptr(void* ptr_to_free) {
    if(ptr_to_free) {
        free(ptr_to_free);
    }
}

void clear() {
    printf("\033[H\033[J");
}

char* get_self_exec_path(char* self_name_arg0)
{
    char* cwd = getcwd(NULL, 0);
    char* exec_name = self_name_arg0;
    char* path_plus_name = NULL;

    if(str_startswith("./", exec_name)) {
        /* ownlinuxFSX_installer has 22 bytes with delimiter included but remove the . and the / stays */
        char* exec_name_no_prefix = malloc(21 * sizeof(char));
        exit_w_one_on_failed_xalloc(exec_name_no_prefix);

        exec_name++;

        for(u_int8_t i = 0; i <= 21; i++) {
            exec_name_no_prefix[i] = *exec_name;
            exec_name++;
        }
        exec_name_no_prefix[22] = '\0';
        path_plus_name = strcat_heap_strs(cwd, exec_name_no_prefix);
        free_valid_ptr(exec_name_no_prefix);
    }
    free_valid_ptr(cwd);

    return path_plus_name;
}


uid_t get_uid_from_name(const char *name)
{
    struct passwd *pwd;
    uid_t uid;

    /* If char* (=string) is NULL or empty string return -1*/
    if (name == NULL || *name == '\0')
        return -1;

    pwd = getpwnam(name);

    /* If resulting pointer to the passwd struct is NULL */
    /* Passwd is a struct whose members each represent one field in /etc/passwd */
    if (pwd == NULL)
        return -1;

    /* From the struct return the member that contains the uid from the uid field in /etc/passwd */
    return pwd->pw_uid;
}

char* get_name_non_system_user_from_uid(uid_t uid)
{
    struct passwd *pwd;

    /* If uid not valid or empty string return ""*/
    if (uid <= 999)
        return "";

    pwd = getpwuid(uid);

    /* If resulting pointer to the passwd struct is NULL */
    /* Passwd is a struct whose members each represent one field in /etc/passwd */
    if (pwd == NULL)
        return "";

    /* From the struct return the member that contains the name from the name field in /etc/passwd */
    return pwd->pw_name;
}

void write_append_file(const char* pathname, const char* content)
{
    ssize_t size = 0;
    size = strlen(content);

    int fd = open(pathname, O_APPEND | O_WRONLY);

    if(fd == -1) {
        printf("Error when calling open() to write %s!\n", pathname);
        exit(1);
    }

    if(write(fd, content, size) != size) {
        perror("Error while writing file.");
        exit(1);
    }

    close(fd);
}

u_int32_t get_bytesize_str_array(char** str_array, u_int16_t array_size)
{
    u_int32_t byte_counter = 0;

    for(u_int8_t i = 0; i < array_size; i++) {
        byte_counter += strlen(str_array[i]) + 1; /* Remember strlen() never accounts for '\0'*/
    }

    /* If you use the calculated size to concatenate a string later, */
    /* you need to remove the bytes for the delimiters in-between, again! */
    return byte_counter;
}

char* strcat_const_strs(char* dst, char* src, char* dst_buf)
{
    u_int8_t strlen_src = strlen(src);

    char src_buf[strlen_src];
    null_char_buffer(src_buf, strlen_src);

    strcpy(src_buf, src);
    strcpy(dst_buf, dst);

    strncat(dst_buf, src_buf, strlen_src + 2);
    return dst_buf;
}

char* strcat_heap_strs(char* str1, char* str2)
{
    u_int32_t len_str1 = strlen(str1);
    u_int32_t len_str2 = strlen(str2);

    char* dst = malloc((len_str1 + len_str2 + 2) * sizeof(char));
    exit_w_one_on_failed_xalloc(dst); 

    char* startpos = dst;

    null_char_buffer(dst, (len_str1 + len_str2 + 1));

    for(u_int32_t i = 0; i < len_str1; i++) {
        *dst = str1[i];
        dst++;
    }

    for(u_int32_t j = 0; j < len_str2 + 1; j++) {
        *dst = str2[j];
        dst++;
    }

    dst = startpos;
    return dst;
}

char* num_to_str(u_int32_t num)
{
    u_int8_t number_of_digits = get_length_digits(num);
    char* dst = malloc((number_of_digits + 1) * sizeof(char));
    exit_w_one_on_failed_xalloc(dst);
    null_char_buffer(dst, (number_of_digits + 1));
    char* startpos = dst;
    u_int32_t highest_modulo_to_use = (u_int32_t)pow(10, (number_of_digits - 1));

    for(u_int8_t i = 0; i < number_of_digits; i++) {
        *dst = single_digit_to_char((num - (num % highest_modulo_to_use)) / highest_modulo_to_use);
        num = num - (highest_modulo_to_use * atoi(dst));
        highest_modulo_to_use = highest_modulo_to_use / 10;
        dst++;
    }

    dst = startpos;

    return dst;
}

char single_digit_to_char(u_int8_t single_digit_num) {
    char c = '\0';
    switch(single_digit_num) {
        case 0:
            c = 48;
            break;
        case 1:
            c = 49;
            break;
        case 2:
             c = 50;
             break;
        case 3:
            c = 51;
            break;
        case 4:
            c = 52;
            break;
        case 5:
            c = 53;
            break;
        case 6:
            c = 54;
            break;
        case 7:
            c = 55;
            break;
        case 8:
            c = 56;
            break;
        case 9:
            c = 56;
            break;
        default:
            printf("Char provided is not a single digit number\n");
            break;
    }
    return c;
}

char* convert_uid_gid_to_str(pid_t id)
{
    if(999 < NPROCS_MINUS_1 < 10000) {
        char buf[4];
        char* id_str = num_to_str(id);
        return id_str;
    } else {
        return "";
    }
}

u_int8_t string_is_empty(char* str)
{
    if(strlen(str) < 1) {
        return 0;
    } else {
        return 1;
    }
}

pid_t get_non_system_user_next_free_uid()
{
    struct passwd* pwd;
    uid_t uid = 1000;
    pwd = getpwuid(uid);

    while(pwd != NULL) {
        uid++;
        pwd = getpwuid(uid);
    }
    printf("Returning next free uid: %d\n", uid);
    return uid;
}

u_int8_t get_length_digits(u_int32_t num)
{
    u_int8_t loop_counter = 0;
    u_int32_t compare = 0;

    while(num != compare) {
        compare = num % (int)(pow(10, (loop_counter + 1)));
        loop_counter++;
    }
    return loop_counter;
}

char* convert_proc_num_to_str()
{
    char* num_procs_str = num_to_str(NPROCS_MINUS_1);

    if(atoi(num_procs_str) == -1) {
        num_procs_str = "1";
    }
    return num_procs_str;
}

void set_makeflags_env_var()
{
    /* Create value for MAKEFLAGS env var */
    /* Just create a buffer large enough for the -j string plus the delimiter so 3*/
    char* num_nprocs_str = convert_proc_num_to_str();

    char dst_buf[3];
    char* makeflags_env_var_val = strcat_const_strs("-j", num_nprocs_str, dst_buf);
    setenv("MAKEFLAGS", makeflags_env_var_val, ENV_VAR_WRITE_PROTECT_OFF);
}

void set_ninjajobs_env_var()
{
    /* Create value for NINJAJOBS env var */
    char buf_num_procs_as_str[get_length_digits(NPROCS_MINUS_1)];
    char* ninjajobs_env_var_val = num_to_str(NPROCS_MINUS_1);
    setenv("NINJAJOBS", ninjajobs_env_var_val, ENV_VAR_WRITE_PROTECT_OFF);
}

void write_create_file(char* pathname, mode_t mode, char* content) {
    ssize_t size = 0;
    size = strlen(content);

    int fd = open(pathname, O_WRONLY | O_CREAT, mode);

    if(fd == -1) {
        printf("Error when calling open() to write %s!\n", pathname);
        exit(1);
    }

    if(write(fd, content, size) != size) {
        perror("Error while writing file.");
        exit(1);
    }

    close(fd);
}

u_int32_t get_pseudo_size_null_terminated_self_aware_str_arr(char** array)
{
    /* Will only work with null terminated arrays */
    /* and the second last element must be the array size till that point as a string */
    u_int32_t i = 0;
    char** startpos = array;
    while(*array != 0) {
        i++;
        array++;
    }

    array = startpos;

    return i - 2;
}

u_int32_t get_real_size_null_terminated_self_aware_str_arr(char** array)
{
    return get_pseudo_size_null_terminated_self_aware_str_arr(array) + 2;
}

u_int8_t check_last_char_str_is_single_digit(char* str)
{
    u_int16_t strlen_no_delim = strlen(str);
    char* new_str = (&str[strlen_no_delim - 1]);

    if(atoi(new_str))
        return 1;
    else
        return 0;
}

u_int8_t check_blk_dev_valid(char** blk_devs, char* blk_dev, u_int32_t amount_blk_devs)
{
    for(u_int32_t i = 0; i < amount_blk_devs; i++) {
        if((strcmp(blk_dev, blk_devs[i])) == 0)
            return 1;

        if((strcmp(blk_dev, blk_devs[i]) != 0) && i == amount_blk_devs - 1) {
            printf("The drive/partition you specified is not valid. Try again!\n");
            return 0;
        }
    }
}

u_int8_t eval_yes_no_question(char* question) {
    char answer = '\0';
    u_int8_t answer_is_invalid = (answer != 'Y' && answer != 'y' && answer != 'N' && answer != 'n');

     while(answer_is_invalid) {
        printf(question);

        /** Took me forever to figure this out. After the first yes no question program
            would always print questions twice. The answer is the space befor %c-
            citing from stackoverflow:
            "The problem is, when you enter Y and press ENTER, the new line is still in the input buffer,
            adding a space before %c could consume it.**/
        scanf(" %c", &answer);
        if(answer == 'Y' || answer == 'y')
            return 1;
        else if(answer == 'N' || answer == 'n')
            return 0;
        else
            continue;
     }
}

u_int8_t check_str_contains_char(char findme, char* src_str) {
    char* startpos = NULL;
    startpos = src_str;

    while(*src_str != '\0') {
        if(*src_str == findme) {
            src_str = startpos;
            return 1;
        }
        src_str++;
    }
    src_str = startpos;

    return 0;
}

char** split_str_at_char(char splitat, char* src_str)
{
    /* Helper pointer that stores start position of src_str to reset it later */
    char* startpos = NULL;
    startpos = src_str;

    /**
        First array element will contain first half of the string
        Second array element will contain second half of the string
    **/
    char** res_arr = malloc(2 * sizeof(char*));
    exit_w_one_on_failed_xalloc(res_arr);

    /**
       At least we need 2 bytes for each string (1 char + 1 delimiter char)
       First array element gets 3 though because in case of no split char found
       we return "-1"
    **/
    char* el_zero = malloc(3 * sizeof(char));
    exit_w_one_on_failed_xalloc(el_zero);
    char* el_one = malloc(2 * sizeof(char));
    exit_w_one_on_failed_xalloc(el_one);
    res_arr[0] = el_zero;
    res_arr[1] = el_one;

    /* Assert success by using 'if(atoi(res_arr[0]) != -1)' in your main code */
    res_arr[0] = "-1";

    if(!check_str_contains_char(splitat, src_str))
        return res_arr;

    u_int32_t loop_counter = 0;
    u_int8_t arr_idx = 0;

    while(*src_str != '\0') {
        /** Each loop iteration adds one char so we need to realloc more memory
            for the char pointers (string) for the array element that is being
            assigned to at the moment. Before the split char is found it is element 0
            afterwards it is 1.
        **/
        if(arr_idx == 0) {
            el_zero = realloc(el_zero, ((loop_counter + 1) * sizeof(char)));
            exit_w_one_on_failed_xalloc(el_zero);
            res_arr[0] = el_zero;
        }
        else if(arr_idx == 1) {
            el_one = realloc(el_one, ((loop_counter + 1) * sizeof(char)));
            exit_w_one_on_failed_xalloc(el_one);
            res_arr[1] = el_one;
        }

        /** When the char to split at is found we raise the array index to be
            assigned to from 0 to 1, we raise the src_str pointer address also
            because we want to skip the split char and we reset the loop counter
            to zero. So next iteration the second part of the string is written
            to element 1 (=second element!)
        **/
        if(*src_str == splitat) {
            el_zero = realloc(el_zero, ((loop_counter + 1) * sizeof(char)));
            exit_w_one_on_failed_xalloc(el_zero);
            res_arr[arr_idx][loop_counter] = '\0';
            arr_idx++;
            src_str++;
            loop_counter = 0;
        }

        /* Actually assign the current char to the suiting position */
        res_arr[arr_idx][loop_counter] = *src_str;

        src_str++;
        loop_counter++;
    }

    el_one = realloc(el_one, ((loop_counter + 1) * sizeof(char)));
    exit_w_one_on_failed_xalloc(el_one);
    res_arr[1][loop_counter] = '\0';

    /* Reset position of src_str char pointer (=string to) start */
    src_str = startpos;

    return res_arr;
}

void loop_till_blk_dev_valid(char** blk_devs, char* user_choice, char* question, u_int32_t amount_blk_devs)
{
    while(!check_blk_dev_valid(blk_devs, user_choice, amount_blk_devs)) {
        printf(question);
        scanf(" %s", user_choice);
    }
}

void null_char_buffer(char* buf, u_int32_t size) {
    for(u_int32_t i = 0; i < size; i++) {
        buf[i] = '\0';
    }
}

u_int8_t str_startswith(char* startswith, char* dst)
{
    while(*startswith != '\0') {
        if(*startswith != *dst) {
            return 0;
        }
        startswith++;
        dst++;
    }
    return 1;
}
