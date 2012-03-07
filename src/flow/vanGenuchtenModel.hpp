#ifndef _VANGENUCHTENMODEL_H_
#define _VANGENUCHTENMODEL_H_

#include "WaterRetentionBaseModel.hpp"

class vanGenuchtenModel : public WaterRetentionBaseModel {
  
public:
  vanGenuchtenModel(std::string region_, double m_, double alpha_, 
		    double sr_, double p_atm_);

  
  // overridden from WaterRetentionBaseModel
  double k_relative(double p);
  double saturation(double p);
  double d_saturation(double p);  

  void update_p_atm(double new_p_atm);
  const double get_p_atm() { return p_atm; };

  double pressure(double saturation);

private:
  double quadr_fn(double x, double p_ct, double f_pc_t);

  const double m;     // van Genuchten m
  double n;           // van Genuchten n
  const double alpha; // van Genuchten alpha 
  double p_atm;       // atmospheric pressure
  const double sr;    // van Genuchten effective saturation

};


#endif
