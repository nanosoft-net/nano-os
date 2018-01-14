/*
Copyright(c) 2017 Cedric Jimenez

This file is part of Nano-OS.

Nano-OS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nano-OS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Nano-OS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nano_os_debug.h"

/* Check if module is enabled */
#if (NANO_OS_DEBUG_ENABLED == 1u)

#include "nano_os.h"
#include "nano_os_data.h"
#include "nano_os_user.h"
#include "nano_os_tools.h"

/** \brief Total size in bytes of a debug request :
 *         Header : 2
 *         Command : 2
 *         Param 1 : 4
 *         Param 2 : 4
 */
#define NANO_OS_DEBUG_REQ_SIZE              12u

/** \brief Minimal size in bytes of a debug response :
 *         Header : 2
 *         Command : 2
 *         Return code : 1
 *         Data size : 1
 */
#define NANO_OS_DEBUG_MIN_RESP_SIZE         6u

/** \brief Maximal size in bytes of the debug response data field */
#define NANO_OS_DEBUG_MAX_RESP_DATA_SIZE    255u

/** \brief Maximal size in bytes of a debug response */
#define NANO_OS_DEBUG_MAX_RESP_SIZE         (NANO_OS_DEBUG_MIN_RESP_SIZE + NANO_OS_DEBUG_MAX_RESP_DATA_SIZE)


/** \brief Debug frame header */
#define NANO_OS_DEBUG_FRAME_HEADER   0x0DF0u

/** \brief First byte of a debug frame header */
#define NANO_OS_DEBUG_FRAME_HEADER1  0xF0u

/** \brief Second byte of a debug frame header */
#define NANO_OS_DEBUG_FRAME_HEADER2  0x0Du



/** \brief Nano OS debug commands */
typedef enum _nano_os_debug_cmd_t
{
    /** \brief Get Nano OS version */
    NDC_GET_VERSION = 0x00u,

    /** \brief Get the number of tasks */
    NDC_GET_TASK_COUNT  = 0x10u,
    /** \brief Get the list of the tasks */
    NDC_GET_TASK_LIST  = 0x11u,
    /** \brief Get the list of the tasks with their status*/
    NDC_GET_TASK_STATUS = 0x12u,
    /** \brief Get task information */
    NDC_GET_TASK_INFO  = 0x13u,

    /** \brief Get the number of objects */
    NDC_GET_OBJECT_COUNT  = 0x20u,
    /** \brief Get the list of the objects */
    NDC_GET_OBJECT_LIST  = 0x21u,
    /** \brief Get information on an object */
    NDC_GET_OBJECT_INFO  = 0x22u,

    /** \brief Get heap information */
    NDC_GET_HEAP_INFO = 0x40u,
    /** \brief Dump the heap structure */
    NDC_DUMP_HEAP_STRUCTURE = 0x41u,


    /** \brief Read data from a memory address */
    NDC_READ_DATA = 0xF0u,
    /** \brief Write data to a memory address */
    NDC_WRITE_DATA = 0xF1u

} nano_os_debug_cmd_t;


/** \brief Nano OS debug request */
typedef struct _nano_os_debug_request_t
{
    /** \brief Command */
    uint16_t command;
    /** \brief Parameter 1 */
    uint32_t param1;
    /** \brief Parameter 2 */
    uint32_t param2;
} nano_os_debug_request_t;




/** \brief Debug task */
static void* NANO_OS_DEBUG_Task(void* const param);

/** \brief Serialize an 8 bits integer in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeU8(const uint8_t data);

/** \brief Serialize a 16 bits integer in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeU16(const uint16_t data);

/** \brief Serialize a 32 bits integer in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeU32(const uint32_t data);

/** \brief Serialize a string in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeString(const char* const string, const uint8_t string_len);

/** \brief Send a response frame */
static nano_os_error_t NANO_OS_DEBUG_SendResponse(const nano_os_debug_request_t* const request, const nano_os_error_t error_code,
                                                  const uint8_t data_size, const bool has_more_data);

/** \brief Send a error response */
static nano_os_error_t NANO_OS_DEBUG_SendErrorResponse(const nano_os_debug_request_t* const request, const nano_os_error_t error_code);

/** \brief Handler for the get version request */
static nano_os_error_t NANO_OS_DEBUG_GetVersionHandler(const nano_os_debug_request_t* const request);


#if (NANO_OS_DEBUG_TASK_REQS_ENABLED == 1u)
/** \brief Handler for the get task count request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskCountHandler(const nano_os_debug_request_t* const request);
/** \brief Handler for the get task list request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskListHandler(const nano_os_debug_request_t* const request);
/** \brief Handler for the get task status request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskStatusHandler(const nano_os_debug_request_t* const request);
/** \brief Handler for the get task info request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskInfoHandler(const nano_os_debug_request_t* const request);
#endif /* (NANO_OS_DEBUG_TASK_REQS_ENABLED == 1u) */


#if (NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED == 1u)
/** \brief Handler for the get the number of objects request */
static nano_os_error_t NANO_OS_DEBUG_GetWaitObjectCountHandler(const nano_os_debug_request_t* const request);
/** \brief Handler for the get the list of the objects request */
static nano_os_error_t NANO_OS_DEBUG_GetWaitObjectListHandler(const nano_os_debug_request_t* const request);
/** \brief Handler for the get information on an object request */
static nano_os_error_t NANO_OS_DEBUG_GetWaitObjectInfoHandler(const nano_os_debug_request_t* const request);
#endif /* (NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED == 1u) */


#if (NANO_OS_DEBUG_MEMORY_REQS_ENABLED == 1u)
/** \brief Handler for the read data from a memory address request */
static nano_os_error_t NANO_OS_DEBUG_ReadDataHandler(const nano_os_debug_request_t* const request);
/** \brief Handler for the write data to a memory address request */
static nano_os_error_t NANO_OS_DEBUG_WriteDataHandler(const nano_os_debug_request_t* const request);
#endif /* (NANO_OS_DEBUG_MEMORY_REQS_ENABLED == 1u) */




/** \brief Initialize the debug module */
nano_os_error_t NANO_OS_DEBUG_Init(const nano_os_port_init_data_t* const port_init_data)
{
    nano_os_task_init_data_t task_init_data;
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_init_data != NULL)
    {
        /* Initialize the hardware drivers of the user module which will send/receive the debug packets */
        ret = NANO_OS_USER_DebugHwInit();
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Create debug task */
            (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = "Debug task";
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.base_priority = NANO_OS_DEBUG_TASK_PRIORITY;
            task_init_data.stack_origin = g_nano_os.debug_task_stack;
            task_init_data.stack_size = NANO_OS_DEBUG_TASK_STACK_SIZE;
            task_init_data.task_func = NANO_OS_DEBUG_Task;
            task_init_data.param = NULL;
            #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
            (void)MEMCPY(&task_init_data.port_init_data, &port_init_data->debug_task_init_data, sizeof(nano_os_port_task_init_data_t));
            #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */

            ret = NANO_OS_TASK_Create(&g_nano_os.debug_task, &task_init_data);
        }
    }

    return ret;
}

/** \brief Decoding state of the debug task */
typedef enum _nano_os_debug_task_state_t
{
    /** \brief Waiting request */
    NOSDTS_WAITING_REQUEST = 0u,
    /** \brief Waiting end of header */
    NOSDTS_WAITING_EOH = 1u,
    /** \brief Waiting command */
    NOSDTS_WAITING_COMMAND = 2u,
    /** \brief Waiting param 1 */
    NOSDTS_WAITING_PARAM1 = 3u,
    /** \brief Waiting param 2 */
    NOSDTS_WAITING_PARAM2 = 4u
} nano_os_debug_task_state_t;

/** \brief Debug task */
static void* NANO_OS_DEBUG_Task(void* const param)
{
    uint8_t byte;
    uint8_t nb_bytes;
    uint8_t nb_bytes_left;
    bool end_of_frame = false;
    nano_os_error_t ret;
    nano_os_debug_request_t request;
    nano_os_debug_task_state_t state = NOSDTS_WAITING_REQUEST;
    NANO_OS_UNUSED(param);

    /* Initialize the user module which will send/receive the debug packets */
    ret = NANO_OS_USER_DebugInit();
    NANO_OS_ERROR_ASSERT_RET(ret);

    /* Task loop */
    while(true)
    {
        /* Command receive loop */
        do
        {
            /* Wait for an incoming byte */
            ret = NANO_OS_USER_DebugReadByte(&byte);
            NANO_OS_ERROR_ASSERT_RET(ret);

            /* Decode byte */
            switch(state)
            {
                case NOSDTS_WAITING_REQUEST:
                {
                    /* Wait first header byte */
                    if (byte == NANO_OS_DEBUG_FRAME_HEADER1)
                    {
                        state = NOSDTS_WAITING_EOH;
                    }
                }
                break;

                case NOSDTS_WAITING_EOH:
                {
                    /* Wait second header byte */
                    if (byte == NANO_OS_DEBUG_FRAME_HEADER2)
                    {
                        nb_bytes = 0u;
                        nb_bytes_left = sizeof(uint16_t);
                        (void)MEMSET(&request, 0, sizeof(nano_os_debug_request_t));
                        state = NOSDTS_WAITING_COMMAND;
                    }
                    else
                    {
                        state = NOSDTS_WAITING_REQUEST;
                    }
                }
                break;

                case NOSDTS_WAITING_COMMAND:
                {
                    /* Waiting command */
                    request.command += NANO_OS_CAST(uint16_t, (byte << (nb_bytes * 8u)));
                    nb_bytes++;
                    nb_bytes_left--;
                    if (nb_bytes_left == 0u)
                    {
                        nb_bytes = 0u;
                        nb_bytes_left = sizeof(uint32_t);
                        state = NOSDTS_WAITING_PARAM1;
                    }
                }
                break;

                case NOSDTS_WAITING_PARAM1:
                {
                    /* Waiting first parameter */
                    request.param1 += NANO_OS_CAST(uint32_t, (byte << (nb_bytes * 8u)));
                    nb_bytes++;
                    nb_bytes_left--;
                    if (nb_bytes_left == 0u)
                    {
                        nb_bytes = 0u;
                        nb_bytes_left = sizeof(uint32_t);
                        state = NOSDTS_WAITING_PARAM2;
                    }
                }
                break;

                case NOSDTS_WAITING_PARAM2:
                {
                    /* Waiting second parameter */
                    request.param2 += NANO_OS_CAST(uint32_t, (byte << (nb_bytes * 8u)));
                    nb_bytes++;
                    nb_bytes_left--;
                    if (nb_bytes_left == 0u)
                    {
                        /* End of request frame */
                        end_of_frame = true;
                        state = NOSDTS_WAITING_REQUEST;
                    }
                }
                break;

                default:
                {
                    /* Invalid state, should never happen */
                    NANO_OS_ERROR_ASSERT(false, NOS_ERR_FAILURE);
                }
                break;
            }

            /* Check end of frame */
            if (end_of_frame)
            {
                /* Handle request */
                end_of_frame = false;
                switch (request.command)
                {
                    case NDC_GET_VERSION:
                    {
                        /* Get version */
                        ret = NANO_OS_DEBUG_GetVersionHandler(&request);
                    }
                    break;


                    #if (NANO_OS_DEBUG_TASK_REQS_ENABLED == 1u)

                    case NDC_GET_TASK_COUNT:
                    {
                        /* Get task count */
                        ret = NANO_OS_DEBUG_GetTaskCountHandler(&request);
                    }
                    break;

                    case NDC_GET_TASK_LIST:
                    {
                        /* Get task list */
                        ret = NANO_OS_DEBUG_GetTaskListHandler(&request);
                    }
                    break;

                    case NDC_GET_TASK_STATUS:
                    {
                        /* Get task status */
                        ret = NANO_OS_DEBUG_GetTaskStatusHandler(&request);
                    }
                    break;

                    case NDC_GET_TASK_INFO:
                    {
                        /* Get task info */
                        ret = NANO_OS_DEBUG_GetTaskInfoHandler(&request);
                    }
                    break;

                    #endif /* (NANO_OS_DEBUG_TASK_REQS_ENABLED == 1u) */


                    #if (NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED == 1u)

                    case NDC_GET_OBJECT_COUNT:
                    {
                        /* Get the number of objects */
                        ret = NANO_OS_DEBUG_GetWaitObjectCountHandler(&request);
                    }
                    break;

                    case NDC_GET_OBJECT_LIST:
                    {
                        /* Get the list of the objects */
                        ret = NANO_OS_DEBUG_GetWaitObjectListHandler(&request);
                    }
                    break;

                    case NDC_GET_OBJECT_INFO:
                    {
                        /* Get information on an object */
                        ret = NANO_OS_DEBUG_GetWaitObjectInfoHandler(&request);
                    }
                    break;

                    #endif /* (NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED == 1u) */


                    #if (NANO_OS_DEBUG_MEMORY_REQS_ENABLED == 1u)

                    case NDC_READ_DATA:
                    {
                        /* Read data from a memory address */
                        ret = NANO_OS_DEBUG_ReadDataHandler(&request);
                    }
                    break;

                    case NDC_WRITE_DATA:
                    {
                        /* Write data to a memory address */
                        ret = NANO_OS_DEBUG_WriteDataHandler(&request);
                    }
                    break;

                    #endif /* (NANO_OS_DEBUG_MEMORY_REQS_ENABLED == 1u) */


                    default:
                    {
                        /* Unsupported command */
                        ret = NANO_OS_DEBUG_SendErrorResponse(&request, NOS_ERR_NOT_SUPPORTED);
                    }
                    break;
                }
                NANO_OS_ERROR_ASSERT_RET(ret);

                /* Finalize packet */
                ret = NANO_OS_USER_DebugFinalizePacket();
                NANO_OS_ERROR_ASSERT_RET(ret);
            }

        }
        while (true);
    }

    return NULL;
}

/** \brief Serialize an 8 bits integer in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeU8(const uint8_t data)
{
    return NANO_OS_USER_DebugWriteToPacket(&data, 1u);
}

/** \brief Serialize a 16 bits integer in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeU16(const uint16_t data)
{
    uint8_t buffer[2u];
    buffer[0u] = NANO_OS_CAST(uint8_t, (data & 0xFFu));
    buffer[1u] = NANO_OS_CAST(uint8_t, ((data >> 8u) & 0xFFu));
    return NANO_OS_USER_DebugWriteToPacket(buffer, sizeof(buffer));
}

/** \brief Serialize a 32 bits integer in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeU32(const uint32_t data)
{
    uint8_t buffer[4u];
    buffer[0u] = NANO_OS_CAST(uint8_t, (data & 0xFFu));
    buffer[1u] = NANO_OS_CAST(uint8_t, ((data >> 8u) & 0xFFu));
    buffer[2u] = NANO_OS_CAST(uint8_t, ((data >> 16u) & 0xFFu));
    buffer[3u] = NANO_OS_CAST(uint8_t, ((data >> 24u) & 0xFFu));
    return NANO_OS_USER_DebugWriteToPacket(buffer, sizeof(buffer));
}

/** \brief Serialize a string in a response packet */
static nano_os_error_t NANO_OS_DEBUG_SerializeString(const char* const string, const uint8_t string_len)
{
    nano_os_error_t ret;
    ret = NANO_OS_USER_DebugWriteToPacket(&string_len, 1u);
    if ((ret == NOS_ERR_SUCCESS) && (string_len != 0u))
    {
        ret = NANO_OS_USER_DebugWriteToPacket(NANO_OS_CAST(const uint8_t* const, string), string_len);
    }
    return ret;
}

/** \brief Send a response frame */
static nano_os_error_t NANO_OS_DEBUG_SendResponse(const nano_os_debug_request_t* const request, const nano_os_error_t error_code,
                                                  const uint8_t data_size, const bool has_more_data)
{
    nano_os_error_t ret;

    /* Initialize a new debug packet */
    ret = NANO_OS_USER_DebugInitPacket(NANO_OS_DEBUG_MIN_RESP_SIZE + data_size);
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Write header */
        ret = NANO_OS_DEBUG_SerializeU16(NANO_OS_DEBUG_FRAME_HEADER);

        /* Write command */
        if (has_more_data)
        {
            ret |= NANO_OS_DEBUG_SerializeU16(request->command | 0x8000u);
        }
        else
        {
            ret |= NANO_OS_DEBUG_SerializeU16(request->command);
        }

        /* Write return code */
        ret |=NANO_OS_DEBUG_SerializeU8(error_code);

        /* Write data size */
        ret |=NANO_OS_DEBUG_SerializeU8(data_size);
    }

    return ret;
}

/** \brief Send a error response */
static nano_os_error_t NANO_OS_DEBUG_SendErrorResponse(const nano_os_debug_request_t* const request, const nano_os_error_t error_code)
{
    return NANO_OS_DEBUG_SendResponse(request, error_code, 0u, false);
}

/** \brief Handler for the get version request */
static nano_os_error_t NANO_OS_DEBUG_GetVersionHandler(const nano_os_debug_request_t* const request)
{
    nano_os_error_t ret;

    /* Port name */
    const uint8_t port_name_len = NANO_OS_CAST(uint8_t, STRNLEN(NANO_OS_PORT_NAME, 200u));

    /* Get the current version */
    nano_os_version_t nano_os_version;
    nano_os_version_t nano_os_port_version;
    ret = NANO_OS_GetVersion(&nano_os_version, &nano_os_port_version);
    NANO_OS_ERROR_ASSERT_RET(ret);

    /* Send response header */
    ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, 4u + (port_name_len + 1u), false);
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Send version */
        ret = NANO_OS_DEBUG_SerializeU8(nano_os_version.major);
        ret |= NANO_OS_DEBUG_SerializeU8(nano_os_version.minor);

        /* Send port version */
        ret |= NANO_OS_DEBUG_SerializeU8(nano_os_port_version.major);
        ret |= NANO_OS_DEBUG_SerializeU8(nano_os_port_version.minor);

        /* Send port name */
        ret |= NANO_OS_DEBUG_SerializeString(NANO_OS_PORT_NAME, port_name_len);
    }

    return ret;
}


#if (NANO_OS_DEBUG_TASK_REQS_ENABLED == 1u)

/** \brief Handler for the get task count request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskCountHandler(const nano_os_debug_request_t* const request)
{
    nano_os_error_t ret;

    /* Send response header */
    ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, 2u, false);
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Send task count */
        ret = NANO_OS_DEBUG_SerializeU16(g_nano_os.task_count);
    }

    return ret;
}

/** \brief Handler for the get task list request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskListHandler(const nano_os_debug_request_t* const request)
{
    uint16_t total_size;
    nano_os_error_t ret;
    nano_os_task_t* task = g_nano_os.tasks;

    /* Compute total size */
    total_size = g_nano_os.task_count * sizeof(uint16_t);

    do
    {
        /* Compute frame size */
        bool has_more_data = false;
        uint8_t frame_size = NANO_OS_CAST(uint8_t, total_size);
        if (total_size > 0xFEu)
        {
            has_more_data = true;
            frame_size = 0xFEu;
        }
        total_size -= frame_size;

        /* Send response header */
        ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, frame_size, has_more_data);
        if (ret == NOS_ERR_SUCCESS)
        {
            do
            {
                /* Send task id */
                ret = NANO_OS_DEBUG_SerializeU16(task->object_id);

                /* Next task */
                task = task->global_next;
                frame_size -= sizeof(uint16_t);
            }
            while ((frame_size != 0u) && (ret == NOS_ERR_SUCCESS));
        }
    }
    while ((total_size != 0u) && (ret == NOS_ERR_SUCCESS));

    return ret;
}

/** \brief Handler for the get task status request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskStatusHandler(const nano_os_debug_request_t* const request)
{
    uint16_t total_size;
    nano_os_error_t ret;
    nano_os_task_t* task = g_nano_os.tasks;

    /* Compute total size */
    total_size = g_nano_os.task_count * (sizeof(uint8_t) + 2 * sizeof(uint16_t));

    do
    {
        /* Compute frame size */
        bool has_more_data = false;
        uint8_t frame_size = NANO_OS_CAST(uint8_t, total_size);
        if (total_size > 0xFFu)
        {
            has_more_data = true;
            frame_size = 0xFFu;
        }
        total_size -= frame_size;

        /* Send response header */
        ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, frame_size, has_more_data);
        if (ret == NOS_ERR_SUCCESS)
        {
            do
            {
                /* Send task state */
                ret = NANO_OS_DEBUG_SerializeU8(task->state);

                /* Send waiting object id */
                if ((task->state == NOS_TS_PENDING) && (task->waiting_object != NULL))
                {
                    ret |= NANO_OS_DEBUG_SerializeU16(task->waiting_object->object_id);
                }
                else
                {
                    ret |= NANO_OS_DEBUG_SerializeU16(0xFFFFu);
                }

                /* Send task CPU usage */
                #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
                ret = NANO_OS_DEBUG_SerializeU16(task->cpu_usage);
                #else
                ret = NANO_OS_DEBUG_SerializeU16(0xFFFFu);
                #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */

                /* Next task */
                task = task->global_next;
                frame_size -= (sizeof(uint8_t) + 2 * sizeof(uint16_t));
            }
            while ((frame_size != 0u) && (ret == NOS_ERR_SUCCESS));
        }
    }
    while ((total_size != 0u) && (ret == NOS_ERR_SUCCESS));

    return ret;
}

/** \brief Handler for the get task info request */
static nano_os_error_t NANO_OS_DEBUG_GetTaskInfoHandler(const nano_os_debug_request_t* const request)
{
    bool found = false;
    nano_os_error_t ret;
    nano_os_task_t* task = g_nano_os.tasks;

    /* Look for the requested task */
    while ((task != NULL) && !found)
    {
        if (task->object_id == request->param1)
        {
            found = true;
        }
        else
        {
            task = task->global_next;
        }
    }
    if (found)
    {
        /* Compute response size */
        #if (NANO_OS_TASK_NAME_ENABLED == 1u)
        const uint8_t name_len = NANO_OS_CAST(uint8_t, STRNLEN(task->name, 200u));
        #else
        const uint8_t name_len = 0u;
        #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */

        const uint16_t total_size = sizeof(uint8_t) + 2 * sizeof(uint32_t) + (sizeof(uint8_t) + name_len);

        /* Send response header */
        ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, total_size, false);
        if (ret == NOS_ERR_SUCCESS)
        {
            uint32_t stack_size_left, stack_size;

            /* Send base priority */
            ret = NANO_OS_DEBUG_SerializeU8(task->base_priority);

            /* Send stack informations */
            #if (NANO_OS_STATS_GETSTACKUSAGE_ENABLED == 1u)
            ret |= NANO_OS_STATS_GetStackUsage(task, &stack_size_left, &stack_size);
            #else
            stack_size = 0xFFFFFFFFu;
            stack_size_left = 0xFFFFFFFFu;
            #endif /* (NANO_OS_STATS_GETSTACKUSAGE_ENABLED == 1u) */
            ret |= NANO_OS_DEBUG_SerializeU32(stack_size);
            ret |= NANO_OS_DEBUG_SerializeU32(stack_size_left);

            /* Send name */
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            ret |= NANO_OS_DEBUG_SerializeString(task->name, name_len);
            #else
            ret |= NANO_OS_DEBUG_SerializeString("", name_len);
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
        }
    }
    else
    {
        /* Task doesn't exist */
        ret = NANO_OS_DEBUG_SendErrorResponse(request, NOS_ERR_INVALID_OBJECT);
    }

    return ret;
}

#endif /* (NANO_OS_DEBUG_TASK_REQS_ENABLED == 1u) */


#if (NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED == 1u)

/** \brief Handler for the get the number of objects request */
static nano_os_error_t NANO_OS_DEBUG_GetWaitObjectCountHandler(const nano_os_debug_request_t* const request)
{
    nano_os_error_t ret;

    /* Send response header */
    ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, 2u, false);
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Send wait object count */
        ret = NANO_OS_DEBUG_SerializeU16(g_nano_os.wait_object_count);
    }

    return ret;
}

/** \brief Handler for the get the list of the objects request */
static nano_os_error_t NANO_OS_DEBUG_GetWaitObjectListHandler(const nano_os_debug_request_t* const request)
{
    uint16_t total_size;
    nano_os_error_t ret;
    nano_os_wait_object_t* wait_object = g_nano_os.wait_objects;

    /* Compute total size */
    total_size = g_nano_os.wait_object_count * sizeof(uint16_t);

    do
    {
        /* Compute frame size */
        bool has_more_data = false;
        uint8_t frame_size = NANO_OS_CAST(uint8_t, total_size);
        if (total_size > 0xFEu)
        {
            has_more_data = true;
            frame_size = 0xFEu;
        }
        total_size -= frame_size;

        /* Send response header */
        ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, frame_size, has_more_data);
        if (ret == NOS_ERR_SUCCESS)
        {
            do
            {
                /* Send task id */
                ret = NANO_OS_DEBUG_SerializeU16(wait_object->object_id);

                /* Next object */
                wait_object = wait_object->next;
                frame_size -= sizeof(uint16_t);
            }
            while ((frame_size != 0u) && (ret == NOS_ERR_SUCCESS));
        }
    }
    while ((total_size != 0u) && (ret == NOS_ERR_SUCCESS));

    return ret;
}

/** \brief Handler for the get information on an object request */
static nano_os_error_t NANO_OS_DEBUG_GetWaitObjectInfoHandler(const nano_os_debug_request_t* const request)
{
    bool found = false;
    nano_os_error_t ret;
    nano_os_wait_object_t* wait_object = g_nano_os.wait_objects;

    /* Look for the requested wait object */
    while ((wait_object != NULL) && !found)
    {
        if (wait_object->object_id == request->param1)
        {
            found = true;
        }
        else
        {
            wait_object = wait_object->next;
        }
    }
    if (found)
    {
        /* Compute response size */
        #if (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u)
        const uint8_t name_len = NANO_OS_CAST(uint8_t, STRNLEN(wait_object->name, 200u));
        #else
        const uint8_t name_len = 0u;
        #endif /* (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u) */

        const uint16_t total_size = 2 * sizeof(uint8_t) + (sizeof(uint8_t) + name_len);

        /* Send response header */
        ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, total_size, false);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Send type */
            ret = NANO_OS_DEBUG_SerializeU8(wait_object->type);

            /* Send queuing */
            ret |= NANO_OS_DEBUG_SerializeU8(wait_object->queuing);

            /* Send name */
            #if (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u)
            ret |= NANO_OS_DEBUG_SerializeString(wait_object->name, name_len);
            #else
            ret |= NANO_OS_DEBUG_SerializeString("", name_len);
            #endif /* (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u) */
        }
    }
    else
    {
        /* Wait object doesn't exist */
        ret = NANO_OS_DEBUG_SendErrorResponse(request, NOS_ERR_INVALID_OBJECT);
    }

    return ret;
}

#endif /* (NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED == 1u) */


#if (NANO_OS_DEBUG_MEMORY_REQS_ENABLED == 1u)

/** \brief Handler for the read data from a memory address request */
static nano_os_error_t NANO_OS_DEBUG_ReadDataHandler(const nano_os_debug_request_t* const request)
{
    nano_os_error_t ret;

    /* Send response header */
    ret = NANO_OS_DEBUG_SendResponse(request, NOS_ERR_SUCCESS, 4u, false);
    if (ret == NOS_ERR_SUCCESS)
    {
        const uint32_t* const read_address = NANO_OS_CAST(const uint32_t*, request->param1);

        /* Send value */
        ret = NANO_OS_DEBUG_SerializeU32(*read_address);
    }

    return ret;
}

/** \brief Handler for the write data to a memory address request */
static nano_os_error_t NANO_OS_DEBUG_WriteDataHandler(const nano_os_debug_request_t* const request)
{
    nano_os_error_t ret;
    uint32_t* const write_address = NANO_OS_CAST(uint32_t*, request->param1);

    /* Write value */
    (*write_address) = request->param2;

    /* Send response */
    ret = NANO_OS_DEBUG_SendErrorResponse(request, NOS_ERR_SUCCESS);

    return ret;
}

#endif /* (NANO_OS_DEBUG_MEMORY_REQS_ENABLED == 1u) */

#endif /* (NANO_OS_DEBUG_ENABLED == 1u) */
