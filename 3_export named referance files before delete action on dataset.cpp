// export named referance files before delete action on dataset

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

#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +2)

extern DLLAPI int CCS_custom_exit_register_callbacks();
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args );
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args);
extern DLLAPI int check_release_status_list(METHOD_message_t* msg, va_list args);

///////////////////////////////////////////////////////////////////////////////////////////////
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
	METHOD_find_method("Dataset", "AE_delete_dataset", &method_id);
	METHOD_add_action(method_id, METHOD_pre_action_type, (METHOD_function_t)check_release_status_list, NULLTAG);
	//METHOD_add_pre_condition(method_id, (METHOD_function_t)check_release_status_list, NULLTAG);
	return 0;
}
extern DLLAPI int check_release_status_list(METHOD_message_t* msg, va_list args)
{
	
	tag_t tDataset = msg->object_tag;
    //tag_t tDataset = va_arg(args, tag_t);
	char* cDatasetTypeName = NULL;
	tag_t tDatasetType = NULLTAG;
	int iCount = 0;
	char** cReferences = NULL;
	int jCount = 0;
	tag_t* tNamedReferances = NULLTAG;
	char originalpath[100] = "C:\\Data\\";
	char* cFileName = NULL;
	
	WSOM_ask_object_type2(tDataset, &cDatasetTypeName);
	AE_find_datasettype2(cDatasetTypeName, &tDatasetType);
	AE_ask_datasettype_refs(tDatasetType, &iCount, &cReferences);
	for (int i = 0; i < iCount; i++)
	{
		AE_ask_all_dataset_named_refs2(tDataset, cReferences[i], &jCount, &tNamedReferances);
		for (int j = 0; j < jCount; j++)
		{
			IMF_ask_original_file_name2(tNamedReferances[j], &cFileName);
			tc_strcat(originalpath, cFileName);
			IMF_export_file(tNamedReferances[j], originalpath);
			tc_strcpy(originalpath, "C:\\Data\\");
		}

	}

	return 0;
}
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	TC_write_syslog("\n\n DLL register success for Log out\n\n");
	return 0;
}
