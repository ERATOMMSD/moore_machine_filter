all: rel2abs_truncate abs2rel convMonaa

LOG_DIRS = $(wildcard log/*/)
ACCEL = $(LOG_DIRS:/=/filt-all-accel.tsv)
GEAR = $(LOG_DIRS:/=/filt-all-gear.tsv)
TORQUE = $(LOG_DIRS:/=/filt-all-torque.tsv)

log/%/logs: log/%/filt-all-accel.tsv log/%/filt-all-gear.tsv log/%/filt-all-torque.tsv

logs: $(ACCEL) $(GEAR) $(TORQUE)

rel2abs_truncate: rel2abs_truncate.c
	$(CC) -O3 $< -o $@
abs2rel: abs2rel.c
	$(CC) -O3 $< -o $@ 
convMonaa: convMonaa.cc
	$(CXX) --std=c++14 -O3 $< -o $@ 

log/%/filt-time-length-accel.png: log/%/filt-all-accel.tsv ./plot_filt_time-length_0-10.sh
	./plot_filt_time-length_0-10.sh $< $@
log/%/filt-time-length-gear.png: log/%/filt-all-gear.tsv ./plot_filt_time-length_0-10.sh
	./plot_filt_time-length_0-10.sh $< $@
log/%/filt-time-length-torque.png: log/%/filt-all-torque.tsv ./plot_filt_time-length_0-10.sh
	./plot_filt_time-length_0-10.sh $< $@
log/%/filt-time-length-accel.tikz.tex: log/%/filt-all-accel.tsv ./plot_filt_time-length_0-10.sh
	./plot_filt_time-length_0-10.sh $< $@ tikz
log/%/filt-time-length-gear.tikz.tex: log/%/filt-all-gear.tsv ./plot_filt_time-length_0-10.sh
	./plot_filt_time-length_0-10.sh $< $@ tikz
log/%/filt-time-length-torque.tikz.tex: log/%/filt-all-torque.tsv ./plot_filt_time-length_0-10.sh
	./plot_filt_time-length_0-10.sh $< $@ tikz

log/%/filt-vs-monaa-accel.png: log/%/filt-all-accel.tsv ./plot_filt-vs-monaa-0-10.sh
	./plot_filt-vs-monaa-0-10.sh $< $@
log/%/filt-vs-monaa-gear.png: log/%/filt-all-gear.tsv ./plot_filt-vs-monaa-0-10.sh
	./plot_filt-vs-monaa-0-10.sh $< $@
log/%/filt-vs-monaa-torque.png: log/%/filt-all-torque.tsv ./plot_filt-vs-monaa-0-10.sh
	./plot_filt-vs-monaa-0-10.sh $< $@
log/%/filt-vs-monaa-accel.tikz.tex: log/%/filt-all-accel.tsv ./plot_filt-vs-monaa-0-10.sh
	./plot_filt-vs-monaa-0-10.sh $< $@ tikz
log/%/filt-vs-monaa-gear.tikz.tex: log/%/filt-all-gear.tsv ./plot_filt-vs-monaa-0-10.sh
	./plot_filt-vs-monaa-0-10.sh $< $@ tikz
log/%/filt-vs-monaa-torque.tikz.tex: log/%/filt-all-torque.tsv ./plot_filt-vs-monaa-0-10.sh
	./plot_filt-vs-monaa-0-10.sh $< $@ tikz

log/%/filt-RAM-length-accel.png: log/%/filt-all-accel.tsv ./plot_filt_RAM-length.sh
	./plot_filt_RAM-length.sh $< $@
log/%/filt-RAM-length-gear.png: log/%/filt-all-gear.tsv ./plot_filt_RAM-length.sh
	./plot_filt_RAM-length.sh $< $@
log/%/filt-RAM-length-torque.png: log/%/filt-all-torque.tsv ./plot_filt_RAM-length.sh
	./plot_filt_RAM-length.sh $< $@
log/%/filt-RAM-length-accel.tikz.tex: log/%/filt-all-accel.tsv ./plot_filt_RAM-length.sh
	./plot_filt_RAM-length.sh $< $@ tikz
log/%/filt-RAM-length-gear.tikz.tex: log/%/filt-all-gear.tsv ./plot_filt_RAM-length.sh
	./plot_filt_RAM-length.sh $< $@ tikz
log/%/filt-RAM-length-torque.tikz.tex: log/%/filt-all-torque.tsv ./plot_filt_RAM-length.sh
	./plot_filt_RAM-length.sh $< $@ tikz

log/%/filt-rate-N-accel.png: log/%/filt-all-accel.tsv ./plot_filt_rate-N.sh
	./plot_filt_rate-N.sh $< $@
log/%/filt-rate-N-gear.png: log/%/filt-all-gear.tsv ./plot_filt_rate-N.sh
	./plot_filt_rate-N.sh $< $@
log/%/filt-rate-N-torque.png: log/%/filt-all-torque.tsv ./plot_filt_rate-N.sh
	./plot_filt_rate-N.sh $< $@
log/%/filt-rate-N-accel.tikz.tex: log/%/filt-all-accel.tsv ./plot_filt_rate-N.sh
	./plot_filt_rate-N.sh $< $@ tikz
log/%/filt-rate-N-gear.tikz.tex: log/%/filt-all-gear.tsv ./plot_filt_rate-N.sh
	./plot_filt_rate-N.sh $< $@ tikz
log/%/filt-rate-N-torque.tikz.tex: log/%/filt-all-torque.tsv ./plot_filt_rate-N.sh
	./plot_filt_rate-N.sh $< $@ tikz

log/%/filt-rate-length-accel.png: log/%/filt-all-accel.tsv ./plot_filt_rate-length.sh
	./plot_filt_rate-length.sh $< $@
log/%/filt-rate-length-gear.png: log/%/filt-all-gear.tsv ./plot_filt_rate-length.sh
	./plot_filt_rate-length.sh $< $@
log/%/filt-rate-length-torque.png: log/%/filt-all-torque.tsv ./plot_filt_rate-length.sh
	./plot_filt_rate-length.sh $< $@
log/%/filt-rate-length-accel.tikz.tex: log/%/filt-all-accel.tsv ./plot_filt_rate-length.sh
	./plot_filt_rate-length.sh $< $@ tikz
log/%/filt-rate-length-gear.tikz.tex: log/%/filt-all-gear.tsv ./plot_filt_rate-length.sh
	./plot_filt_rate-length.sh $< $@ tikz
log/%/filt-rate-length-torque.tikz.tex: log/%/filt-all-torque.tsv ./plot_filt_rate-length.sh
	./plot_filt_rate-length.sh $< $@ tikz

log/%/filt-time-monaa-length-accel.png: log/%/filt-all-accel.tsv ./plot_monaa_filt_time_0-10.sh
	./plot_monaa_filt_time_0-10.sh $< $@
log/%/filt-time-monaa-length-gear.png: log/%/filt-all-gear.tsv ./plot_monaa_filt_time_0-10.sh
	./plot_monaa_filt_time_0-10.sh $< $@
log/%/filt-time-monaa-length-torque.png: log/%/filt-all-torque.tsv ./plot_monaa_filt_time_0-10.sh
	./plot_monaa_filt_time_0-10.sh $< $@
log/%/filt-time-monaa-length-accel.tikz.tex: log/%/filt-all-accel.tsv ./plot_monaa_filt_time_0-10.sh
	./plot_monaa_filt_time_0-10.sh $< $@ tikz
log/%/filt-time-monaa-length-gear.tikz.tex: log/%/filt-all-gear.tsv ./plot_monaa_filt_time_0-10.sh
	./plot_monaa_filt_time_0-10.sh $< $@ tikz
log/%/filt-time-monaa-length-torque.tikz.tex: log/%/filt-all-torque.tsv ./plot_monaa_filt_time_0-10.sh
	./plot_monaa_filt_time_0-10.sh $< $@ tikz

log/%/filt-all-accel.tsv: accel.wc log/%/filt-time-accel.tsv log/%/filt-monaa-time-accel.tsv log/%/filt-wc-accel.tsv log/%/no-filt-monaa-time-accel.tsv 
	paste $^ | awk '{print $$1,$$3,$$4,$$5,$$8,$$9,$$12,$$14}' | awk '$$2<=10' > $@
log/%/filt-all-gear.tsv: gear.wc log/%/filt-time-gear.tsv log/%/filt-monaa-time-gear.tsv log/%/filt-wc-gear.tsv log/%/no-filt-monaa-time-gear.tsv 
	paste $^ | awk '{print $$1,$$3,$$4,$$5,$$8,$$9,$$12,$$14}' | awk '$$2<=10' > $@
log/%/filt-all-torque.tsv: torque.wc log/%/filt-time-torque.tsv log/%/filt-monaa-time-torque.tsv log/%/filt-wc-torque.tsv log/%/no-filt-monaa-time-torque.tsv 
	paste $^ | awk '{print $$1,$$3,$$4,$$5,$$8,$$9,$$12,$$14}' | awk '$$2<=10' > $@
log/%/filt-time-accel.tsv: log/%/filt-time-accel-*-Experiment3_AT-*.tsv*.log make_filt-time.sh
	./make_filt-time.sh $@ filt-time-accel-
log/%/filt-monaa-time-accel.tsv: log/%/filt-monaa-time-accel-*-Experiment3_AT-*.tsv*.log  make_filt-time.sh
	./make_filt-time.sh $@ filt-monaa-time-accel-
log/%/no-filt-monaa-time-accel.tsv: log/%/no-filt-monaa-time-accel-*-Experiment3_AT-*.tsv*.log  make_no-filt-time.sh
	./make_no-filt-time.sh $@ no-filt-monaa-time-accel-
log/%/filt-time-gear.tsv: log/%/filt-time-gear-*-Experiment3_AT-*-gear-signal.dat*.log make_filt-time.sh
	./make_filt-time.sh $@ filt-time-gear-
log/%/no-filt-monaa-time-gear.tsv: log/%/no-filt-monaa-time-gear-*-Experiment3_AT-*-gear-signal.dat*.log make_no-filt-time.sh
	./make_no-filt-time.sh $@ no-filt-monaa-time-gear-
log/%/filt-monaa-time-gear.tsv: log/%/filt-monaa-time-gear-*-Experiment3_AT-*-gear-signal.dat*.log make_filt-time.sh
	./make_filt-time.sh $@ filt-monaa-time-gear-
log/%/filt-time-torque.tsv: log/%/filt-time-torque-*-torque-*.dat*.log make_filt-torque-time.sh
	./make_filt-torque-time.sh $@ filt-time-torque-
log/%/filt-monaa-time-torque.tsv: log/%/filt-monaa-time-torque-*-torque-*.dat*.log make_filt-time.sh
	./make_filt-torque-time.sh $@ filt-monaa-time-torque-
log/%/no-filt-monaa-time-torque.tsv: log/%/no-filt-monaa-time-torque-*-torque-*.dat*.log make_no-filt-time.sh
	./make_no-filt-time.sh $@ no-filt-monaa-time-torque-
log/%/filt-wc-accel.tsv: log/%/filt-accel-*-Experiment3_AT-*.tsv.txt make_filt-wc.sh
	./make_filt-wc.sh $@ filt-accel-
log/%/filt-wc-gear.tsv: log/%/filt-gear-*-Experiment3_AT-*-gear-signal.dat.txt make_filt-wc.sh
	./make_filt-wc.sh $@ filt-gear-
log/%/filt-wc-torque.tsv: log/%/filt-torque-*-torque-*.dat.txt make_filt-torque-wc.sh
	./make_filt-torque-wc.sh $@ filt-torque-
torque.wc: ~/Data/timedPatternMatching/torque-*.dat
	for input in ~/Data/timedPatternMatching/torque-*.dat; do cat $$input | sed 1d | wc -l; done | sort -n > /tmp/torque.wc1
	for a in $$(seq 6); do cat /tmp/torque.wc1; done > $@
gear.wc: ~/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat
	for input in ~/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat; do cat $$input | sed 1d | wc -l ;done | sort -n > /tmp/gear.wc1
	for a in $$(seq 6); do cat /tmp/gear.wc1; done  > $@
accel.wc: ~/Data/timedPatternMatching/Experiment3_AT-*.tsv
	for input in ~/Data/timedPatternMatching/Experiment3_AT-*.tsv; do ./convMonaa.sh < $$input | wc -l ;done | sort -n > /tmp/accel.wc1
	for a in $$(seq 6); do cat /tmp/accel.wc1; done  > $@

