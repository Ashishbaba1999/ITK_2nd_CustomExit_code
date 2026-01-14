// change the status of revision as same as previous revison




//@<COPYRIGHT>@
//==================================================
//Copyright $2025.
//Siemens Product Lifecycle Management Software Inc.
//All Rights Reserved.
//==================================================
//@<COPYRIGHT>@

/* 
 * @file 
 *
 *   This file contains the declaration for the Extension T2assign_status
 *
 */
 
#ifndef T2ASSIGN_STATUS_HXX
#define T2ASSIGN_STATUS_HXX
#include <tccore/method.h>

#include<iostream>
#include<epm/epm.h>
#include<tccore/aom.h>
#include<tccore/item.h>
#include<tccore/grm.h>
#include<pom/pom/pom.h>
#include<tccore/aom_prop.h>
#include<ae/datasettype.h>
#include<ae/dataset.h>
#include<tccore/project.h>

#define DLLAPI _declspec (dllexport)
extern DLLAPI int assign_status(EPM_action_message_t msg);


#include <T2Handler_customization1/libt2handler_customization1_exports.h>
#ifdef __cplusplus
         extern "C"{
#endif
                 
extern T2HANDLER_CUSTOMIZATION1_API int T2assign_status(METHOD_message_t* msg, va_list args);
                 
#ifdef __cplusplus
                   }
#endif
                
#include <T2Handler_customization1/libt2handler_customization1_undef.h>
                
#endif  // T2ASSIGN_STATUS_HXX


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//@<COPYRIGHT>@
//==================================================
//Copyright $2025.
//Siemens Product Lifecycle Management Software Inc.
//All Rights Reserved.
//==================================================
//@<COPYRIGHT>@

/* 
 * @file 
 *
 *   This file contains the implementation for the Extension T2assign_status
 *
 */
#include <T2Handler_customization1/T2assign_status.hxx>

int T2assign_status( METHOD_message_t * msg, va_list /*args*/ )
{
	using namespace std;

	int iFail= ITK_ok;
	tag_t obj=NULLTAG;
	obj=msg->object_tag;
	EPM_register_action_handler("EPM_assign_status", "Custom Action Handler to assign status to IR", (EPM_action_handler_t)assign_status);
	TC_write_syslog("\n\n DLL Registered Success after Login for Handlers \n\n");

	return iFail;

}

int assign_status(EPM_action_message_t msg)
{
	int iTargetCount = 0;
	int iNO_arguments = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int istatusCount = 0;

	char* cClassName = NULL;
	int iCount = 0;
	int istatuses = 0;

	tag_t tTask = NULLTAG;
	tag_t tRootTask = NULLTAG;
	tag_t* tTargetList = NULLTAG;
	tag_t tClassID = NULLTAG;
	tag_t item = NULLTAG;
	tag_t* rev_list = NULLTAG;
	tag_t prev_rev = NULLTAG;
	char* statuses= NULL;

	EPM_ask_root_task(tTask, &tRootTask);
	if (tRootTask != NULLTAG)
	{
		EPM_ask_attachments(tRootTask, EPM_target_attachment, &iTargetCount, &tTargetList);
		if (iTargetCount > 0)
		{

			for (j = 0; j < iTargetCount; j++)
			{
				POM_class_of_instance(tTargetList[j], &tClassID);
				POM_name_of_class(tClassID, &cClassName);
				if (tc_strcmp(cClassName, "ItemRevision") == 0)
				{
					ITEM_ask_item_of_rev (tTargetList[j], &item);
					ITEM_list_all_revs (item, &iCount, &rev_list);
					for (int i = 0; i < iCount; i++)
                        {
                            if (rev_list[i] == tTargetList[j]);
                                {
                                    if (i > 0)  // previous revision exists
                                    prev_rev = rev_list[i - 1];
                                        AOM_UIF_ask_value (prev_rev, "release_status_list" , &statuses);
                                    	AOM_refresh(tTargetList[j],TRUE);
                                        AOM_UIF_set_value(tTargetList[j], "release_status_list",statuses);
                                        AOM_save_with_extensions(tTargetList[i]);
                                        AOM_refresh(tTargetList[j],FALSE);


                                }
                        }

				}
			}
		}
	}
	if(tTargetList)MEM_free(tTargetList);
	if(rev_list)MEM_free(rev_list);

}


