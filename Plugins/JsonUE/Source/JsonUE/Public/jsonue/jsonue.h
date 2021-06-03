
#pragma once

#include <map>
#include <vector>

namespace JsonUE {
	enum ValueType : uint8{
		nullValue = 0, ///< 'null' value
		intValue,      ///< signed integer value
		uintValue,     ///< unsigned integer value
		realValue,     ///< double value
		stringValue,   ///< UTF-8 string value
		booleanValue,  ///< bool value
		arrayValue,    ///< array value (ordered list)
		objectValue    ///< object value (collection of name/value pairs).
	};
	class Writer;
	class JSONUE_API Value {
		friend class Writer;
		typedef std::map<FString, Value> ObjectValues;
		typedef std::vector<Value> ArrayValues;
		using ArrayIndex =  int32 ;
		using Members = std::vector<FString>;
	public:
		void initBasic(ValueType type, uint8 Precision=0);
	public:

		Value(ValueType type = nullValue);
		Value(int32 value);
		Value(uint32 value);
		Value(int64 value);
		Value(uint64 value);
		Value(double value, uint8 precision=4);
		Value(const char* value); ///< Copy til first 0. (NULL causes to seg-fault.)
		//Value(const char* begin, const char* end); ///< Copy all, incl zeroes.

		Value(const wchar_t* value); //
		Value(const FString value);
		Value(bool value);
		Value(const Value& other);
		Value(Value&& other);
		~Value();


		Value& operator=(Value& other);
		Value& operator=(Value&& other);

		/// copy everything.
		void copy(const Value& other);
		/// copy values but leave comments and source offsets in place.
		void copyPayload(const Value& other);

		/// Compare payload only, not comments etc.
		bool operator<(const Value& other) const;
		bool operator<=(const Value& other) const;
		bool operator>=(const Value& other) const;
		bool operator>(const Value& other) const;
		bool operator==(const Value& other) const;
		bool operator!=(const Value& other) const;
		int compare(const Value& other) const;

		FString asString() const;
		int64 asInt64() const;
		uint64 asUInt64() const;
		float asFloat() const;
		double asDouble() const;
		bool asBool() const;

		bool isNull() const;
		bool isBool() const;
		bool isInt64() const;
		bool isUInt64() const;
		bool isIntegral() const;
		bool isDouble() const;
		bool isNumeric() const;
		bool isString() const;
		bool isArray() const;
		bool isObject() const;
		
		bool isConvertibleTo(ValueType other) const;
		/// Number of values in array or object
		int32 size() const;
		bool empty() const;

		/// Return !isNull()
		explicit operator bool() const;
		void clear() ;
		void resize(int32 newSize) ;
		//@{
		/// Access an array element (zero based index). If the array contains less
		/// than index element, then null value are inserted in the array so that
		/// its size is index+1.
		/// (You may need to say 'value[0u]' to get your compiler to distinguish
		/// this from the operator[] which takes a string.)
		Value& operator[](int32 index);
		const Value& operator[](int32 index) const ;

		Value get(ArrayIndex index, const Value& defaultValue) const;

		bool isValidIndex(ArrayIndex index) const;

		Value& append(const Value& value);
		Value& append(Value&& value);

		bool insert(ArrayIndex index, const Value& newValue);
		bool insert(ArrayIndex index, Value&& newValue);


		Value& operator[](const char* key);
		/// Access an object value by name, returns null if there is no member with
		/// that name.
		// const Value& operator[](const char* key) const;
		/// Access an object value by name, create a null member if it does not exist.
		/// \param key may contain embedded nulls.
		Value& operator[](const FString& key);
		/// Access an object value by name, returns null if there is no member with
		/// that name.
		/// \param key may contain embedded nulls.
		const Value& operator[](const FString& key) const;


		Value& operator-=(int32 index);
		Value& operator-=(const FString& key);
		Value& operator-=(const char* key);

		Value const* find(FString const key) const;

		Value get(const FString key, const Value& defaultValue) const;

		void removeMember(const FString& key);
		bool removeMember(const FString& key, Value* removed);

		bool removeIndex(ArrayIndex index, Value* removed);

		bool isMember(const FString& key) const;
		Members getMemberNames() const;


		ValueType type() const;
		static Value& nullSingleton();

	private:
		Value& resolveReference(const FString key);
		void releasePayload();
		void dupMeta(const Value& other);
		void swap(Value& other);
		void swapPayload(Value& other);

		void dupPayload(const Value& other);
	private:
		uint8 precision;
		struct {
			// Really a ValueType, but types should agree for bitfield packing.
			ValueType value_type_ : 8;
			// Unless allocated_, string_ must be null-terminated.
			//unsigned int allocated_ : 1;
		} bits_;

		union ValueHolder {
			int64 int_;
			uint64 uint_;
			double real_;
			bool bool_;
			FString* string_; // if allocated_, ptr to { unsigned, char[] }.
			ObjectValues* map_;
			ArrayValues* array_;
		} value_;
		void setType(ValueType v) {
			bits_.value_type_ = v;
		}
		//bool isAllocated() const { return bits_.allocated_; }
		//void setIsAllocated(bool v) { bits_.allocated_ = v; }
		

		// [start, limit) byte offsets in the source JSON text from which this Value
		// was extracted.
		//ptrdiff_t start_;
		//ptrdiff_t limit_;


  /// Minimum signed integer value that can be stored in a Json::Value.
		static constexpr int64 minLargestInt = int64(~(uint64(-1) / 2));
		/// Maximum signed integer value that can be stored in a Json::Value.
		static constexpr int64 maxLargestInt = int64(uint64(-1) / 2);
		/// Maximum unsigned integer value that can be stored in a Json::Value.
		static constexpr uint64 maxLargestUInt = uint64(-1);

		/// Minimum signed int value that can be stored in a Json::Value.
		static constexpr int32 minInt = int32(~(uint32(-1) / 2));
		/// Maximum signed int value that can be stored in a Json::Value.
		static constexpr int32 maxInt = int32(uint32(-1) / 2);
		/// Maximum unsigned int value that can be stored in a Json::Value.
		static constexpr uint32 maxUInt = uint32(-1);
		/// Minimum signed 64 bits int value that can be stored in a Json::Value.
		static constexpr int64 minInt64 = int64(~(uint64(-1) / 2));
		/// Maximum signed 64 bits int value that can be stored in a Json::Value.
		static constexpr int64 maxInt64 = int64(uint64(-1) / 2);
		/// Maximum unsigned 64 bits int value that can be stored in a Json::Value.
		static constexpr uint64 maxUInt64 = uint64(-1);
		// The constant is hard-coded because some compiler have trouble
		// converting Value::maxUInt64 to a double correctly (AIX/xlC).
		// Assumes that UInt64 is a 64 bits integer.
		static constexpr double maxUInt64AsDouble = 18446744073709551615.0;
	};
	class JSONUE_API Writer {
	public:
		Value  defaultStyle();// 
		Value& getStyle();
		void setStyle(Value&& config);
		FString toString(const Value& value);

	private:
		void ToString(const Value& value, FString& content, FString tab,Value& config);
		void ObjectToString(const Value& value, FString& content, FString tab, Value& config);
		void ArrayToString(const Value& value, FString& content, FString tab, Value& config);

		Value style;
	};
	class JSONUE_API Reader {
	private:
		using Char = TCHAR;
	public:
		bool parse(const FString& document, Value& root);

	private:
		bool parse(Value& root);
		bool parseObject(Value& root);
		bool parseArray(Value& root);

		void skipSpaces();
		Char getNextChar();
		bool readString();
		bool readNumber(FString& strNum, int& isBool);

		bool readComment();
		bool readCStyleComment();
		bool readCppStyleComment();
		bool match(const Char* pattern, int patternLength);
		bool readValue(const TCHAR*& current_, const TCHAR*& end_);
		int32 FullString(FString& dst, const TCHAR* begin_, const TCHAR* end_);
		void PrintError(const TCHAR* desc,const TCHAR* begin_, const TCHAR* end_);
		inline bool isValid();
		inline bool isSpaces(const TCHAR c);
		inline void Skip(int len);
		inline TCHAR getCurChar(int pos=0);
		inline const TCHAR* Current();
	private:
		const TCHAR* begin;
		const TCHAR* current;
		const TCHAR* end;
	};
	JSONUE_API double FStringToDouble(FString value);
	JSONUE_API float FStringToFloat(FString value);
	JSONUE_API FString DoubleToFString(double value, uint8 precision);
	JSONUE_API FString FloatToFString(float value, uint8 precision);
	JSONUE_API Value ToDouble(FString, uint8 precision);

}

