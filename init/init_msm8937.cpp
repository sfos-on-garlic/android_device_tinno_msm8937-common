/*
   Copyright (C) 2007, The Android Open Source Project
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (c) 2017, The LineageOS Project

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

#define FP_DEV_FLE "/sys/devices/platform/fp_drv/fp_drv_info"

using android::base::Trim;
using android::base::ReadFileToString;

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info *) __system_property_find(prop);

    if (pi != nullptr) {
         __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void init_fingerprint_properties()
{
    std::string fp_dev;

    if (ReadFileToString(FP_DEV_FLE, &fp_dev)) {
        LOG(INFO) << "Loading Fingerprint HAL for sensor version " << fp_dev;
        if (!strncmp(fp_dev.c_str(), "silead_fp", 9)) {
            property_override("ro.hardware.fingerprint", "silead");
            property_override("persist.sys.fp.goodix", "0");
        } else if (!strncmp(fp_dev.c_str(), "goodix_fp", 9)) {
            property_override("ro.hardware.fingerprint", "goodix");
            property_override("persist.sys.fp.goodix", "1");
        } else if (!strncmp(fp_dev.c_str(), "elan_fp", 7)) {
            property_override("ro.hardware.fingerprint", "elan");
            property_override("persist.sys.fp.goodix", "0");
        } else if (!strncmp(fp_dev.c_str(), "chipone_fp", 10)) {
            property_override("ro.hardware.fingerprint", "chipone");
            property_override("persist.sys.fp.goodix", "0");
        } else {
            LOG(ERROR) << "Unsupported fingerprint sensor: " << fp_dev;
            property_override("ro.hardware.fingerprint", "none");
            property_override("persist.sys.fp.goodix", "0");
        }
    }
    else {
        LOG(ERROR) << "Failed to detect sensor version";
        property_override("ro.hardware.fingerprint", "none");
        property_override("persist.sys.fp.goodix", "0");
    }
}

void vendor_load_properties()
{
	LOG(INFO) << "Loading vendor specific properties";
    init_fingerprint_properties();

    // Misc
    property_override("ro.apex.updatable", "false");
}
