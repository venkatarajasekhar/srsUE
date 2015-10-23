/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2015 The srsUE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef USIM_H
#define USIM_H

#include <string>
#include "common/log.h"
#include "common/common.h"
#include "common/interfaces.h"

namespace srsue {

class usim
    :public usim_interface_nas
    ,public usim_interface_rrc
{
public:
  usim();
  void init(std::string imsi_, std::string imei_, std::string k_, srslte::log *usim_log_);
  void stop();

  void get_imsi_vec(uint8_t* imsi_, uint32_t n);
  void get_imei_vec(uint8_t* imei_, uint32_t n);

  void generate_authentication_response(uint8_t  *rand,
                                        uint8_t  *autn_enb,
                                        uint16_t  mcc,
                                        uint16_t  mnc,
                                        bool     *net_valid,
                                        uint8_t  *res);

  void generate_nas_keys(uint8_t *k_nas_enc,
                         uint8_t *k_nas_int);

  void generate_as_keys(uint32_t count_ul,
                        uint8_t *k_rrc_enc,
                        uint8_t *k_rrc_int,
                        uint8_t *k_up_enc,
                        uint8_t *k_up_int);


private:
  srslte::log *usim_log;

  // User data
  uint64_t imsi;
  uint64_t imei;
  uint8_t  k[16];

  // Security variables
  uint8_t  rand[16];
  uint8_t  ck[16];
  uint8_t  ik[16];
  uint8_t  ak[6];
  uint8_t  mac[8];
  uint8_t  autn[16];
  uint8_t  k_asme[32];
  uint8_t  k_enb[32];

};

} // namespace srsue


#endif // USIM_H