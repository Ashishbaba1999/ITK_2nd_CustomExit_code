// Action handler to attach Dataset to Target Busiiness object after workflow completion

#include<stdio.h>
#include<tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include<tccore/custom.h>
#include<epm/epm.h>

#include<tccore/aom_prop.h>
#include<fclasses/tc_string.h>
#include<tc/emh.h>
#include<tccore/workspaceobject.h>
#include<ae/ae.h>
#include<sa/tcfile.h>
#include<tccore/grm.h>
#include<tccore/aom.h>
#include<tccore/project.h>
#include<ae/ae.h>

#define DLLAPI _declspec(dllexport)
#define TypeError (EMH_USER_error_base +3)
#define CountError (EMH_USER_error_base +4)

extern DLLAPI int CCS_Handlers_register_callbacks();
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args);
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args);

extern DLLAPI int assign_dataset(EPM_action_message_t);




////////////////////////////////////////////////////////////////////////////////
#include "Header.h"


extern DLLAPI int CCS_Handlers_register_callbacks()
{
	CUSTOM_register_exit("CCS_Handlers", "USER_init_module", (CUSTOM_EXIT_ftn_t)plm_execute_callbacks1);
	CUSTOM_register_exit("CCS_Handlers", "USER_exit_module", (CUSTOM_EXIT_ftn_t)plm_execute_callbacks2);
	return 0;
}
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args)
{
	//METHOD_id_t method_id;
	*decision = ALL_CUSTOMIZATIONS;
	//TC_write_syslog("\n\n DLL register success after login\n\n");
	EPM_register_action_handler("CCS_assign_dataset", "Custom action Handler",(EPM_action_handler_t)assign_dataset);
	return 0;
}
extern DLLAPI int assign_dataset(EPM_action_message_t msg)
{
	tag_t tTask = NULLTAG;
	tag_t tRootTask = NULLTAG;
	int iCount = 0;
	tag_t* tAttachments = NULLTAG;
	int iNo_arguments = 0;
	char* cArgName = NULL;
	char* cArgValue = NULL;
	//char* cProjectID = NULL;
	char* cType = NULL;
	char* cName = NULL;
	char* cRelationName = NULL;
	tag_t tDatasetType = NULLTAG;
	tag_t tRelationType = NULLTAG;
	tag_t tRelation = NULLTAG;
	tag_t tClassID = NULLTAG;
	tag_t tDataset = NULLTAG;
	char* cClassName = NULL;
	//tag_t tProject = NULLTAG;
	

	
	tTask= msg.task;
	EPM_ask_root_task(tTask, &tRootTask);
	EPM_ask_attachments(tRootTask, EPM_target_attachment, &iCount, &tAttachments);
	if (iCount > 0)
	{
		iNo_arguments = TC_number_of_arguments(msg.arguments);
		for (int j=0; j < iNo_arguments; j++)
		{
			ITK_ask_argument_named_value(TC_next_argument(msg.arguments), &cArgName, &cArgValue);
			if (tc_strcmp(cArgName, "dataset_type") == 0)
			{
				cType = (char*)MEM_alloc(100);
				tc_strcpy(cType, "");
				tc_strcpy(cType, cArgValue);
			}
			else if (tc_strcmp(cArgName, "dataset_name") == 0)
			{
				cName = (char*)MEM_alloc(100);
				tc_strcpy(cName, "");
				tc_strcpy(cName, cArgValue);
			}else if (tc_strcmp(cArgName, "relation_type") == 0)
			{
				cRelationName = (char*)MEM_alloc(100);
				tc_strcpy(cRelationName, "");
				tc_strcpy(cRelationName, cArgValue);
			}
		}
		AE_find_datasettype2(cType, &tDatasetType);
		AE_create_dataset_with_id(tDatasetType, cName, "", "", "", &tDataset);
		AE_save_myself(tDataset);
		GRM_find_relation_type(cRelationName, &tRelationType);
		for (int i = 0; i < iCount; i++)
		{
			POM_class_of_instance(tAttachments[i], &tClassID);
			POM_name_of_class(tClassID, &cClassName);
			if (tc_strcmp(cClassName, "P2CCS_PartRevision") == 0)
			{
				GRM_create_relation(tAttachments[i], tDataset, tRelationType, NULLTAG, &tRelation);
				GRM_save_relation(tRelation);			
			}

		}
	}
	if (tAttachments)MEM_free(tAttachments);

	return 0;
}


extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	TC_write_syslog("\n\n DLL register success for Log out\n\n");
	return 0;
}

