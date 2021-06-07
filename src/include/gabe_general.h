/**
 * Definitions for GABE registers that don't seem to fit anywhere else
 */

#ifndef __GABE_GENERAL_H
#define __GABE_GENERAL_H

#define GABE_MSTR_CTRL      0xAFE880
#define GABE_CTRL_PWR_LED   0x01     // Controls the LED in the Front of the case (Next to the reset button)
#define GABE_CTRL_SDC_LED   0x02     // Controls the LED in the Front of the Case (Next to SDCard)
#define GABE_CTRL_STS_LED0  0x04     // Control Status LED0 (General Use) - C256 Foenix U Only
#define GABE_CTRL_STS_LED1  0x08     // Control Status LED0 (General Use) - C256 Foenix U Only
#define GABE_CTRL_BUZZER    0x10     // Controls the Buzzer
#define GABE_CTRL_WRM_RST   0x80     // Warm Reset (needs to Setup other registers)

#endif
