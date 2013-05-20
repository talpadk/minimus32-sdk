#include "LUFAConfig_test_only.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface_ = {
  .Config =
  {
    .ControlInterfaceNumber   = 0,
    .DataINEndpoint           =
    {
      .Address          = CDC_TX_EPADDR,
      .Size             = CDC_TXRX_EPSIZE,
      .Banks            = 1,
    },
    .DataOUTEndpoint =
    {
      .Address          = CDC_RX_EPADDR,
      .Size             = CDC_TXRX_EPSIZE,
      .Banks            = 1,
					},
    .NotificationEndpoint =
    {
      .Address          = CDC_NOTIFICATION_EPADDR,
      .Size             = CDC_NOTIFICATION_EPSIZE,
      .Banks            = 1,
    },
  },
};

char connected_=0;


/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void) {
  connected_ = 1;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void){
  connected_ = 0;
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void){
  bool ConfigSuccess = true;
  ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface_);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void){
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface_);
}

//Misc Boilerplate stuff
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
  .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
  
  .USBSpecification       = VERSION_BCD(01.10),
  .Class                  = CDC_CSCP_CDCClass,
  .SubClass               = CDC_CSCP_NoSpecificSubclass,
  .Protocol               = CDC_CSCP_NoSpecificProtocol,
  
  .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
  
  .VendorID               = 0x03EB,
  .ProductID              = 0x2044,
  .ReleaseNumber          = VERSION_BCD(00.01),
  
  .ManufacturerStrIndex   = 0x01,
  .ProductStrIndex        = 0x02,
  .SerialNumStrIndex      = USE_INTERNAL_SERIAL,
  
  .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
  .Config = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},
    
    .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
    .TotalInterfaces        = 2,
    
    .ConfigurationNumber    = 1,
    .ConfigurationStrIndex  = NO_DESCRIPTOR,
    
    .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
    
    .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
  },
  
  .CDC_CCI_Interface = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
    
    .InterfaceNumber        = 0,
    .AlternateSetting       = 0,
    
    .TotalEndpoints         = 1,
    
    .Class                  = CDC_CSCP_CDCClass,
    .SubClass               = CDC_CSCP_ACMSubclass,
    .Protocol               = CDC_CSCP_ATCommandProtocol,
    
    .InterfaceStrIndex      = NO_DESCRIPTOR
  },
  
  .CDC_Functional_Header = {
    .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
    .Subtype                = CDC_DSUBTYPE_CSInterface_Header,
    
    .CDCSpecification       = VERSION_BCD(01.10),
  },
  
  .CDC_Functional_ACM = {
    .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
    .Subtype                = CDC_DSUBTYPE_CSInterface_ACM,
    
    .Capabilities           = 0x06,
  },
  
  .CDC_Functional_Union = {
    .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
    .Subtype                = CDC_DSUBTYPE_CSInterface_Union,
    
    .MasterInterfaceNumber  = 0,
    .SlaveInterfaceNumber   = 1,
  },
  
  .CDC_NotificationEndpoint = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
    
    .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
    .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
    .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
    .PollingIntervalMS      = 0xFF
  },
  
  .CDC_DCI_Interface = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
    
    .InterfaceNumber        = 1,
    .AlternateSetting       = 0,
    
    .TotalEndpoints         = 2,
    
    .Class                  = CDC_CSCP_CDCDataClass,
    .SubClass               = CDC_CSCP_NoDataSubclass,
    .Protocol               = CDC_CSCP_NoDataProtocol,
    
    .InterfaceStrIndex      = NO_DESCRIPTOR
  },

  .CDC_DataOutEndpoint = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
    
    .EndpointAddress        = CDC_RX_EPADDR,
    .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
    .EndpointSize           = CDC_TXRX_EPSIZE,
    .PollingIntervalMS      = 0x05
  },
  
  .CDC_DataInEndpoint = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
    
    .EndpointAddress        = CDC_TX_EPADDR,
    .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
    .EndpointSize           = CDC_TXRX_EPSIZE,
    .PollingIntervalMS      = 0x05
  }
};

const USB_Descriptor_String_t PROGMEM LanguageString = {
  .Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},
  .UnicodeString          = {LANGUAGE_ID_ENG}
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
  .Header                 = {.Size = USB_STRING_LEN(21), .Type = DTYPE_String},
  .UnicodeString          = L"The Minimus32 Toolkit"
};

const USB_Descriptor_String_t PROGMEM ProductString = {
  .Header                 = {.Size = USB_STRING_LEN(20), .Type = DTYPE_String},
  .UnicodeString          = L"LUFA CDC Serial Demo"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress) {
  const uint8_t  DescriptorType   = (wValue >> 8);
  const uint8_t  DescriptorNumber = (wValue & 0xFF);
  
  const void* Address = NULL;
  uint16_t    Size    = NO_DESCRIPTOR;
  
  switch (DescriptorType)
    {
    case DTYPE_Device:
      Address = &DeviceDescriptor;
      Size    = sizeof(USB_Descriptor_Device_t);
      break;
    case DTYPE_Configuration:
      Address = &ConfigurationDescriptor;
      Size    = sizeof(USB_Descriptor_Configuration_t);
      break;
    case DTYPE_String:
      switch (DescriptorNumber)
	{
	case 0x00:
	  Address = &LanguageString;
	  Size    = pgm_read_byte(&LanguageString.Header.Size);
	  break;
	case 0x01:
	  Address = &ManufacturerString;
	  Size    = pgm_read_byte(&ManufacturerString.Header.Size);
	  break;
	case 0x02:
	  Address = &ProductString;
	  Size    = pgm_read_byte(&ProductString.Header.Size);
	  break;
	}
      
      break;
    }
  
  *DescriptorAddress = Address;
  return Size;
}

