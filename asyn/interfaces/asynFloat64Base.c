/*  asynFloat64Base.c */
/***********************************************************************
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory, and the Regents of the University of
* California, as Operator of Los Alamos National Laboratory, and
* Berliner Elektronenspeicherring-Gesellschaft m.b.H. (BESSY).
* asynDriver is distributed subject to a Software License Agreement
* found in file LICENSE that is included with this distribution.
***********************************************************************/

/*  11-OCT-2004 Marty Kraimer
*/

#include <asynDriver.h>
#include <epicsTypes.h>
#include <epicsStdio.h>

#define epicsExportSharedSymbols

#include "asynFloat64.h"

static asynStatus initialize(const char *portName, asynInterface *pfloat64Interface);

static asynFloat64Base float64Base = {initialize};
epicsShareDef asynFloat64Base *pasynFloat64Base = &float64Base;

static asynStatus writeDefault(void *drvPvt, asynUser *pasynUser,
                               epicsFloat64 value);
static asynStatus readDefault(void *drvPvt, asynUser *pasynUser,
                              epicsFloat64 *value);
static asynStatus registerInterruptUser(void *drvPvt,asynUser *pasynUser,
       interruptCallbackFloat64 callback, void *userPvt, void **registrarPvt);
static asynStatus cancelInterruptUser(void *registrarPvt, asynUser *pasynUser);

static asynStatus initialize(const char *portName,
    asynInterface *pfloat64Interface)
{
    asynFloat64 *pasynFloat64 = (asynFloat64 *)pfloat64Interface->pinterface;

    if(!pasynFloat64->write) pasynFloat64->write = writeDefault;
    if(!pasynFloat64->read) pasynFloat64->read = readDefault;
    if(!pasynFloat64->registerInterruptUser)
        pasynFloat64->registerInterruptUser = registerInterruptUser;
    if(!pasynFloat64->cancelInterruptUser)
        pasynFloat64->cancelInterruptUser = cancelInterruptUser;
    return pasynManager->registerInterface(portName,pfloat64Interface);
}

static asynStatus writeDefault(void *drvPvt, asynUser *pasynUser,
    epicsFloat64 value)
{
    const char *portName;
    asynStatus status;
    int        addr;
    
    status = pasynManager->getPortName(pasynUser,&portName);
    if(status!=asynSuccess) return status;
    status = pasynManager->getAddr(pasynUser,&addr);
    if(status!=asynSuccess) return status;
    epicsSnprintf(pasynUser->errorMessage,pasynUser->errorMessageSize,
        "write is not supported\n");
    asynPrint(pasynUser,ASYN_TRACE_ERROR,
        "%s %d write is not supported\n",portName,addr);
    return asynError;
}

static asynStatus readDefault(void *drvPvt, asynUser *pasynUser,
    epicsFloat64 *value)
{
    const char *portName;
    asynStatus status;
    int        addr;
    
    status = pasynManager->getPortName(pasynUser,&portName);
    if(status!=asynSuccess) return status;
    status = pasynManager->getAddr(pasynUser,&addr);
    if(status!=asynSuccess) return status;
    epicsSnprintf(pasynUser->errorMessage,pasynUser->errorMessageSize,
        "write is not supported\n");
    asynPrint(pasynUser,ASYN_TRACE_ERROR,
        "%s %d read is not supported\n",portName,addr);
    return asynError;
}
    
static asynStatus registerInterruptUser(void *drvPvt,asynUser *pasynUser,
      interruptCallbackFloat64 callback, void *userPvt, void **registrarPvt)
{
    const char *portName;
    asynStatus status;
    int        addr;
    interruptNode *pinterruptNode;
    asynFloat64Interrupt *pasynFloat64Interrupt;
    void *pinterruptPvt;
    
    status = pasynManager->getPortName(pasynUser,&portName);
    if(status!=asynSuccess) return status;
    status = pasynManager->getAddr(pasynUser,&addr);
    if(status!=asynSuccess) return status;
    status = pasynManager->getInterruptPvt(pasynUser, asynFloat64Type,
                                           &pinterruptPvt);
    if(status!=asynSuccess) return status;
    pasynFloat64Interrupt = pasynManager->memMalloc(sizeof(asynFloat64Interrupt));
    pinterruptNode = pasynManager->createInterruptNode(pinterruptPvt);
    if(status!=asynSuccess) return status;
    pinterruptNode->drvPvt = pasynFloat64Interrupt;
    pasynFloat64Interrupt->addr = addr;
    pasynFloat64Interrupt->reason = pasynUser->reason;
    pasynFloat64Interrupt->drvUser = pasynUser->drvUser;
    pasynFloat64Interrupt->callback = callback;
    pasynFloat64Interrupt->userPvt = userPvt;
    *registrarPvt = pinterruptNode;
    asynPrint(pasynUser,ASYN_TRACE_FLOW,
        "%s %d registerInterruptUser\n",portName,addr);
    return pasynManager->addInterruptUser(pinterruptNode);
}

static asynStatus cancelInterruptUser(void *registrarPvt, asynUser *pasynUser)
{
    interruptNode *pinterruptNode = (interruptNode *)registrarPvt;
    asynStatus status;
    const char *portName;
    int        addr;

    status = pasynManager->getPortName(pasynUser,&portName);
    if(status!=asynSuccess) return status;
    status = pasynManager->getAddr(pasynUser,&addr);
    if(status!=asynSuccess) return status;
    asynPrint(pasynUser,ASYN_TRACE_FLOW,
        "%s %d cancelInterruptUser\n",portName,addr);
    pasynManager->memFree(pinterruptNode->drvPvt, sizeof(asynFloat64Interrupt));
    status = pasynManager->removeInterruptUser(pinterruptNode);
    return status;
}
