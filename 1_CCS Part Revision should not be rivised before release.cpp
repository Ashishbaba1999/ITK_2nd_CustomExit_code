// CCS Part Revision should not be rivised before release

#include<stdio.h>
#include<tcinit/tcinit.h>
#include<tc/tc_startup.h>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/aom_prop.h>
#include<fclasses/tc_string.h>
#include<tc/emh.h>

#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +2)

extern DLLAPI int CCS_custom_exit_register_callbacks();
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args );
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args);
extern DLLAPI int check_release_status_list(METHOD_message_t* msg, va_list args);

////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Header.h"


extern DLLAPI int CCS_custom_exit_register_callbacks()
{
	CUSTOM_register_exit("CCS_custom_exit", "USER_init_module", (CUSTOM_EXIT_ftn_t)plm_execute_callbacks1);
	CUSTOM_register_exit("CCS_custom_exit", "USER_exit_module", (CUSTOM_EXIT_ftn_t)plm_execute_callbacks2);
	return 0;
}
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args)
{
	METHOD_id_t method_id;
	*decision = ALL_CUSTOMIZATIONS;
	TC_write_syslog("\n\n DLL register success after login\n\n");
	METHOD_find_method("P2CCS_PartRevision", "ITEM_copy_rev", &method_id);
	METHOD_add_pre_condition(method_id, (METHOD_function_t)check_release_status_list, NULLTAG);
	return 0;
}
extern DLLAPI int check_release_status_list(METHOD_message_t* msg, va_list args)
{
	tag_t tSourceRev = NULLTAG;
	tSourceRev = va_arg(args, tag_t);
	char* cValue = NULL;
	char* cName = NULL;

	AOM_UIF_ask_value(tSourceRev, "release_status_list", &cValue);
	AOM_UIF_ask_value(tSourceRev, "object_name", &cName);

	if (tc_strlen(cValue) > 0)
	{
		TC_write_syslog("\n revise success\n");
	}
	else
	{
		EMH_store_error_s1(EMH_severity_error, PLM_error, cName);
		return PLM_error;
	}
	return 0;
}
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	TC_write_syslog("\n\n DLL register success for Log out\n\n");
	return 0;
}
