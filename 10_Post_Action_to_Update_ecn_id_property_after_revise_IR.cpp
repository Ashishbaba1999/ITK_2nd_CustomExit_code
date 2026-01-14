// ============================================================================
// Purpose   : To update ECN custom property on Item Revision during revise.
// Logic     : When Item Revision is revised (001,002,003...), update property
//             ECN_ID as follows:
//             - Rev A ? "0"
//             - Rev B ? "1"
//             - Rev C? "2"
//             - Rev D ? "3"
//             - Rev E ? "A"
//             - Rev F ? "B"
//             - Rev G ? "C"
//             and so on.
// Author    : Ashish (Custom Exit Example)
// ============================================================================



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
 *   This file contains the implementation for the Extension T2set_ecn_id
 *
 */
#include <T2Post_Action1/T2set_ecn_id.hxx>
#include <tccore/aom.h>
#include<iostream>
#include<tccore/item.h>
#include<tccore/aom_prop.h>
#include<fclasses/tc_string.h>
#include <string>

using namespace std;

static void getECNValue(const char* revId, char* ecnValue)
{
    char revChar = revId[0];

    if (revChar >= 'A' && revChar <= 'D') {
        // A-D -> "0".."3"
        ecnValue[0] = '0' + (revChar - 'A');
        ecnValue[1] = '\0';
    } else if (revChar >= 'E') {
        // E onwards -> 'A', 'B', 'C'...
        ecnValue[0] = 'A' + (revChar - 'E');
        ecnValue[1] = '\0';
    } else {
        ecnValue[0] = '\0';
    }

}



int T2set_ecn_id( METHOD_message_t * msg, va_list /*args*/ )
{
    tag_t newRevTag = msg->object_tag;

    // Get Revision ID
    char* revIdStr = NULL;
    AOM_ask_value_string(newRevTag, "item_revision_id", &revIdStr);

    char ecnValue[2] = {0};  // Single char + null terminator
    getECNValue(revIdStr, ecnValue);

    MEM_free(revIdStr);

    // Set ECN_ID property
    AOM_lock(newRevTag);
    AOM_set_value_string(newRevTag, "ECN_ID", ecnValue);
    AOM_save_without_extensions(newRevTag);
    AOM_unlock(newRevTag);

    return ITK_ok;
}



