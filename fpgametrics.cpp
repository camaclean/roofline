// Copyright(c) 2018, Intel Corporation
//
// Redistribution  and  use  in source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of  source code  must retain the  above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name  of Intel Corporation  nor the names of its contributors
//   may be used to  endorse or promote  products derived  from this  software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
// IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
// LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
// CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
// SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
// INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
// CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/**
 * @file fpgametrics.c
 * @brief A code sample illustrates the basic usage metric API.
 *
 *
 */

#include "fpgametrics.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <stdexcept>

namespace nersc::opae {

FpgaProperties::FpgaProperties()
{
    fpga_result res = FPGA_OK;
    res = fpgaGetProperties(NULL, &props);
    if (res != FPGA_OK)
        throw std::runtime_error("creating properties object");
}

void FpgaProperties::setObjectType(fpga_objtype ot)
{
    fpga_result res = FPGA_OK;

    res = fpgaPropertiesSetObjectType(props, ot);
    if (res != FPGA_OK)
        throw std::runtime_error("setting object type");
}

void FpgaProperties::setBus(uint8_t bus)
{
    fpga_result res = FPGA_OK;

	res = fpgaPropertiesSetBus(props, bus);
    if (res != FPGA_OK)
        throw std::runtime_error("setting bus");
}

fpga_properties* FpgaProperties::properties()
{
    return &props;
}

FpgaProperties::~FpgaProperties()
{
	fpgaDestroyProperties(&props);
}

Fpga::Fpga(int bus)
    : filter{}, open_flags{0}
{
    open_flags |= FPGA_OPEN_SHARED;

    fpga_result res = FPGA_OK;
    uint32_t num_matches_fpgas = 0;

    filter.setObjectType(FPGA_DEVICE);

    if (bus != -1)
        filter.setBus(bus);

	res = fpgaEnumerate(filter.properties(), 1, &token, 1, &num_matches_fpgas);
    if (num_matches_fpgas <= 0) {
        res = FPGA_NOT_FOUND;
	}
    if (res != FPGA_OK)
        throw std::runtime_error("no matching fpga");

    if (num_matches_fpgas > 1) {
        fpgaDestroyToken(&token);
        throw std::runtime_error(std::string("Found more than one suitable fpga: ") + std::to_string(num_matches_fpgas));
    }

	res = fpgaOpen(token, &handle, open_flags);
    if (res != FPGA_OK)
    {
        fpgaDestroyToken(&token);
        throw std::runtime_error("opening fpga");
    }
}

float Fpga::powerUsage()
{
    fpga_result res = FPGA_OK;
    uint64_t power_metric_num = 8;
	struct fpga_metric power_metric;

	res = fpgaGetMetricsByIndex(handle, &power_metric_num, 1, &power_metric);

    if (res != FPGA_OK)
        throw std::runtime_error("getting power");

    return power_metric.value.dvalue;
}

Fpga::~Fpga()
{
    fpga_result res = FPGA_OK;

	res = fpgaClose(handle);
    res = fpgaDestroyToken(&token);
}

}

/*
int main(int argc, char *argv[])
{
    nersc::opae::Fpga fpga{};

    printf("Power Usage: %f Watts\n", fpga.powerUsage());

    return 0;
}
*/

// vim: tabstop=4 shiftwidth=4 softtabstop=4 expandtab :
