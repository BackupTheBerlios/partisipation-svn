#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xmlrpc.h>
#include <xmlrpc_server.h>
#include <xmlrpc_server_abyss.h>

#include <remote/server/xmlrpc/xmlrpc_server.h>
#include <remote/server/xmlrpc/registration_remote.h>
#include <remote/server/xmlrpc/calls_remote.h>
#include <remote/server/xmlrpc/accounts_remote.h>
#include <remote/server/xmlrpc/volume_remote.h>
#include <remote/server/xmlrpc/extras_remote.h>
#include <util/threads/thread_management.h>

const int SERVER_PORT = 7777;
const char *REGISTER_PREFIX = "core";

int serverRunning = 0;

void xs_init() {
}

void generate_method_name(const char *suffix, char *target) {
	strcpy(target, REGISTER_PREFIX);
	strcat(target, ".");
	strcat(target, suffix);
}

void *xmlrpc_server_thread(void *param) {

	xmlrpc_server_abyss_parms serverparm;
	xmlrpc_registry *registryP;
	xmlrpc_env env;
	char name[80];

	/*
	 * server initializiation 
	 */

	xs_init();

	xmlrpc_env_init(&env);

	registryP = xmlrpc_registry_new(&env);

	// registration management:
	generate_method_name("registerGui", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &register_gui_RP, NULL);

	// call management:
	generate_method_name("makeCall", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name, &make_call_RP,
							   NULL);

	generate_method_name("endCall", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name, &end_call_RP,
							   NULL);

	generate_method_name("acceptCall", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &accept_call_RP, NULL);

	// account management:

	generate_method_name("accountGetAll", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_get_all_RP, NULL);

	generate_method_name("accountSet", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_set_RP, NULL);

	generate_method_name("accountGet", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_get_RP, NULL);

	generate_method_name("accountCreate", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_create_RP, NULL);

	generate_method_name("accountDelete", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_delete_RP, NULL);

	generate_method_name("accountSave", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_save_RP, NULL);

	generate_method_name("register", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_register_RP, NULL);

	generate_method_name("unregister", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &account_unregister_RP, NULL);

	// volume management:

	generate_method_name("setSpeakerVolume", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &set_speaker_volume_RP, NULL);

	generate_method_name("getSpeakerVolume", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &get_speaker_volume_RP, NULL);

	generate_method_name("setMicroVolume", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &set_micro_volume_RP, NULL);

	generate_method_name("getMicroVolume", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name,
							   &get_micro_volume_RP, NULL);

	// extras management:

	generate_method_name("sendDtmf", name);
	xmlrpc_registry_add_method(&env, registryP, NULL, name, &send_dtmf_RP,
							   NULL);

	/*
	 * In the modern form of the Abyss API, we supply parameters in memory
	 * like a normal API.  We select the modern form by setting
	 * config_file_name to NULL: 
	 */
	serverparm.config_file_name = NULL;
	serverparm.registryP = registryP;
	serverparm.port_number = SERVER_PORT;
	serverparm.log_file_name = "/tmp/xmlrpc_log";

	printf("Running XML-RPC server...\n");

	xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));

	/*
	 * xmlrpc_server_abyss() never returns 
	 */

	thread_terminated();
	return 0;
}

int start_xmlrpc_server_thread() {
	int rc;

	if (serverRunning) {
		// xmlrpc server thread is already running
		return 0;
	}

	rc = start_thread(xmlrpc_server_thread, NULL);
	if (rc == 0) {
		// ERROR
		return 0;
	}
	serverRunning = 1;
	return 1;
}
