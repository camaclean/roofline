#ifndef NERSC__OPAE__FPGAMETRICS
#define NERSC__OPAE__FPGAMETRICS

#include <opae/fpga.h>

namespace nersc::opae {

class FpgaProperties
{
public:
    FpgaProperties();

    void setObjectType(fpga_objtype ot);
    void setBus(uint8_t bus);

    fpga_properties* properties();

    ~FpgaProperties();

private:
    fpga_properties props;
};

class Fpga {
public:
    Fpga(int bus = -1);

    float powerUsage();

    ~Fpga();
private:
    FpgaProperties filter;
    int open_flags;
    fpga_token token;
    fpga_handle handle;
};
}
#endif
