
roofline_1_power_gb.png: plot_power_gbps.py ert_plot.py
	./plot_power_gbps.py

roofline_1_power_gf.png: plot_power_gflops.py ert_plot.py
	./plot_power_gflops.py

roofline_1_power.png: plot_power_roofline_fp.py ert_plot.py
	./plot_power_roofline_fp.py

roofline_power_types.png: plot_power_roofline_types.py ert_plot.py
	./plot_power_roofline_types.py

roofline_power_types_combined.png: plot_power_roofline_types_combined.py ert_plot.py
	./plot_power_roofline_types_combined.py

roofline_power_types_unroll.png: plot_power_roofline_types_optimal_unroll.py ert_plot.py
	./plot_power_roofline_types_optimal_unroll.py

roofline_resources_power_fp.png: plot_resources_power_fp.py ert_plot.py
	./plot_resources_power_fp.py

roofline_resources_power_types.png: plot_resources_power_types.py ert_plot.py
	./plot_resources_power_types.py

roofline_resources_power_types_combined.png: plot_resources_power_types_combined.py ert_plot.py
	./plot_resources_power_types_combined.py

roofline_resources_power_types_optimal_unroll.png: plot_resources_power_types_optimal_unroll.py ert_plot.py
	./plot_resources_power_types_optimal_unroll.py

roofline_1.png: plot_roofline_fp.py ert_plot.py
	./plot_roofline_fp.py

roofline_types.png: plot_roofline_types.py ert_plot.py
	./plot_roofline_types.py

roofline_types_combined.png: plot_roofline_types_combined.py ert_plot.py
	./plot_roofline_types_combined.py

roofline_types_unroll.png: plot_roofline_types_optimal_unroll.py ert_plot.py
	./plot_roofline_types_optimal_unroll.py

fp_bandwidth.png: plot_bandwidth_fp.py ert_plot.py
	./plot_bandwidth_fp.py

roofline_power_types_ai.png: plot_power_roofline_types_ai.py ert_plot.py
	./plot_power_roofline_types_ai.py

all: roofline_1_power_gb.png roofline_1_power_gf.png roofline_1_power.png roofline_power_types.png roofline_power_types_unroll.png roofline_resources_power_fp.png roofline_resources_power_types.png roofline_resources_power_types_optimal_unroll.png roofline_1.png roofline_types.png roofline_types_unroll.png fp_bandwidth.png roofline_types_combined.png roofline_power_types_combined.png roofline_resources_power_types_combined.png roofline_power_types_ai.png

.PHONY: clean

clean:
	rm -f *.png

.DEFAULT_GOAL := all
