#include <opae/fpga.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    fpga_properties    filter = NULL;
    fpga_token         afc_token;
    fpga_handle        afc_handle;
    fpga_guid          guid;
    uint32_t           num_matches;

    volatile uint64_t *dsm_ptr    = NULL;
    volatile uint64_t *status_ptr = NULL;
    volatile uint64_t *input_ptr  = NULL;
    volatile uint64_t *output_ptr = NULL;

    uint64_t        dsm_wsid;
    uint64_t        input_wsid;
    uint64_t        output_wsid;
    fpga_result     res = FPGA_OK;

    if (uuid_parse(NLB0_AFUID, guid) < 0) {
        fprintf(stderr, "Error parsing guid '%s'\n", NLB0_AFUID);
        goto out_exit;
    }

    /* Look for AFC with MY_AFC_ID */
    res = fpgaGetProperties(NULL, &filter);
    ON_ERR_GOTO(res, out_exit, "creating properties object");

    res = fpgaPropertiesSetObjectType(filter, FPGA_AFC);
    ON_ERR_GOTO(res, out_destroy_prop, "setting object type");

    res = fpgaPropertiesSetGuid(filter, guid);
    ON_ERR_GOTO(res, out_destroy_prop, "setting GUID");

    /* TODO: Add selection via BDF / device ID */

    res = fpgaEnumerate(&filter, 1, &afc_token, 1, &num_matches);
    ON_ERR_GOTO(res, out_destroy_prop, "enumerating AFCs");

    if (num_matches < 1) {
        fprintf(stderr, "AFC not found.\n");
        res = fpgaDestroyProperties(&filter);
        return FPGA_INVALID_PARAM;
    }
out_destroy_prop:
    res = fpgaDestroyProperties(&filter);
out_exit:
    return res;
}
