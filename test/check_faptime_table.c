#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../src/faptime_table.h"

START_TEST(check_table_generation)
{
	char *table = calloc(128, sizeof(table[0]));
	ck_assert_msg(table != NULL, "Allocation should succeed");
	/* Ensure a table too big fails */
	ck_assert_int_eq(faptime_random_table(100, table), -1);

	/* Ensure an appropriately sized table succeeds and is the correct length */
	ck_assert_int_eq(faptime_random_table(10, table), 10);
	ck_assert_int_eq(strnlen(table, 10+1), 10);
}
END_TEST

START_TEST(check_table_validation)
{
	const int fail = 0;
	const int pass = 1;
	struct fixture {
		char *table;
		char *allowed;
		int expect;

	};
	#define NUM_FIXTURES 4
	struct fixture *fixtures[NUM_FIXTURES];
	fixtures[0]->table	 = "abc";
	fixtures[0]->allowed = "xyz";
	fixtures[0]->expect	 = fail;

	fixtures[1]->table	 = "aa";
	fixtures[1]->allowed = "xyz";
	fixtures[1]->expect	 = fail;

	fixtures[2]->table	 = "xxyyzz";
	fixtures[2]->allowed = "xyz";
	fixtures[2]->expect	 = fail;


	fixtures[3]->table	 = "zyx";
	fixtures[3]->allowed = "xyz";
	fixtures[3]->expect	 = pass;


	for (int i = 0; i < NUM_FIXTURES; i++) {
		ck_assert_int_eq(fixtures[i]->expect,
						 faptime_valid_table(fixtures[i]->table,
											 strlen(fixtures[i]->table),
											 fixtures[i]->allowed));
	}

}
END_TEST
