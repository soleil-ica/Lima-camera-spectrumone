/*============================================================================*/
/*             National Instruments / System Configuration API                */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2010-2017.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:   nisyscfg_pxi.h                                                    */
/* Purpose: Include file for PXI specific attributes to be used with the      */
/*          nisyscfg interface defined in nisyscfg.h                          */
/*                                                                            */
/*============================================================================*/

#ifndef ___nisyscfg_pxi_h___
#define ___nisyscfg_pxi_h___

#include <nisyscfg.h>

/* Declares the possible values of NISysCfgPxiPropertyClk10Source */
typedef enum
{
   NISysCfgPxiClk10SrcUnknown          = -1, /* Not applicable, or not software-readable */
   NISysCfgPxiClk10SrcInternal         = 0,  /* Internal Oscillator */
   NISysCfgPxiClk10SrcBuiltInConnector = 1,  /* External clock via the built-in connector */
   NISysCfgPxiClk10SrcTimingModule     = 2   /* System Timing Module */
} NISysCfgPxiClock10Sources;

/* Declares the possible values of NISysCfgPxiPropertyFanMode */
typedef enum
{
   NISysCfgPxiFanModesAuto       = 1, /* Default operating mode */
   NISysCfgPxiFanModesSafeManual = 2, /* Allows caller to manipulate the fan
                                         speed within safe boundaries by
                                         setting NISysCfgPxiPropertyFanUserRpm */
   NISysCfgPxiFanModesHigh       = 4, /* Fans run at the maximum speed for the
                                         current cooling profile */
} NISysCfgPxiFanModes;

/* Declares the possible values of NISysCfgPxiPropertyCoolingProfiles */
typedef enum
{
   NISysCfgPxiCoolingProfile38W  = 1, /* Default operating mode */
   NISysCfgPxiCoolingProfile58W  = 2  /* More aggressive cooling profile for
                                         cooling high power modules */
} NISysCfgPxiCoolingProfiles;

/* Declares the possible values of NISysCfgPxiIndexedPropertyPowerSupplyState */
typedef enum
{
   NISysCfgPxiPowerSupplyStateOff     = 0, /* Off or not present */
   NISysCfgPxiPowerSupplyStateOn      = 1, /* On */
   NISysCfgPxiPowerSupplyStateFaulted = 2  /* Faulted */
} NISysCfgPxiPowerSupplyState;

typedef enum
{
   /* Chassis attributes */
   NISysCfgPxiPropertyPxiChassisNumber               = 184565760,  /* unsigned int */

   /* Clock attributes */
   NISysCfgPxiPropertyClk10Source                    = 184635392,  /* NISysCfgPxiClock10Sources */

   /* Fan control attributes */
   NISysCfgPxiPropertyFanMode                        = 185597952,  /* NISysCfgPxiFanModes */
   NISysCfgPxiPropertyFanUserRpm                     = 185602048,  /* unsigned int */
   NISysCfgPxiPropertySupportedFanModes              = 185606144,  /* bitmask of NISysCfgPxiFanModes */
   NISysCfgPxiPropertyFanManualRpmLowerBound         = 185634816,  /* unsigned int */
   NISysCfgPxiPropertyFanManualRpmUpperBound         = 185638912,  /* unsigned int */
   NISysCfgPxiPropertyCoolingProfile                 = 185610240,  /* NISysCfgPxiCoolingProfiles */
   NISysCfgPxiPropertySupportedCoolingProfiles       = 185614336,  /* bitmask of NISysCfgPxiCoolingProfiles */

   /* Power supply attributes */
   NISysCfgPxiPropertyPowerSupplyBayCount            = 186777600,  /* int */
   NISysCfgPxiPropertyPowerSuppliesRedundant         = 186798080   /* unsigned int */

} NISysCfgPxiProperty;

typedef enum
{
   /* Power supply index attributes */
   NISysCfgPxiIndexedPropertyPowerSupplyName         = 186781696,  /* char * */
   NISysCfgPxiIndexedPropertyPowerSupplyState        = 186789888,  /* NISysCfgPxiPowerSupplyState */
   NISysCfgPxiIndexedPropertyPowerSupplyPower        = 186793984   /* unsigned int (Watts) */

} NISysCfgPxiIndexedProperty;

#endif
