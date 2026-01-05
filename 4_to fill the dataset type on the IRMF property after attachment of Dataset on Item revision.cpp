// to fill the dataset type(PDF) on the IRMF property (user_data_1) after attachment of Dataset on Item revision

#include<stdio.h>
#include<tcinit/tcinit.h>
#include<tc/tc_startup.h>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/aom_prop.h>
#include<fclasses/tc_string.h>
#include<tc/emh.h>
#include<tccore/workspaceobject.h>
#include<ae/ae.h>
#include<sa/tcfile.h>
#include<tccore/grm.h>
#include<tccore/aom.h>

#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +2)

extern DLLAPI int CCS_custom_exit_register_callbacks();
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args );
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args);
extern DLLAPI int check_release_status_list(METHOD_message_t* msg, va_list args);





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	METHOD_find_method("IMAN_specification", "GRM_create", &method_id);
	METHOD_add_action(method_id, METHOD_post_action_type, (METHOD_function_t)check_release_status_list, NULLTAG);
	return 0;
}
extern DLLAPI int check_release_status_list(METHOD_message_t* msg, va_list args)
{
	
    tag_t tPrimary = va_arg(args, tag_t);
	tag_t tSecondary = va_arg(args, tag_t);
	tag_t tRelationType = NULLTAG;
	int iCount = 0;
	tag_t* tSecObj = NULLTAG;
	char* cType = NULL;

	AOM_ask_value_string(tSecondary, "object_type", &cType);
	GRM_find_relation_type("IMAN_master_form_rev", &tRelationType);
	GRM_list_secondary_objects_only(tPrimary, tRelationType, &iCount, &tSecObj);
	for (int i = 0; i < iCount; i++)
	{
		AOM_lock(tSecObj[i]);
		AOM_UIF_set_value(tSecObj[i], "user_data_1", cType);
		AOM_save_without_extensions(tSecObj[i]);
		AOM_unlock(tSecObj[i]);

	}
	if (tSecObj)MEM_free(tSecObj);
	return 0;
}
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	TC_write_syslog("\n\n DLL register success for Log out\n\n");
	return 0;
}


