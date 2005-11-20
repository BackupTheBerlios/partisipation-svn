/** 
 * @file registrar_manager.c
 * A very simple state machine that (un-)registers user accounts.
 * 
 * SIP Accounts are managed in a stateful way, i.e. it knows the current
 * registration status and automatically updates a registration imminent to
 * expire. The registrar manager receives its commands from the GUI. Events are
 * provided by the SIP stack and forwarded by the event dispatcher.
 * 
 * Multiple (un-)registration attempts by the GUI for the same account are 
 * blocked. Autoregistration is also handled here, but has to called to the
 * proper moment.
 * 
 * This is the implementation of the registrar manager interface. The state 
 * machine is not implemented as one or at least not very obvious. Event 
 * queues, for instance, are not needed because there can only arrive one
 * event at a time for one specific account.
 *
 * @author Matthias Liebig
 */

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
#include <core/sip_output/registrar_manager.h>

/**
 * Logging prefix for registrar manager.
 */
#define REGISTRAR_MGR_MSG_PREFIX "[registrar manager] "

/**
 * Here the registration status of an account and some other flags are stored
 * (internal use only).
 */
typedef struct {
	int accountId; /**< The ID of the account.*/
	int isRegistered; /**< Whether the account is registered at the registrar. */
	int isRefreshed; /**< Whether the registration was just updated. */
	int regId; /**< The registration ID provided by eXosip. */
	int eventArrived; /**< Whether an event has just arrived. */
	int doShutdown;	/**< Whether a shutdown of a registration thread was 
						requested. */
	int isShutdown;	/**< Whether a registration thread is shutdown. */
	int waitingOnRegOK;	/**< Whether an OK for an initial REGISTER is 
							expected. */
	int waitingOnRefreshOK;	/**< Whether an OK for a n-th REGISTER is 
							expected. */
	int waitingOnUnregOK; /**< Whether an OK for a REGISTER with expire=0 is
							expected. */
	int informGui; /**< Whether the success of an unregistration has to be 
					announced to the GUI. */
} accountstatus;

/**
 * Global array to store all account status information.
 */
accountstatus *accInfos;

/**
 * Lock to prevent concurring access of the global array.
 */
pthread_mutex_t accInfoLock;

/**
 * Initialize an account status (internal use only).
 * @param pos position of accoun status information that has to be cleared in
 * the global array.
 */
void clear_account_info(int pos) {
	accInfos[pos].accountId = -1;
	accInfos[pos].isRegistered = 0;
	accInfos[pos].isRefreshed = 0;
	accInfos[pos].regId = -1;
	accInfos[pos].eventArrived = 0;
	accInfos[pos].doShutdown = 0;
	accInfos[pos].isShutdown = 0;
	accInfos[pos].waitingOnRegOK = 0;
	accInfos[pos].waitingOnRefreshOK = 0;
	accInfos[pos].waitingOnUnregOK = 0;
	accInfos[pos].informGui = 1;
}

int rm_init() {
	int i;						// counter
	int rc;						// return code

	// initialize mutex lock
	rc = pthread_mutex_init(&accInfoLock, NULL);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to gain mutex lock");
		return 0;
	}
	// reserve memory for status information about accounts
	// (maximum size is taken from account management)
	accInfos =
		(accountstatus *) malloc(config.accounts.accountManagement.
								 maxAccountIdAmount *
								 sizeof(accountstatus));

	// initialize data explicitely so we can be sure it is not being used yet
	for (i = 0; i < config.accounts.accountManagement.maxAccountIdAmount;
		 i++) {
		clear_account_info(i);
	}
	return 1;
}

int rm_destroy() {
	int rc;						// return code

	// release memory for status information about accounts
	if (accInfos) {
		free(accInfos);
	}
	// release mutex lock
	rc = pthread_mutex_destroy(&accInfoLock);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to release mutex lock");
		return 0;
	}

	return 1;
}

/**
 * Find account status information by account ID (internal use only).
 * @param accountId the account ID
 * @param pos a pointer where the position in the global array of the account
 * may be stored - use NULL if you do not wish to store it
 * @return whether the account was found (boolean)
 */
int find_acc_by_id(int accountId, int *pos) {
	int i;						// counter
	for (i = 0; i < config.accounts.accountManagement.maxAccountIdAmount;
		 i++) {
		if (accInfos[i].accountId == accountId) {
			// account found
			if (pos) {
				// assign only when given
				*pos = i;
			}
			return 1;
		}
	}

	// account not found
	return 0;
}

/**
 * Find an empty position in the global array (internal use only).
 * @return the position if an empty position could be found, -1 otherwise
 */
int find_empty_acc_info() {
	int i;						// counter
	for (i = 0; i < config.accounts.accountManagement.maxAccountIdAmount;
		 i++) {
		if (accInfos[i].accountId == -1) {
			// empty position found
			return i;
		}
	}

	// ERROR: all positions filled:
	return -1;
}

/**
 * Find account status information by registration ID (internal use only).
 * @param regId the registration ID
 * @return the position if the account could be found, -1 otherwise
 */
int find_acc_by_regid(int regId) {
	int i;						// counter
	for (i = 0; i < config.accounts.accountManagement.maxAccountIdAmount;
		 i++) {
		if (accInfos[i].regId == regId) {
			// account found, return position
			return i;
		}
	}

	// account not found:
	return -1;
}

/**
 * Common error handling of the registration thread (internal use only). In 
 * most cases the GUI has to be informed that the registration was not 
 * successfull and the account information has to be cleared.
 * @param pos the position of the account information of the current account
 * @param accountId the account ID of the current account
 * @param acquireLock to clear the account information we need to gain the
 * lock - not needed if it is already locked (boolean)
 */
void leave_reg_thrd_with_error(int pos, int accountId, int acquireLock) {
	int rc;						// return code

	// try to change the registration status at the GUI:
	rc = go_change_reg_status(accountId, 0);
	if (!rc) {
		// changing reg. status failed, continue anyway
		LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "GUI registration"
				  " status update failed");
	}
	// only lock if required
	if (acquireLock) {
		// lock, because we want to write when further events may arrive
		rc = pthread_mutex_lock(&accInfoLock);
		if (rc != 0) {
			// failed to gain lock, we have to leave
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could "
					  "not be acquired, error: %d", rc);
			thread_terminated();
		}
	}
	// if position previously filled:
	if (pos != -1) {
		// mark account as not being used:
		clear_account_info(pos);
	}
	// release lock (in any case):
	rc = pthread_mutex_unlock(&accInfoLock);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be "
				  "released, error %d", rc);
		thread_terminated();
	}
	// exit thread:
	thread_terminated();
}

/**
 * A thread that actually handles the registration request of the GUI (internal
 * use only). See rm_register_account() for details.
 * @param args the account ID is stored here
 * @return empty
 */
void *registration_thread(void *args) {
	int accountId;				// current account
	int pos;					// position of account status information
	int rc;						// return code
	int counter;				// counter for registration expire
	int timeoutCtr;				// timeout counter for sipstack events
	struct account *acc;		// account data from account management
	char *from;					// SIP from field
	char *registrar;			// SIP registrar field

	// accountId is given:
	accountId = (int) args;

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "registration thread entered, "
			  "accountId: %d", accountId);

	// try to gain lock because we want exclusive responsibility for this
	// account and we need to use a new position for account status info:
	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		// failed to gain lock, exit (fatal error)
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be"
				  "acquired, error: %d", rc);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "mutex lock acquired");

	// is another thread handling (un-)registration of this account?
	rc = find_acc_by_id(accountId, NULL);
	if (rc) {
		// account is blocked by another thread (impatient GUI user?)
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "account already in use");

		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"Account is already in use.",
								"Either this account is already "
								"registered or it is currently tried to "
								"unregister this account.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// inform GUI and exit:
		leave_reg_thrd_with_error(-1, accountId, 0);
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "account is not in use");

	// try to find empty position to save account status information
	// (there has to be one position for every account of account information)
	pos = find_empty_acc_info();
	if (pos == -1) {
		// no position found (fatal error):
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "no empty position found");
		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"Maximum number of active accounts "
								"exceeded.",
								"You have registered more accounts than "
								"are allowed by the Core configuration.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// inform GUI and exit:
		leave_reg_thrd_with_error(-1, accountId, 0);
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "position found: %d", pos);

	// if we set an account ID the array position is marked as used:
	accInfos[pos].accountId = accountId;
	accInfos[pos].isRegistered = 0;

	// get account data from account management:
	acc = am_get_account(accountId);
	if (!acc) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "account management did not "
				  "return matching account data.");

		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"No such account available.",
								"Account management did not return "
								"matching account data.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// inform GUI and exit:
		leave_reg_thrd_with_error(pos, accountId, 0);
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "account management found matching "
			  "account data.");

	// check fields required for registration:
	if (!acc->domain || !acc->username || !acc->registrar) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
				  "domain, username or registrar is missing");

		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"Account information is invalid.",
								"Your account is missing either domain, "
								"user name or registrar. Please check "
								"your account information.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// inform GUI and exit:
		leave_reg_thrd_with_error(pos, accountId, 0);
		return NULL;
	}

	if (strcmp(acc->domain, "") == 0 || strcmp(acc->username, "") == 0
		|| strcmp(acc->registrar, "") == 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
				  "domain, username or registrar is empty");

		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"Account information is invalid.",
								"Your account is missing either domain, "
								"user name or registrar. Please check "
								"your account information.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// inform GUI and exit:
		leave_reg_thrd_with_error(pos, accountId, 0);
		return NULL;
	}
	// build SIP From and SIP Registrar:
	from = (char *) malloc(1024 * sizeof(char));
	registrar = (char *) malloc(1024 * sizeof(char));
	sprintf(from, "sip:%s@%s", acc->username, acc->domain);
	sprintf(registrar, "sip:%s", acc->registrar);

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "from: %s registrar: %s", from,
			  registrar);

	// now send initial REGISTER to registrar
	int regId = sipstack_send_register(from, registrar,
									   config.core.sipOutput.
									   registrarManager.expire);
	if (regId == -1) {
		// no valid registration ID returned because of an error
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "send register failed");

		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"Sending registration message failed.",
								"Failed to send your registration "
								"request to the given registrar."
								"Please check whether your account "
								"data is correct and whether your "
								"internet connection is working "
								"correctly.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// free strings:
		free(from);
		free(registrar);

		// inform GUI and exit:
		leave_reg_thrd_with_error(pos, accountId, 0);
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "send register succeeded, "
			  "regId: %d", regId);

	accInfos[pos].regId = regId;
	accInfos[pos].waitingOnRegOK = 1;	// tell dispatcher which OK is expected

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "now waiting on OK");

	// marked account as "used"
	rc = pthread_mutex_unlock(&accInfoLock);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "releasing mutex lock "
				  "failed, error: %d", rc);

		// free strings:
		free(from);
		free(registrar);

		// inform GUI and exit:
		leave_reg_thrd_with_error(pos, accountId, 0);
		return NULL;
	}
	// now wait on response:
	timeoutCtr = 0;
	while (!accInfos[pos].eventArrived) {
		sched_yield();
		usleep(100000);			// 0.1 seconds
		timeoutCtr++;
		if (timeoutCtr ==
			config.core.sipOutput.registrarManager.timeout * 10) {
			break;
		}
	}
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "done waiting");

	// clear event waiting flags:
	accInfos[pos].eventArrived = 0;
	accInfos[pos].waitingOnRegOK = 0;

	// free strings:
	if (from) {
		free(from);
	}
	if (registrar) {
		free(registrar);
	}
	// test if we did receive an OK (isRegistered is set by dispatcher):
	if (!accInfos[pos].isRegistered) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "registering failed");

		rc = go_show_user_event(accountId, "ERROR",
								"Error registering account",
								"Sending registration message failed.",
								"Failed to send your registration "
								"request to the given registrar."
								"Please check whether your account "
								"data is correct and whether your "
								"internet connection is working "
								"correctly.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// inform GUI and exit:
		leave_reg_thrd_with_error(pos, accountId, 1);
		return NULL;
	}
	// inform GUI that registration succeeded:
	rc = go_change_reg_status(accountId, 1);
	if (!rc) {
		// could not contact GUI (error):
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "GUI registration status update"
				  " failed");

		// inform GUI and exit (it is unlikely to succeed but try anyway):
		leave_reg_thrd_with_error(pos, accountId, 1);
		return NULL;
	}

	LOG_INFO(REGISTRAR_MGR_MSG_PREFIX "registration was successful for "
			 "account: %d", accountId);
	// **********************************************************************

	// now we are responsible for updating registration
	counter = 0;
	while (!accInfos[pos].doShutdown) {
		sleep(1);				// test every second for shutdown or imminent expire
		counter++;
		if (counter ==
			config.core.sipOutput.registrarManager.expire -
			config.core.sipOutput.registrarManager.preExpireRange) {

			// now expire is nearly reached, update registration:

			counter = 0;

			// tell dispatcher on which OK we are waiting for:
			accInfos[pos].waitingOnRefreshOK = 1;

			// send n-th REGISTER to registrar:
			rc = sipstack_send_update_register(regId,
											   config.core.sipOutput.
											   registrarManager.expire);
			if (rc == 0) {
				// send REGISTER failed
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "send update register"
						  " failed");
				rc = go_show_user_event(accountId, "ERROR",
										"Error refreshing account "
										"registration",
										"Sending automated registration "
										"message failed.",
										"Failed to send registration "
										"refresh to the given registrar. "
										"Please check whether your account "
										"data is correct and whether your "
										"internet connection is working "
										"correctly.");
				if (!rc) {
					LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
							  "failed to inform the GUI");
				}
				// inform GUI and exit:
				leave_reg_thrd_with_error(pos, accountId, 1);
				return NULL;
			}

			LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "now waiting on OK");

			timeoutCtr = 0;
			// now wait on response:
			while (!accInfos[pos].eventArrived) {
				sched_yield();
				usleep(100000);	// 0.1 seconds
				timeoutCtr++;
				if (timeoutCtr ==
					config.core.sipOutput.registrarManager.timeout * 10) {
					break;
				}
			}
			LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "done waiting");

			// test if OK arrived:
			if (!accInfos[pos].isRefreshed) {
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
						  "updating registration failed");
				rc = go_show_user_event(accountId, "ERROR",
										"Error refreshing account "
										"registration",
										"Sending automated registration "
										"message failed.",
										"Failed to send registration "
										"refresh to the given registrar. "
										"Please check whether your account "
										"data is correct and whether your "
										"internet connection is working "
										"correctly.");
				if (!rc) {
					LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
							  "failed to inform the GUI");
				}
				// inform GUI and exit:
				leave_reg_thrd_with_error(pos, accountId, 1);
				return NULL;
			}
			// clear flags:
			accInfos[pos].isRefreshed = 0;
			accInfos[pos].eventArrived = 0;
			accInfos[pos].waitingOnRefreshOK = 0;

			LOG_INFO(REGISTRAR_MGR_MSG_PREFIX "registration updated for "
					 "account: %d", accountId);
		}
	}

	// if an unregister-thread requested quitting this thread we tell it that
	// we are done (no lock required because other thread is only reading):
	accInfos[pos].doShutdown = 0;
	accInfos[pos].isShutdown = 1;
	thread_terminated();
	return NULL;
}

/**
 * A thread that actually handles the unregistration request of the GUI
 * (internal use only). See rm_unregister_account() for details.
 * @param args the account ID is stored here
 * @return empty
 */
void *unregistration_thread(void *args) {
	int accountId;				// current account
	int pos;					// position of account status information
	int rc;						// return code
	int timeoutCtr;				// timeout counter for sipstack events

	// accountId is given:
	accountId = (int) args;

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "unregistration thread entered, "
			  "accountId: %d", accountId);

	// try to gain lock because we want exclusive responsibility for 
	// unregistering this account - also we want to write data:
	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		// failed to gain lock, exit (fatal error)
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be"
				  "acquired, error: %d", rc);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "mutex lock acquired");

	// a registration/update thread should be active and have saved account
	// information - find position in array of it: 
	rc = find_acc_by_id(accountId, &pos);
	if (!rc) {
		// no registration/update thread active
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "account not in use");

		rc = go_show_user_event(accountId, "ERROR",
								"Error unregistering account",
								"Account is not in use.",
								"This account is already unregistered.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// unlock and exit:
		pthread_mutex_unlock(&accInfoLock);
		thread_terminated();
		return NULL;
	}
	// prevent race conditions because we have to release the lock after
	// setting doShutdown to 1:
	if (accInfos[pos].doShutdown || accInfos[pos].isShutdown) {
		// a second unregister thread was started just when we are trying to
		// shutdown the active registration thread
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "another thread is already "
				  "unregistering");
		rc = go_show_user_event(accountId, "ERROR",
								"Error unregistering account",
								"Already unregistering.",
								"It is currently tried to unregister this "
								"account.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// unlock and exit:
		pthread_mutex_unlock(&accInfoLock);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "account is in use");

	// shutdown registration/update thread:
	accInfos[pos].doShutdown = 1;

	// release lock to enable abnormal termination (otherwise a dead lock
	// might occur if registration thread is trying to shutdown in case of
	// an error while updating):
	rc = pthread_mutex_unlock(&accInfoLock);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be "
				  "released, error %d", rc);
		thread_terminated();
		return NULL;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "wait for registration thread "
			  "to finish");

	// now wait on registration thread:
	// isShutdown == 1 means regular shutdown
	// accountId == -1 means the account status info was cleared because 
	// of an error
	while (!accInfos[pos].isShutdown && accInfos[pos].accountId != -1) {
		sched_yield();
		usleep(100000);			// 0.1 seconds
	}
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "done waiting");

	// we should not send an unregister if account is not registered
	if (!accInfos[pos].isRegistered || accInfos[pos].accountId == -1) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "unregister failed: account "
				  "is not registered");
		rc = go_show_user_event(accountId, "ERROR",
								"Error unregistering account",
								"Account is not in use.",
								"This account is already unregistered.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// exit:
		thread_terminated();
		return NULL;
	}
	// tell dispatcher which OK is expected:
	accInfos[pos].eventArrived = 0;
	accInfos[pos].waitingOnUnregOK = 1;

	// send REGISTER with expire=0 to registrar:
	rc = sipstack_send_unregister(accInfos[pos].regId);
	if (!rc) {
		// sending REGISTER failed
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "send unregister failed");
		rc = go_show_user_event(accountId, "ERROR",
								"Error unregistering account",
								"Sending unregistration message failed.",
								"Failed to send your unregistration "
								"request to the given registrar."
								"Please check whether your account "
								"data is correct and whether your "
								"internet connection is working "
								"correctly.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		// exit:
		thread_terminated();
		return NULL;
	}
	// now wait on response:
	timeoutCtr = 0;
	while (!accInfos[pos].eventArrived) {
		sched_yield();
		usleep(100000);			// 0.1 seconds
		timeoutCtr++;
		if (timeoutCtr ==
			config.core.sipOutput.registrarManager.timeout * 10) {
			break;
		}
	}
	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "done waiting");

	// clear flags:
	accInfos[pos].eventArrived = 0;
	accInfos[pos].waitingOnUnregOK = 0;

	// test if account is really unregistered:
	if (accInfos[pos].isRegistered) {
		// account is still in use:
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "unregister failed: account "
				  "is still registered");
		if (accInfos[pos].informGui) {
			rc = go_show_user_event(accountId, "ERROR",
									"Error unregistering account",
									"Sending unregistration message failed.",
									"Failed to send your unregistration "
									"request to the given registrar."
									"Please check whether your account "
									"data is correct and whether your "
									"internet connection is working "
									"correctly.");
			if (!rc) {
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
						  "failed to inform the GUI");
			}
		}
		// don't terminate because there is no point in believing the account 
		// was still registered with the registrar
	}

	if (accInfos[pos].informGui) {
		// inform GUI that unregister succeeded:
		rc = go_change_reg_status(accountId, 0);
		if (!rc) {
			// failed to contact GUI
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "GUI registration status "
					  "update failed");

			// exit:
			thread_terminated();
			return NULL;
		}
	}
	// mark account as no longer used:
	clear_account_info(pos);

	LOG_INFO(REGISTRAR_MGR_MSG_PREFIX
			 "unregister of account %d succeeded", accountId);

	// we are done:
	thread_terminated();
	return NULL;
}

/**
 * A thread that actually handles the auto_registration request when a GUI is
 * being started (internal use only). See rm_register_auto() for details.
 * @param args empty
 * @return empty
 */
void *autoregistration_thread(void *args) {
	struct account *accounts[config.accounts.accountManagement.
							 maxAccountIdAmount];
	int len;					// length of account data array
	int i;						// counter
	int rc;						// return code

	// retrieve all account data:
	am_get_all_accounts(accounts, &len);

	// check for accounts that have to be registered automatically
	for (i = 0; i < len; i++) {
		if (accounts[i]->autoregister) {
			// found one and start registration thread:
			rc = rm_register_account(accounts[i]->id);
			if (!rc) {
				// thread starting failed
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "starting registering "
						  "thread for account %d failed", accounts[i]->id);
				// don't inform GUI because rm_register_account already does it
			}
			// no guarantee that thread execution is error-free at this point
		}
	}

	// we are done:
	thread_terminated();
	return NULL;
}

/**
 * A thread that actually handles the request for unregistering all active
 * accounts when a GUI is being shut down (internal use only). See 
 * rm_unregister_all() for details.
 * @param args empty
 * @return empty
 */
void *unregisterall_thread(void *args) {
	int rc;
	int i;

	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		// failed to gain lock, fatal error
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be"
				  "acquired, error: %d", rc);
		return 0;
	}

	for (i = 0; i < config.accounts.accountManagement.maxAccountIdAmount;
		 i++) {
		if (accInfos[i].accountId != -1) {
			accInfos[i].informGui = 0;
		}
		if (accInfos[i].accountId != -1 && accInfos[i].isRegistered) {
			rc = rm_unregister_account(accInfos[i].accountId);
			if (!rc) {
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
						  "failed to automatically"
						  " unregister account: %d",
						  accInfos[i].accountId);
			}
		}
	}
	rc = pthread_mutex_unlock(&accInfoLock);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be "
				  "released, error %d", rc);
		thread_terminated();
		return NULL;
	}
	// we are done:
	thread_terminated();
	return NULL;
}

int rm_register_account(int accountId) {
	int rc;						// return code

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "entering rm_register_account()");

	rc = start_thread(registration_thread, (void *) accountId);
	if (!rc) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "leaving rm_register_account():"
				  " failed to start thread");
		rc = go_show_user_event(accountId, "FAILURE",
								"Error registering account",
								"Failed to start registration thread.",
								"The Core failed to perform your "
								"registration request because of an internal "
								"error.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		return 0;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "leaving rm_register_account()");
	return 1;					// pseudo success
}

int rm_unregister_account(int accountId) {
	int rc;						// return code

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "entering rm_unregister_account()");

	rc = start_thread(unregistration_thread, (void *) accountId);
	if (!rc) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "leaving "
				  "rm_unregister_account(): failed to start thread");
		rc = go_show_user_event(accountId, "FAILURE",
								"Error unregistering account",
								"Failed to start unregistration thread.",
								"The Core failed to perform your "
								"unregistration request because of an "
								"internal error.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		return 0;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "leaving rm_unregister_account()");
	return 1;					// pseudo success
}

int rm_register_auto() {
	int rc;						// return code

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "entering rm_register_auto()");

	rc = start_thread(autoregistration_thread, NULL);
	if (!rc) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "leaving rm_register_auto(): "
				  "failed to start thread");
		rc = go_show_user_event(-1, "FAILURE",
								"Error automatically registering account",
								"Failed to start autoregistration thread.",
								"The Core failed to perform your "
								"automated registration request because of an "
								"internal error.");
		if (!rc) {
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "failed to inform the GUI");
		}
		return 0;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "leaving rm_register_auto()");
	return 1;					// pseudo success
}

int rm_unregister_all() {
	int rc;						// return code

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "entering rm_unregister_all()");

	rc = start_thread(unregisterall_thread, NULL);
	if (!rc) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "leaving rm_unregister_all(): "
				  "failed to start thread");
		// don't inform GUI because it may be already gone
		return 0;
	}

	LOG_DEBUG(REGISTRAR_MGR_MSG_PREFIX "leaving rm_unregister_all()");
	return 1;					// pseudo success
}

int rm_receive_register_event(sipstack_event * event) {
	int rc;						// return code
	int pos;					// array position

	// sanity check:
	if (!event) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
				  "register_event: NULL received");
		return 0;
	}
	// acquire lock to prevent multiple events for same account are handled at
	// the same time
	rc = pthread_mutex_lock(&accInfoLock);
	if (rc != 0) {
		// failed to gain lock, fatal error
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be"
				  "acquired, error: %d", rc);
		return 0;
	}
	// event can only be in reaction of register, update register or 
	// unregister - therefore an active thread has to be there
	pos = find_acc_by_regid(event->regId);	// rely on eXosip registration ID
	if (pos == -1) {
		// no active thread found:
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "no active (un-)registration "
				  "thread with this regId: %d", event->regId);

		// unlock and exit:
		pthread_mutex_unlock(&accInfoLock);
		return 0;
	}
	// examine event:
	switch (event->type) {
		case EXOSIP_REGISTRATION_NEW:
			// never used by eXosip atm
			break;
		case EXOSIP_REGISTRATION_SUCCESS:
			// eXosip does not distinguish between OK types
			if (accInfos[pos].waitingOnRegOK) {
				accInfos[pos].isRegistered = 1;
			} else if (accInfos[pos].waitingOnRefreshOK) {
				accInfos[pos].isRefreshed = 1;
			} else if (accInfos[pos].waitingOnUnregOK) {
				accInfos[pos].isRegistered = 0;
			} else {
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "expected OK type "
						  "not set");
			}
			break;
		case EXOSIP_REGISTRATION_FAILURE:
			if (accInfos[pos].waitingOnRegOK) {
				accInfos[pos].isRegistered = 0;
			} else if (accInfos[pos].waitingOnRefreshOK) {
				accInfos[pos].isRegistered = 0;
				accInfos[pos].isRefreshed = 0;
			} else if (accInfos[pos].waitingOnUnregOK) {
				accInfos[pos].isRegistered = 1;
			} else {
				LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "expected OK type "
						  "not set");
			}
			break;
		case EXOSIP_REGISTRATION_REFRESHED:
			// never used by eXosip atm
			accInfos[pos].isRegistered = 1;
			accInfos[pos].isRefreshed = 1;
			break;
		case EXOSIP_REGISTRATION_TERMINATED:
			// never used by eXosip atm
			accInfos[pos].isRegistered = 0;
			accInfos[pos].isRefreshed = 0;
			break;
		default:
			LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "wrong event type, only "
					  "REGISTRATION events allowed");
			break;
	}

	// tell thread that we have changed the flags:
	accInfos[pos].eventArrived = 1;

	// unlock because we are done using account status info
	rc = pthread_mutex_unlock(&accInfoLock);
	if (rc != 0) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX "mutex lock could not be "
				  "released, error %d", rc);
		return 0;
	}
	// finally free sipstack event:
	rc = sipstack_event_free(event);
	if (!rc) {
		LOG_ERROR(REGISTRAR_MGR_MSG_PREFIX
				  "failed to release sipstack event");
		return 0;
	}

	return 1;					// pseudo success (thread still can report an error)
}
