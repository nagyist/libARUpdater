/**
 * @file ARUPDATER_Manager.c
 * @brief libARUpdater Manager c file.
 * @date07/01/2014
 * @author david.flattin.ext@parrot.com
 **/
#include <stdlib.h>
#include <stdio.h>
#include <libARSAL/ARSAL_Print.h>

#include <libARUpdater/ARUPDATER_Error.h>
#include <libARUpdater/ARUPDATER_Manager.h>
#include "ARUPDATER_Manager.h"
#include "ARUPDATER_Utils.h"

#define ARUPDATER_MANAGER_TAG   "ARUPDATER_Manager"

ARUPDATER_Manager_t* ARUPDATER_Manager_New(const char *const plfFolder, eARUPDATER_ERROR *error)
{
    ARUPDATER_Manager_t *manager = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    /* Check parameters */
    if(err == ARUPDATER_OK)
    {
        /* Create the Manager */
        manager = malloc (sizeof (ARUPDATER_Manager_t));
        if (manager == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }

    /* Initialize to default values */
    if(err == ARUPDATER_OK)
    {
        manager->updater = ARUPDATER_Updater_New(&err);
    }
    
    if(err == ARUPDATER_OK)
    {
        manager->plfSender = ARUPDATER_PlfSender_New(&err);
    }
    
    if (err == ARUPDATER_OK)
    {
        manager->plfFolder = (char*) malloc(strlen(plfFolder) + 1);
        if (manager->plfFolder == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if (ARUPDATER_OK == err)
    {
        strcpy(manager->plfFolder, plfFolder);
    }
    
    /* delete the Manager if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_MANAGER_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_Manager_Delete (&manager);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    }
    
    return manager;
}

void ARUPDATER_Manager_Delete (ARUPDATER_Manager_t **managerPtrAddr)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, "     ");

    if (managerPtrAddr != NULL)
    {
        ARUPDATER_Manager_t *manager = *managerPtrAddr;
        
        if (manager != NULL)
        {
            ARUPDATER_Updater_Delete(&(manager->updater));
            ARUPDATER_PlfSender_Delete(&(manager->plfSender));
            
            free(manager->plfFolder);
            
            free(manager);
            *managerPtrAddr = NULL;
        }
    }
}

eARUPDATER_ERROR ARUPDATER_Manager_GetPlfVersion(ARUPDATER_Manager_t *manager, const char *const plfFileName, int *version, int *edition, int *extension)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if ((manager != NULL) &&
        (plfFileName != NULL) &&
        (version != NULL) &&
        (edition != NULL) &&
        (extension != NULL))
    {
        char *filePath = malloc(strlen(manager->plfFolder) + strlen(plfFileName) + 1);
        strcpy(filePath, manager->plfFolder);
        strcat(filePath, plfFileName);
        
        error = ARUPDATER_Utils_GetPlfVersion(filePath, version, edition, extension);
    }
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_Manager_PrepareCheckLocaleVersion(ARUPDATER_Manager_t *manager, const char *const device, const char *const plfFileName, ARUPDATER_Updater_ShouldDownloadPlfCallback_t shouldDownloadCallback, void *downloadArg, ARUPDATER_Updater_PlfDownloadProgressCallback_t progressCallback, void *progressArg, ARUPDATER_Updater_PlfDownloadCompletionCallback_t completionCallback, void *completionArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if ((device != NULL) &&
        (plfFileName != NULL))
    {
        error = ARUPDATER_Updater_PrepareCheckLocaleVersion(manager->updater, device, manager->plfFolder, plfFileName, shouldDownloadCallback, downloadArg, progressCallback, progressArg, completionCallback, completionArg);
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    return error;
}


eARUPDATER_ERROR ARUPDATER_Manager_CheckLocaleVersionThreadRun (void *managerArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if (managerArg != NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (error != ARUPDATER_OK)
    {
        ARUPDATER_Manager_t *manager = (ARUPDATER_Manager_t*)managerArg;
        error = ARUPDATER_Updater_CheckLocaleVersionThreadRun(manager->updater);
    }
    
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, " ");
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_Manager_CancelThread (ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if (manager != NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (error != ARUPDATER_OK)
    {
        error = ARUPDATER_Updater_CancelThread(manager->updater);
    }
    
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, " ");
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_Manager_PrepareSendToDrone(ARUPDATER_Manager_t *manager, const char *const plfFileName, ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback, void *progressArg, ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback, void *completionArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    if (plfFileName != NULL)
    {
        error = ARUPDATER_PlfSender_PrepareSendToDrone(manager->plfSender, manager->plfFolder, plfFileName, progressCallback, progressArg, completionCallback, completionArg);
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    return error;
}


eARUPDATER_ERROR ARUPDATER_Manager_SendToDroneThreadRun (void *managerArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if (managerArg != NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (error != ARUPDATER_OK)
    {
        ARUPDATER_Manager_t *manager = (ARUPDATER_Manager_t*)managerArg;
        error = ARUPDATER_PlfSender_SendToDroneThreadRun(manager->updater);
    }
    
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, " ");
    
    return error;
}