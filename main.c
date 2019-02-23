#include <getopt.h>
#include <linux/seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <kafel.h>

void usage() {
  fprintf(stderr, "kafel-run [-I PATH]... -k kafel.policy -- PROG [ARGS..]\n");
  exit(1);
}

int main(int argc, char **argv) {
  int c;
  struct sock_fprog prog;
  kafel_ctxt_t ctxt = kafel_ctxt_create();
  FILE *input = 0;

  while ((c = getopt(argc, argv, "hI:k:")) != -1) switch (c) {
      case 'h':
        usage();
      case 'I':
        kafel_add_include_search_path(ctxt, optarg);
        break;
      case 'k':
        if (input) {
          fprintf(stderr, "option -k can only be specified once.\n");
          exit(1);
        }
        input = fopen(optarg, "r");
        if (!input) {
          perror("error opening config file");
          exit(1);
        }
        break;
      case '?':
        if (optopt == 'k' || optopt == 'I') {
          fprintf(stderr, "option -%c requires an argument.\n", optopt);
          exit(1);
        } else {
          fprintf(stderr, "unknown option `-%c'.\n", optopt);
          exit(1);
        }
      default:
        abort();
    }

  if (!input) {
    fprintf(stderr, "option -c (config file) is required.\n");
    exit(1);
  }

  if (argc - optind < 1) {
    usage();
  }

  kafel_set_input_file(ctxt, input);

  if (kafel_compile(ctxt, &prog)) {
    fprintf(stderr, "policy compilation failed: %s", kafel_error_msg(ctxt));
    kafel_ctxt_destroy(&ctxt);
    exit(1);
  }

  kafel_ctxt_destroy(&ctxt);

  if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
    perror("disabling new priviledges failed");
    exit(1);
  }

  if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog, 0, 0) < 0) {
    perror("installing filter failed");
    exit(1);
  }
  free(prog.filter);
  fclose(input);

  if (execvp(argv[optind], argv + optind) == -1) {
    perror("exec error");
    exit(1);
  }

  return 0;
}