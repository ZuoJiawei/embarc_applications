/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Achim Kraus, Bosch Software Innovations GmbH - Please refer to git log
 *    
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
 * Implements an object for testing purpose
 *
 *                 Multiple
 * Object |  ID  | Instances | Mandatoty |
 *  Test  | 1024 |    Yes    |    No     |
 *
 *  Ressources:
 *              Supported    Multiple
 *  Name | ID | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  test |  1 |    R/W     |    No     |    Yes    | Integer | 0-255 |       |             |
 *  exec |  2 |     E      |    No     |    Yes    |         |       |       |             |
 *
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dev_iic.h"
#include "board.h"
#include "imu.h"
#include "value.h"



#include "embARC.h"
#include "embARC_debug.h"

#define PRV_RESOURCE_3_SIZE 190
#define PRV_TLV_BUFFER_SIZE 64


#define LWM2M_HRATE_STA_OBJECT_ID          3338

#define WARN_HRATE_ID          5800   

#define LWM2M_EMSK_INSTANCE_ID  0


/*
 * Multiple instance objects can use userdata to store data that will be shared between the different instances.
 * The lwm2m_object_t object structure - which represent every object of the liblwm2m as seen in the single instance
 * object - contain a chained list called instanceList with the object specific structure prv_instance_t:
 */
typedef struct _prv_instance_
{
    /*
     * The first two are mandatories and represent the pointer to the next instance and the ID of this one. The rest
     * is the instance scope user data (uint8_t test in this case)
     */
    struct _prv_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t shortID;               // matches lwm2m_list_t::id
    bool warn_hrate_abnor;
} prv_instance_t;


static uint8_t prv_get_value(lwm2m_tlv_t * tlvP,
                             prv_instance_t * targetP)
{
    // There are no multiple instance resources
    tlvP->type = LWM2M_TYPE_RESOURCE;
   
        switch (tlvP->id)
        {
        case WARN_HRATE_ID:
            targetP->warn_hrate_abnor = data_report_wn.warn_hrate_abnor;
            lwm2m_tlv_encode_bool(targetP->warn_hrate_abnor, tlvP);
            if (0 != tlvP->length) return COAP_205_CONTENT;
            else return COAP_500_INTERNAL_SERVER_ERROR;
            break;
        
        default:
            return COAP_404_NOT_FOUND;
        }

    
}

static uint8_t prv_read(uint16_t instanceId,
                        int * numDataP,
                        lwm2m_tlv_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    uint8_t result;
    int i;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

        if (*numDataP == 0)
        {
            uint16_t resList[] = {
                    
                    WARN_HRATE_ID
            };
            int nbRes = sizeof(resList)/sizeof(uint16_t);

            *dataArrayP = lwm2m_tlv_new(nbRes);
            if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
            *numDataP = nbRes;
            for (i = 0 ; i < nbRes ; i++)
            {
                (*dataArrayP)[i].id = resList[i];
            }

        }
        i = 0;
        do
        {
            result = prv_get_value((*dataArrayP) + i, targetP);
            i++;
        } while (i < *numDataP && result == COAP_205_CONTENT);

        return result;
   
}

static uint8_t prv_write(uint16_t instanceId,
                         int numData,
                         lwm2m_tlv_t * dataArray,
                         lwm2m_object_t * objectP)
{
    
    return COAP_204_CHANGED;
}

static uint8_t prv_delete(uint16_t id,lwm2m_object_t * objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}

static uint8_t prv_create(uint16_t instanceId,
                          int numData,
                          lwm2m_tlv_t * dataArray,
                          lwm2m_object_t * objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}

static uint8_t prv_exec(uint16_t instanceId,
                        uint16_t resourceId,
                        uint8_t * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;  
}

static void prv_close(lwm2m_object_t * objectP)
{
    LWM2M_LIST_FREE(objectP->instanceList);
    if (objectP->userData != NULL)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
}



void display_hratestatus_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
    EMBARC_PRINTF("  /%u: hratestatus object, instances:\r\n", object->objID);
    prv_instance_t * instance = (prv_instance_t *)object->instanceList;
    while (instance != NULL)
    {
        EMBARC_PRINTF("    /%u/%u: shortId: %u, btn: %u\r\n",
                object->objID, instance->shortID,
                instance->shortID, instance->btn);
        instance = (prv_instance_t *)instance->next;
    }
#endif
}

lwm2m_object_t * get_hratestatus_object(void)
{
    lwm2m_object_t * hratestatusObj;
   
    hratestatusObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != hratestatusObj)
    {
        int i;    
        
        prv_instance_t * targetP;

        memset(hratestatusObj, 0, sizeof(lwm2m_object_t));

        hratestatusObj->objID = LWM2M_HRATE_STA_OBJECT_ID;
        for (i=0 ; i < 1 ; i++)
        {
            targetP = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
            if (NULL == targetP) {
                lwm2m_free(hratestatusObj);
                return NULL;
            }
            memset(targetP, 0, sizeof(prv_instance_t));
            
            targetP->shortID = LWM2M_EMSK_INSTANCE_ID + i;
            targetP->warn_hrate_abnor = data_report_wn.warn_hrate_abnor;
            hratestatusObj->instanceList = LWM2M_LIST_ADD(hratestatusObj->instanceList, targetP);
            
           
        }
        /*
         * From a single instance object, two more functions are available.
         * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
         *   provided a check is done for verifying his disponibility, or a new one is generated.
         * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
         *   allocated to it)
         */
        hratestatusObj->readFunc    = prv_read;
        hratestatusObj->writeFunc   = prv_write;
        hratestatusObj->createFunc  = prv_create;
        hratestatusObj->deleteFunc  = prv_delete;
        hratestatusObj->executeFunc = prv_exec;
        hratestatusObj->closeFunc   = prv_close;
        
    }

    return hratestatusObj;
}
