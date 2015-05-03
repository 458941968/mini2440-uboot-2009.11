//*****************************************************************************
//
// usbdhid.h - Definitions used by HID class devices.
//
// Copyright (c) 2008-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 8049 of the Stellaris USB Library.
//
//*****************************************************************************

#ifndef __USBDHID_H__
#define __USBDHID_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup hid_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the USB HID Device.
//! This does not include the configuration descriptor which is automatically
//! ignored by the composite device class.
//
// For reference this is
// sizeof(g_sHIDInterfaceSection) +  sizeof(g_sHIDDescriptorSection) +
// sizeof(g_sHIDInEndpointSection) + sizeof(g_sHIDOutEndpointSection)
//
//*****************************************************************************
#define COMPOSITE_DHID_SIZE     (32)

//*****************************************************************************
//
// Macros used to create the static Report Descriptors.
//
//*****************************************************************************

//*****************************************************************************
//
//! This is a macro to assist adding Usage Page entries in HID report
//! descriptors.
//!
//! \param ucValue is the Usage Page value.
//!
//! This macro takes a value and prepares it to be placed as a Usage Page entry
//! into a HID report structure.  These are defined by the USB HID
//! specification.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UsagePage(ucValue)      0x05, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Usage entries in HID report descriptors.
//!
//! \param ucValue is the Usage value.
//!
//! This macro takes a value and prepares it to be placed as a Usage entry into
//! a HID report structure.  These are defined by the USB HID specification.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Usage(ucValue)          0x09, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Usage Minimum entries in HID report
//! descriptors.
//!
//! \param ucValue is the Usage Minimum value.
//!
//! This macro takes a value and prepares it to be placed as a Usage Minimum
//! entry into a HID report structure.  This is the first or minimum value
//! associated with a usage value.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UsageMinimum(ucValue)   0x19, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Usage Maximum entries in HID report
//! descriptors.
//!
//! \param ucValue is the Usage Maximum value.
//!
//! This macro takes a value and prepares it to be placed as a Usage Maximum
//! entry into a HID report structure.  This is the last or maximum value
//! associated with a usage value.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UsageMaximum(ucValue)   0x29, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Logical Minimum entries in HID report
//! descriptors.
//!
//! \param cValue is the Logical Minimum value.
//!
//! This macro takes a value and prepares it to be placed as a Logical Minimum
//! entry into a HID report structure.  This is the actual minimum value for a
//! range of values associated with a field.
//!
//! \return Not a function.
//
//*****************************************************************************
#define LogicalMinimum(cValue)  0x15, ((cValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Logical Maximum entries in HID report
//! descriptors.
//!
//! \param cValue is the Logical Maximum value.
//!
//! This macro takes a value and prepares it to be placed as a Logical Maximum
//! entry into a HID report structure.  This is the actual maximum value for a
//! range of values associated with a field.
//!
//! \return Not a function.
//
//*****************************************************************************
#define LogicalMaximum(cValue)  0x25, ((cValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Physical Minimum entries in HID report
//! descriptors.
//!
//! \param sValue is the Physical Minimum value.  It is a signed, 16 bit
//! number.
//!
//! This macro takes a value and prepares it to be placed as a Physical Minimum
//! entry into a HID report structure.  This is value is used in conversion of
//! the control logical value, as returned to the host in the relevant report,
//! to a physical measurement in the appropriate units.
//!
//! \return Not a function.
//
//*****************************************************************************
#define PhysicalMinimum(sValue) 0x36, ((sValue) & 0xFF),                      \
                                (((sValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Physical Maximum entries in HID report
//! descriptors.
//!
//! \param sValue is the Physical Maximum value.  It is a signed, 16 bit
//! number.
//!
//! This macro takes a value and prepares it to be placed as a Physical Maximum
//! entry into a HID report structure.  This is value is used in conversion of
//! the control logical value, as returned to the host in the relevant report,
//! to a physical measurement in the appropriate units.
//!
//! \return Not a function.
//
//*****************************************************************************
#define PhysicalMaximum(sValue) 0x46, ((sValue) & 0xFF),                      \
                                (((sValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Collection entries in HID report
//! descriptors.
//!
//! \param ucValue is the type of Collection.
//!
//! This macro takes a value and prepares it to be placed as a Collection
//! entry into a HID report structure.  This is the type of values that are
//! being grouped together, for instance input, output or features can be
//! grouped together as a collection.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Collection(ucValue)     0xa1, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding End Collection entries in HID report
//! descriptors.
//!
//! This macro can be used to place an End Collection entry into a HID report
//! structure.  This is a tag to indicate that a collection of entries has
//! ended in the HID report structure.  This terminates a previous Collection()
//! entry.
//!
//! \return Not a function.
//
//*****************************************************************************
#define EndCollection           0xc0

//*****************************************************************************
//
//! This is a macro to assist adding Report Count entries in HID report
//! descriptors.
//!
//! \param ucValue is the number of items in a report item.
//!
//! This macro takes a value and prepares it to be placed as a Report Count
//! entry into a HID report structure.  This is number of entries of Report
//! Size for a given item.
//!
//! \return Not a function.
//
//*****************************************************************************
#define ReportCount(ucValue)    0x95, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Report ID entries in HID report
//! descriptors.
//!
//! \param ucValue is the identifier prefix for the current report.
//!
//! This macro takes a value and prepares it to be placed as a Report ID
//! entry into a HID report structure.  This value is used as a 1 byte prefix
//! for the report it is contained within.
//!
//! \return Not a function.
//
//*****************************************************************************
#define ReportID(ucValue)       0x85, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Report Size entries in HID report
//! descriptors.
//!
//! \param ucValue is the size, in bits, of items in a report item.
//!
//! This macro takes a value and prepares it to be placed as a Report Size
//! entry into a HID report structure.  This is size in bits of the entries of
//! of a report entry.  The Report Count specifies how many entries of Report
//! Size are in a given item.  These can be individual bits or bit fields.
//!
//! \return Not a function.
//
//*****************************************************************************
#define ReportSize(ucValue)     0x75, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Input entries in HID report descriptors.
//!
//! \param ucValue is bit mask to specify the type of a set of input report
//! items.  Note that if the USB_HID_INPUT_BITF flag is required, the Input2
//! macro (which uses a 2 byte version of the Input item tag) must be used
//! instead of this macro.
//!
//! This macro takes a value and prepares it to be placed as an Input entry
//! into a HID report structure.  This specifies the type of an input item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of input for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Input(ucValue)          0x81, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Input entries in HID report descriptors.
//!
//! \param usValue is bit mask to specify the type of a set of input report
//! items.  Note that this macro uses a version of the Input item tag with a
//! two byte payload and allows any of the 8 possible data bits for the tag to
//! be used.  If USB_HID_INPUT_BITF (bit 8) is not required, the Input macro
//! may be used instead.
//!
//! This macro takes a value and prepares it to be placed as an Input entry
//! into a HID report structure.  This specifies the type of an input item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of input for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Input2(usValue)         0x82, ((usValue) & 0xff),                     \
                                (((usValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Feature entries in HID report descriptors.
//!
//! \param ucValue is bit mask to specify the type of a set of feature report
//! items.  Note that if the USB_HID_FEATURE_BITF flag is required, the
//! Feature2 macro (which uses a 2 byte version of the Feature item tag) must
//! be used instead of this macro.
//!
//! This macro takes a value and prepares it to be placed as a Feature entry
//! into a HID report structure.  This specifies the type of a feature item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of feature for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Feature(ucValue)        0xB1, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Feature entries in HID report descriptors.
//!
//! \param usValue is bit mask to specify the type of a set of feature report
//! items.  Note that this macro uses a version of the Feature item tag with a
//! two byte payload and allows any of the 8 possible data bits for the tag to
//! be used.  If USB_HID_FEATURE_BITF (bit 8) is not required, the Feature
//! macro may be used instead.
//!
//! This macro takes a value and prepares it to be placed as a Feature entry
//! into a HID report structure.  This specifies the type of a feature item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of feature for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Feature2(usValue)       0xB2, ((usValue) & 0xff),                     \
                                (((usValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Output entries in HID report descriptors.
//!
//! \param ucValue is bit mask to specify the type of a set of output report
//! items.  Note that if the USB_HID_OUTPUT_BITF flag is required, the Output2
//! macro (which uses a 2 byte version of the Output item tag) must be used
//! instead of this macro.
//!
//! This macro takes a value and prepares it to be placed as an Output entry
//! into a HID report structure.  This specifies the type of an output item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of output for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Output(ucValue)         0x91, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Output entries in HID report descriptors.
//!
//! \param usValue is bit mask to specify the type of a set of output report
//! items.  Note that this macro uses a version of the Output item tag with a
//! two byte payload and allows any of the 8 possible data bits for the tag to
//! be used.  If USB_HID_OUTPUT_BITF (bit 8) is not required, the Output macro
//! may be used instead.
//!
//! This macro takes a value and prepares it to be placed as an Output entry
//! into a HID report structure.  This specifies the type of an output item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of output for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Output2(usValue)        0x92, ((usValue) & 0xff),                     \
                                (((usValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Unit Exponent entries in HID report
//! descriptors.
//!
//! \param cValue is the required exponent in the range [-8, 7].
//!
//! This macro takes a value and prepares it to be placed as a Unit Exponent
//! entry into a HID report structure.  This is the exponent applied to
//! PhysicalMinimum and PhysicalMaximum when scaling and converting control
//! values to "real" units.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UnitExponent(cValue)    0x55, ((cValue) & 0x0f)

//*****************************************************************************
//
//! This is a macro to assist adding Unit entries for uncommon units in HID
//! report descriptors.
//!
//! \param ulValue is the definition of the unit required as defined in section
//! 6.2.2.7 of the USB HID device class definition document.
//!
//! This macro takes a value and prepares it to be placed as a Unit entry into
//! a HID report structure.  Note that individual macros are defined for common
//! units and this macro is intended for use when a complex or uncommon unit
//! is needed.  It allows entry of a 5 nibble unit definition into the report
//! descriptor.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Unit(ulValue)           0x67, (ulValue) & 0x0f),                      \
                                (((ulValue) >> 8) & 0xFF),                    \
                                (((ulValue) >> 16) & 0xFF),                   \
                                (((ulValue) >> 24) & 0xFF)

//*****************************************************************************
//
//! This macro inserts a Unit entry for centimeters into a report descriptor.
//!
//*****************************************************************************
#define UnitDistance_cm         0x66, 0x11, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for inches into a report descriptor.
//!
//*****************************************************************************
#define UnitDistance_i          0x66, 0x13, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for degrees into a report descriptor.
//!
//*****************************************************************************
#define UnitRotation_deg        0x66, 0x14, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for radians into a report descriptor.
//!
//*****************************************************************************
#define UnitRotation_rad        0x66, 0x12, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for grams into a report descriptor.
//!
//*****************************************************************************
#define UnitMass_g              0x66, 0x01, 0x01

//*****************************************************************************
//
//! This macro inserts a Unit entry for seconds into a report descriptor.
//!
//*****************************************************************************
#define UnitTime_s              0x66, 0x01, 0x10

//*****************************************************************************
//
//! This macro inserts a Unit entry for temperature in Kelvin into a report
//! descriptor.
//!
//*****************************************************************************
#define UnitTemp_K              0x67, 0x01, 0x00, 0x01, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for temperature in Fahrenheit into a report
//! descriptor.
//!
//*****************************************************************************
#define UnitTemp_F              0x67, 0x03, 0x00, 0x01, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for velocity in cm/s into a report
//! descriptor.
//!
//*****************************************************************************
#define UnitVelocitySI          0x66, 0x11, 0xF0

//*****************************************************************************
//
//! This macro inserts a Unit entry for momentum in (grams * cm)/s into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitMomentumSI          0x66, 0x11, 0xF1

//*****************************************************************************
//
//! This macro inserts a Unit entry for acceleration in cm/s**2 into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitAccelerationSI      0x66, 0x11, 0xE0

//*****************************************************************************
//
//! This macro inserts a Unit entry for force in (cm * grams)/s**2 into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitForceSI             0x66, 0x11, 0xE1

//*****************************************************************************
//
//! This macro inserts a Unit entry for energy in (grams * cm**2)/s**2 into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitEnergySI            0x66, 0x21, 0xE1

//*****************************************************************************
//
//! This macro inserts a Unit entry for angular acceleration in degrees/s**2
//! into a report descriptor.
//!
//*****************************************************************************
#define UnitAngAccelerationSI   0x66, 0x12, 0xE0

//*****************************************************************************
//
//! This macro inserts a Unit entry for voltage into a a report descriptor.
//!
//*****************************************************************************
#define UnitVoltage             0x67, 0x21, 0xD1, 0xF0, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for voltage into a a report descriptor.
//!
//*****************************************************************************
#define UnitCurrent_A           0x67, 0x01, 0x00, 0x10, 0x00


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBDHID_H__
