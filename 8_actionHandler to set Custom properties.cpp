// action Handler to set custom properties 
// 1) p2Designer        ------> owning_user
// 2) p2DesignStartOn   ------> creation_date     
// to target business object after complition of workfolw

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
#include<tccore/aom_prop.h>
#include<tccore/project.h>
#include<sa/user.h>

#define DLLAPI _declspec(dllexport)

extern DLLAPI int CCS_Handlers_register_callbacks();
extern DLLAPI int plm_execute_callbacks1(int* decision, va_list args);
extern DLLAPI int plm_execute_callbacks2(int* decision, va_list args);

extern DLLAPI int fill_custom_prop(EPM_action_message_t);


////////////////////////////////////////////////////////////////////////

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
	EPM_register_action_handler("CCS_fill_custom_prop", "Custom action Handler", (EPM_action_handler_t)fill_custom_prop);
	return 0;
}
extern DLLAPI int fill_custom_prop(EPM_action_message_t msg)
{
	tag_t tTask = NULLTAG;
	tag_t tRootTask = NULLTAG;
	int iCount = 0;
	tag_t* tAttachments = NULLTAG;
	int iNo_arguments = 0;
	char* cArgName = NULL;
	char* cArgValue = NULL;
	char* cProjectID = NULL;
	tag_t tClassID = NULLTAG;
	char* cClassName = NULL;
	tag_t tProject = NULLTAG;
	char* cUserName = NULLTAG;
	char* cCreationDate = NULLTAG;



	tTask = msg.task;
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
				AOM_ask_value_string(tAttachments[i], "owning_user", &cUserName);
				AOM_ask_value_string(tAttachments[i], "creation_date", &cCreationDate);

				AOM_lock(tAttachments[i]);
				AOM_UIF_set_value(tAttachments[i], "p2Designer", cUserName);
				AOM_save_without_extensions(tAttachments[i]);
				AOM_unlock(tAttachments[i]);

				AOM_lock(tAttachments[i]);
				AOM_UIF_set_value(tAttachments[i], "p2DesignStartOn", cCreationDate);
				AOM_save_without_extensions(tAttachments[i]);
				AOM_unlock(tAttachments[i]);

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

