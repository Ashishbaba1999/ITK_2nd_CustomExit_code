// Rule Handler to check count of named referance files attaches to target bussiness object

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

#define DLLAPI _declspec(dllexport)
#define TypeError (EMH_USER_error_base +3)
#define CountError (EMH_USER_error_base +4)

extern DLLAPI int CCS_Handlers_register_callbacks();
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args);
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args);

EPM_decision_t check_named_refrence(EPM_rule_message_t);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	TC_write_syslog("\n\n DLL register success after login\n\n");
	EPM_register_rule_handler("CCS_check_named_ref", "Custom Rule Handler",(EPM_rule_handler_t)check_named_refrence);
	return 0;
}
EPM_decision_t check_named_refrence(EPM_rule_message_t msg)
{
	tag_t tTask = NULLTAG;
	tag_t tRootTask = NULLTAG;
	int iCount = 0;
	tag_t* tAttachments = NULLTAG;
	tag_t tClassID = NULLTAG;
	char* cClassName = NULL;
	tag_t tRelationType = NULLTAG;
	int jCount = 0;
	tag_t* tSecObjects = NULLTAG;
	char* cDatasetType = NULLTAG;
	int kCount = 0;
	tag_t* tRefObjects = NULLTAG;

	char* cTargetObjName = NULL;
	int k = 0;
	char* cSecObjName = NULL;


	EPM_decision_t decision = EPM_nogo;
	tTask= msg.task;
	EPM_ask_root_task(tTask, &tRootTask);
	EPM_ask_attachments(tRootTask, EPM_target_attachment, &iCount, &tAttachments);
	if (iCount > 0)
	{
		for (int i = 0; i < iCount; i++)
		{
			POM_class_of_instance(tAttachments[i], &tClassID);
			POM_name_of_class(tClassID, &cClassName);
			if (tc_strcmp(cClassName, "P2CCS_PartRevision") == 0)
			{
				GRM_find_relation_type("IMAN_specification", &tRelationType);
				GRM_list_secondary_objects_only(tAttachments[i], tRelationType, &jCount, &tSecObjects);
				{
					if (jCount > 0)
					{
						for (int j = 0; j < jCount; j++)
						{
						    
							AOM_ask_value_string(tSecObjects[j], "object_type", &cDatasetType);
							if (tc_strcmp(cDatasetType, "PDF") == 0)
							{
								k++;
								AE_ask_dataset_named_refs(tSecObjects[j], &kCount, &tRefObjects);
								if (kCount == 1)
								{
									decision = EPM_go;
								}
								else
								{
									AOM_UIF_ask_value(tSecObjects[j], "object_name", &cSecObjName);
									EMH_store_error_s1(EMH_severity_error, CountError, cSecObjName);

								}
							}
							
						}
						if (k == 0)
						{
							AOM_UIF_ask_value(tAttachments[i], "object_name", &cTargetObjName);
							EMH_store_error_s1(EMH_severity_error, TypeError, cTargetObjName);

						}
					}
					else
					{
						AOM_UIF_ask_value(tAttachments[i], "object_name", &cTargetObjName);
						EMH_store_error_s1(EMH_severity_error, TypeError, cTargetObjName);
					}

				}
			}

		}
	}
	if (tAttachments)MEM_free(tAttachments);
	if (tSecObjects)MEM_free(tSecObjects);
	if (tRefObjects)MEM_free(tRefObjects);

	return decision;


}


extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	TC_write_syslog("\n\n DLL register success for Log out\n\n");
	return 0;
}

