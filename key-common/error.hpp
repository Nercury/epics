#pragma once

#include <string>
#include <sstream>
#include <memory>

#include <boost/format.hpp>

#include <key-common/types.h>

template<class T>
struct fun_res_obj {
	uint16_t error_code;
	std::shared_ptr<T> result;
	std::shared_ptr<std::string> error_message;
	inline bool ok() { return error_code == 0 && result.use_count() > 0; }
	inline bool not_ok() { return !ok(); }
	inline std::string message(bool print_error_code = false) {
		std::ostringstream ss;
		if (print_error_code && error_code != 0)
			ss << "Error " << error_code << ".";
		if (error_message.use_count() > 0)
			ss << (ss.tellp() > 0 ? " " : "") << *error_message;
		return ss.str();
	}
};

typedef struct {
	uint16_t error_code;
	std::shared_ptr<std::string> error_message;
	inline bool ok() { return error_code == 0; }
	inline bool not_ok() { return !ok(); }
	inline std::string message(bool print_error_code = false) {
		std::ostringstream ss;
		if (print_error_code && error_code != 0)
			ss << "Error " << error_code << ".";
		if (error_message.use_count() > 0)
			ss << (ss.tellp() > 0 ? " " : "") << *error_message;
		return ss.str();
	}
} fun_res;

template<class T>
inline fun_res_obj<T> fun_ok(std::shared_ptr<T> & result) { 
	fun_res_obj<T> res = { 0, result }; return res; 
}

inline fun_res fun_ok() { 
	fun_res res = { 0 }; return res; 
}

template<class T>
inline fun_res_obj<T> fun_error(const char * error_message, uint16_t error_code = 1) {
	fun_res_obj<T> res;
	res.error_code = error_code;
	res.error_message = std::make_shared<std::string>(error_message); 
	return res;
}

template<class T>
inline fun_res_obj<T> fun_error(std::string error_message, uint16_t error_code = 1) {
	fun_res_obj<T> res;
	res.error_code = error_code;
	res.error_message = std::make_shared<std::string>(error_message); 
	return res;
}

template<class T>
inline fun_res_obj<T> fun_error(boost::format & error_message, uint16_t error_code = 1) {
	fun_res_obj<T> res;
	res.error_code = error_code;
	res.error_message = std::make_shared<std::string>(error_message.str()); 
	return res;
}

inline fun_res fun_error(const char * error_message, uint16_t error_code = 1) {
	fun_res res;
	res.error_code = error_code;
	res.error_message = std::make_shared<std::string>(error_message); 
	return res;
}

inline fun_res fun_error(std::string error_message, uint16_t error_code = 1) {
	fun_res res;
	res.error_code = error_code;
	res.error_message = std::make_shared<std::string>(error_message); 
	return res;
}

inline fun_res fun_error(boost::format & error_message, uint16_t error_code = 1) {
	fun_res res;
	res.error_code = error_code;
	res.error_message = std::make_shared<std::string>(error_message.str()); 
	return res;
}