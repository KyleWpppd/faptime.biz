#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../src/faptime_table.h"

#define ONERROR_RETURN_NULL 0
#define ONERROR_ABORT 1
#define ONERROR_FAIL_TEST 2

START_TEST(test_table_generation)
{
	char *table = NULL;
	/* Ensure an appropriately sized base succeeds and is the correct length */
    int tablelen = faptime_random_table_from_base(&table, 10);
    ck_assert_int_eq(tablelen, 10);
    ck_assert_msg(table != NULL, "Table should not be NULL");
    ck_assert_int_eq(strnlen(table, 10+1), 10);
	free(table);
	table = NULL;

	/* Ensure a base too big fails */
	ck_assert_int_eq(faptime_random_table_from_base(&table, 1000), -1);
	free(table);
	table = NULL;


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

	struct fixture fixtures[] = {
		{.table="abc", .allowed="xyz", .expect=fail},
		{.table="aa", .allowed="xyz", .expect=fail},
		{.table="xxyyzz", .allowed="xyz", .expect=fail},
		{.table="zyx", .allowed="xyz", .expect=pass},
		{.table="/^", .allowed=NULL, .expect=fail},
		{.table="abc", .allowed=NULL, .expect=pass},
	};

	char *msg = NULL;
	for (size_t i = 0; i < sizeof(fixtures); i++) {
		asprintf(&msg, "Expected %s (%d) for table '%s' with allowed chars '%s'.\n",
				 fixtures[i].expect ? "pass" : "fail", fixtures[i].expect,
				 fixtures[i].table, fixtures[i].allowed);
		ck_assert_msg(fixtures[i].expect ==
						faptime_valid_table(fixtures[i].table,
											fixtures[i].allowed),
					  msg);
		free(msg);
	}


}
END_TEST

#define TOSTR(x) ('x')
START_TEST(test_table_chartoi) {
	faptime_table_t *ft = faptime_table_from_string("0123456789", NULL);
    ck_assert_msg(ft != NULL, "Table generation should succeed for a valid string");

	for (int i=0; i < 10; i++) {
		ck_assert_int_eq(i, faptime_table_chartoi(ft, TOSTR(i)));
	}
} END_TEST

START_TEST(test_table_antoi) {
	/* A table that maps to ordinal numbers should return the same int value */
	faptime_table_t *ft = faptime_table_from_string("0123456789", NULL);

	char *str = NULL;
	str = strdup("1234");
	ck_assert_int_eq(1234, faptime_table_antoi(ft, str, strlen(str)));;
	free(str);

	str = strdup("0");
	ck_assert_int_eq(0, faptime_table_antoi(ft, str, strlen(str)));
	free(str);
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

	TCase *tc_table_conversion;
	tc_table_conversion = tcase_create("Table Numerical Conversion");
	tcase_add_test(tc_table_conversion, test_table_chartoi);
	tcase_add_test(tc_table_conversion, test_table_antoi);
	suite_add_tcase(s, tc_table_conversion);
	return s;
}

int main(void)
{
	int number_failed;
	 Suite *s;
	 SRunner *sr;

	 s = table_suite();
	 sr = srunner_create(s);
     srunner_set_fork_status(sr, CK_NOFORK);
	 srunner_run_all(sr, CK_NORMAL);
	 number_failed = srunner_ntests_failed(sr);
	 srunner_free(sr);
	 return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
