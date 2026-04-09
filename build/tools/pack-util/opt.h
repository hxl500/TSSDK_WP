#ifndef __OPT_H__
#define __OPT_H__

#include <stdlib.h>

#ifdef PATH_CHAR_MAX
#error PATH_CHAR_MAX has been defined
#endif
#define PATH_CHAR_MAX    (256)

#ifdef COMM_MAX
#error COMM_MAX has been defined
#undef COMM_MAX
#endif
#define COMM_MAX    (32)

typedef struct opt_st {
    int verbose;
    int version;
    int help;

    int verify;

    char output[PATH_CHAR_MAX];
    char priv_key[PATH_CHAR_MAX];
    char otp_key[PATH_CHAR_MAX];
    char iv[PATH_CHAR_MAX];
    char user_id[PATH_CHAR_MAX];
    char pub_key_hash[PATH_CHAR_MAX];
    int boot_image_version;

    char input[PATH_CHAR_MAX];
} opt_t;

int opt_destroy(opt_t *opt);
void opt_info(opt_t *opt);
void opt_help(void);
opt_t *opt_parse(int argc,char *argv[]);

#endif
