/*
 * Copyright (c) 2008-2009 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "MacICACameraControl.h"
# include  <sstream>
# include  <iomanip>
# include  <stdlib.h>

using namespace std;

MacPTPCameraControl::MacPTPCameraControl(ICAObject dev)
: MacICACameraControl(dev),
    exposure_program_(0x500e /* PTP ExposureProgramMode */),
    fnumber_(0x5007 /* PTP FNumber */),
    iso_    (0x500f /* PTP ExposureIndex */)
{
      uint32_t result_code;

      ptp_get_property_desc_(exposure_program_, result_code);
      ptp_get_property_desc_(fnumber_, result_code);
      ptp_get_property_desc_(iso_, result_code);
}

MacPTPCameraControl::~MacPTPCameraControl()
{
}

ICAError MacICACameraControl::ica_send_message_(void*buf, size_t buf_len)
{
      ICAObjectSendMessagePB msg;
      memset(&msg, 0, sizeof(msg));

      msg.object = dev_;
      msg.message.messageType = kICAMessageCameraPassThrough;
      msg.message.startByte = 0;
      msg.message.dataPtr = buf;
      msg.message.dataSize = buf_len;
      msg.message.dataType = kICATypeData;

      return ICAObjectSendMessage(&msg, 0);
}

uint16_t MacPTPCameraControl::ptp_get_property_u16_(unsigned prop_code,
						    uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 2-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1015; // GetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 2;
      ptp_buf->data[0] = 0;
      ptp_buf->data[1] = 0;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      uint16_t val = ptp_buf->data[1];
      val <<= 8; val |= ptp_buf->data[0];
      return val;
}

uint32_t MacPTPCameraControl::ptp_get_property_u32_(unsigned prop_code,
						    uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 4-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1015; // GetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 4;
      ptp_buf->data[0] = 0;
      ptp_buf->data[1] = 0;
      ptp_buf->data[2] = 0;
      ptp_buf->data[3] = 0;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      uint32_t val = ptp_buf->data[3];
      val <<= 8; val |= ptp_buf->data[2];
      val <<= 8; val |= ptp_buf->data[1];
      val <<= 8; val |= ptp_buf->data[0];
      return val;
}

void MacPTPCameraControl::ptp_set_property_u16_(unsigned prop_code,
						uint16_t val,
						uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 2-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1016; // SetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruSend;
      ptp_buf->dataSize = 2;
      ptp_buf->data[0] = (val >> 0) & 0xff;
      ptp_buf->data[1] = (val >> 8) & 0xff;

      debug_log << "ptp_set_property_u16_: commandCode=" << ptp_buf->commandCode
		<< " params[0]=" << ptp_buf->params[0]
		<< " dataSize=" << ptp_buf->dataSize
		<< " data=" << hex
		<< setw(2) << ptp_buf->data[0]
		<< setw(2) << ptp_buf->data[1]
		<< endl << flush;
	    
      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;
}

void MacPTPCameraControl::ptp_set_property_u32_(unsigned prop_code,
						uint32_t val,
						uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 4-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1016; // SetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruSend;
      ptp_buf->dataSize = 4;
      ptp_buf->data[0] = (val >> 0) & 0xff;
      ptp_buf->data[1] = (val >> 8) & 0xff;
      ptp_buf->data[2] = (val >>16) & 0xff;
      ptp_buf->data[3] = (val >>24) & 0xff;

      debug_log << "ptp_set_property_u32_: commandCode=" << ptp_buf->commandCode
		<< " params[0]=" << ptp_buf->params[0]
		<< " dataSize=" << ptp_buf->dataSize
		<< " data=" << hex
		<< setw(2) << ptp_buf->data[0]
		<< setw(2) << ptp_buf->data[1]
		<< setw(2) << ptp_buf->data[2]
		<< setw(2) << ptp_buf->data[3]
		<< dec << endl << flush;

      ICAError err = ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;
      if (err != 0) result_code = -1;
}

MacPTPCameraControl::prop_desc_t::prop_desc_t(uint16_t prop_code)
{
      prop_code_ = prop_code;
      type_code_ = 0;
      set_flag_ = false;
}

MacPTPCameraControl::prop_desc_t::~prop_desc_t()
{
      switch (type_code_) {
	  case 1:
	    if (enum_int8_) delete enum_int8_;
	    break;
	  case 2:
	    if (enum_uint8_) delete enum_uint8_;
	    break;
	  case 3:
	    if (enum_int16_) delete enum_int16_;
	    break;
	  case 4:
	    if (enum_uint16_) delete enum_uint16_;
	    break;
	  case 5:
	    if (enum_int32_) delete enum_int32_;
	    break;
	  case 6:
	    if (enum_uint32_) delete enum_uint32_;
	    break;
	  default:
	    break;
      }
}

int MacPTPCameraControl::prop_desc_t::get_enum_count() const
{
      switch (type_code_) {
	  case 4:
	    if (enum_uint16_) return enum_uint16_->size();
	    else return 0;
	  default:
	    return 0;
      }
      return -1;
}

template<> uint16_t MacPTPCameraControl::prop_desc_t::get_enum_index(int idx)
{
      assert(type_code_ == 4);
      assert(enum_uint16_ != 0);
      assert((int)enum_uint16_->size() > idx);
      return (*enum_uint16_)[idx];
}

void MacPTPCameraControl::prop_desc_t::set_type_code(uint16_t code)
{
      assert(type_code_ == 0);
      type_code_ = code;
}

void MacPTPCameraControl::prop_desc_t::set_flag(bool flag)
{
      set_flag_ = flag;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int8_t>(int8_t val)
{
      assert(type_code_ == 1);
      fact_int8_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint8_t>(uint8_t val)
{
      assert(type_code_ == 2);
      fact_uint8_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int16_t>(int16_t val)
{
      assert(type_code_ == 3);
      fact_int16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint16_t>(uint16_t val)
{
      assert(type_code_ == 4);
      fact_uint16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int32_t>(int32_t val)
{
      assert(type_code_ == 5);
      fact_int32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint32_t>(uint32_t val)
{
      assert(type_code_ == 6);
      fact_uint32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int8_t>(const std::vector<int8_t>&ref)
{
      assert(type_code_ == 1);
      enum_int8_ = new std::vector<int8_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint8_t>(const std::vector<uint8_t>&ref)
{
      assert(type_code_ == 2);
      enum_uint8_ = new std::vector<uint8_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int16_t>(const std::vector<int16_t>&ref)
{
      assert(type_code_ == 3);
      enum_int16_ = new std::vector<int16_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint16_t>(const std::vector<uint16_t>&ref)
{
      assert(type_code_ == 4);
      enum_uint16_ = new std::vector<uint16_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int32_t>(const std::vector<int32_t>&ref)
{
      assert(type_code_ == 5);
      enum_int32_ = new std::vector<int32_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint32_t>(const std::vector<uint32_t>&ref)
{
      assert(type_code_ == 6);
      enum_uint32_ = new std::vector<uint32_t> (ref);
}

template <class T> static T val_from_bytes(UInt8*&buf);

template <> static int8_t val_from_bytes<int8_t>(UInt8*&buf)
{
      int8_t val = (int8_t) buf[0];
      buf += 1;
      return val;
}

template <> static uint8_t val_from_bytes<uint8_t>(UInt8*&buf)
{
      uint8_t val = (uint8_t) buf[0];
      buf += 1;
      return val;
}

template <> static int16_t val_from_bytes<int16_t>(UInt8*&buf)
{
      uint16_t val = (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 2;
      return (int16_t)val;
}

template <> static uint16_t val_from_bytes<uint16_t>(UInt8*&buf)
{
      uint16_t val = (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 2;
      return val;
}

template <> static int32_t val_from_bytes<int32_t>(UInt8*&buf)
{
      uint32_t val = (uint32_t) buf[3];
      val <<= 8;
      val |= (uint32_t) buf[2];
      val <<= 8;
      val |= (uint32_t) buf[1];
      val <<= 8;
      val |= (uint32_t) buf[0];
      buf += 4;
      return (int32_t)val;
}

template <> static uint32_t val_from_bytes<uint32_t>(UInt8*&buf)
{
      uint16_t val = (uint16_t) buf[3];
      val <<= 8;
      val |= (uint16_t) buf[2];
      val <<= 8;
      val |= (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 4;
      return val;
}

void MacPTPCameraControl::ptp_get_property_desc_(prop_desc_t&desc,
						 uint32_t&result_code)
{
      const size_t data_buf_size = 1024;
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + data_buf_size-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1014; // GetDevicePropDesc
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = desc.get_property_code();
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 1024;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      UInt8*dptr = ptp_buf->data;

	// data[0]
	// data[1] -- Property code back
      uint16_t prop = val_from_bytes<uint16_t>(dptr);
      assert(prop == desc.get_property_code());

	// data[2]
	// data[3] -- data type code
      uint16_t dtype = val_from_bytes<uint16_t>(dptr);
      desc.set_type_code(dtype);

	// data[4] -- GetSet flag
      uint8_t get_set_flag = val_from_bytes<uint8_t>(dptr);
      desc.set_flag(get_set_flag);

	// Starting at data[5]...
	//   -- Factory Default value
	//   -- Current value  (we ignore the current value here)
      switch (dtype) {
	  case 0:  // UNDEFINED
	  case 1:  // INT8
	    desc.set_factory_default(val_from_bytes<int8_t>(dptr));
	    val_from_bytes<int8_t>(dptr);
	    break;
	  case 2:  // UINT8
	    desc.set_factory_default(val_from_bytes<uint8_t>(dptr));
	    val_from_bytes<uint8_t>(dptr);
	    break;
	  case 3:  // INT16
	    desc.set_factory_default(val_from_bytes<int16_t>(dptr));
	    val_from_bytes<int16_t>(dptr);
	    break;
	  case 4:  // UINT16
	    desc.set_factory_default(val_from_bytes<uint16_t>(dptr));
	    val_from_bytes<uint16_t>(dptr);
	    break;
	  case 5:  // INT32
	    desc.set_factory_default(val_from_bytes<int32_t>(dptr));
	    val_from_bytes<int32_t>(dptr);
	    break;
	  case 6:  // UINT32
	    desc.set_factory_default(val_from_bytes<uint32_t>(dptr));
	    val_from_bytes<uint32_t>(dptr);
	    break;
	  case 7:  // INT64
	  case 8:  // UINT64
	  case 9:  // INT128;
	  case 10: // UINT128;
	  default:
	    break;
      }

	// The form flag (2==ENUM)
      uint8_t form_flag = *dptr++;

      if (form_flag == 2) { // ENUM
	    uint16_t count = val_from_bytes<uint16_t>(dptr);

	    switch (dtype) {
		case 1: { // INT8
		      vector<int8_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<int8_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 2: { // UINT8
		      vector<uint8_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<uint8_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 3: { // INT16
		      vector<int16_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<int16_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 4: { // UINT16
		      vector<uint16_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<uint16_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 5: { // INT32
		      vector<int32_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<int32_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 6: { // UINT32
		      vector<uint32_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<uint32_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		default:
		  break;
	    }
      } else {
      }
}

void MacPTPCameraControl::get_exposure_program_index(vector<string>&values)
{
      values.resize(exposure_program_.get_enum_count());
	// The ExposureProgramMode is by definition (PTP) a UINT16.
      assert(exposure_program_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    uint16_t value = exposure_program_.get_enum_index<uint16_t>(idx);
	    switch (value) {
		case 0x0000: // UNDEFINED
		  tmp << "NONE" << ends;
		  break;
		case 0x0001: // Manual
		  tmp << "Manual" << ends;
		  break;
		case 0x0002: // Automatic
		  tmp << "Automatic" << ends;
		  break;
		case 0x0003: // Aperture Priority
		  tmp << "Aperture Priority" << ends;
		  break;
		case 0x0004: // Shutter Priority
		  tmp << "Shutter Priority" << ends;
		  break;
		case 0x0005: // Program Creative (greater depth of field)
		  tmp << "Program Creative" << ends;
		  break;
		case 0x0006: // Program Action (faster shutter)
		  tmp << "Program Action" << ends;
		  break;
		case 0x0007: // Portrait
		  tmp << "Portrait" << ends;
		  break;
		default:
		  tmp << "Vendor program: 0x"
		      << setw(4) << hex << value << ends;
		  break;
	    }
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_exposure_program_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(exposure_program_.get_property_code(), rc);
      for (int idx = 0 ; idx < exposure_program_.get_enum_count() ; idx += 1) {
	    if (val == exposure_program_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_exposure_program_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= exposure_program_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(exposure_program_.get_property_code(),
			    exposure_program_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_exposure_program_ok()
{
      return exposure_program_.set_ok();
}

static const unsigned nikon_exposure_list_cnt = 53;
static const struct {
      const char*text;
      uint32_t code;
} nikon_exposure_list[nikon_exposure_list_cnt] = {
      { "1/4000", 0x00010f0a },
      { "1/3200", 0x00010c80 },
      { "1/2500", 0x000109c4 },
      { "1/2000", 0x000107d0 },
      { "1/1600", 0x00010640 },
      { "1/1250", 0x000104e2 },
      { "1/1000", 0x000103e8 },
      { "1/800",  0x00010320 },
      { "1/640",  0x00010280 },
      { "1/500",  0x000101f4 },
      { "1/400",  0x00010190 },
      { "1/320",  0x000100fa },
      { "1/250",  0x000100fa },
      { "1/200",  0x000100c8 },
      { "1/160",  0x000100a0 },
      { "1/125",  0x0001007d },
      { "1/100",  0x00010064 },
      { "1/80",   0x00010050 },
      { "1/60",   0x0001003c },
      { "1/50",   0x00010032 },
      { "1/40",   0x00010028 },
      { "1/30",   0x0001001e },
      { "1/25",   0x00010019 },
      { "1/20",   0x00010014 },
      { "1/15",   0x0001000f },
      { "1/13",   0x0001000d },
      { "1/10",   0x0001000a },
      { "1/8",    0x00010008 },
      { "1/6",    0x00010006 },
      { "1/5",    0x00010005 },
      { "1/4",    0x00010004 },
      { "1/3",    0x00010003 },
      { "1/2.5",  0x000a0019 },
      { "1/2",    0x00010002 },
      { "1/1.6",  0x000a0010 },
      { "1/1.3",  0x000a000d },
      { "1s",     0x00010001 },
      { "1.3s",   0x000d000a },
      { "1.6s",   0x0010000a },
      { "2s",     0x00020001 },
      { "2.5s",   0x0019000a },
      { "3s",     0x00030001 },
      { "4s",     0x00040001 },
      { "5s",     0x00050001 },
      { "6s",     0x00060001 },
      { "8s",     0x00080001 },
      { "10s",    0x000a0001 },
      { "13s",    0x000d0001 },
      { "15s",    0x000f0001 },
      { "20s",    0x00140001 },
      { "25s",    0x00190001 },
      { "30s",    0x001e0001 },
      { "Bulb",   0xffffffff }
};

void MacPTPCameraControl::get_exposure_time_index(vector<string>&values)
{
      values.resize(nikon_exposure_list_cnt);
      for (unsigned idx = 0 ; idx < nikon_exposure_list_cnt ; idx += 1)
	    values[idx] = nikon_exposure_list[idx].text;
}

int MacPTPCameraControl::get_exposure_time_index()
{
      uint32_t rc;
      uint32_t val = ptp_get_property_u32_(0xd100 /* NIKON ExposureTime */, rc);
      for (unsigned idx = 0 ; idx < nikon_exposure_list_cnt ; idx += 1) {
	    if (val == nikon_exposure_list[idx].code)
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_exposure_time_index(int use_index)
{
      debug_log << "set_exposure_time_index: use_index=" << use_index << endl << flush;
      if (use_index < 0)
	    return;
      if (use_index >= (int)nikon_exposure_list_cnt)
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u32_(0xd100 /* NIKON ExposureTime */,
			    nikon_exposure_list[use_index].code,
			    rc);
}

bool MacPTPCameraControl::set_exposure_time_ok()
{
      return true;
}

void MacPTPCameraControl::get_fnumber_index(vector<string>&values)
{
      values.resize(fnumber_.get_enum_count());
	// The FNumber is by definition (PTP) a UINT16.
      assert(fnumber_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    tmp << "f/" << (fnumber_.get_enum_index<uint16_t>(idx) / 100.0) << ends;
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_fnumber_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(fnumber_.get_property_code(), rc);
      for (int idx = 0 ; idx < fnumber_.get_enum_count() ; idx += 1) {
	    if (val == fnumber_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_fnumber_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= fnumber_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(fnumber_.get_property_code(),
			    fnumber_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_fnumber_ok()
{
      return fnumber_.set_ok();
}

void MacPTPCameraControl::get_iso_index(vector<string>&values)
{
      values.resize(iso_.get_enum_count());
	// The FNumber is by definition (PTP) a UINT16.
      assert(fnumber_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    tmp << iso_.get_enum_index<uint16_t>(idx) << ends;
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_iso_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(iso_.get_property_code(), rc);
      for (int idx = 0 ; idx < iso_.get_enum_count() ; idx += 1) {
	    if (val == iso_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_iso_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= iso_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(iso_.get_property_code(),
			    iso_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_iso_ok()
{
      return iso_.set_ok();
}

int MacPTPCameraControl::debug_property_get(unsigned prop,
					    unsigned dtype,
					    unsigned long&value)
{
      uint32_t rc = 0;
      switch (dtype) {
	  case 0x0004: // UINT16
	    value = ptp_get_property_u16_(prop, rc);
	    break;
	  case 0x0006: // UINT32
	    value = ptp_get_property_u32_(prop, rc);
	    break;
	  default:
	    return -1;
      }

      return (int)rc;
}

int MacPTPCameraControl::debug_property_set(unsigned prop,
					    unsigned dtype,
					    unsigned long value)
{
      uint32_t rc;
      switch (dtype) {
	  case 0x0004: // UINT16
	    ptp_set_property_u16_(prop, value, rc);
	    break;
	  case 0x0006: // UINT32
	    ptp_set_property_u32_(prop, value, rc);
	  default:
	    return -1;
      }
      return (int)rc;
}

static unsigned long val_from_bytes(UInt8*buf, size_t size)
{
      unsigned long val = 0;
      for (size_t idx = 0 ; idx < size ; idx += 1) {
	    val <<= 8UL;
	    val |= buf[size-idx-1];
      }
      return val;
}

string MacPTPCameraControl::debug_property_describe(unsigned prop_code)
{
      const size_t data_buf_size = 1024;
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + data_buf_size-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1014; // GetDevicePropDesc
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 1024;

      ica_send_message_(ptp_buf, sizeof buf);
	//result_code = ptp_buf->resultCode;

	// The send_message_ should cause the data to be written to
	// the data[] array, and the dataSize replaced with the actual
	// data count retrieved.

      char tmp_buf[32];
      string out ("Description of property: ");
      unsigned long tmp_val = 0;
      tmp_val = (ptp_buf->data[1] << 8) | ptp_buf->data[0];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%04lx\n", tmp_val);
      out += tmp_buf;

      tmp_val = (ptp_buf->data[3] << 8) | ptp_buf->data[2];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%04lx\n", tmp_val);
      out += "Data type: ";
      out += tmp_buf;

      size_t value_size = 0;
      switch (tmp_val) {
	  case 0:
	    break;
	  case 1: // 8
	  case 2:
	    value_size = 1;
	    break;
	  case 3:
	  case 4:
	    value_size = 2;
	    break;
	  case 5:
	  case 6:
	    value_size = 4;
	    break;
	  case 7:
	  case 8:
	    value_size = 8;
	    break;
	  default:
	    value_size = 0;
	    break;
      }

      tmp_val = ptp_buf->data[4];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%02lx\n", tmp_val);
      out += "GetSet flag: ";
      out += tmp_buf;

      int idx = 5;
      tmp_val = val_from_bytes(ptp_buf->data+idx, value_size);
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%lx\n", tmp_val);
      out += "Factury default: ";
      out += tmp_buf;
      idx += value_size;

      tmp_val = val_from_bytes(ptp_buf->data+idx, value_size);
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%lx\n", tmp_val);
      out += "Current value: ";
      out += tmp_buf;
      idx += value_size;

      int form_flag = ptp_buf->data[idx++];
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%02x\n", form_flag);
      out += "Form flag: ";
      out += tmp_buf;

      unsigned enum_count = 0;
      switch (form_flag) {
	  case 2: // ENUM
	    enum_count = val_from_bytes(ptp_buf->data+idx, 2);
	    idx += 2;

	    snprintf(tmp_buf, sizeof tmp_buf, "%u\n", enum_count);
	    out += "Enum count: ";
	    out += tmp_buf;
	    for (unsigned enum_idx = 0; enum_idx < enum_count; enum_idx += 1) {
		  tmp_val = val_from_bytes(ptp_buf->data+idx, value_size);
		  idx += value_size;
		  snprintf(tmp_buf, sizeof tmp_buf, " 0x%lx\n", tmp_val);
		  out += tmp_buf;
	    }
	  default:
	    break;
      }

      return out;
}
