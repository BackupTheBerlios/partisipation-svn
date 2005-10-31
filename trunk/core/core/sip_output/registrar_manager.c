#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <util/threads/thread_management.h>
#include <accounts/account_core_interface.h>
#include <sipstack/sip_stack_interface.h>
#include <util/logging/logger.h>
#include <core/gui_output/gui_callback_sender.h>

#define MAX_REGISTRAR_ACCOUNTS 64
#define EXPIRE 10
#define REGISTRAR_MGR_MSG_PREFIX "[registrar manager] "
#define REGISTRATION_TIMEOUT 10

typedef struct {
	int accountId;
	int isRegistered;
	int isRefreshed;
	int regId;
	int eventArrived;
	int doShutdown;
} accountstatus;

accountstatus *accInfos;

pthread_mutex_t accInfoLock;

void clear_account_info(int pos) {
	accInfos[pos].accountId = -1;
	accInfos[pos].isRegistered = 0;
	accInfos[pos].isRefreshed = 0;
	accInfos[pos].regId = -1;
	accInfos[pos].eventArrived = 0;
	accInfos[pos].doShutdown = 0;
}

int rm_init() {
	int i;
	int rc;

	rc = pthread_mutex_init(&accInfoLock, NULL);
	if (rc != 0) {
		// ERROR
		return 0;
	}

	accInfos =
		(accountstatus *) malloc(MAX_REGISTRAR_ACCOUNTS *
								 sizeof(accountstatus));
	for (i = 0; i < MAX_REGISTRAR_ACCOUNTS; i++) {
		clear_account_info(i);
	}
	return 1;
}

int rm_destroy() {
	int rc;

	if (accInfos) {
		free(accInfos);
	}

	rc = pthread_mutex_destroy(&accInfoLock);
	if (rc != 0) {
		// ERROR
		return 0;
	}

	return 1;
}

int find_acc_by_id(int accountId, accountstatus * result) {
	int i;
	for (i = 0; i < MAX_REGISTRAR_ACCOUNTS; i++) {
		if (accInfos[i].accountId == accountId) {
			if (result) {
				*result = accInfos[i];
			}
			return 1;
		}
	}

	return 0;
}

int find_empty_acc_info() {
	int i;
	for (i = 0; i < MAX_REGISTRAR_ACCOUNTS; i++) {
		if (accInfos[i].accountId == -1) {
			return i;
		}
	}

	// ERROR: all positions filled:
	return -1;
}

int find_acc_by_regid(int regId) {
	int i;
	for (i = 0; i < MAX_REGISTRAR_ACCOUNTS; i++) {
		if (accInfos[i].regId == regId) {
			return i;
		}
	}

	// not found:
	return -1;
}

void leave_reg_thrd_with_error(int pos, int accountId) {
	int rc;

	rc = go_change_reg_status(accountId, 0);
	if (!rc) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "GUI registration"
				  " status update failed");
	}

	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "mutex lock could "
				  "not be acquired, error: %d", rc);
		thread_terminated();
	}

	clear_account_info(pos);

	pthread_mutex_unlock(&accInfoLock);
	thread_terminated();

}

void *registration_thread(void *args) {
	int accountId;
	int pos;
	int rc;
	int counter;
	int timeoutCtr;
	struct account *acc;
	char *from;
	char *registrar;

	accountId = (int) args;

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "registration thread entered, "
			  "accountId: %d", accountId);

	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be"
				  "acquired, error: %d", rc);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "mutex lock acquired");

	rc = find_acc_by_id(accountId, NULL);
	if (rc) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "account already in use");
		// ERROR: account already in use
		pthread_mutex_unlock(&accInfoLock);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "account is not in use");

	pos = find_empty_acc_info();
	if (pos == -1) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "no empty position found");
		// ERROR: no free position found
		pthread_mutex_unlock(&accInfoLock);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "position found: %d", pos);

	accInfos[pos].accountId = accountId;
	accInfos[pos].isRegistered = 0;

	acc = am_get_account(accountId);
	if (!acc) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
				  "account management did not return"
				  " matching account data.");
		// ERROR
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "account management found matching "
			  "account data.");

	if (!acc->domain || !acc->username || !acc->registrar) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
				  "domain, username or registrar is missing");
		// ERROR
		thread_terminated();
		return NULL;
	}

	if (strcmp(acc->domain, "") == 0 || strcmp(acc->username, "") == 0
		|| strcmp(acc->registrar, "") == 0) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
				  "domain, username or registrar is empty");
		// ERROR
		thread_terminated();
		return NULL;
	}
	// build SIP From and SIP Registrar:
	from = (char *) malloc(1024 * sizeof(char));
	registrar = (char *) malloc(1024 * sizeof(char));
	sprintf(from, "sip:%s@%s", acc->username, acc->domain);
	sprintf(registrar, "sip:%s", acc->registrar);

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "from: %s registrar: %s", from,
			  registrar);

	// now send initial REGISTER
	int regId = sipstack_send_register(from, registrar, EXPIRE);
	if (regId == -1) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "send register failed");
		// ERROR 
		free(from);
		free(registrar);

		leave_reg_thrd_with_error(pos, accountId);
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
			  "send register succeeded, regId: %d", regId);

	accInfos[pos].regId = regId;

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "now waiting on OK");

	// marked account as "used"
	pthread_mutex_unlock(&accInfoLock);

	// now wait on response:
	timeoutCtr = 0;
	while (!accInfos[pos].eventArrived) {
		sched_yield();
		usleep(100000);			// 0.1 seconds
		timeoutCtr++;
		if (timeoutCtr == REGISTRATION_TIMEOUT * 10) {
			break;
		}
	}
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "done waiting");

	accInfos[pos].eventArrived = 0;

	if (from) {
		free(from);
	}
	if (registrar) {
		free(registrar);
	}

	if (!accInfos[pos].isRegistered) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "registering failed");
		// ERROR: retry?

		leave_reg_thrd_with_error(pos, accountId);
		return NULL;
	}

	rc = go_change_reg_status(accountId, 1);
	if (!rc) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "GUI registration status update"
				  " failed");

		leave_reg_thrd_with_error(pos, accountId);
		return NULL;
	}
	// now we are responsible for updating registration
	counter = 0;
	while (!accInfos[pos].doShutdown) {
		sleep(1);
		counter++;
		if (counter == EXPIRE) {
			counter = 0;

			rc = sipstack_send_update_register(regId, EXPIRE);
			if (rc == 0) {
				LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "send update register"
						  " failed");

				leave_reg_thrd_with_error(pos, accountId);
				return NULL;
			}

			LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "now waiting on OK");

			timeoutCtr = 0;
			// now wait on response:
			while (!accInfos[pos].eventArrived) {
				sched_yield();
				usleep(100000);	// 0.1 seconds
				timeoutCtr++;
				if (timeoutCtr == REGISTRATION_TIMEOUT * 10) {
					break;
				}
			}
			LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "done waiting");

			if (!accInfos[pos].isRefreshed) {
				LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
						  "updating registration failed");
				// ERROR: retry?

				leave_reg_thrd_with_error(pos, accountId);
				return NULL;
			}

			accInfos[pos].isRefreshed = 0;
			accInfos[pos].eventArrived = 0;

			LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "registration updated for "
					  "account: %d", accountId);
		}
	}

	thread_terminated();
	return NULL;
}

void *unregistration_thread(void *args) {
	return NULL;
}

void *autoregistration_thread(void *args) {
	return NULL;
}

int rm_register_account(int accountId) {
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "entering rm_register_account()");
	int rc;
	rc = start_thread(registration_thread, (void *) accountId);
	if (!rc) {
		// ERROR
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
				  "leaving rm_register_account(): ERROR");
		return 0;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX " leaving rm_register_account()");
	return 1;
}

int rm_unregister_account(int accountId) {
	int rc;
	rc = start_thread(unregistration_thread, (void *) accountId);
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int rm_register_auto() {
	int rc;
	rc = start_thread(autoregistration_thread, NULL);
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int rm_receive_register_event(sipstack_event * event) {
	int rc;
	int pos;

	if (!event) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX
				  "register_event: NULL received");
		// ERROR
		return 0;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "event:");
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\ttype: %d", event->type);
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\tstatusCode: %d",
			  event->statusCode);
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\tmessage: %s", event->message);
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\tcallId: %d", event->callId);
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\tdialogId: %d", event->dialogId);
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\ttransactionId: %d",
			  event->transactionId);
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "\tregId: %d", event->regId);

	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be"
				  "acquired, error: %d", rc);
		return 0;
	}

	pos = find_acc_by_regid(event->regId);
	if (pos == -1) {
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "no active registration thread "
				  "with this regId: %d", event->regId);
		pthread_mutex_unlock(&accInfoLock);
		return 0;
	}

	switch (event->type) {
		case EXOSIP_REGISTRATION_NEW:
			// ?
			break;
		case EXOSIP_REGISTRATION_SUCCESS:
			accInfos[pos].isRegistered = 1;
			accInfos[pos].isRefreshed = 1;
			break;
		case EXOSIP_REGISTRATION_FAILURE:
			accInfos[pos].isRegistered = 0;
			accInfos[pos].isRefreshed = 0;
			break;
		case EXOSIP_REGISTRATION_REFRESHED:
			accInfos[pos].isRegistered = 1;
			accInfos[pos].isRefreshed = 1;
			break;
		case EXOSIP_REGISTRATION_TERMINATED:
			accInfos[pos].isRegistered = 0;
			accInfos[pos].isRefreshed = 0;
			break;
		default:
			// wrong event type, only REGISTRATION events allowed
			break;
	}
	accInfos[pos].eventArrived = 1;

	pthread_mutex_unlock(&accInfoLock);

	rc = sipstack_event_free(event);
	if (!rc) {
		return 0;
	}
	return 1;
}
