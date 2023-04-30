/**
  ******************************************************************************
  * @file       usbd_audio_stcard.c
  * @brief      This file provides functions to USB driver.
  *
  * @author     darkyfoxy [*GitHub*](https://github.com/darkyfoxy)
  * @version    0.01
  * @date       30.04.2023
  *
  ******************************************************************************
  * @copyright  <h3>Copyright (c) 2023 Pavlov V.</h3>
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usbd_audio_stcard.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_AUDIO
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_AUDIO_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_AUDIO_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_AUDIO_Private_Macros
  * @{
  */
#define AUDIO_SAMPLE_FREQ(frq)         (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define AUDIO_PACKET_SZE(frq)          (uint8_t)(((frq * 2U * 2U)/1000U) & 0xFFU), \
                                       (uint8_t)((((frq * 2U * 2U)/1000U) >> 8) & 0xFFU)

/**
  * @}
  */


uint8_t zero_buff[AUDIO_PACKET_SIZE] = {0};

/** @defgroup USBD_AUDIO_Private_FunctionPrototypes
  * @{
  */
static uint8_t USBD_AUDIO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_AUDIO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t USBD_AUDIO_Setup(USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req);

static uint8_t *USBD_AUDIO_GetCfgDesc(uint16_t *length);
static uint8_t *USBD_AUDIO_GetDeviceQualifierDesc(uint16_t *length);
static uint8_t USBD_AUDIO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_AUDIO_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_AUDIO_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_AUDIO_EP0_TxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_AUDIO_SOF(USBD_HandleTypeDef *pdev);

static uint8_t USBD_AUDIO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_AUDIO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static void AUDIO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void AUDIO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

/**
  * @}
  */

/** @defgroup USBD_AUDIO_Private_Variables
  * @{
  */
USBD_AUDIO_HandleTypeDef USBD_AUDIO_Handle;

USBD_ClassTypeDef USBD_AUDIO =
{
  USBD_AUDIO_Init,
  USBD_AUDIO_DeInit,
  USBD_AUDIO_Setup,
  USBD_AUDIO_EP0_TxReady,
  USBD_AUDIO_EP0_RxReady,
  USBD_AUDIO_DataIn,
  USBD_AUDIO_DataOut,
  USBD_AUDIO_SOF,
  USBD_AUDIO_IsoINIncomplete,
  USBD_AUDIO_IsoOutIncomplete,
  USBD_AUDIO_GetCfgDesc,
  USBD_AUDIO_GetCfgDesc,
  USBD_AUDIO_GetCfgDesc,
  USBD_AUDIO_GetDeviceQualifierDesc,
};

/* USB AUDIO device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIO_CfgDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /* Configuration 1 */
   0x09,                                 /* bLength */
  USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
  LOBYTE(USB_AUDIO_CONFIG_DESC_SIZ),    /* wTotalLength  192 bytes*/
  HIBYTE(USB_AUDIO_CONFIG_DESC_SIZ),
  0x03,                                 /* bNumInterfaces */                                                         /*edit*/
  0x01,                                 /* bConfigurationValue */
  0x00,                                 /* iConfiguration */
  0x80,                                 /* bmAttributes: Bus Powered according to user configuration */              /*edit*/
  0xFA,                                 /* bMaxPower = 500 mA */                                                     /*edit*/
  /* 09 byte*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* USB Speaker Standard interface descriptor */
    AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    AUDIO_CTRL_IF,                        /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 byte*/

    /* USB Speaker Class-specific AC Interface Descriptor */
    0x0A,                                 /* bLength  9 + 1 */                                                       /*edit*/
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
    0x00,          /* 1.00 */             /* bcdADC */
    0x01,
    0x46,                                 /* wTotalLength = 39 + 31 = 70*/                                           /*edit*/
    0x00,
    0x02,                                 /* bInCollection */                                                        /*edit*/
    AUDIO_OUT_IF,                         /* baInterfaceNr(1) */                                                     /*edit*/
    AUDIO_IN_IF,                          /* baInterfaceNr(2) */                                                     /*edit*/
    /* 10 byte*/

      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      /* USB Speaker Input Terminal Descriptor */
      AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
      AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
      AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
      0x01,                                 /* bTerminalID */
      0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
      0x01,
      0x00,                                 /* bAssocTerminal */
      0x01,                                 /* bNrChannels */                                                        /*edit*/
      0x04,                                 /* wChannelConfig 0x0004  Mono */                                        /*edit*/
      0x00,
      0x00,                                 /* iChannelNames */
      0x00,                                 /* iTerminal */
      /* 12 byte*/

      /* USB Speaker Audio Feature Unit Descriptor */
      0x09,                                 /* bLength */
      AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
      AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
      AUDIO_OUT_STREAMING_CTRL,             /* bUnitID */
      0x01,                                 /* bSourceID */
      0x01,                                 /* bControlSize */
      AUDIO_CONTROL_MUTE,                   /* bmaControls(0) */
      0x00,                                 /* bmaControls(1) */
      0x00,                                 /* iTerminal */
      /* 09 byte*/

      /*USB Speaker Output Terminal Descriptor */
      0x09,      /* bLength */
      AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
      AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
      0x03,                                 /* bTerminalID */
      0x01,                                 /* wTerminalType  0x0301*/
      0x03,
      0x00,                                 /* bAssocTerminal */
      0x02,                                 /* bSourceID */
      0x00,                                 /* iTerminal */
      /* 09 byte*/

      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      /* USB Mic Input Terminal Descriptor */
      AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
      AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
      AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
      0x04,                                 /* bTerminalID */                                                        /*edit*/
      0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_MICRO   0x0201 */                    /*edit*/
      0x02,                                                                                                          /*edit*/
      0x00,                                 /* bAssocTerminal */
      0x01,                                 /* bNrChannels */                                                        /*edit*/
      0x04,                                 /* wChannelConfig 0x0004  Mono */                                        /*edit*/
      0x00,
      0x00,                                 /* iChannelNames */
      0x00,                                 /* iTerminal */
      /* 12 byte*/

      /* USB Mic Audio Feature Unit Descriptor */
      0x09,                                 /* bLength */
      AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
      AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
      0x05,                                 /* bUnitID */                                                            /*edit*/
      0x04,                                 /* bSourceID */                                                          /*edit*/
      0x01,                                 /* bControlSize */
      AUDIO_CONTROL_MUTE,                   /* bmaControls(0) */
      0x00,                                 /* bmaControls(1) */
      0x00,                                 /* iTerminal */
      /* 09 byte*/

      /*USB mic Output Terminal Descriptor */
      0x09,      /* bLength */
      AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
      AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
      0x06,                                 /* bTerminalID */                                                        /*edit*/
      0x01,                                 /* wTerminalType  0x0101*/                                               /*edit*/
      0x01,                                                                                                          /*edit*/
      0x00,                                 /* bAssocTerminal */
      0x05,                                 /* bSourceID */                                                          /*edit*/
      0x00,                                 /* iTerminal */
      /* 09 byte*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwidth */
    /* Interface 1, Alternate Setting 0                                             */
    AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    AUDIO_OUT_IF,                         /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 byte*/

    /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
    /* Interface 1, Alternate Setting 1                                           */
    AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    AUDIO_OUT_IF,                         /* bInterfaceNumber */
    0x01,                                 /* bAlternateSetting */
    0x01,                                 /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 byte*/

    /* USB Speaker Audio Streaming Interface Descriptor */
    AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
    0x01,                                 /* bTerminalLink */
    0x01,                                 /* bDelay */
    0x01,                                 /* wFormatTag AUDIO_FORMAT_PCM  0x0001 */
    0x00,
    /* 07 byte*/

    /* USB Speaker Audio Type I Format Interface Descriptor */
    0x0B,                                 /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
    0x01,                                 /* bNrChannels */                                                          /*edit*/
    0x02,                                 /* bSubFrameSize :  2 Bytes per frame (16bits) */
    0x10,                                 /* bBitResolution (16-bits per sample) */
    0x01,                                 /* bSamFreqType only one frequency supported */
    AUDIO_SAMPLE_FREQ(USBD_AUDIO_FREQ),   /* Audio sampling frequency coded on 3 bytes */
    /* 11 byte*/

    /* Endpoint 1 - Standard Descriptor */
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
    USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
    AUDIO_OUT_EP,                         /* bEndpointAddress 1 out endpoint */
    USBD_EP_TYPE_ISOC,                    /* bmAttributes */
    AUDIO_PACKET_SIZE,                    /* wMaxPacketSize in Bytes (Freq(Samples)*2(HalfWord)) */                  /*edit*/
    0x00,
    AUDIO_FS_BINTERVAL,                   /* bInterval */
    0x00,                                 /* bRefresh */
    0x00,                                 /* bSynchAddress */
    /* 09 byte*/

    /* Endpoint - Audio Streaming Descriptor*/
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
    AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
    0x00,                                 /* bmAttributes */
    0x00,                                 /* bLockDelayUnits */
    0x00,                                 /* wLockDelay */
    0x00,
    /* 07 byte*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* USB Mic Standard AS Interface Descriptor - Audio Streaming Zero Bandwidth */
    /* Interface 1, Alternate Setting 0                                             */
    AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    AUDIO_IN_IF,                                 /* bInterfaceNumber */                                              /*edit*/
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 byte*/

    /* USB Mic Standard AS Interface Descriptor - Audio Streaming Operational */
    /* Interface 1, Alternate Setting 1                                           */
    AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    AUDIO_IN_IF,                                 /* bInterfaceNumber */                                              /*edit*/
    0x01,                                 /* bAlternateSetting */
    0x01,                                 /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 byte*/

    /* USB Mic Audio Streaming Interface Descriptor */
    AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
    0x06,                                 /* bTerminalLink */                                                        /*edit*/
    0x01,                                 /* bDelay */
    0x01,                                 /* wFormatTag AUDIO_FORMAT_PCM  0x0001 */
    0x00,
    /* 07 byte*/

    /* USB Mic Audio Type I Format Interface Descriptor */
    0x0B,                                 /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
    0x01,                                 /* bNrChannels */                                                          /*edit*/
    0x02,                                 /* bSubFrameSize :  2 Bytes per frame (16bits) */
    0x10,                                   /* bBitResolution (16-bits per sample) */
    0x01,                                 /* bSamFreqType only one frequency supported */
    AUDIO_SAMPLE_FREQ(USBD_AUDIO_FREQ),   /* Audio sampling frequency coded on 3 bytes */
    /* 11 byte*/

    /* Endpoint 1 - Standard Descriptor */
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
    USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
    AUDIO_IN_EP,                          /* bEndpointAddress 1 out endpoint */                                      /*edit*/
    USBD_EP_TYPE_ISOC,                    /* bmAttributes */
    AUDIO_PACKET_SIZE,                     /* wMaxPacketSize in Bytes (Freq(Samples)*2(HalfWord)) */                 /*edit*/
    0x00,
    AUDIO_FS_BINTERVAL,                   /* bInterval */
    0x00,                                 /* bRefresh */
    0x00,                                 /* bSynchAddress */
    /* 09 byte*/

    /* Endpoint - Audio Streaming Descriptor*/
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
    AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
    0x00,                                 /* bmAttributes */
    0x00,                                 /* bLockDelayUnits */
    0x00,                                 /* wLockDelay */
    0x00,
    /* 07 byte*/
} ;

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_AUDIO_Private_Functions
  * @{
  */

/**
  * @brief  USBD_AUDIO_Init
  *         Initialize the AUDIO interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_AUDIO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)

  {
    USBD_AUDIO_HandleTypeDef *haudio;

    /* Open EP OUT */
    USBD_LL_OpenEP (pdev, AUDIO_OUT_EP, USBD_EP_TYPE_ISOC, AUDIO_PACKET_SIZE);
    pdev->ep_out[AUDIO_OUT_EP & 0xFU].is_used = 1U;

    /* Open EP IN */
    USBD_LL_OpenEP (pdev, AUDIO_IN_EP, USBD_EP_TYPE_ISOC, AUDIO_PACKET_SIZE);
    pdev->ep_in[AUDIO_IN_EP & 0xFU].is_used = 1U;

    /* Allocate Audio structure */
    pdev->pClassData = (void*) &USBD_AUDIO_Handle;

    if (pdev->pClassData == NULL)
    {
      return USBD_FAIL;
    }
    else
    {
      haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

      for (uint8_t i = 0; i <= USBD_MAX_NUM_INTERFACES; i++)
      {
        haudio->alt_setting [i] = 0U;
      }

      /* Initialize the Audio Out Buffer */
      haudio->out.wr_ptr = 0U;
      haudio->out.rd_ptr = 0U;
      haudio->out.buff_enable = 0U;
      memset ((uint8_t*) haudio->out.buff, 0, AUDIO_TOTAL_BUF_SIZE);

      /* Initialize the Audio In Buffer */
      haudio->in.wr_ptr = 0U;
      haudio->in.rd_ptr = 0U;
      haudio->in.buff_enable = 0U;
      memset ((uint8_t*) haudio->in.buff, 0, AUDIO_TOTAL_BUF_SIZE);

      /* Initialize the Audio output Hardware layer */
      if (((USBD_AUDIO_ItfTypeDef*) pdev->pUserData)->Init (USBD_AUDIO_FREQ,
                                                            AUDIO_DEFAULT_VOLUME,
                                                            0U) != 0)
      {
        return USBD_FAIL;
      }

      /* Prepare Out endpoint to receive 1st packet */
      USBD_LL_PrepareReceive (pdev, AUDIO_OUT_EP, haudio->out.buff, AUDIO_PACKET_SIZE);

      USBD_LL_FlushEP (pdev, AUDIO_IN_EP);
      USBD_LL_Transmit (pdev, AUDIO_IN_EP, zero_buff, AUDIO_PACKET_SIZE);

    }
    return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_Init
  *         DeInitialize the AUDIO layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_AUDIO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /* Close EP OUT */
    USBD_LL_CloseEP (pdev, AUDIO_OUT_EP);
    pdev->ep_out[AUDIO_OUT_EP & 0xFU].is_used = 0U;

    /* Close EP IN */
    USBD_LL_CloseEP (pdev, AUDIO_IN_EP);
    pdev->ep_in[AUDIO_IN_EP & 0xFU].is_used = 0U;

    /* DeInit  physical Interface components */
    if (pdev->pClassData != NULL)
    {
      ((USBD_AUDIO_ItfTypeDef*) pdev->pUserData)->DeInit (0U);
      pdev->pClassData = NULL;
    }

    return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_Setup
  *         Handle the AUDIO specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_AUDIO_Setup(USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req)
{
  USBD_AUDIO_HandleTypeDef *haudio;
  uint16_t len;
  uint8_t  *pbuf;
  uint16_t status_info = 0U;
  uint8_t  ret = USBD_OK;

  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS:
    switch (req->bRequest)
    {
    case AUDIO_REQ_GET_CUR:
      AUDIO_REQ_GetCurrent (pdev, req);
      break;

    case AUDIO_REQ_SET_CUR:
      AUDIO_REQ_SetCurrent (pdev, req);
      break;

    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;
      break;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_STATUS:
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        USBD_CtlSendData (pdev, (uint8_t *) (void *) &status_info, 2U);
      }
      else
      {
        USBD_CtlError (pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_GET_DESCRIPTOR:
      if( (req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_AUDIO_CfgDesc + 18;
        len  = MIN (pbuf[0], req->wLength);

        USBD_CtlSendData (pdev, pbuf, len);
      }
      break;

    case USB_REQ_GET_INTERFACE :
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        USBD_CtlSendData (pdev, (uint8_t*) (void*) &haudio->alt_setting[req->wIndex], 1U);
      }
      else
      {
        USBD_CtlError (pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_SET_INTERFACE :
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        if ((uint8_t) (req->wValue) <= USBD_MAX_NUM_INTERFACES)
        {
          haudio->alt_setting [req->wIndex] = (uint8_t) (req->wValue);

          /* Handles Alternate Settings 0 of Audio OUT interface */
          if (haudio->alt_setting[AUDIO_OUT_IF] == 0)
          {
            ((USBD_AUDIO_ItfTypeDef *) pdev->pUserData)->AudioCmd (&haudio->out.buff[0],
                                                                   AUDIO_TOTAL_BUF_SIZE,
                                                                   AUDIO_CMD_STOP);
          }

          /* Handles Alternate Settings 1 of Audio IN interface */
          if (haudio->alt_setting[AUDIO_IN_IF] == 1)
          {
            if (!haudio->in.buff_enable)
            {
              haudio->in.buff_enable = 2U;
            }
          }
          else
          {
            haudio->in.buff_enable = 0U;
            USBD_LL_FlushEP (pdev, AUDIO_IN_EP);
          }
        }
        else
        {
          /* Call the error management function (command will be nacked */
          USBD_CtlError (pdev, req);
          ret = USBD_FAIL;
        }
      }
      else
      {
        USBD_CtlError (pdev, req);
        ret = USBD_FAIL;
      }
      break;

      default:
        USBD_CtlError (pdev, req);
        ret = USBD_FAIL;
        break;
      }
      break;
    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;
      break;
    }

  return ret;
}

/**
  * @brief  USBD_AUDIO_GetCfgDesc
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_AUDIO_GetCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_AUDIO_CfgDesc);

  return USBD_AUDIO_CfgDesc;
}

/**
  * @brief  USBD_AUDIO_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_AUDIO_HandleTypeDef *haudio;

  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (epnum == (AUDIO_IN_EP & 0x7F))
  {
    haudio->in.rd_ptr += AUDIO_PACKET_SIZE;

    if (haudio->in.rd_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      ((USBD_AUDIO_ItfTypeDef *) pdev->pUserData)->AudioCmd(&haudio->in.buff[0],
                                                            AUDIO_TOTAL_BUF_SIZE,
                                                            AUDIO_CMD_RECORD);
    }

    if (haudio->in.rd_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      haudio->in.rd_ptr = 0U;
    }

    USBD_LL_FlushEP (pdev, AUDIO_IN_EP);
    USBD_LL_Transmit (pdev, AUDIO_IN_EP, &haudio->in.buff[haudio->in.rd_ptr], AUDIO_PACKET_SIZE);
  }

  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIO_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_AUDIO_HandleTypeDef *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (haudio->control.cmd == AUDIO_REQ_SET_CUR)
  {/* In this driver, to simplify code, only SET_CUR request is managed */

    if (haudio->control.unit == AUDIO_OUT_STREAMING_CTRL)
    {
      ((USBD_AUDIO_ItfTypeDef *) pdev->pUserData)->MuteCtl (haudio->control.data[0]);
      haudio->control.cmd = 0U;
      haudio->control.len = 0U;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIO_EP0_TxReady(USBD_HandleTypeDef *pdev)
{
  UNUSED(pdev);

  /* Only OUT control data are processed */
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_AUDIO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */

static uint8_t USBD_AUDIO_SOF(USBD_HandleTypeDef *pdev)
{
  UNUSED(pdev);
  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
void USBD_AUDIO_Sync(USBD_HandleTypeDef *pdev, AUDIO_OffsetTypeDef offset)
{
  UNUSED(pdev);
  UNUSED(offset);

}

/**
  * @brief  USBD_AUDIO_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_AUDIO_HandleTypeDef *haudio;

  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (epnum == (AUDIO_IN_EP & 0x7F))
  {
    haudio->in.rd_ptr += AUDIO_PACKET_SIZE;

    if (haudio->in.rd_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      ((USBD_AUDIO_ItfTypeDef *) pdev->pUserData)->AudioCmd(&haudio->in.buff[0],
                                                            AUDIO_TOTAL_BUF_SIZE,
                                                            AUDIO_CMD_RECORD);
    }

    if (haudio->in.rd_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      haudio->in.rd_ptr = 0U;
    }

    USBD_LL_FlushEP (pdev, AUDIO_IN_EP);
    USBD_LL_Transmit (pdev, AUDIO_IN_EP, &haudio->in.buff[haudio->in.rd_ptr], AUDIO_PACKET_SIZE);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_AUDIO_HandleTypeDef *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (epnum == AUDIO_OUT_EP)
  {
    haudio->out.wr_ptr += AUDIO_PACKET_SIZE;

    if (haudio->out.wr_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      /* All buffers are full: roll back */
      haudio->out.wr_ptr = 0U;

      ((USBD_AUDIO_ItfTypeDef*) pdev->pUserData)->AudioCmd (&haudio->out.buff[0],
                                                            AUDIO_TOTAL_BUF_SIZE,
                                                            AUDIO_CMD_PLAY);
    }

    /* Prepare Out endpoint to receive next audio packet */
    USBD_LL_PrepareReceive (pdev, AUDIO_OUT_EP, &haudio->out.buff[haudio->out.wr_ptr], AUDIO_PACKET_SIZE);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_AUDIO_HandleTypeDef *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (epnum == AUDIO_OUT_EP)
  {
    haudio->out.wr_ptr += AUDIO_PACKET_SIZE;

    if (haudio->out.wr_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      /* All buffers are full: roll back */
      haudio->out.wr_ptr = 0U;

      ((USBD_AUDIO_ItfTypeDef*) pdev->pUserData)->AudioCmd (&haudio->out.buff[0],
                                                            AUDIO_TOTAL_BUF_SIZE,
                                                            AUDIO_CMD_PLAY);
    }

    /* Prepare Out endpoint to receive next audio packet */
    USBD_LL_PrepareReceive (pdev, AUDIO_OUT_EP, &haudio->out.buff[haudio->out.wr_ptr], AUDIO_PACKET_SIZE);
  }

  return USBD_OK;
}

/**
  * @brief  AUDIO_Req_GetCurrent
  *         Handles the GET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_AUDIO_HandleTypeDef *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  memset (haudio->control.data, 0, 64U);

  /* Send the current mute state */
  USBD_CtlSendData (pdev, haudio->control.data, req->wLength);
}

/**
  * @brief  AUDIO_Req_SetCurrent
  *         Handles the SET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_AUDIO_HandleTypeDef *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (req->wLength)
  {
    /* Prepare the reception of the buffer over EP0 */
    USBD_CtlPrepareRx (pdev, haudio->control.data, req->wLength);

    haudio->control.cmd  = AUDIO_REQ_SET_CUR;      /* Set the request value */
    haudio->control.len  = (uint8_t) req->wLength; /* Set the request data length */
    haudio->control.unit = HIBYTE (req->wIndex);   /* Set the request target unit */
  }
}

/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_AUDIO_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_AUDIO_DeviceQualifierDesc);

  return USBD_AUDIO_DeviceQualifierDesc;
}

/**
  * @brief  USBD_AUDIO_RegisterInterface
  * @param  fops: Audio interface callback
  * @retval status
  */
uint8_t USBD_AUDIO_RegisterInterface(USBD_HandleTypeDef *pdev,
                                     USBD_AUDIO_ItfTypeDef *fops)
{
  if(fops != NULL)
  {
    pdev->pUserData= fops;
  }
  return USBD_OK;
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */
