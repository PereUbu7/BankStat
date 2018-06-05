#include <stdio.h>

void help()
{
     printf("bankStat: Make statistics from you bank account\n");
     printf("-----------------------------------------------\n");
     printf("\nArguments:\n");
     printf("--verbose\tWrite out info during runtime\n");
     printf("--brief\t\tOnly write if error or written file\n");
     printf("\n");
     printf("APPEND_FROM_BANK or -a\targument:file to append\tAppend bank file of transactions to existing transaction list\n");
     printf("CALC_MONTH_SUM or -m\targument:file to write sum table into\tSum up all costs per month in each tag\n");
	 printf("SHOW_TAGS or -x\t\tPrints all tags\n");
	 printf("SHOW_KEYWORDS or -s\targument:tag name\tPrints all keywords in a specific tag\n");
	 printf("ADD_TAG or -t\targument:\"tag name\"\tAdds a tag to the list of tags\n");
	 printf("ADD_KEYWORD or -k\targument:\"tag name|keyword name\"\tAdds a keyword to a certain tag\t");
	 printf("SHOW_TRANSACTION_INTERVAL or -d\t\tPrints start and end date of stored transactions\n");
}
