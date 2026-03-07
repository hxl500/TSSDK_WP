#include <opt.h>
#include <string.h>
#include <stdlib.h>
#include <log.h>
#include <getopt.h>

#define mfree(ptr)  do{if(ptr) {free(ptr);ptr = NULL;}}while(0)

typedef struct optdes_st{
    char *s;    //short option
    char *l;    //long option
    char *d;    //description
}optdes_t;

const optdes_t help[] = {
    {"V", "verbose", "verbose information"},
    {"v", "version", "version information"},
    {"h", "help", "display this message"},

    {"o", "output", "output file, default to stdout [output]"},
    {"p", "private-key", "SM2 private key [input]"},
    {"k", "user-root-key", "user root key, used to encrypt secure header with ecb mode [input]"},
    {"e", "soc-vendor-key", "soc vendor key, input, used to encrypt user id and boot image with cbc mode [input]"},
    {"i", "iv", "chain mode initial vector, input, must as length as SM4 block size, used to encrypt user id and boot image [input]"},
    {"u", "user-id", "user id file, input, if not specified, SM2 default user id will be used [input]"},
    {"a", "public-key-hash", "public key hash using SM3, output, burn this value in OTP [output]"},
    {"n", "boot-image-version", "boot image version, used for unti-rollback [input]"},
    {"y", "verify", "verify generated flash image and bootrom secure boot flow"},
};

void opt_help(void)
{
    info("\npack-util [OPTIONS] BOOT-IMAGE-FILE\n");
    info("\nflash image pack tool.\n");
	info("if no -p, --private-key option, pack boot image in none secure mode, otherwise pack boot image in secure mode\n\n");
    int i;
    for(i = 0;i<sizeof(help)/sizeof(help[0]);++i) {
        info("-%s, --%s\n", help[i].s, help[i].l);
        info("        %s\n", help[i].d);
    }
    info("\n");
    return;
}

int opt_destroy(opt_t *opt)
{
    if(opt)
        free(opt);
    return 0;
}

void opt_info(opt_t *opt)
{
    if(NULL == opt) {
        info("NULL option\n");
        return;
    }
    /* info("[%p]\n", opt); */

    info("%-10s %s\n", "verbose", opt->verbose?"TRUE":"FALSE");
    info("%-10s %s\n", "help", opt->help?"TRUE":"FALSE");
    info("%-10s %s\n", "verify", opt->verify?"TRUE":"FALSE");

    info("%-10s %s\n", "boot image", opt->input);
    info("%-10s %s\n", "flash image", opt->output);
    info("%-10s %s\n", "private key", opt->priv_key);
    info("%-10s %s\n", "user root key", opt->otp_key);
    info("%-10s %s\n", "iv", opt->iv);
    info("%-10s %s\n", "user id", opt->user_id);
    info("%-10s %s\n", "public key hash", opt->pub_key_hash);
    info("%-10s 0x%x\n", "boot image version", opt->boot_image_version);
}

opt_t* opt_parse(int argc, char* argv[])
{
    opt_t* opt = malloc(sizeof(opt_t));
    if (NULL == opt) {
        err("malloc failed\n");
        return NULL;
    }
    memset(opt, 0x00, sizeof(*opt));
    while (1) {
        static struct option long_options[] = {
            /* These options set a flag. */
            { "verbose", no_argument, 0, 'V' },
            { "version", no_argument, 0, 'v' },
            { "help", no_argument, 0, 'h' },

            { "verify", no_argument, 0, 'y' },

            { "output", required_argument, 0, 'o' },
            { "private-key", required_argument, 0, 'p' },
            { "user-root-key", required_argument, 0, 'k' },
            { "iv", required_argument, 0, 'i' },
            { "user-id", required_argument, 0, 'u' },
            { "public-key-hash", required_argument, 0, 'a' },
            { "boot-image-version", required_argument, 0, 'n' },

            { 0, 0, 0, 0 },
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        int c = getopt_long(argc, argv, "o:p:k:i:u:a:n:Vvhy",
            long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 'v':
            opt->version = 1;
            break;
        case 'V':
            opt->verbose = 1;
            break;
        case 'h':
            opt->help = 1;
            break;

        case 'y':
            opt->verify = 1;
            break;

        case 'o':
            strcpy(opt->output, optarg);
            break;
        case 'p':
            strcpy(opt->priv_key, optarg);
            break;
        case 'k':
            strcpy(opt->otp_key, optarg);
            break;
        case 'i':
            strcpy(opt->iv, optarg);
            break;
        case 'u':
            strcpy(opt->user_id, optarg);
            break;
        case 'a':
            strcpy(opt->pub_key_hash, optarg);
            break;
        case 'n':
            opt->boot_image_version = strtol(optarg, NULL, 0);
            break;

        case '?':
            /* getopt_long already printed an error message. */
            goto err0;
            break;
        default:
            abort();
        }
    }

    /* the last argument is input file */
    if (argc > optind)
        strcpy(opt->input, argv[optind]);

    return opt;
err0:
    opt_destroy(opt);
    return NULL;
}
