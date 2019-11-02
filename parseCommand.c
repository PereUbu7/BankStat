#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "main.h"
#include "help.h"

/* Flag set by --verbose. */
static int verbose_flag;

int main(int argc, char **argv)
{
  int c, append = 0, update = 0, calc_month_sum = 0, add_tag = 0, add_keyword = 0, transaction_out = 0, calc_out = 0, transaction_in = 0, calc_in = 0;
  char *appendFile, *calc_month_sumFile, *add_tagFILE, *add_keywordFile, *transaction_outFile, *calc_outFile, *transaction_inFile, *calc_inFile;

  // test(verbose_flag);

  while (1)
  {
    static struct option long_options[] =
        {
            /* These options set a flag. */
            {"verbose", no_argument, &verbose_flag, 1},
            {"brief", no_argument, &verbose_flag, 0},
            /* These options dont set a flag.
             We distinguish them by their indices. */

            // Append new transactions from bank file
            {"APPEND_FROM_BANK", required_argument, 0, 'a'},

            // Sort transaction list and remove twin elements
            {"UPDATE", no_argument, 0, 'u'},

            // Calculates monthly sum for "tag"
            {"CALC_MONTH_SUM", required_argument, 0, 'm'},

            // Prints all avaiable tags
            {"SHOW_TAGS", no_argument, 0, 'x'},

            // Prints all keywords in a named tag
            {"SHOW_KEYWORDS", required_argument, 0, 's'},

            // Adds a named tag
            {"ADD_TAG", required_argument, 0, 't'},

            // Adds a keyword to a tag
            {"ADD_KEYWORD", required_argument, 0, 'k'},
            {"TRANSACTION_OUT", required_argument, 0, 'O'},
            {"CALC_OUT", required_argument, 0, 'o'},
            {"TRANSACTION_IN", required_argument, 0, 'I'},
            {"CALC_IN", required_argument, 0, 'i'},

            // Prints first and last date in transaction list
            {"SHOW_TRANSACTION_INTERVAL", no_argument, 0, 'd'},

            // Prints help text
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}};
    /* getopt_long stores the option index here. */
    int option_index = 0;

    // Return -1 is end of options
    if ((c = getopt_long(argc, argv, "a:um:t:k:O:o:I:i:", long_options, &option_index)) == -1)
      break;
    //if(verbose_flag) printf("Setting verbose flag\n");
    switch (c)
    {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
        break;
      printf("option %s", long_options[option_index].name);
      if (optarg)
        printf(" with arg %s", optarg);
      printf("\n");
      break;

    case 'a':
      if (verbose_flag)
        printf("Appending bank file %s to stored transactions\n", optarg);
      append = 1;
      appendFile = optarg;
      appendRoutine(appendFile, verbose_flag);
      break;

    case 'u':
      if (verbose_flag)
        puts("Updating transaction list\n");
      update = 1;
      break;

    case 'm':
      if (verbose_flag)
        printf("Calculating monthly sum for tags: %s\n", optarg);
      calc_month_sum = 1;
      calc_month_sumFile = optarg;
      writeTagCostTabRoutine(calc_month_sumFile, verbose_flag);
      break;

    case 'x':
      if (verbose_flag)
        printf("Show stored tags\n");
      printTagsRoutine(verbose_flag);
      break;

    case 's':
      if (verbose_flag)
        printf("Show keywords in tag:%s\n", optarg);
      printKeyWordsRoutine(optarg, verbose_flag);
      break;

    case 't':
      if (verbose_flag)
        printf("Adding tag:%s\n", optarg);
      addTagRoutine(optarg, verbose_flag);
      break;

    case 'k':
      if (verbose_flag)
        printf("Adding keyword:%s\n", optarg);
      addKeywordRoutine(optarg, verbose_flag);
      break;

    case 'd':
      if (verbose_flag)
        printf("Show transaction interval.\n");
      printTransInterval(verbose_flag);
      break;

    case 'h':
      help();
      break;

    case '?':
      help();
      break;

    default:
      help();
      //abort ();
    }
  }


  /* Instead of reporting --verbose
     and --brief as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
  {
    printf("non-option ARGV-elements: ");
    while (optind < argc)
      printf("%s ", argv[optind++]);
    putchar('\n');
  }

  system("PAUSE");

  exit(0);
}
