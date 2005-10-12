/**
 * @file sip_listener_interface.h
 * @brief Sip Listener Interface
 *
 * This file provide the API needed by the sip stack adapter to send events to its caller (call back).
 *
 * The API is needed to realize an asynchronus communication.
 *
 * caller: SIP Stack
 * receipient: SIP Listener (e.g. a SIP statemachine)
 * The sip listener (a class implementing this interface) receives an event of type <i>sipstack_event</i>.
 *
 * @author Enrico Hartung <enrico@iptel.org>
 */

/**
 * @defgroup siplistener Sip Listener
 * @ingroup sipstack
 * @{
 */
#ifndef HSIP_LISTENER_INTERFACE_USED
#define HSIP_LISTENER_INTERFACE_USED

#include <sipstack/sip_stack_interface.h>

void sip_listener_receive_event(sipstack_event event);

#endif

/** @} */
