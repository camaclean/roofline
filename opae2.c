#include <opae/fpga.h>
#include <opae/types.h>
#include <uuid/uuid.h>

typedef struct fpga_metric {
    uint64_t metric_num;    // Metric index num
    metric_value value;     // Metric value
    bool isvalid;           // Metric value is valid
} fpga_metric;

#define NLB0_AFUID "D8424DC4-A4A3-C413-F89E-433683F9040B"
#define METRICS_MAX_NUM 256

#define ON_ERR_GOTO(res, label, desc)              \
    do {                                           \
	if ((res) != FPGA_OK) {                    \
	    print_err((desc), (res));              \
	    goto label;                            \
	}                                          \
    } while (0)

struct bdf_info {
    uint8_t bus;
};

struct config {
    struct target {
	int bus;
	int open_flags;
    } target;
}

config = {
    .target = {
	.bus = -1,
	.open_flags = 0
    }
};

void print_err(const char *s, fpga_result res)
{
    fprintf(stderr, "Error %s: %s\n", s, fpgaErrStr(res));
}

fpga_result get_bus_info(fpga_token tok, struct bdf_info *finfo){
    fpga_result res = FPGA_OK;
    fpga_properties props;
    res = fpgaGetProperties(tok, &props);
    ON_ERR_GOTO(res, out, "reading properties from Token");

    res = fpgaPropertiesGetBus(props, &finfo->bus);
    ON_ERR_GOTO(res, out_destroy, "Reading bus from properties");

out_destroy:
    res = fpgaDestroyProperties(&props);
    ON_ERR_GOTO(res, out, "fpgaDestroyProps");
out:
    return res;
}

fpga_result find_fpga(fpga_guid interface_id,
	fpga_token *accelerator_token,
	uint32_t *num_matches_accelerators)
{
    fpga_properties filter = NULL;
    fpga_result res;

    /* Get number of FPGAs in system */
    res = fpgaGetProperties(NULL, &filter);
    ON_ERR_GOTO(res, out, "creating properties object");

    res = fpgaPropertiesSetObjectType(filter, FPGA_ACCELERATOR);
    ON_ERR_GOTO(res, out_destroy, "setting object type");

    res = fpgaPropertiesSetGUID(filter, interface_id);
    ON_ERR_GOTO(res, out_destroy, "setting GUID");

    if (-1 != config.target.bus) {
	res = fpgaPropertiesSetBus(filter, config.target.bus);
	ON_ERR_GOTO(res, out_destroy, "setting bus");
    }

    res = fpgaEnumerate(&filter, 1, accelerator_token, 1, num_matches_accelerators);
    ON_ERR_GOTO(res, out_destroy, "enumerating accelerators");

out_destroy:
    res = fpgaDestroyProperties(&filter);
    ON_ERR_GOTO(res, out, "destroying properties object");
out:
    return res;
}

typedef enum metrics_inquiry { FPGA_ALL, FPGA_POWER, FPGA_THERMAL, FPGA_PERF } metrics_inquiry;

fpga_result get_metrics(fpga_token token,
	metrics_inquiry inquiry,
	fpga_metric_info *metrics_info,
	uint64_t *num_metrics_info,
	fpga_metric *metrics,
	uint64_t *num_metrics)
{
    if (!metrics_info || !metrics || !num_metrics || !num_metrics_info) {
	return FPGA_INVALID_PARAM;
    }

    fpga_result res = FPGA_OK;
    fpga_result ret = FPGA_OK;
    fpga_handle handle;

    /* open FPGA */
    res = fpgaOpen(token, &handle, FPGA_OPEN_SHARED);
    ON_ERR_GOTO(res, out_exit, "opening FPGA");

    res = fpgaGetNumMetrics(handle, num_metrics_info);
    ON_ERR_GOTO(res, out_close,
	    "getting number of metrics");

    res = fpgaGetMetricsInfo(handle, metrics_info, num_metrics_info);
    ON_ERR_GOTO(res, out_close,
	    "getting metrics info");

    /* get metrics */
    uint64_t id_array[METRICS_MAX_NUM];
    uint64_t i = 0;
    uint64_t j = 0;
    switch (inquiry) {
	case FPGA_ALL:
	    for (i = 0; i < *num_metrics_info; ++i) {
		id_array[j++] = i;
	    }
	    break;
	case FPGA_POWER:
	    for (i = 0; i < *num_metrics_info; ++i) {
		if (metrics_info[i].metric_type == FPGA_METRIC_TYPE_POWER) {
		    id_array[j++] = i;
		}
	    }
	    break;
	case FPGA_THERMAL:
	    for (i = 0; i < *num_metrics_info; ++i) {
		if (metrics_info[i].metric_type == FPGA_METRIC_TYPE_THERMAL) {
		    id_array[j++] = i;
		}
	    }
	    break;
	case FPGA_PERF:
	    for (i = 0; i < *num_metrics_info; ++i) {
		if (metrics_info[i].metric_type == FPGA_METRIC_TYPE_PERFORMANCE_CTR) {
		    id_array[j++] = i;
		}
	    }
	    break;
    }

    *num_metrics = j;

    if (*num_metrics == 0) {
	goto out_close;
    }

    res = fpgaGetMetricsByIndex(handle, id_array, *num_metrics, metrics);
    ON_ERR_GOTO(res, out_close, "getting metrics");

out_close:
    /* close FPGA */
    ret = (res != FPGA_OK) ? res : ret;
    res = fpgaClose(handle);
    ON_ERR_GOTO(res, out_exit, "closing FPGA");

out_exit:
    ret = (res != FPGA_OK) ? res : ret;
    return ret;
}

void print_metrics(const fpga_metric_info *metrics_info,
	uint64_t num_metrics_info,
	const fpga_metric *metrics, uint64_t num_metrics)
{
    uint64_t i = 0;
    for (i = 0; i < num_metrics; ++i) {
	uint64_t idx = metrics[i].metric_num;

	if (metrics[i].isvalid) {

	    if (idx < num_metrics_info) {
		printf("(%2ld) %-27s : ", i + 1, metrics_info[idx].metric_name);

		switch (metrics_info[idx].metric_datatype) {
		    case FPGA_METRIC_DATATYPE_INT:
			printf("%" PRId64 "", metrics[i].value.ivalue);
			break;
		    case FPGA_METRIC_DATATYPE_DOUBLE: /* FALLTHROUGH */
		    case FPGA_METRIC_DATATYPE_FLOAT:
			printf("%0.2f", metrics[i].value.dvalue);
			break;
		    case FPGA_METRIC_DATATYPE_BOOL:
			printf("%d", metrics[i].value.bvalue);
			break;
		    default:
			OPAE_ERR("Metrics Invalid datatype");
			break;
		}

		printf(" %s\n", metrics_info[idx].metric_units);
	    }
	} else {
	    // Failed to read metric value
	    fprintf(stdout, "(%2ld) %-27s : %s\n", i + 1, metrics_info[idx].metric_name, "N/A");
	}

    }
}

static void print_power_info(fpga_token token)
{
    fpga_properties props;
    fpga_metric_info metrics_info[METRICS_MAX_NUM] = { { 0 } };
    fpga_metric metrics[METRICS_MAX_NUM] = { { 0 } };
    uint64_t num_metrics;
    uint64_t num_metrics_info;
    fpga_result res = FPGA_OK;

    res = fpgaGetProperties(token, &props);
    ON_ERR_GOTO(res, out_destroy, "reading properties from token");

    //fpgainfo_board_info(token);
    //fpgainfo_print_common("//****** POWER ******//", props);

    res = get_metrics(token, FPGA_POWER, metrics_info, &num_metrics_info, metrics, &num_metrics);
    ON_ERR_GOTO(res, out_destroy, "reading metrics from BMC");

    print_metrics(metrics_info, num_metrics_info, metrics, num_metrics);

out_destroy:
    res = fpgaDestroyProperties(&props);
    ON_ERR_GOTO(res, out_exit, "destroying properties");

out_exit:
    return;
}

int main(int argc, char *argv[])
{
    fpga_token         accelerator_token;
    fpga_handle        accelerator_handle;
    uint32_t           num_matches_accelerators = 0;
    fpga_guid          guid;
    struct bdf_info info;
    fpga_result res = FPGA_OK;


    if (uuid_parse(NLB0_AFUID, guid) < 0) {
	res = FPGA_EXCEPTION;
    }
    ON_ERR_GOTO(res, out_exit, "parsing guid");

    /* Look for accelerator with NLB0_AFUID */
    res = find_fpga(guid, &accelerator_token, &num_matches_accelerators);
    ON_ERR_GOTO(res, out_exit, "finding FPGA accelerator");

    if (num_matches_accelerators <= 0) {
	res = FPGA_NOT_FOUND;
    }
    ON_ERR_GOTO(res, out_exit, "no matching accelerator");

    if (num_matches_accelerators > 1) {
	fprintf(stderr, "Found more than one suitable accelerator. ");
	res = get_bus_info(accelerator_token, &info);
	ON_ERR_GOTO(res, out_exit, "getting bus num");
	printf("Running on bus 0x%02X. \n", info.bus);
    }
    /* Open accelerator and map MMIO */
    res = fpgaOpen(accelerator_token, &accelerator_handle, config.target.open_flags);
    ON_ERR_GOTO(res, out_destroy_tok, "opening accelerator");

    print_power_info(accelerator_token);

out_destroy_tok:
    res = fpgaDestroyToken(&accelerator_token);
    ON_ERR_GOTO(res, out_exit, "destroying token");
out_exit:
    return res;
}
