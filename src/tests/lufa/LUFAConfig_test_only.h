/** 
 * @file
 * 
 * @brief Should only be used/included i the test/sample programs.
 * 
 * Should only be used/included i the test/sample programs.
 * If you create another/new program you must create your own config
 * 
 */

#include "USB.h"

/** Endpoint address of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 2)

/** Endpoint address of the CDC device-to-host data IN endpoint. */
#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN  | 3)

/** Endpoint address of the CDC host-to-device data OUT endpoint. */
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 4)

/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPSIZE        8

/** Size in bytes of the CDC data IN and OUT endpoints. */
#define CDC_TXRX_EPSIZE                16


typedef struct {
  USB_Descriptor_Configuration_Header_t    Config;
  
  // CDC Control Interface
  USB_Descriptor_Interface_t               CDC_CCI_Interface;
  USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
  USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
  USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
  USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;
  
  // CDC Data Interface
  USB_Descriptor_Interface_t               CDC_DCI_Interface;
  USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
  USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} USB_Descriptor_Configuration_t;


uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
				    const uint8_t wIndex,
				    const void** const DescriptorAddress)
  ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

///The serial interface handle for the test program
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface_;

///Connection status for the test serial port
char connected_;
