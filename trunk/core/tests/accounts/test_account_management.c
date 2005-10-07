#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>
#include "account_management.h"
#include "list.h"

// *INDENT-OFF*

START_TEST(test_account_management) {
	
	int MAX_ACCOUNT_NUMBER = 10;
	
	int accIds[MAX_ACCOUNT_NUMBER];
	
	set_xml_source("./accounts.xml");
	
	/* Test account_create */

	int len_before = get_length();
	
	int acc_id = account_create();
	
	int len_after = get_length();
   
	if (len_before != len_after-1) {
		fail("FAILED: no new account has been created.");
	}
      
	if (acc_id != tail->acc->id) {
		fail("FAILED: new account has wrong ID.");
	}
   
	/* Test account_get_all */
   
	int i;
	
	accIds[0] = acc_id;
	for (i = 1; i < MAX_ACCOUNT_NUMBER; i++) {
   		accIds[i] = account_create();
	}
	
    int *ids;
    int len;

    ids = (int *) malloc(MAX_ACCOUNT_NUMBER * sizeof(int));
    account_get_all(ids, &len);

    for (i = 0; i < len; i++) {
		if (ids[i] != accIds[i]) {
			fail("FAILED: got wrong accounts.");	
		}
	}
	free(ids);
   
	struct account *a = get_node(acc_id)->acc;
   
	/* Test account_set */
   
	account_set(acc_id, "name", "Nice new account");
	
	if (strcmp(a->name, "Nice new account") != 0) {
		fail("FAILED: wrong name set.");	
	}
	
	/* Test account_get */
	char *c = (char *) malloc(500 * sizeof(char));
	
	account_get(acc_id, "name", c);
	
    if (strcmp(c, "Nice new account") != 0) {
		fail("FAILED: wrong name got.");	
	}
	
	free(c);
	
	/* Test account_delete */
	len_before = get_length();
	account_delete(acc_id);
	
	len_after = get_length();
	
	if (len_before != len_after+1) {
		fail("FAILED: no account has been deleted."); 
	}
	
	ids = (int *) malloc(MAX_ACCOUNT_NUMBER * sizeof(int));
	account_get_all(ids, &len);
	
	for (i = 0; i < len; i++) {
		if (ids[i] == acc_id) {
			fail("FAILED: wrong account deleted.");	
		}
    }
	free(ids);
	
}
END_TEST 

// *INDENT-ON*

Suite *account_management_suite(void) {
	Suite *s = suite_create("Account Management Test Suite\n\n");
	TCase *tc_create = tcase_create("Account Management");

	suite_add_tcase(s, tc_create);

	tcase_add_test(tc_create, test_account_management);

	return s;
}

int main(void) {
	int nf = 0;
	Suite *s = account_management_suite();
	SRunner *sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
