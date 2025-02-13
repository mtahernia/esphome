#pragma once

#include <array>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "remote_base.h"

namespace esphome {
namespace remote_base {

class CarrierData {
 public:
  // Make default
  CarrierData() {}
  // Make from initializer_list
  CarrierData(std::initializer_list<uint8_t> data) {
    std::copy_n(data.begin(), std::min(data.size(), this->data_.size()), this->data_.begin());
  }
  // Make from vector
  CarrierData(const std::vector<uint8_t> &data) {
    std::copy_n(data.begin(), std::min(data.size(), this->data_.size()), this->data_.begin());
  }

  uint8_t *data() { return this->data_.data(); }
  const uint8_t *data() const { return this->data_.data(); }
  uint8_t size() const { return this->data_.size(); }
  bool is_valid() const { return this->data_[OFFSET_CS] == this->calc_cs_(); }
  void finalize() { this->data_[OFFSET_CS] = this->calc_cs_(); }
  bool is_compliment(const CarrierData &rhs) const;
  std::string to_string() const { return format_hex_pretty(this->data_.data(), this->data_.size()); }
  // compare only 40-bits
  bool operator==(const CarrierData &rhs) const {
    return std::equal(this->data_.begin(), this->data_.begin() + OFFSET_CS, rhs.data_.begin());
  }
  enum CarrierDataType : uint8_t {
    CARRIER_TYPE_CONTROL = 0xA1,
    CARRIER_TYPE_SPECIAL = 0xA2,
    CARRIER_TYPE_FOLLOW_ME = 0xA4,
  };
  CarrierDataType type() const { return static_cast<CarrierDataType>(this->data_[0]); }
  template<typename T> T to() const { return T(*this); }
  uint8_t &operator[](size_t idx) { return this->data_[idx]; }
  const uint8_t &operator[](size_t idx) const { return this->data_[idx]; }

 protected:
  uint8_t get_value_(uint8_t idx, uint8_t mask = 255, uint8_t shift = 0) const {
    return (this->data_[idx] >> shift) & mask;
  }
  void set_value_(uint8_t idx, uint8_t value, uint8_t mask = 255, uint8_t shift = 0) {
    this->data_[idx] &= ~(mask << shift);
    this->data_[idx] |= (value << shift);
  }
  void set_mask_(uint8_t idx, bool state, uint8_t mask = 255) { this->set_value_(idx, state ? mask : 0, mask); }
  static const uint8_t OFFSET_CS = 5;
  // 48-bits data
  std::array<uint8_t, 6> data_;
  // Calculate checksum
  uint8_t calc_cs_() const;
};

class CarrierProtocol : public RemoteProtocol<CarrierData> {
 public:
  void encode(RemoteTransmitData *dst, const CarrierData &src) override;
  optional<CarrierData> decode(RemoteReceiveData src) override;
  void dump(const CarrierData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Carrier)

template<typename... Ts> class CarrierAction : public RemoteTransmitterActionBase<Ts...> {
  TEMPLATABLE_VALUE(std::vector<uint8_t>, code)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    CarrierData data(this->code_.value(x...));
    data.finalize();
    CarrierProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
