
#include "jsonue/jsonue.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace JsonUE {
	template <typename T, typename U>
	static inline bool InRange(double d, T min, U max) {
		return d >= static_cast<double>(min) && d <= static_cast<double>(max);
	}
	Value::Value(ValueType type ) {
		initBasic(type);
		switch (type) {
		case stringValue:
			value_.string_ = new FString();
			break;
		case arrayValue:
			value_.array_ = new ArrayValues();
			break;
		case objectValue:
			value_.map_ = new ObjectValues();
			break;
		case booleanValue:
			value_.bool_ = false;
			break;
		default:
			memset(&value_,0,sizeof(value_));
		}
	}

	void Value::initBasic(ValueType type, uint8 Precision) {
		setType(type);
		precision = Precision;
	}

	Value::Value(int64 value) {
		initBasic(intValue);
		value_.int_ = value;
	}

	Value::Value(uint64 value) {
		initBasic(uintValue);
		value_.uint_ = value;
	}

	Value::Value(int32 value) {
		initBasic(intValue);
		value_.int_ = value;
	}

	Value::Value(uint32 value) {
		initBasic(uintValue);
		value_.uint_ = value;
	}

	Value::Value(double value,uint8 Precision) {
		initBasic(realValue, Precision);// 默认精度为2
		value_.real_ = value;
		//precision = Precision;// 默认精度为2
	}

	Value::Value(const char* value) {
		value_.string_ = new FString();// value;
		initBasic(stringValue, true);
		if (value != nullptr){
			*value_.string_ = UTF8_TO_TCHAR(value);
			if(value_.string_->Len()==0&& value[0]!='\0')// 默认转换失败
				*value_.string_ = ANSI_TO_TCHAR(value);
		}
	}

	Value::Value(const wchar_t* value) {
		initBasic(stringValue, true);
		value_.string_ = new FString(value);// value;
	}

	Value::Value(const FString value) {
		initBasic(stringValue);
		value_.string_ = new FString(value);// value;
	}
	Value::Value(bool value) {
		initBasic(booleanValue);
		value_.bool_ = value;
	}
	Value::Value(const Value& other) {
		dupPayload(other);
		//dupMeta(other);
	}

	Value::Value(Value&& other) {
		initBasic(nullValue);
		swap(other);
	}

	Value::~Value() {
		releasePayload();
		value_.uint_ = 0;
	}

	Value& Value::operator=(Value& other) {
		Value(other).swap(*this);
		//this->precision = other.precision;
		return *this;
	}

	Value& Value::operator=(Value&& other) {
		other.swap(*this);
		//this->precision = other.precision;
		return *this;
	}

	void Value::copyPayload(const Value& other) {
		releasePayload();
		dupPayload(other);
	}

	void Value::copy(const Value& other) {
		copyPayload(other);
		dupMeta(other);
	}


	int Value::compare(const Value& other) const {
		if (*this < other)
			return -1;
		if (*this > other)
			return 1;
		return 0;
	}

	bool Value::operator<(const Value& other) const {
		int typeDelta = type() - other.type();
		if (typeDelta)
			return typeDelta < 0;
		switch (type()) {
		case nullValue:
			return false;
		case intValue:
			return value_.int_ < other.value_.int_;
		case uintValue:
			return value_.uint_ < other.value_.uint_;
		case realValue:
			return value_.real_ < other.value_.real_;
		case booleanValue:
			return value_.bool_ < other.value_.bool_;
		case stringValue: {
			return *value_.string_ < *other.value_.string_;
		}
		case arrayValue: {
			auto thisSize = value_.array_->size();
			auto otherSize = other.value_.array_->size();
			if(value_.array_->size() != other.value_.array_->size())
				return thisSize < otherSize;
			for (int index = 0; index < value_.array_->size(); index++) {
				if ((*value_.array_)[index] >= (*other.value_.array_)[index])
					return false;
			}
			return true;
		}
		case objectValue: {
			auto thisSize = value_.map_->size();
			auto otherSize = other.value_.map_->size();
			if (thisSize < otherSize)
				return thisSize < otherSize;
			auto it = value_.map_->begin();
			for (; it!= value_.map_->end();it++) {
				auto right = other.value_.map_->find(it->first);
				if (right== other.value_.map_->end() || it->second >= right->second) return false;
			}
			return true;
		}
		default:
			return false;
		}
		return false; // unreachable
	}

	bool Value::operator<=(const Value& other) const { return !(other < *this); }

	bool Value::operator>=(const Value& other) const { return !(*this < other); }

	bool Value::operator>(const Value& other) const { return other < *this; }

	bool Value::operator==(const Value& other) const {
		if (type() != other.type())
			return false;
		switch (type()) {
		case nullValue:
			return true;
		case intValue:
			return value_.int_ == other.value_.int_;
		case uintValue:
			return value_.uint_ == other.value_.uint_;
		case realValue:
			return value_.real_ == other.value_.real_;
		case booleanValue:
			return value_.bool_ == other.value_.bool_;
		case stringValue: {
			return *value_.string_ == *other.value_.string_;
		}
		case arrayValue: {
			auto thisSize = value_.array_->size();
			auto otherSize = other.value_.array_->size();
			if (value_.array_->size() != other.value_.array_->size())
				return false;
			for (int index = 0; index < value_.array_->size(); index++) {
				if ((*value_.array_)[index] != (*other.value_.array_)[index])
					return false;
			}
			return true;
		}
		case objectValue:{
			auto thisSize = value_.map_->size();
			auto otherSize = other.value_.map_->size();
			if (thisSize != otherSize)
				return false;
			auto it = value_.map_->begin();
			for (; it != value_.map_->end(); it++) {
				auto right = other.value_.map_->find(it->first);
				if (right == other.value_.map_->end() || it->second != right->second) return false;
			} 
			return true;
		}
		default:
			return false;
		}
		return false; // unreachable
	}

	bool Value::operator!=(const Value& other) const { return !(*this == other); }


	FString Value::asString() const {
		if(stringValue==type())return *value_.string_;
		std::stringstream ssBuff;
		ssBuff<<std::left;
		switch (type()) {
		case intValue:ssBuff<< value_.int_; break;
		case uintValue:ssBuff << value_.uint_; break;
		case booleanValue:ssBuff<<value_.bool_?"true":"false"; break;
		case realValue: {// 设置输出精度
			return DoubleToFString(value_.real_, precision);
		}
		}
		return FString(ssBuff.str().c_str());
	}

	int64 Value::asInt64() const {
		switch (type()) {
		case intValue:
			return value_.int_;
		case uintValue:
			return int64(value_.uint_);
		case realValue:
			return int64(value_.real_);
		case nullValue:
			return 0;
		case booleanValue:
			return value_.bool_ ? 1 : 0;
		default:
			break;
		}
		return 0;
	}

	uint64 Value::asUInt64() const {
		switch (type()) {
		case intValue:
		case uintValue:
			return value_.int_;
		case realValue:
			return uint64(value_.real_);
		case nullValue:
			return 0;
		case booleanValue:
			return value_.bool_ ? 1 : 0;
		default:
			break;
		}
		return 0;
	}

	double Value::asDouble() const {
		switch (type()) {
		case intValue:
			return static_cast<double>(value_.int_);
		case uintValue:
			return static_cast<double>(value_.uint_);
		case realValue:
			return value_.real_;
		case nullValue:
			return 0.0;
		case booleanValue:
			return value_.bool_ ? 1.0 : 0.0;
		default:
			break;
		}
		return 0.0;
	}

	float Value::asFloat() const {
		switch (type()) {
		case intValue:
			return static_cast<float>(value_.int_);
		case uintValue:
			return static_cast<float>(value_.uint_);
		case realValue:
			return static_cast<float>(value_.real_);
		case nullValue:
			return 0.0;
		case booleanValue:
			return value_.bool_ ? 1.0F : 0.0F;
		default:
			break;
		}
		return 0.0;
	}

	bool Value::asBool() const {
		switch (type()) {
		case booleanValue:
			return value_.bool_;
		case intValue:
			return value_.int_ != 0;
		case uintValue:
			return value_.uint_ != 0;
		case realValue: {
			// According to JavaScript language zero or NaN is regarded as false
			const auto value_classification = std::fpclassify(value_.real_);
			return value_classification != FP_ZERO && value_classification != FP_NAN;
		}
		default:
			break;
		}
		return false;
	}

	static bool IsIntegral(double d) {
		double integral_part;
		return modf(d, &integral_part) == 0.0;
	}

	bool Value::isNull() const { return type() == nullValue; }

	bool Value::isBool() const { return type() == booleanValue; }

	bool Value::isInt64() const {
		switch (type()) {
		case intValue:
			return true;
		case uintValue:
			return value_.uint_ <= uint64(maxInt64);
		case realValue:
			return value_.real_ >= double(minInt64) &&
				value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
		default:
			break;
		}
		return false;
	}

	bool Value::isUInt64() const {
		switch (type()) {
		case intValue:
			return value_.int_ >= 0;
		case uintValue:
			return true;
		case realValue:
			return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble && IsIntegral(value_.real_);
		default:
			break;
		}
		return false;
	}

	bool Value::isIntegral() const {
		switch (type()) {
		case intValue:
		case uintValue:
			return true;
		case realValue:
			return value_.real_ >= double(minInt64) &&
				value_.real_ < maxUInt64AsDouble && IsIntegral(value_.real_);
		default:
			break;
		}
		return false;
	}

	bool Value::isDouble() const {
		return type() == intValue || type() == uintValue || type() == realValue;
	}

	bool Value::isNumeric() const { return isDouble(); }

	bool Value::isString() const { return type() == stringValue; }

	bool Value::isArray() const { return type() == arrayValue; }

	bool Value::isObject() const { return type() == objectValue; }
	
	bool Value::isConvertibleTo(ValueType other) const {
		switch (other) {
		case nullValue:
			return (isNumeric() && asDouble() == 0.0) ||
				(type() == booleanValue && !value_.bool_) ||
				(type() == stringValue && asString().IsEmpty()) ||
				(type() == arrayValue && value_.array_->size()==0) ||
				(type() == objectValue && value_.map_->size() == 0) ||
				type() == nullValue;
		case intValue:
			return isInt64() ||
				(type() == realValue && InRange(value_.real_, minInt64, maxInt64)) ||
				type() == booleanValue || type() == nullValue;
		case uintValue:
			return isUInt64() ||
				(type() == realValue && InRange(value_.real_, 0, maxUInt64)) ||
				type() == booleanValue || type() == nullValue;
		case realValue:
			return isNumeric() || type() == booleanValue || type() == nullValue;
		case booleanValue:
			return isNumeric() || type() == booleanValue || type() == nullValue;
		case stringValue:
			return isNumeric() || type() == booleanValue || type() == stringValue || type() == nullValue;
		case arrayValue:
			return type() == arrayValue || type() == nullValue;
		case objectValue:
			return type() == objectValue || type() == nullValue;
		}
		return false;
	}

	/// Number of values in array or object
	int32 Value::size() const {
		switch (type()) {
		case arrayValue: // size of the array is highest index + 1
			return value_.array_->size();
		case objectValue:
			return value_.map_->size();
		}
		return 0;
	}

	bool Value::empty() const {
		if (isNull() || isArray() || isObject())
			return size() == 0U;
		return false;
	}


	Value::operator bool() const { return !isNull(); }

	void Value::clear() {
		releasePayload();
	}

	void Value::resize(int32 newSize) {
		if (type() == nullValue)
			*this = Value(arrayValue);
		if (type() != arrayValue)return;
		value_.array_->resize(newSize);
	}

	Value& Value::operator[](int32 index) {
		if (type()== nullValue)
			*this = Value(arrayValue);
		if (type() != arrayValue) {// 不为数组
			return nullSingleton();
		}
		if (value_.array_->size() <= index)resize(index + 1);
		return (*value_.array_)[index];
	}

	const Value& Value::operator[](int32 index) const {
		if (value_.array_->size() <= index) {//resize(index + 1);
			return nullSingleton();
		}
		return (*value_.array_)[index];
	}

	Value Value::get(ArrayIndex index, const Value& defaultValue) const {
		const Value* value = &((*this)[index]);
		return value->type() == nullValue ? defaultValue : *value;
	}
	
	bool Value::isValidIndex(ArrayIndex index) const { return index < size(); }
	
	Value& Value::append(const Value& value) { return append(Value(value)); }

	Value& Value::append(Value&& value) {
		if (type() == nullValue) {// 
			*this = Value(value.type());
		}
		// 
		if (type() != value.type())return *this;// 如果类型不一致不能append
		switch (type()) {
		case stringValue:
			value_.string_->Append(*value.value_.string_);
		case arrayValue:
			value_.array_->insert(value_.array_->end(), value.value_.array_->begin(), value.value_.array_->end()); break;
		case objectValue: {
			for (auto it = value.value_.map_->begin(); it != value.value_.map_->end(); it++) {
				(*value_.map_)[it->first] = it->second;
			}
		}
		}
		return *this;
	}

	bool Value::insert(ArrayIndex index, const Value& newValue) {
		return insert(index, Value(newValue));
	}

	bool Value::insert(ArrayIndex index, Value&& newValue) {
		if (type() == nullValue)
			*this = Value(arrayValue);
		if (type() != arrayValue)return false;
		if (size() < index) {
			return false;
		}
		else if (size() < index) value_.array_->push_back(std::move(newValue));
		else  value_.array_->insert(value_.array_->begin()+ index, std::move(newValue));
		return true;
	}

	Value const* Value::find(FString const key) const {
		if (type() != objectValue)
			return nullptr;
		ObjectValues::const_iterator it = value_.map_->find(key);
		if (it == value_.map_->end())
			return nullptr;
		return &(*it).second;
	}
	//const Value& Value::operator[](const char* key) const {
	//	Value const* found = find(key);
	//	if (!found)
	//		return nullSingleton();
	//	return *found;
	//}

	Value const& Value::operator[](const FString& key) const {
		Value const* found = find(key);
		if (!found)
			return nullSingleton();
		return *found;
	}

	Value& Value::operator[](const char* key) {
		return resolveReference(FString(key));
	}

	Value& Value::operator[](const FString& key) {
		return resolveReference(key);
	}


	Value& Value::operator-=(int32 index) {
		if(type()!=arrayValue || value_.array_->size()<= index|| index<0)return *this;
		value_.array_->erase(value_.array_->begin()+index);
		return *this;
	}
	Value& Value::operator-=(const FString& key) {
		removeMember(key);
		return *this;
	}

	Value& Value::operator-=(const char* key) {
		removeMember(FString(key));
		return *this;
	}

	Value Value::get(const FString key, const Value& defaultValue) const {
		Value const* found = find(key);
		return !found ? defaultValue : *found;
	}

	void Value::removeMember(const FString& key) {
		value_.map_->erase(key);
	}

	bool Value::removeMember(const FString& key, Value* removed) {
		if (type() != objectValue) {
			return false;
		}
		auto it = value_.map_->find(key);
		if (it == value_.map_->end())
			return false;
		if (removed)
			*removed = std::move(it->second);
		value_.map_->erase(it);
		return true;
	}

	bool Value::removeIndex(ArrayIndex index, Value* removed) {
		if (type() != arrayValue || size() <= index) {
			return false;
		}
		*removed = (*value_.array_)[index];
		value_.array_->erase(value_.array_->begin()+ index);
		return true;
	}

	bool Value::isMember(const FString& key) const {
		return nullptr != find(key);
	}

	Value::Members Value::getMemberNames() const {
		if (type() != objectValue)
			return Value::Members();
		Members members;
		members.reserve(value_.map_->size());
		ObjectValues::const_iterator it = value_.map_->begin();
		ObjectValues::const_iterator itEnd = value_.map_->end();
		for (; it != itEnd; ++it) {
			members.push_back(it->first);
		}
		return members;
	}

// Access an object value by name, create a null member if it does not exist.
// @pre Type of '*this' is object or null.
// @param key is null-terminated.
	Value& Value::resolveReference(const FString key) {
		if (type() == nullValue)
			*this = Value(objectValue);
		if (type() != objectValue) {
			return nullSingleton();
		}
		return (*value_.map_)[key];
	}

	void Value::releasePayload() {
		switch (type()) {
		case stringValue:
			if(value_.string_) delete value_.string_;
			break;
		case arrayValue:
			if (value_.array_) delete value_.array_;
			break;
		case objectValue:
			if (value_.map_) delete value_.map_;
			break;
		default:
			break;
		}
		memset(&value_, 0, sizeof(value_));
		setType(nullValue);
	}

	ValueType Value::type() const {
		return bits_.value_type_;
	}

	void Value::dupMeta(const Value& other) {
		//comments_ = other.comments_;
		//start_ = other.start_;
		//limit_ = other.limit_;
	}

	void Value::swap(Value& other) {
		swapPayload(other);
		//std::swap(comments_, other.comments_);
		//std::swap(start_, other.start_);
		//std::swap(limit_, other.limit_);
	}

	void Value::swapPayload(Value& other) {
		std::swap(bits_, other.bits_);
		std::swap(value_, other.value_);
		std::swap(precision, other.precision);
	}

	void Value::dupPayload(const Value& other) {
		setType(other.type());
		switch (type()) {
		case nullValue:
		case intValue:
		case uintValue:
		case realValue:
		case booleanValue:
			precision = other.precision;
			value_ = other.value_;
			break;
		case stringValue:
			value_.string_ = new FString(*other.value_.string_);
			break;
		case arrayValue:
			value_.array_ = new ArrayValues(*other.value_.array_);
			break;
		case objectValue:
			value_.map_ = new ObjectValues(*other.value_.map_);
			break;
		default:
			return ;
		}
	}
	// static
	Value& Value::nullSingleton() {
		static Value nullStatic;
		nullStatic.releasePayload();
		return nullStatic;
	}


	Value Writer::defaultStyle() {
		Value setting;
		setting["tab"] = "\t"; // 控制缩进
		setting["enter"] = "\n"; // 控制换行
		return setting;
	}

	Value& Writer::getStyle() {
		return style;
	}

	void Writer::setStyle(Value&& config) {
		style.clear();
		style = config;
	}

	FString Writer::toString(const Value& value) {
		FString content;
		ToString(value, content, "", style);
		return content;
	}


	void Writer::ToString(const Value& value, FString& content, FString tab, Value& config) {
		switch (value.type()) {
		case nullValue: content.Append("null"); return;
		case realValue:
		case uintValue:
		case intValue: content.Append(value.asString()); return;
		case booleanValue:content.Append(value.asBool()?"true":"false"); return;
		case stringValue: content.Append("\"").Append(value.asString()).Append("\""); return;
		case objectValue: ObjectToString(value, content, tab, config); return;
		case arrayValue: ArrayToString(value, content, tab, config); return;
		}
	}

	void Writer::ObjectToString(const Value& value, FString& content, FString tab, Value& config) {
		FString enter = config["enter"].asString();
		FString nTab = tab+ config["tab"].asString();
		content.Append("{");
		int index = 0;
		for (auto it = value.value_.map_->begin(); it != value.value_.map_->end(); it++, index++) {
			if (index)content.Append(",");
			content.Append(enter).Append(nTab).Append("\"").Append(it->first).Append("\":");
			ToString(it->second, content, nTab, config);
		}
		if (index)content.Append(enter).Append(tab);
		content.Append("}");
	}

	void Writer::ArrayToString(const Value& value, FString& content, FString tab, Value& config) {
		FString enter = config["enter"].asString();
		FString nTab = tab + config["tab"].asString();
		content.Append("[");
		int index = 0;
		for (auto it = value.value_.array_->begin(); it != value.value_.array_->end(); it++, index++) {
			if (index)content.Append(",");
			content.Append(enter).Append(nTab);
			ToString(*it, content, nTab, config);
		}
		if (index)content.Append(enter).Append(tab);
		content.Append("]");
	}

	inline bool Reader::isSpaces(const TCHAR c){
		return c == ' ' || c == '\t' || c == '\r' || c == '\n';
	}
	inline void Reader::Skip(int len) {
		current += len;
	}
	inline TCHAR Reader::getCurChar(int pos) {
		return *(current+pos);
	}
	inline const TCHAR* Reader::Current() {
		return current;
	}

	void Reader::skipSpaces() {
		for (; isValid(); current++) {
			if (!isSpaces(*current))
				break;// 非空格
		}
	}

	Reader::Char Reader::getNextChar() {
		return isValid() ? *current++ : 0;
	}

	bool Reader::readString() {
		Char c = TCHAR('\0');
		while (isValid()) {
			c = getNextChar();
			if (c == '"')break;
			else if(c == '\\')Skip(1);// 转义字符跳过一个
		}
		return c == TCHAR('"');
	}

	bool Reader::readCStyleComment() {
		while (isValid()) {
			Char c = getNextChar();
			if (c == Char('*') && getCurChar() == Char('/')) {
				Skip(1);
				return true;
			}
		}
		return false;
	}

	bool Reader::readCppStyleComment() {
		while (isValid()) {
			Char c = getNextChar();
			if (c == '\n')
				break;
			if (c == '\r') {
				// Consume DOS EOL. It will be normalized in addComment.
				if (isValid() && getCurChar() == '\n')
					Skip(1);
				// Break on Moc OS 9 EOL.
				break;
			}
		}
		return true;
	}

	bool Reader::readComment() {
		Char c = getNextChar();
		if (c == Char( '*'))
			return readCStyleComment();
		else if (c == Char('/'))
			return readCppStyleComment();
		return false;
	}

	bool Reader::readNumber(FString& strNum, int& isFloat) {
		#define ISNUMBER(c) ('0' <= (c)&& (c)<= '9')
		const TCHAR* p = Current();
		Char c = '0'; 
		strNum.Empty();
		isFloat = -1;
		int isE=0;
		if (getCurChar() == '-' && ISNUMBER(getCurChar(1))) { strNum.AppendChar(getNextChar()); }
		for (; isValid(); Skip(1)) {
			if (ISNUMBER(getCurChar())) {
				strNum.AppendChar(getCurChar());
				if (isE != 1 && isFloat != -1)isFloat++;// 记录小数精度
			}
			else if (isFloat ==-1  && getCurChar() == '.') {// 分割
				if (ISNUMBER(getCurChar(1))) {//如果下个
					strNum.AppendChar(getCurChar());
					isFloat=0;
				}
				else if ((getCurChar(1) =='e'|| getCurChar(1) =='E') && ISNUMBER(getCurChar(2))) {
					strNum.AppendChar(getNextChar());// 先放入一个小数点
					strNum.AppendChar(getCurChar());// 再放入科学计数
					isFloat=0;
				}
				else {// 结束数字
					return strNum.Len()!=0;
				}
			}
			else if ((isE == 0)&&(getCurChar() == 'e' || getCurChar() == 'E') && ISNUMBER(getCurChar(1))) { // 科学计数法
				strNum.AppendChar(getNextChar());// 先放入一个小数点
				isE = 1;
			}
			else {
				return strNum.Len() != 0;
			}
		}
		return false ;
	}

	bool Reader::match(const Char* pattern, int patternLength) {
		if (memcmp(Current(), pattern, patternLength * sizeof(TCHAR)))
			return false;
		Skip(patternLength);
		return true;
	}

	bool Reader::parse(const FString& document, Value& root) {
		root.clear();
		const TCHAR* begin_ = document.GetCharArray().GetData();
		const TCHAR* end_ = document.GetCharArray().GetData() + document.Len();
		current = begin = begin_;
		end = end_;
		return parse(root);
	}

	int32 Reader::FullString(FString& dst, const TCHAR* begin_, const TCHAR* end_) {
		dst=FString();
		for (; begin_ < end_; begin_++) {
			dst.AppendChar(*begin_);
		}
		return dst.Len();
	}

	void Reader::PrintError(const TCHAR* desc,const TCHAR* begin_, const TCHAR* end_) {
		FString errInfo;
		// 出错位置的前后10个字符一起打印便于查找问题
		if (Current() + 10 < begin_) begin_ -= 10;
		if (end_ + 10 < end) end_ += 10;
		FullString(errInfo, begin_, end_);
		UE_LOG(LogTemp,Warning,TEXT("%s:%s"), desc,*errInfo);
	}


	bool Reader::parseArray(Value& root) {
		//FString Key;
		FString FirstStr;
		int isFloat = 0;
		int sizeRoot = root.size();
		const TCHAR* prePos = Current();
		for (; isValid(); ) {// 循环解析
			skipSpaces();// 去空格
			prePos = Current();
			TCHAR c = getNextChar();
			switch (c) { //
			case '{':// 这里有一个object对象
				if (!parseObject(root[root.size()]))return false;// 解析格式错误
				break;
			case '[':// 这里表示有一个数组对象需要解析
				if (!parseArray(root[root.size()]))return false;// 解析格式错误
				break;
			case ']':// 当前数组解析完毕
				return true;
				break;
			case '"': {
				// 这里是一个Key 或者是 Value
				if (!readString()) {
					PrintError(TEXT("字符串解析错误"), prePos, Current());
					return false;
				}
				FullString(FirstStr, prePos + 1, Current() - 1);
				root[root.size()] = FirstStr;
				break;
			}
			case '/'://这里是注释解析
				if (!readComment()) {
					PrintError(TEXT("注释解析错误"), prePos, Current());
					return false;
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-': {
				Skip(-1);
				if (!readNumber(FirstStr, isFloat) || 20 <= FirstStr.Len()) {
					PrintError(TEXT("数字解析错误"), prePos, Current());
					return false;
				};
				if (0<=isFloat) {// 浮点数
					root[root.size()] = Value(FCString::Atof(*FirstStr), isFloat);
				}
				else {//整形数据
					root[root.size()] = Value(FCString::Atoi64(*FirstStr));
				}
				break;
			}
			case 't':
				if (!match(L"rue", 3)) {
					PrintError(TEXT("true 解析错误"), prePos, Current() + 3);
					return false;
				}
				root[root.size()] = Value(true);
				break;
			case 'f':
				if (!match(L"alse", 4)) {
					PrintError(TEXT("false 解析错误"), prePos, Current() + 4);
					return false;
				}
				root[root.size()] = Value(false);
				break;
			case 'n':
				if (!match(L"ull", 3)) {
					PrintError(TEXT("null 解析错误"), prePos, Current() + 3);
					return false;
				}
				root[root.size()];
				break;
			case ',':
				break;
			case 0:
				return true;
			default:
				PrintError(TEXT("other 解析错误"), prePos, Current() + 3);
				return false;
			}
		}
		return true;
	}

	bool Reader::parseObject(Value& root) {
		FString Key;
		FString FirstStr;
		int isFloat = 0;
		int sizeRoot= root.size();
		const TCHAR* prePos = Current();
		for (; isValid(); ) {// 循环解析
			if (sizeRoot != root.size()) {// 说明上次已经循环添加完毕一个对象
				sizeRoot = root.size();
				FirstStr=Key=FString();
			}
			skipSpaces();// 去空格
			prePos = Current();
			TCHAR c = getNextChar();
			switch (c) { //
			case '{':// 这里有一个object对象
				if(!parseObject(root[Key]))return false ;// 解析格式错误
				break;
			case '}':// 表示当前对象解析完毕
				return true;
				break;
			case ':':// 这里读取到分隔符
				Key = FirstStr;
				break;
			case '[':// 这里表示有一个数组对象需要解析
				if (!parseArray(root[Key]))return false;// 解析格式错误
				break;
			case ']':
				PrintError(TEXT("对象包含错误的结束符"), prePos, Current());
				return false;
				break;
			case '"': {
				// 这里是一个Key 或者是 Value
				if (!readString()) {
					PrintError(TEXT("字符串解析错误"), prePos, Current());
					return false;
				}
				FullString(FirstStr,prePos+1, Current()-1);
				if (Key.Len()) {//不为空说明已经读取到了key这个值是一个value值
					root[Key] = FirstStr;
				}
				break;
			}
			case '/'://这里是注释解析
				if (!readComment()) {
					PrintError(TEXT("注释解析错误"), prePos, Current());
					return false;
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-': {
				Skip(-1);
				if (!readNumber(FirstStr, isFloat)|| 20<=FirstStr.Len()) {
					PrintError(TEXT("数字解析错误"), prePos, begin);
					return false;
				};
				if (0 <= isFloat) {// 浮点数
					root[Key] = Value(FCString::Atof(*FirstStr), isFloat);
				}
				else {//整形数据
					root[Key] = Value(FCString::Atoi64(*FirstStr));
				}
				break;
			}
			case 't':
				if (!match(L"rue", 3)) {
					PrintError(TEXT("true 解析错误"), prePos, Current() +3);
					return false;
				}
				root[Key] = Value(true);
				break;
			case 'f':
				if (!match(L"alse", 4)) {
					PrintError(TEXT("false 解析错误"), prePos, Current() + 4);
					return false;
				}
				root[Key] = Value(false);
				break;
			case 'n':
				if (!match(L"ull", 3)) {
					PrintError(TEXT("null 解析错误"), prePos, Current() + 3);
					return false;
				}
				root[Key];
				break;
			case ',':
				break;
			case 0:
				return true;
				//PrintError(TEXT("null 解析错误"), prePos, begin + 3);
				//return false;
			default:
				PrintError(TEXT("other 解析错误"), prePos, begin + 3);
				return false;
			}
		}
		return true;
	}

	inline bool Reader::isValid() {
		return current<end ;
	}

	bool Reader::parse(Value& root) {
		const TCHAR* prePos = begin;
		for (; isValid();) {// 循环解析
			skipSpaces();// 去空格
			TCHAR c = getNextChar();
			switch (c) { //
			case '{':
				return parseObject(root);
				break;
			case '[':
				return parseArray(root);
				break;
			case 0:
				return true;
			default:
				PrintError(TEXT("Reader::parse解析错误"), prePos, begin + 3);
				return false;
				break;
			}
		}
		return true;
	}

	double FStringToDouble(FString value){
		std::stringstream ss;
		double dValue;
		ss << TCHAR_TO_UTF8(*value);
		ss >> dValue;
		return dValue;
	}
	FString DoubleToFString(double value, uint8 precision) {
		std::stringstream ss;
		ss.precision(precision);
		ss.setf(std::ios::fixed);
		ss << value;
		return FString(ss.str().c_str());
	}

	float FStringToFloat(FString value) {
		std::stringstream ss;
		float dValue;
		ss << TCHAR_TO_UTF8(*value);
		ss >> dValue;
		return dValue;
	}
	FString FloatToFString(float value, uint8 precision) {
		std::stringstream ss;
		ss.precision(precision);
		ss.setf(std::ios::fixed);
		ss << value;
		return FString(ss.str().c_str());
	}
	Value ToDouble(FString num, uint8 precision) {
		return Value(FStringToDouble(num), precision);
	}


}
