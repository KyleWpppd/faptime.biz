#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#include "../src/faptime_table.h"

#define ONERROR_RETURN_NULL 0
#define ONERROR_ABORT 1
#define ONERROR_FAIL_TEST 2
#define DONE() do { fprintf(stderr, "DONE\n"); } while(0)

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
	fprintf(stderr, "Testing table validation\n");
	const int fail = 0;
	const int pass = 1;
	struct fixture {
		char *table;
		char *allowed;
		int expect;

	};

	struct fixture fixtures[] = {
		{.table=NULL,      .allowed=NULL, .expect=fail},
		{.table=NULL,      .allowed="abc", .expect=fail},
		{.table="",        .allowed="abc", .expect=fail},
		{.table="abc",     .allowed="xyz", .expect=fail},
		{.table="aa",      .allowed="xyz", .expect=fail},
		{.table="xxyyzz",  .allowed="xyz", .expect=fail},
		{.table="zyx",     .allowed="xyz", .expect=pass},
		{.table="/^",      .allowed=NULL, .expect=fail},
		{.table="abc",     .allowed=NULL, .expect=pass},
	};

	size_t fixture_ct = sizeof(fixtures) / sizeof(fixtures[0]);
	for (size_t i = 0; i < fixture_ct; i++) {
		int is_valid = faptime_valid_table(fixtures[i].table, fixtures[i].allowed);
		fprintf(stderr, "Running test for '%s' and '%s'. Expecting %s: ",
				!fixtures[i].table ? "(null)" : fixtures[i].table,
				!fixtures[i].allowed ? "(null)" : fixtures[i].allowed,
				fixtures[i].expect ? "PASS" : "FAIL"
				);
		int pass = (is_valid == fixtures[i].expect);
		fprintf(stderr, "%s\n", pass ? "OK" : "ERROR");
		ck_assert(pass);
	}
	fprintf(stderr, "DONE\n");
}
END_TEST

#define TOSTR1(x) (#x[0])
#define TOSTR(x) TOSTR1(x)
START_TEST(test_table_chartoi) {
	fprintf(stderr, "Testing conversion from character to integer base\n");
	faptime_table_t *ft = faptime_table_from_string("0123456789", NULL);
	ck_assert_msg(ft != NULL, "Table generation should succeed for a valid string");

	for (int i=0; i < 10; i++) {
		char intrep[2];
		sprintf(intrep, "%d", i);
		int tableval = faptime_table_chartoi(ft, intrep[0]);
		fprintf(stderr, "Testing that in the table '%s', '%d' == %d: %s\n", ft->table, i, tableval, tableval == i ? "OK" : "ERROR");
		ck_assert_int_eq(i, tableval);
	}
	faptime_free_table(ft);
	ft = faptime_table_from_string("abc", NULL);
	for (int i=0; i < ft->len; i++) {
		char c = ft->table[i];
		int tableval = faptime_table_chartoi(ft, c);
		fprintf(stderr, "Testing that in the table '%s', '%c' == %d: %s\n", ft->table, c, i, tableval == i ? "OK" : "ERROR");
		ck_assert_int_eq(i, tableval);
	}
	faptime_free_table(ft);
	DONE();
} END_TEST

START_TEST(test_table_antoi) {
	fprintf(stderr, "Testing conversion from character string to integer\n");
	/* A table that maps to ordinal numbers should return the same int value */
	faptime_table_t *ft = faptime_table_from_string("0123456789", NULL);

	struct fixture {
		char *str;
		int expect;
	};

	struct fixture fixtures[] = {
		{.str=NULL,         .expect=-1},
		{.str="",           .expect=0},
		{.str="0",          .expect=0},
		{.str="1",          .expect=1},
		{.str="10",         .expect=10},
		{.str="1234",       .expect=1234},
		{.str="badstring",  .expect=-1},
	};

	int fixture_len = sizeof(fixtures) / sizeof(fixtures[0]);
	for (int i = 0; i < fixture_len; i++) {
		int val = faptime_table_antoi(ft, fixtures[i].str);
		int pass = val == fixtures[i].expect;
		fprintf(stderr, "Testing that given table '%s', the string '%s' evaluates to %d: %s (%d)\n",
				ft->table, fixtures[i].str, fixtures[i].expect, pass ? "OK" : "ERROR", val);
		ck_assert_int_eq(val, fixtures[i].expect);
	}
	faptime_free_table(ft);
	fprintf(stderr, "DONE\n");
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
