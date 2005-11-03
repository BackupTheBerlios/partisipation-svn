/** 
 * @file registrar_manager.h
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
 * This is the interface of the registrar manager - it is used exclusively by
 * the core. Multiple implementations of this interface are not likely.
 *
 * @author Matthias Liebig
 */

#ifndef HREGISTRAR_MANAGER_USED
#define HREGISTRAR_MANAGER_USED

#include <sipstack/sip_stack_interface.h>

/**
 * Initialize registrar manager. Create lock and reserve memory. Call this
 * before calling any other function of the registrar manager.
 * @return if initialization was successfull (boolean)
 */
int rm_init();

/**
 * Register one specific account at the SIP registrar. This method is called
 * by the GUI. Because we want to assure an asynchronous communication it
 * starts a thread to do its task. This thread lives as long the account is
 * registered because it is responsible for updating the registration if it
 * expires.
 *
 * The account information are retrieved using the account management.
 * @param accountId the ID of the account that shall be registered
 * @return whether starting the thread (not registering!) was successfull - it
 * is only a pseudo success (boolean)
 */
int rm_register_account(int accountId);

/**
 * Unregister one specific account at the SIP registrar. This method is called
 * by the GUI. Because we want to assure an asynchronous communication it
 * starts a thread to do its task. Normally there is another thread running to
 * update the registration if required. This thread is shut down gracefully.
 * @param accountId the ID of the account that shall be no longer registered
 * @return whether starting the thread (not unregistering!) was successfull -
 * it is only a pseudo success (boolean)
 */
int rm_unregister_account(int accountId);

/**
 * Register all accounts with autoregister=true. This simply calls 
 * rm_register_account() for every account with this property enabled.
 * @return whether starting the auto registration thread was successfull - it
 * is only a pseudo success (boolean)
 */
int rm_register_auto();

/**
 * This dispatches any incoming events from SIP stack. It has to find out which
 * (un-)registration thread waits on the event and of which type the event is.
 * An event is not forwarded to the thread - it is evaluated and only the 
 * account's registration status is updated.
 * @param event the incoming SIP stack event
 * @return whether evaluating the event was successfull - it is only a pseudo
 * success (boolean)
 */
int rm_receive_register_event(sipstack_event * event);

/**
 * Destroy lock and free memory. Call this function when you are done using
 * the registrar manager. 
 * @return if releasing registrar manager was successfull (boolean)
 */
int rm_destroy();

#endif
