#include "sip_stack_interface.h"

#include <eXosip2/eXosip.h>
#include <osip2/osip.h>
#include <osipparser2/osip_parser.h>

/**
 * Adds or edits a sip stack call and returns a sip stack event.
 *
 * @param callId call id
 * @param event eXosip event
 * @return sip stack event
 */
sipstack_event cm_map_event(int callId, eXosip_event_t * event);

/**
 * Check whether a call with this call id exists in the call list.
 *
 * @param callId call id
 */
int cm_call_exists(int callId);

sipstack_call cm_get_call_by_call_id(int callId);

/**
 * Return call id of call with this sip dialog id.
 *
 * @param sipDialogId a sip dialog id
 */
int cm_get_call_id_by_sip_dialog_id(int sipDialogId);

/**
 * Return call id of call with this sip transaction id.
 *
 * @param sipDialogId a sip dialog id
 */
int cm_get_call_id_by_sip_transaction_id(int sipTransactionId);

int cm_get_call_id_by_sip_call_id(int sipCallId);

int cm_get_sip_dialog_id_by_call_id(int callId);

int cm_get_sip_transaction_id_by_call_id(int callId);

int cm_get_sip_call_id_by_call_id(int callId);

int cm_terminate_current_transaction(int callId);

sipstack_call cm_build_sipstack_call(int callId, int sipCallId,
									 int sipDialogId,
									 queue sipTransactionIds,
									 sipstack_transaction_type type);

int cm_add_call(sipstack_call);
