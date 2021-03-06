#pragma once

#include "model.hpp"

namespace IOSKJ {

/**
 * Performance statistics used for evaluating management procedures
 */
class Performance : public Structure<Performance> {
public:

	Performance(int replicate,int procedure):
		replicate(replicate),
		procedure(procedure)
		{}

	/**
	 * Replicate evaluation which this performance 
	 * relates to
	 */
	int replicate;

	/**
	 * Candidate procedure which this performance 
	 * relates to
	 */
	int procedure;

	/**
	 * Number of time steps where performance measures
	 * were recorded. Mainly used for testing.
	 */
	Count times;

	/**
	 * Mean of total catch
	 */
	Mean catches_total;

	/**
	 * Mean of PS catch
	 */
	Mean catches_ps;

	/**
	 * Mean of PL catch
	 */
	Mean catches_pl;

	/**
	 * Mean of GN catch
	 */
	Mean catches_gn;

	/**
	 * Variance of catches
	 */
	Variance catches_var;

	/**
	 * Mean absolute percentage changes in catches
	 */
	Mapc catches_mapc;

	/**
	 * Proportion of times where
	 * catch is lower than baseline
	 */
	Mean catches_lower;

	/**
	 * Proportion of times where
	 * catch is zero
	 */
	Mean catches_shut;

	/**
	 * Proportion of times where there is an increase/decrease 
	 * in the management control e..g catch limit
	 */
	Mean control_ups;
	Mean control_downs;

	/**
	 * Mean of stock status % B0
	 */
	GeometricMean status_mean;

	/**
	 * Probability of stock being below 10% BO
	 */
	Mean status_b10;

	/**
	 * Probability of stock being below 20% BO 
	 */
	Mean status_b20;

	/**
	 * Mean ratio of F/Fmsy
	 */
	GeometricMean f_ratio;

	/**
	 * Mean ratio of B/Bmsy
	 */
	GeometricMean b_ratio;

	/**
	 * Proportion of time spent in each Kobe plot quadrant:
	 * 
	 * 	A (green)  : B>Bmsy F<Fmsy
	 * 	B (yellow) : B>Bmsy F>Fmsy
	 * 	C (yellow) : B<Bmsy F<Fmsy
	 * 	D (red)    : B<Bmsy F>Fmsy
	 */
	Mean kobe_a;
	Mean kobe_b;
	Mean kobe_c;
	Mean kobe_d;

	/**
	 * Number of years taken to move from Kobe plot 
	 * quadrants B,C or D back into A
	 */
	Mean kobe_to_a;
		int kobe_out_a;

	/**
	 * Baseline CPUE for regions/gears used to calculate relative
	 * catch rates
	 */
	Array<double,Region,Method> cpue_baseline;

	/**
	 * Mean CPUE
	 *
	 * Only the three main region/method combinations are
	 * output (see `reflect()` method)
	 */
	Array<GeometricMean,Region,Method> cpue_mean;

	/**
	 * Reflection
	 */
	template<class Mirror>
	void reflect(Mirror& mirror){
		mirror
			.data(replicate,"replicate")
			.data(procedure,"procedure")
			.data(times,"times")
			.data(catches_total,"catches_total")
			.data(catches_ps,"catches_ps")
			.data(catches_pl,"catches_pl")
			.data(catches_gn,"catches_gn")
			.data(catches_var,"catches_var")
			.data(catches_mapc,"catches_mapc")
			.data(catches_shut,"catches_shut")
			.data(catches_lower,"catches_lower")
			.data(control_ups,"control_ups")
			.data(control_downs,"control_downs")
			.data(status_mean,"status_mean")
			.data(status_b10,"status_b10")
			.data(status_b20,"status_b20")
			.data(f_ratio,"f_ratio")
			.data(b_ratio,"b_ratio")
			.data(kobe_a,"kobe_a")
			.data(kobe_b,"kobe_b")
			.data(kobe_c,"kobe_c")
			.data(kobe_d,"kobe_d")
			.data(kobe_to_a,"kobe_to_a")
			.data(cpue_mean(WE,PS),"cpue_mean_we_ps")
			.data(cpue_mean(MA,PL),"cpue_mean_ma_pl")
			.data(cpue_mean(EA,GN),"cpue_mean_ea_gn")
		;
	}

	/**
	 * Record performance measures
	 */
	void record(uint time, const Model& model, double control = 1){
		//uint year = IOSKJ::year(time);
		uint quarter = IOSKJ::quarter(time);

		times.append();

		// Catch magnitude
		auto catch_total = model.catches_taken(sum);
		catches_total.append(catch_total);
		auto catches_by_method = model.catches_taken(sum,by(methods));
		catches_ps.append(catches_by_method(PS));
		catches_pl.append(catches_by_method(PL));
		catches_gn.append(catches_by_method(GN));

		// Years catch goes below baseline
		catches_lower.append(catch_total < 425000/4.0);

		// Catch variability
		if(quarter == 0 and catch_total>0){
			catches_var.append(catch_total);
			catches_mapc.append(catch_total);
		}
		// Shutdown defined as quarterly catches <10% of recent average
		// catches of about 400000t
		catches_shut.append(catch_total<1000);

		// Changes in MP control e.g. catch or effort limit
		if (quarter == 0) {
			if (std::isfinite(control_last_)) {
				control_ups.append(control>control_last_);
				control_downs.append(control<control_last_);
			}
			control_last_ = control;
		}

		// Stock status relative to unfished
		auto status = model.biomass_status();
		status_mean.append(status);
		status_b10.append(status<0.1);
		status_b20.append(status<0.2);

		// Biomass relative to B40
		auto b = model.biomass_spawners(sum)/model.biomass_spawners_40;
		b_ratio.append(b);
		// F relative to F40
		auto f = model.fishing_mortality_get()/model.f_40;
		f_ratio.append(f);

		// Kobe plot
		// Determine quadrant
		char quadrant;
		if(b>=1){
			if(f<=1) quadrant = 'a';
			else quadrant = 'b';
		} else {
			if(f<=1) quadrant = 'c';
			else quadrant = 'd';
		}
		// Update performance measures for proportion of time spent in each quadrant
		kobe_a.append(quadrant=='a');
		kobe_b.append(quadrant=='b');
		kobe_c.append(quadrant=='c');
		kobe_d.append(quadrant=='d');
		// Update performance measure for time taken to get back into quadrant A
		if(quadrant=='a'){
			// If previously outside of A then append the time that 
			// have been outside to the mean and reset time counter to zero.
			if(kobe_out_a>0){
				kobe_to_a.append(kobe_out_a);
				kobe_out_a = 0;
			}
		} else {
			// Outside of A so increment time counter.
			kobe_out_a++;
		}

		// Catch rates 
		// Use vulnerable (i.e. selected) biomass for the three main regions/gears
		// relative to the start year as a measure of catch rates (CPUE)
		if(times==1){
			// Record baselines
			for(auto region : regions){
				for(auto method : methods){
					cpue_baseline(region,method) = model.biomass_vulnerable(region,method);
				}
			}
		} else {
			// Record relative to baseline
			for(auto region : regions){
				for(auto method : methods){
					cpue_mean(region,method).append(
						model.biomass_vulnerable(region,method)/cpue_baseline(region,method)
					);
				}
			}
		}
	}

 private:

	// Last value of MP control
	double control_last_ = NAN;
};

}
