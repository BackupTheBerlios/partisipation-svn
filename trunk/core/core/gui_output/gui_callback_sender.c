#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <util/threads/thread_management.h>
#include <util/logging/logger.h>
#include <remote/callback/gui_callback.h>
#include <core/gui_output/gui_callback_sender.h>
#include <util/queue/queue.h>

/**
 * Logging prefix for GUI callback sender.
 */
#define GUI_CB_SNDR_MSG_PREFIX "[GUI callback sender] "
#define GUI_CB_FUNC_AMOUNT 6
#define GUI_CB_QUEUE_SIZE 64
#define GUI_CB_CH_REG_ST_ID 0
#define GUI_CB_CH_CALL_ST_ID 1
#define GUI_CB_SH_USR_EVT_ID 2
#define GUI_CB_IN_CALL_ID 3
#define GUI_CB_ST_SP_VOL_ID 4
#define GUI_CB_ST_MP_VOL_ID 5

typedef struct {
	int doShutdown;
	void **params;
} cb_info;

typedef struct {
	queue msgPool;
	pthread_mutex_t lock;
	pthread_mutex_t wakeUpLock;
	pthread_cond_t wakeUp;
} cb_queue;

cb_queue *cb_queues;

void *go_change_reg_status_thr(void *args) {
	int rc;
	int finished;
	cb_info *cbi;

	int accountId;
	int registered;

	finished = 0;

	while (!finished) {
		while (!queue_is_empty(cb_queues[GUI_CB_CH_REG_ST_ID].msgPool)) {
			cbi = (cb_info *)
				queue_front_and_dequeue(cb_queues[GUI_CB_CH_REG_ST_ID].
										msgPool);
			if (cbi->doShutdown) {
				finished = 1;
				break;
			}
			if (!cbi) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_change_reg_status thread "
						  "received no parameters");
				thread_terminated();
				return NULL;
			}
			if (!cbi->params) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_change_reg_status thread "
						  "received no parameters");
				free(cbi);
				thread_terminated();
				return NULL;
			}

			accountId = (int) cbi->params[0];
			registered = (int) cbi->params[1];

			rc = change_reg_status(accountId, registered);
			if (!rc) {
				LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
							"go_change_reg_status thread "
							"failed to change_reg_status");
			}

			free(cbi->params);
			free(cbi);
		}

		rc = pthread_mutex_lock(&cb_queues[GUI_CB_CH_REG_ST_ID].
								wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_change_reg_status thread "
					  "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		rc = pthread_cond_wait(&cb_queues[GUI_CB_CH_REG_ST_ID].wakeUp,
							   &cb_queues[GUI_CB_CH_REG_ST_ID].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_change_reg_status thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		rc = pthread_mutex_unlock(&cb_queues[GUI_CB_CH_REG_ST_ID].
								  wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_change_reg_status thread "
					  "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

int go_change_reg_status(int accountId, int registered) {
	int rc;						// return code
	cb_info *cb;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_change_reg_status()");
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\taccountId: %d", accountId);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tregistered: %d", registered);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_CH_REG_ST_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status: "
				  "failed to gain lock");
		return 0;
	}

	cb = (cb_info *) malloc(sizeof(cb_info));
	cb->doShutdown = 0;
	cb->params = (void **) malloc(2 * sizeof(void *));
	cb->params[0] = (void *) accountId;
	cb->params[1] = (void *) registered;

	queue_enqueue((void *) cb, cb_queues[GUI_CB_CH_REG_ST_ID].msgPool);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_CH_REG_ST_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&cb_queues[GUI_CB_CH_REG_ST_ID].wakeUp);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_CH_REG_ST_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_CH_REG_ST_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status: "
				  "failed to gain lock");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_change_reg_status()");
	return 1;					// pseudo success
}

void *go_change_call_status_thr(void *args) {
	int rc;
	int finished;
	cb_info *cbi;

	int callId;
	char *status;

	finished = 0;

	while (!finished) {
		while (!queue_is_empty(cb_queues[GUI_CB_CH_CALL_ST_ID].msgPool)) {
			cbi = (cb_info *)
				queue_front_and_dequeue(cb_queues[GUI_CB_CH_CALL_ST_ID].
										msgPool);
			if (cbi->doShutdown) {
				finished = 1;
				break;
			}
			if (!cbi) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_change_call_status thread "
						  "received no parameters");
				thread_terminated();
				return NULL;
			}
			if (!cbi->params) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_change_call_status thread "
						  "received no parameters");
				free(cbi);
				thread_terminated();
				return NULL;
			}

			callId = (int) cbi->params[0];
			status = (char *) cbi->params[1];

			if (!status) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_change_call_status thread "
						  "received no status parameter");
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			rc = change_call_status(callId, status);
			if (!rc) {
				LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
							"go_change_call_status thread "
							"failed to change_call_status");
			}

			LOG_DEBUG("status %s\n", status);

			free(status);
			free(cbi->params);
			free(cbi);

		}

		rc = pthread_mutex_lock(&cb_queues[GUI_CB_CH_CALL_ST_ID].
								wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_change_call_status thread "
					  "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		rc = pthread_cond_wait(&cb_queues[GUI_CB_CH_CALL_ST_ID].wakeUp,
							   &cb_queues[GUI_CB_CH_CALL_ST_ID].
							   wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_change_call_status thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		rc = pthread_mutex_unlock(&cb_queues[GUI_CB_CH_CALL_ST_ID].
								  wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_change_call_status thread "
					  "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

int go_change_call_status(int callId, char *status) {
	int rc;						// return code
	cb_info *cb;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_change_call_status()");
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tcallId: %d", callId);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tstatus: %s", status);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_CH_CALL_ST_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status: "
				  "failed to gain lock");
		return 0;
	}

	cb = (cb_info *) malloc(sizeof(cb_info));
	cb->doShutdown = 0;
	cb->params = (void **) malloc(2 * sizeof(void *));
	cb->params[0] = (void *) callId;
	cb->params[1] = (void *) malloc(strlen(status) * sizeof(char) + 1);
	strcpy((char *) cb->params[1], status);

	queue_enqueue((void *) cb, cb_queues[GUI_CB_CH_CALL_ST_ID].msgPool);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_CH_CALL_ST_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&cb_queues[GUI_CB_CH_CALL_ST_ID].wakeUp);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_CH_CALL_ST_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_CH_CALL_ST_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status: "
				  "failed to gain lock");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_change_call_status()");
	return 1;					// pseudo success
}

void *go_show_user_event_thr(void *args) {
	int rc;
	int finished;
	cb_info *cbi;

	int accountId;
	char *category;
	char *title;
	char *message;
	char *detailMessage;

	finished = 0;

	while (!finished) {
		while (!queue_is_empty(cb_queues[GUI_CB_SH_USR_EVT_ID].msgPool)) {
			cbi = (cb_info *)
				queue_front_and_dequeue(cb_queues[GUI_CB_SH_USR_EVT_ID].
										msgPool);
			if (cbi->doShutdown) {
				finished = 1;
				break;
			}
			if (!cbi) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_show_user_event thread "
						  "received no parameters");
				thread_terminated();
				return NULL;
			}
			if (!cbi->params) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_show_user_event thread "
						  "received no parameters");
				free(cbi);
				thread_terminated();
				return NULL;
			}

			accountId = (int) cbi->params[0];
			category = (char *) cbi->params[1];
			title = (char *) cbi->params[2];
			message = (char *) cbi->params[3];
			detailMessage = (char *) cbi->params[4];

			if (!category) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_show_user_event thread "
						  "received no category parameter");
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			if (!title) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_show_user_event thread "
						  "received no title parameter");
				free(category);
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			if (!message) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_show_user_event thread "
						  "received no message parameter");
				free(category);
				free(title);
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			if (!detailMessage) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_show_user_event thread "
						  "received no detailMessage parameter");
				free(category);
				free(title);
				free(message);
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			rc = show_user_event(accountId, category, title, message,
								 detailMessage);
			if (!rc) {
				LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
							"go_show_user_event thread "
							"failed to show_user_event");
			}

			free(category);
			free(title);
			free(message);
			free(detailMessage);
			free(cbi->params);
			free(cbi);
		}

		rc = pthread_mutex_lock(&cb_queues[GUI_CB_SH_USR_EVT_ID].
								wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_show_user_event thread " "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		rc = pthread_cond_wait(&cb_queues[GUI_CB_SH_USR_EVT_ID].wakeUp,
							   &cb_queues[GUI_CB_SH_USR_EVT_ID].
							   wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_show_user_event thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		rc = pthread_mutex_unlock(&cb_queues[GUI_CB_SH_USR_EVT_ID].
								  wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_show_user_event thread "
					  "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

int go_show_user_event(int accountId, char *category,
					   char *title, char *message, char *detailMessage) {
	int rc;						// return code
	cb_info *cb;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_show_user_event()");
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\taccountId: %d", accountId);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tcategory: %s", category);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\ttitle: %s", title);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tmessage: %s", message);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tdetailMessage: %s", detailMessage);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_SH_USR_EVT_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event: "
				  "failed to gain lock");
		return 0;
	}

	cb = (cb_info *) malloc(sizeof(cb_info));
	cb->doShutdown = 0;
	cb->params = (void **) malloc(5 * sizeof(void *));
	cb->params[0] = (void *) accountId;
	cb->params[1] = (void *) malloc(strlen(category) * sizeof(char) + 1);
	strcpy((char *) cb->params[1], category);
	cb->params[2] = (void *) malloc(strlen(title) * sizeof(char) + 1);
	strcpy((char *) cb->params[2], title);
	cb->params[3] = (void *) malloc(strlen(message) * sizeof(char) + 1);
	strcpy((char *) cb->params[3], message);
	cb->params[4] =
		(void *) malloc(strlen(detailMessage) * sizeof(char) + 1);
	strcpy((char *) cb->params[4], detailMessage);

	queue_enqueue((void *) cb, cb_queues[GUI_CB_SH_USR_EVT_ID].msgPool);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_SH_USR_EVT_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&cb_queues[GUI_CB_SH_USR_EVT_ID].wakeUp);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_SH_USR_EVT_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_SH_USR_EVT_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event: "
				  "failed to gain lock");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_show_user_event()");
	return 1;					// pseudo success
}

int go_register_core() {
	return register_core();
}

void *go_incoming_call_thr(void *args) {
	int rc;
	int finished;
	cb_info *cbi;

	int accountId;
	int callId;
	char *callerSipUri;
	char *callerDisplayName;

	finished = 0;

	while (!finished) {
		while (!queue_is_empty(cb_queues[GUI_CB_IN_CALL_ID].msgPool)) {
			cbi = (cb_info *)
				queue_front_and_dequeue(cb_queues[GUI_CB_IN_CALL_ID].
										msgPool);
			if (cbi->doShutdown) {
				finished = 1;
				break;
			}
			if (!cbi) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_incoming_call thread "
						  "received no parameters");
				thread_terminated();
				return NULL;
			}
			if (!cbi->params) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_incoming_call thread "
						  "received no parameters");
				free(cbi);
				thread_terminated();
				return NULL;
			}

			accountId = (int) cbi->params[0];
			callId = (int) cbi->params[1];
			callerSipUri = (char *) cbi->params[2];
			callerDisplayName = (char *) cbi->params[3];

			if (!callerSipUri) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call thread "
						  "received no caller URI parameter");
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			if (!callerDisplayName) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call thread "
						  "received no caller display name parameter");
				free(callerSipUri);
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			rc = incoming_call(accountId, callId, callerSipUri,
							   callerDisplayName);

			if (!rc) {
				LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
							"go_incoming_call thread: "
							"incoming_call failed");
			}

			free(callerSipUri);
			free(callerDisplayName);
			free(cbi->params);
			free(cbi);
		}

		rc = pthread_mutex_lock(&cb_queues[GUI_CB_IN_CALL_ID].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_incoming_call thread " "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		rc = pthread_cond_wait(&cb_queues[GUI_CB_IN_CALL_ID].wakeUp,
							   &cb_queues[GUI_CB_IN_CALL_ID].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_incoming_call thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		rc = pthread_mutex_unlock(&cb_queues[GUI_CB_IN_CALL_ID].
								  wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_incoming_call thread " "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

int go_incoming_call(int accountId, int callId,
					 char *callerSipUri, char *callerDisplayName) {
	int rc;						// return code
	cb_info *cb;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_incoming_call()");
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\taccountId: %d", accountId);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tcallId: %d", callId);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tcallerSipUri: %s", callerSipUri);
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tcallerDisplayName: %s",
			  callerDisplayName);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_IN_CALL_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call: "
				  "failed to gain lock");
		return 0;
	}

	cb = (cb_info *) malloc(sizeof(cb_info));
	cb->doShutdown = 0;
	cb->params = (void **) malloc(4 * sizeof(void *));
	cb->params[0] = (void *) accountId;
	cb->params[1] = (void *) callId;
	cb->params[2] =
		(void *) malloc(strlen(callerSipUri) * sizeof(char) + 1);
	strcpy((char *) cb->params[2], callerSipUri);
	cb->params[3] =
		(void *) malloc(strlen(callerDisplayName) * sizeof(char) + 1);
	strcpy((char *) cb->params[3], callerDisplayName);

	queue_enqueue((void *) cb, cb_queues[GUI_CB_IN_CALL_ID].msgPool);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_IN_CALL_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&cb_queues[GUI_CB_IN_CALL_ID].wakeUp);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_IN_CALL_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_IN_CALL_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call: "
				  "failed to gain lock");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_incoming_call()");
	return 1;					// pseudo success
}

void *go_set_speaker_volume_cb_thr(void *args) {
	int rc;
	int finished;
	cb_info *cbi;

	double *lvlPtr;
	double level;

	finished = 0;

	while (!finished) {
		while (!queue_is_empty(cb_queues[GUI_CB_ST_SP_VOL_ID].msgPool)) {
			cbi = (cb_info *)
				queue_front_and_dequeue(cb_queues[GUI_CB_ST_SP_VOL_ID].
										msgPool);
			if (cbi->doShutdown) {
				finished = 1;
				break;
			}
			if (!cbi) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_set_speaker_volume_cb thread "
						  "received no parameters");
				thread_terminated();
				return NULL;
			}
			if (!cbi->params) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_set_speaker_volume_cb thread "
						  "received no parameters");
				free(cbi);
				thread_terminated();
				return NULL;
			}

			lvlPtr = (double *) cbi->params[0];
			if (!lvlPtr) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_set_speaker_volume_cb thread "
						  "received no level parameter");
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			level = *lvlPtr;

			rc = set_speaker_volume_cb(level);
			if (!rc) {
				LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
							"go_set_speaker_volume_cb thread "
							"failed to set_speaker_volume_cb");
			}

			free(lvlPtr);
			free(cbi->params);
			free(cbi);
		}

		rc = pthread_mutex_lock(&cb_queues[GUI_CB_ST_SP_VOL_ID].
								wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_set_speaker_volume_cb thread "
					  "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		rc = pthread_cond_wait(&cb_queues[GUI_CB_ST_SP_VOL_ID].wakeUp,
							   &cb_queues[GUI_CB_ST_SP_VOL_ID].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_set_speaker_volume_cb thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		rc = pthread_mutex_unlock(&cb_queues[GUI_CB_ST_SP_VOL_ID].
								  wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_set_speaker_volume_cb thread "
					  "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

int go_set_speaker_volume_cb(double level) {
	int rc;						// return code
	cb_info *cb;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
			  "entering go_set_speaker_volume_cb()");
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tlevel: %f", level);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_ST_SP_VOL_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	cb = (cb_info *) malloc(sizeof(cb_info));
	cb->doShutdown = 0;
	cb->params = (void **) malloc(1 * sizeof(void *));
	cb->params[0] = (void *) malloc(1 * sizeof(double));
	*((double *) cb->params[0]) = level;

	queue_enqueue((void *) cb, cb_queues[GUI_CB_ST_SP_VOL_ID].msgPool);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_ST_SP_VOL_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&cb_queues[GUI_CB_ST_SP_VOL_ID].wakeUp);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_ST_SP_VOL_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_ST_SP_VOL_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
			  " leaving go_set_speaker_volume_cb()");
	return 1;					// pseudo success
}

void *go_set_micro_volume_cb_thr(void *args) {
	int rc;
	int finished;
	cb_info *cbi;

	double *lvlPtr;
	double level;

	finished = 0;

	while (!finished) {
		while (!queue_is_empty(cb_queues[GUI_CB_ST_MP_VOL_ID].msgPool)) {
			cbi = (cb_info *)
				queue_front_and_dequeue(cb_queues[GUI_CB_ST_MP_VOL_ID].
										msgPool);
			if (cbi->doShutdown) {
				finished = 1;
				break;
			}
			if (!cbi) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_set_micro_volume_cb thread "
						  "received no parameters");
				thread_terminated();
				return NULL;
			}
			if (!cbi->params) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_set_micro_volume_cb thread "
						  "received no parameters");
				free(cbi);
				thread_terminated();
				return NULL;
			}

			lvlPtr = (double *) cbi->params[0];
			if (!lvlPtr) {
				LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
						  "go_set_micro_volume_cb thread "
						  "received no level parameter");
				free(cbi->params);
				free(cbi);
				thread_terminated();
				return NULL;
			}

			level = *lvlPtr;

			rc = set_micro_volume_cb(level);
			if (!rc) {
				LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
							"go_set_micro_volume_cb thread "
							"failed to set_speaker_volume_cb");
			}

			free(lvlPtr);
			free(cbi->params);
			free(cbi);
		}

		rc = pthread_mutex_lock(&cb_queues[GUI_CB_ST_MP_VOL_ID].
								wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_set_micro_volume_cb thread "
					  "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		rc = pthread_cond_wait(&cb_queues[GUI_CB_ST_MP_VOL_ID].wakeUp,
							   &cb_queues[GUI_CB_ST_MP_VOL_ID].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_set_micro_volume_cb thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		rc = pthread_mutex_unlock(&cb_queues[GUI_CB_ST_MP_VOL_ID].
								  wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "go_set_micro_volume_cb thread "
					  "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

int go_set_micro_volume_cb(double level) {
	int rc;						// return code
	cb_info *cb;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_set_micro_volume_cb()");
	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "\tlevel: %f", level);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_ST_MP_VOL_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	cb = (cb_info *) malloc(sizeof(cb_info));
	cb->doShutdown = 0;
	cb->params = (void **) malloc(1 * sizeof(void *));
	cb->params[0] = (void *) malloc(1 * sizeof(double));
	*((double *) cb->params[0]) = level;

	queue_enqueue((void *) cb, cb_queues[GUI_CB_ST_MP_VOL_ID].msgPool);

	rc = pthread_mutex_lock(&cb_queues[GUI_CB_ST_MP_VOL_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&cb_queues[GUI_CB_ST_MP_VOL_ID].wakeUp);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_ST_MP_VOL_ID].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&cb_queues[GUI_CB_ST_MP_VOL_ID].lock);
	if (rc != 0) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_set_micro_volume_cb()");
	return 1;					// pseudo success
}

int go_init() {
	int rc;
	int i;

	cb_queues = (cb_queue *) malloc(GUI_CB_FUNC_AMOUNT * sizeof(cb_queue));

	for (i = 0; i < GUI_CB_FUNC_AMOUNT; i++) {
		cb_queues[i].msgPool = queue_create_queue(GUI_CB_QUEUE_SIZE);

		rc = pthread_mutex_init(&cb_queues[i].lock, NULL);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "failed to initialize "
					  "mutex lock");
			return 0;
		}

		rc = pthread_mutex_init(&cb_queues[i].wakeUpLock, NULL);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "failed to initialize "
					  "mutex lock");
			return 0;
		}

		rc = pthread_cond_init(&cb_queues[i].wakeUp, NULL);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "failed to initialize "
					  "wakeup-condition-variable");
			return 0;
		}
	}

	rc = start_thread(go_change_reg_status_thr, NULL);
	if (!rc) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
				  "failed to start go_change_reg_status thread");
		return 0;
	}

	rc = start_thread(go_change_call_status_thr, NULL);
	if (!rc) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
				  "failed to start go_change_call_status thread");
		return 0;
	}

	rc = start_thread(go_show_user_event_thr, NULL);
	if (!rc) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
				  "failed to start go_show_user_event thread");
		return 0;
	}

	rc = start_thread(go_incoming_call_thr, NULL);
	if (!rc) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
				  "failed to start go_incoming_call thread");
		return 0;
	}

	rc = start_thread(go_set_speaker_volume_cb_thr, NULL);
	if (!rc) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
				  "failed to start go_set_speaker_volume_cb thread");
		return 0;
	}

	rc = start_thread(go_set_micro_volume_cb_thr, NULL);
	if (!rc) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
				  "failed to start go_set_micro_volume_cb thread");
		return 0;
	}

	return 1;
}

int go_destroy() {
	int rc;
	int i;

	for (i = 0; i < GUI_CB_FUNC_AMOUNT; i++) {
		queue_dispose_queue(cb_queues[i].msgPool);

		rc = pthread_mutex_destroy(&cb_queues[i].lock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "failed to release mutex lock");
			return 0;
		}

		rc = pthread_mutex_destroy(&cb_queues[i].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX
					  "failed to release mutex lock");
			return 0;
		}

		rc = pthread_cond_destroy(&cb_queues[i].wakeUp);
		if (rc != 0) {
			LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "failed to release "
					  "wakeup-condition-variable");
			return 0;
		}
	}

	free(cb_queues);
	return 1;
}
