#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../src/faptime_table.h"

#define ONERROR_RETURN_NULL 0
#define ONERROR_ABORT 1
#define ONERROR_FAIL_TEST 2

/* static char *mstrcpy(char *dst, char *str) { */
/*     if (! (dst = calloc(strlen(str)+1, sizeof *str))) { */
/*         ck_abort_msg("Failed to allocate string for test"); */
/*         abort(); */
/*     } */

/*     strncpy(dst, str, strlen(str)); */
/*     return dst; */
/* } */

START_TEST(test_table_generation)
{
	char *table = calloc(128, sizeof(table[0]));
	ck_assert_msg(table != NULL, "Allocation should succeed");
	/* Ensure a table too big fails */
	ck_assert_int_eq(faptime_random_table_string(100, table), -1);

	/* Ensure an appropriately sized table succeeds and is the correct length */
	ck_assert_int_eq(faptime_random_table_string(10, table), 10);
	ck_assert_int_eq(strnlen(table, 10+1), 10);
}
END_TEST

START_TEST(test_table_validation)
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

    struct fixture f0 = {.table="abc", .allowed="xyz", .expect=fail};
    struct fixture f1 = {.table="aa", .allowed="xyz", .expect=fail};
    struct fixture f2 = {.table="xxyyzz", .allowed="xyz", .expect=fail};
    struct fixture f3 = {.table="zyx", .allowed="xyz", .expect=pass};
    fixtures[0] = &f0;
    fixtures[1] = &f1;
    fixtures[2] = &f2;
    fixtures[3] = &f3;

    /* fixtures[0]->expect = fail; */
    /* mstrcpy(fixtures[0]->table, "abc"); */
    /* mstrcpy(fixtures[0]->allowed, "xyz"); */

	/* fixtures[1]->expect = fail; */
	/* mstrcpy(fixtures[1]->table, "aa"); */
    /* mstrcpy(fixtures[1]->allowed, "xyz"); */

	/* fixtures[2]->expect = fail; */
	/* mstrcpy(fixtures[2]->table, "xxyyzz"); */
    /* mstrcpy(fixtures[2]->allowed, "xyz"); */

	/* fixtures[3]->expect = pass; */
    /* mstrcpy(fixtures[3]->table, "zyx"); */
    /* mstrcpy(fixtures[3]->allowed, "xyz"); */

	for (int i = 0; i < NUM_FIXTURES; i++) {
		ck_assert_int_eq(fixtures[i]->expect,
						 faptime_valid_table(fixtures[i]->table,
											 strlen(fixtures[i]->table),
											 fixtures[i]->allowed));
	}


}
END_TEST

Suite * table_suite(void)
{
	Suite *s;
	TCase *tc_table_generation;
	TCase *tc_table_validation;

	s = suite_create("Table Tests");

	/* Table generation test case */
	tc_table_generation = tcase_create("Table Generation");

	tcase_add_test(tc_table_generation, test_table_generation);
	suite_add_tcase(s, tc_table_generation);

	/* Table validation test case */
	tc_table_validation = tcase_create("Table Validation");

	tcase_add_test(tc_table_validation, test_table_validation);
	suite_add_tcase(s, tc_table_validation);

	return s;
}

int main(void)
{
	int number_failed;
	 Suite *s;
	 SRunner *sr;

	 s = table_suite();
	 sr = srunner_create(s);

	 srunner_run_all(sr, CK_NORMAL);
	 number_failed = srunner_ntests_failed(sr);
	 srunner_free(sr);
	 return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
